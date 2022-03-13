#define STRICT
#include "WinAudioQueue.h"

namespace mmapp {

static HANDLE   g_notification_event;
static bool     g_proc_done;

static DWORD WINAPI NotificationProc( LPVOID lpParameter );

WinAudioQueue::WinAudioQueue()
{
    _first_init_done = false;
    _gpds = NULL;
    _pPRM = NULL;
    _sMix = NULL;
    _thread_id = 0;
    _thread_handle = g_notification_event = NULL;
    _min_rate = _max_rate = 0;
    error_ = nullptr;
}

WinAudioQueue::~WinAudioQueue()
{
    shutdown();
}

bool WinAudioQueue::init(HWND hWnd, int32_t channels, int32_t rate, int32_t frames_in_buffer, ISoundSynth *callback)
{
    HRESULT             hr;
    void                *dumptr;
    DSBUFFERDESC        dsbd;
    WAVEFORMATEX        wfx;
    LPDIRECTSOUNDBUFFER mixer;
    LPDIRECTSOUNDNOTIFY notify_interface; 
    DSBPOSITIONNOTIFY   position_notify[2];
    DWORD               mixsize;

    if (!first_init(hWnd)) {
        return(false);
    }

    if (callback == nullptr || _max_rate > 0 && (rate < _min_rate || rate > _max_rate)) {
        error_ = "\nError: Unsupported DirectSound sample rate";
        return(false);
    }

    shutdown();
    _channels = channels;
    _period = frames_in_buffer >> 1;
    _chunk_size = channels * _period * 2;  // half the buffer size
    _callback = callback;

    // Set up the primary direct sound buffer.
    ZeroMemory(&dsbd, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat = NULL;
    hr = _gpds->CreateSoundBuffer(&dsbd, &_pPRM, NULL);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot create primary buffer";
        goto dsinit_err;
    }

    // Set the primary buffer format
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = _channels;
    wfx.nSamplesPerSec = rate;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * channels * 2;
    wfx.nBlockAlign = 4;    // 16 bit * 2
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;
    hr = _pPRM->SetFormat(&wfx);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot set format buffer";
        goto dsinit_err;
    }

    // CREATE A STEREO, NON 3D, SECONDARY STREAMING BUFFER FOR SOFTWARE MIXING
    dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | 
                   DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
    dsbd.dwBufferBytes = _chunk_size * 2; 
    dsbd.lpwfxFormat = &wfx; 
    hr = _gpds->CreateSoundBuffer(&dsbd, &mixer, NULL);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot create mixing buffer";
        goto dsinit_err;
    }
    hr = mixer->QueryInterface(IID_IDirectSoundBuffer8, &dumptr);
    _sMix = (LPDIRECTSOUNDBUFFER8)dumptr;
    mixer->Release();
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot get the soundbuffer8 interface of the mixer buffer";
        goto dsinit_err;
    }

    // CREATE THE SW MIXER THREAD
    g_proc_done = false;
    _thread_handle = CreateThread(NULL, 0, NotificationProc, 
                                  this, 0, &_thread_id);
    if (_thread_handle == NULL) {
        error_ = "\nError: Cannot create the audio mixer thread.";
        goto dsinit_err;
    }
    SetThreadPriority(_thread_handle, THREAD_PRIORITY_TIME_CRITICAL);                              
    
    // CREATE THE NOTIFICATION EVENT
    g_notification_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    
    // SET THE NOTIFICATIONS
    hr = _sMix->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&notify_interface);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot get direct sound notify interface";
        goto dsinit_err;
    }

    position_notify[0].dwOffset = 4;
    position_notify[1].dwOffset = _chunk_size + 4;
    position_notify[0].hEventNotify = g_notification_event;
    position_notify[1].hEventNotify = g_notification_event;
    hr = notify_interface->SetNotificationPositions(2, position_notify);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot set direct sound notification positions";
        goto dsinit_err;
    }
    notify_interface->Release();

    // FILL THE BUFFER WITH SILENCE
    hr = _sMix->Lock(0, 0, &dumptr, &mixsize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    if (!SUCCEEDED(hr)) {
        error_ = "\nError: Cannot lock direct sound mixer";
        goto dsinit_err;
    }
    memset(dumptr, 0, mixsize);
    _sMix->Unlock(dumptr, mixsize, NULL, 0);

    samples_.clear();
    samples_.reserve(_period * _channels);

    // RUN THE BUFFER
    _sMix->Play(0, 0, DSBPLAY_LOOPING);
    _thread_is_running = false;
    return(true);

dsinit_err:
    shutdown();
    return(false);
}

bool WinAudioQueue::first_init(HWND hWnd)
{
    HRESULT hr;
    void    *dumptr;
    DSCAPS caps;

    if (!_first_init_done) {
        if (hWnd == NULL) return(false);

	    // Create the direct sound object.
        hr = CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER,
                              IID_IDirectSound8, &dumptr);
        _gpds = (LPDIRECTSOUND8)dumptr;
        if (!SUCCEEDED(hr) || (NULL == _gpds)) {
            error_ = "\nError: Failed to create DirectSound object";
            return(false);
        }
        // Initialize (Only if not created by DirectSoundCreate)
        hr = _gpds->Initialize(NULL);
        if (!SUCCEEDED(hr) || (NULL == _gpds)) {
            error_ = "\nError: Failed to init DirectSound object";
            return(false);
        }
        caps.dwSize = sizeof(caps);
        hr = _gpds->GetCaps(&caps);
        if (SUCCEEDED(hr)) {
            _min_rate = caps.dwMinSecondarySampleRate;
            _max_rate = caps.dwMaxSecondarySampleRate;
        }

        // Note we need to set the level to priority to set the
        // format of the primary buffer
        hr = _gpds->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
        if (!SUCCEEDED(hr)) {
            error_ = "\nError: DirectSound SetCooperativeLevel failed";
            return(false);
        }
        _first_init_done = true;
    }
    return(true);
}

void WinAudioQueue::restoreLostBuffers(void)
{
    DWORD       status, len1;
    HRESULT     hr;
    void        *data1;    

    // RESTORE THE BASIC BUFFER
    if (_sMix == NULL) return;
    if (!SUCCEEDED(_sMix->GetStatus(&status))) return;
    if ((status & DSBSTATUS_BUFFERLOST) == 0) return;
    if (!SUCCEEDED(_sMix->Restore())) return;
    hr = _sMix->Lock(0, 0, &data1, &len1, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    if (!SUCCEEDED(hr)) return;
    memset(data1, 0, len1);
    _sMix->Unlock(data1, len1, NULL, 0);
}

void WinAudioQueue::shutdown(void)
{
    if (!_thread_is_running) {
        error_ = "\nError: Sound mixing thread didn't run !!";
    }
    if (_thread_handle != NULL) {
        if (PostThreadMessage(_thread_id, WM_QUIT, 0, 0 ) != 0) {
            WaitForSingleObject(_thread_handle, INFINITE );
        }
        CloseHandle(_thread_handle);
        _thread_handle = NULL;
        g_proc_done = true;
    }
    if (_sMix != NULL) {
        _sMix->Stop();
        _sMix->Release();
    }
    if(_pPRM != NULL) {
        _pPRM->Stop();
        _pPRM->Release();
    }
    //if (_gpds != NULL) {
    //    _gpds->Release();
    //}
    if (g_notification_event != NULL) {
        CloseHandle(g_notification_event);
    }
    _pPRM = NULL;
    _sMix = NULL;
    _thread_id = 0;
    _thread_handle = g_notification_event = NULL;
    error_ = nullptr;
}

DWORD WINAPI NotificationProc( LPVOID lpParameter )
{
    MSG     msg;
    DWORD   dwResult;

    while(!g_proc_done) { 
        dwResult = MsgWaitForMultipleObjects(1, &g_notification_event, 
                                             FALSE, INFINITE, QS_ALLEVENTS );
        switch( dwResult )
        {
            case WAIT_OBJECT_0 + 0:
                // g_notification_event is signaled
                ((WinAudioQueue*)lpParameter)->NotifyThread();
                break;

            case WAIT_OBJECT_0 + 1:
                // Messages are available
                while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
                { 
                    if( msg.message == WM_QUIT )
                        g_proc_done = true;
                }
                break;
        }
    }
    return 0;
}

void WinAudioQueue::NotifyThread(void)
{
    DWORD       ppos, len1, len2;
    void        *buf1, *buf2;
    HRESULT     hr;

    // for debug
    _thread_is_running = true;

    // copy to directsound streaming buffer
    //_sMix->GetCurrentPosition(&ppos, NULL);
    _sMix->GetCurrentPosition(NULL, &ppos);
    if (ppos < _chunk_size - 1 || ppos == _chunk_size * 2 - 1) {
        ppos = _chunk_size;
    } else {
        ppos = 0;
    }
    hr = _sMix->Lock(ppos, _chunk_size, &buf1, &len1, &buf2, &len2, 0);
    if (SUCCEEDED(hr)) {
        if (_callback != nullptr) {
            samples_.clear();
            _callback->fillSamples(_channels, _period, &samples_);
            memcpy(buf1, samples_.data(), _chunk_size);
        }
        _sMix->Unlock(buf1, len1, buf2, len2);
    }
}

bool WinAudioQueue::getCapabilities(HWND hWnd, SoundCaps *caps)
{
    if (!first_init(hWnd)) {
        return(false);
    }
    caps->min_rate_ = _min_rate;
    caps->max_rate_ = _max_rate;
    caps->min_channels_ = 2;
    caps->max_channels_ = 2;
    caps->min_frames_ = DSBSIZE_MIN >> 1;
    caps->max_frames_ = DSBSIZE_MAX >> 2;
    if (caps->min_frames_ < 1024) caps->min_frames_ = 1024;
    if (caps->max_frames_ < 1024) caps->max_frames_ = 1024;
    return(true);
}

}