#include "linux_audio.h"
#include <tinyalsa/pcm.h>
#include <stdio.h>

namespace mmapp {

static void *mixer_thread(void *adapter)
{
    ((LinuxAudioAdapter*)adapter)->getSamples();
    return(nullptr);
}

LinuxAudioAdapter::LinuxAudioAdapter()
{
    pcm_ = nullptr;
}

LinuxAudioAdapter::~LinuxAudioAdapter()
{
    shutdown();
}

bool LinuxAudioAdapter::init(int32_t channels, int32_t rate, int32_t frames_in_buffer, ISoundSynth *callback) 
{
    unsigned int card = 0;
    unsigned int device = 0;
    unsigned int period = (unsigned int)frames_in_buffer >> 2;
    int flags = PCM_OUT;

    const struct pcm_config config = {
        .channels = (unsigned int)channels,
        .rate = (unsigned int)rate,
        .period_size = period,
        .period_count = 4,
        .format = PCM_FORMAT_S16_LE,
        .start_threshold = period * 4,
        .stop_threshold = period * 4,
        .silence_threshold = 0, 0, 0
    };

    shutdown();
    struct pcm * pcm = pcm_open(card, device, flags, &config);
    if (pcm == nullptr) {
        return (false);
    } else if (!pcm_is_ready(pcm)){
        pcm_close(pcm);
        return (false);
    }
    pcm_ = pcm;

    channels_ = channels;
    rate_ = rate;
    period_ = period;
    callback_ = callback;
    thread_exit_signal_ = false;
  
    pthread_attr_t attr;

    int err = pthread_attr_init(&attr);
	if (err == 0) {
        pthread_attr_setstacksize (&attr, 1024*32);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
        err = pthread_create(&thread_, &attr, mixer_thread, this);
        pthread_attr_destroy(&attr);
    }
    if (err != 0) {
        shutdown();
        return(false);
    }

    return(true);
}

void LinuxAudioAdapter::shutdown(void) 
{
    if (pcm_ != nullptr) {
        thread_exit_signal_ = true;
        __sync_synchronize();
        pthread_join(thread_, nullptr);
        pcm_close((struct pcm *)pcm_);
    }
    pcm_ = nullptr;
}

void LinuxAudioAdapter::getSamples(void)
{
    std::vector<int16_t> samples;

    samples.reserve(period_ * channels_);
    __sync_synchronize();
    while (!thread_exit_signal_) {
        samples.clear();
        callback_->fillSamples(channels_, period_, &samples);
        pcm_writei((struct pcm *)pcm_, &(samples[0]), period_);
        __sync_synchronize();
    }
}

bool LinuxAudioAdapter::getCapabilities(SoundCaps *caps)
{
    struct pcm_params *pp =	pcm_params_get (0, 0, PCM_OUT);

    if (pp == nullptr) return(false);

    caps->min_rate_ = pcm_params_get_min(pp, PCM_PARAM_RATE);
    caps->max_rate_ = pcm_params_get_max(pp, PCM_PARAM_RATE);
    caps->min_channels_ = pcm_params_get_min(pp, PCM_PARAM_CHANNELS);
    caps->max_channels_ = pcm_params_get_max(pp, PCM_PARAM_CHANNELS);
    caps->min_frames_ = pcm_params_get_min(pp, PCM_PARAM_PERIOD_SIZE) << 2;
    caps->max_frames_ = pcm_params_get_max(pp, PCM_PARAM_PERIOD_SIZE) << 2;

    pcm_params_free(pp);
    return(true);
}

} // namespace