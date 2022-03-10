#ifndef LINUX_AUDIO_H__
#define LINUX_AUDIO_H__

#include "direct/direct.h"

namespace mmapp {

class LinuxAudioAdapter {
    void        *pcm_;
    ISoundSynth *callback_;
    int32_t     channels_;
    int32_t     rate_;
    int32_t     period_;
    pthread_t   thread_;
    bool        thread_exit_signal_;
public:
    LinuxAudioAdapter();
    ~LinuxAudioAdapter();
    bool init(int32_t channels, int32_t rate, int32_t frames_in_buffer, ISoundSynth *callback);
    void shutdown(void);
    bool getCapabilities(SoundCaps *caps);

    // internal use
    void getSamples(void);
};

} // namespace

#endif