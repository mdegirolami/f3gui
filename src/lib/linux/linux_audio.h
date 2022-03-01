#ifndef LINUX_AUDIO_H__
#define LINUX_AUDIO_H__

#include "direct/direct.h"

namespace mmapp {

class LinuxAudioAdapter {
public:
    bool init(int32_t chunk_samples, int32_t rate, ISoundSynth *callback);
    void shutdown(void);
};

} // namespace

#endif