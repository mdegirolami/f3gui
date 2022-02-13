#ifndef __WINAUDIOQUEUE_H_
#define __WINAUDIOQUEUE_H_

#include <windows.h>
#include <windowsx.h>
#include <dsound.h>

#ifndef FAIL
#define FAIL -1
#endif

namespace mmapp {

class WinAudioQueue {
    DWORD                       _chunk_size;            // bytes queued at a time - half the buffer size
    void                        (*_callback)(BYTE*);
    LPDIRECTSOUND8              _gpds;
    LPDIRECTSOUNDBUFFER         _pPRM;
    LPDIRECTSOUNDBUFFER8        _sMix;
    DWORD                       _thread_id;
    HANDLE                      _thread_handle;
    bool                        _thread_is_running;
    bool                        _first_init_done;
    DWORD                       _min_rate;
    DWORD                       _max_rate;

    const char *error_;

public:
    WinAudioQueue();
    ~WinAudioQueue();

    // internal use
    void    NotifyThread(void);

    int Init(HWND hWnd, DWORD chunk_bytes, DWORD rate, void (*callback)(BYTE*));
    void RestoreLostBuffers(void);
    void Shutdown(void);
    const char *getError(void) { return(error_); }
};

}
#endif
