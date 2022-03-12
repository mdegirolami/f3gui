#ifndef __WINAUDIOQUEUE_H_
#define __WINAUDIOQUEUE_H_

#include "direct/direct.h"
#include <windows.h>
#include <windowsx.h>
#include <dsound.h>

#ifndef FAIL
#define FAIL -1
#endif

namespace mmapp {

class WinAudioQueue {
    int32_t                     _channels;
    int32_t                     _period;        // frames in each of the two buffer periods
    int32_t                     _chunk_size;    // bytes per period
    ISoundSynth                 *_callback;
    LPDIRECTSOUND8              _gpds;
    LPDIRECTSOUNDBUFFER         _pPRM;
    LPDIRECTSOUNDBUFFER8        _sMix;
    DWORD                       _thread_id;
    HANDLE                      _thread_handle;
    bool                        _thread_is_running;
    bool                        _first_init_done;
    DWORD                       _min_rate;
    DWORD                       _max_rate;
    std::vector<int16_t>        samples_;
    const char                  *error_;

    bool first_init(HWND hWnd);

public:
    WinAudioQueue();
    ~WinAudioQueue();

    // internal use
    void    NotifyThread(void);

    bool init(HWND hWnd, int32_t channels, int32_t rate, int32_t frames_in_buffer, ISoundSynth *callback);
    void shutdown(void);
    bool getCapabilities(HWND hWnd, SoundCaps *caps);
    void restoreLostBuffers(void);
    const char *getError(void) { return(error_); }
};

}
#endif
