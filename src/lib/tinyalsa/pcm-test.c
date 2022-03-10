#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <tinyalsa/pcm.h>

static double phase = 0;

static void synth(void * frames, size_t frames_count)
{
    short *dst = (short*)frames;
    double step = 6.28 * 440 / 48000;
    for (int ii = 0; ii < frames_count; ++ii) {
        phase += step;
        *dst ++ = (short)(sin(phase) * 16000);
        *dst ++ = (short)(sin(phase * 8) * 16000);
        if (phase > 3.1416*2) phase -= 3.1416*2;
    }
    return;
}

static struct pcm *start_sound(int channels, int rate, int period)
{
    unsigned int card = 0;
    unsigned int device = 0;
    int flags = PCM_OUT;

    const struct pcm_config config = {
        .channels = (unsigned int)channels,
        .rate = (unsigned int)rate,
        .period_size = (unsigned int)period,
        .period_count = 4,
        .format = PCM_FORMAT_S16_LE,
        .start_threshold = (unsigned int)(period * 4),
        .stop_threshold = (unsigned int)(period * 4),
        .silence_threshold = 0, 0, 0
    };

    struct pcm * pcm = pcm_open(card, device, flags, &config);
    if (pcm == NULL) {
        fprintf(stderr, "failed to allocate memory for PCM\n");
        return (NULL);
    } else if (!pcm_is_ready(pcm)){
        fprintf(stderr, "pcm returns error: %s\n", pcm_get_error(pcm));
        pcm_close(pcm);
        fprintf(stderr, "failed to open PCM\n");
        return (NULL);
    }
    return(pcm);
}


void alsa_test(void)
{
    /*
    char buf[2048];
    struct pcm_params *pp =	pcm_params_get (1, 11, PCM_OUT);
    pcm_params_to_string(pp, buf, 2048);
    printf(buf);
    pcm_params_free(pp);
    return;
    */
    int channels = 2;
    int rate = 44100;
    int period = 1024;

    float duration = 1;

    size_t size = period * channels * 2; 
    void *frames = malloc(size);
    if (frames == NULL) {
        fprintf(stderr, "failed to allocate frames\n");
        return;
    }

    struct pcm *pcm = start_sound(channels, rate, period);
    if (pcm == NULL) return;

    size_t frames_count = pcm_bytes_to_frames(pcm, size);
    int iterations = (int)((duration * rate + frames_count - 1)/ frames_count);
    for (int ii = 0; ii < iterations; ++ii) {
        synth(frames, frames_count * channels / 2);
        int err = pcm_writei(pcm, frames, frames_count);
        if (err < 0) {
            printf("error: %s\n", pcm_get_error(pcm));
        }
    } 

    pcm_close(pcm);
    free(frames);
}


