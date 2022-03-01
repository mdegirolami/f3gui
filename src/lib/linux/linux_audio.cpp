#include "linux_audio.h"

namespace mmapp {

bool LinuxAudioAdapter::init(int32_t chunk_samples, int32_t rate, ISoundSynth *callback) 
{
    return(true);
}

void LinuxAudioAdapter::shutdown(void) 
{
}

} // namespace