#ifndef sound_h
#define sound_h

#include "driver/ledc.h"

#define SOUND_CHANNELS 3

struct sound
{
    ledc_timer_config_t timers[SOUND_CHANNELS];
    ledc_channel_config_t channels[SOUND_CHANNELS];

    struct params
    {
        uint8_t note;
        uint8_t updated_p;

    } params[SOUND_CHANNELS];
};

int sound_init(struct sound *);

void sound_beep(struct sound *, uint8_t note, uint16_t duration);

#endif
