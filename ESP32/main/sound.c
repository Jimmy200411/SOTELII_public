#include "sound.h"

#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


struct soundtab_entry
{
    uint16_t freq_hz;
    uint8_t rez;
    uint32_t duty50;
};

static struct soundtab_entry notemap[64] =
{
    {   52, 20,   524288 }, // 32
    {   55, 20,   524288 }, // 33
    {   58, 20,   524288 }, // 34
    {   62, 20,   524288 }, // 35
    {   65, 20,   524288 }, // 36
    {   69, 20,   524288 }, // 37
    {   73, 20,   524288 }, // 38
    {   78, 19,   262144 }, // 39
    {   82, 19,   262144 }, // 40
    {   87, 19,   262144 }, // 41
    {   92, 19,   262144 }, // 42
    {   98, 19,   262144 }, // 43
    {  104, 19,   262144 }, // 44
    {  110, 19,   262144 }, // 45
    {  117, 19,   262144 }, // 46
    {  123, 19,   262144 }, // 47
    {  131, 19,   262144 }, // 48
    {  139, 19,   262144 }, // 49
    {  147, 19,   262144 }, // 50
    {  156, 18,   131072 }, // 51
    {  165, 18,   131072 }, // 52
    {  175, 18,   131072 }, // 53
    {  185, 18,   131072 }, // 54
    {  196, 18,   131072 }, // 55
    {  208, 18,   131072 }, // 56
    {  220, 18,   131072 }, // 57
    {  233, 18,   131072 }, // 58
    {  247, 18,   131072 }, // 59
    {  262, 18,   131072 }, // 60
    {  277, 18,   131072 }, // 61
    {  294, 18,   131072 }, // 62
    {  311, 17,    65536 }, // 63
    {  330, 17,    65536 }, // 64
    {  349, 17,    65536 }, // 65
    {  370, 17,    65536 }, // 66
    {  392, 17,    65536 }, // 67
    {  415, 17,    65536 }, // 68
    {  440, 17,    65536 }, // 69
    {  466, 17,    65536 }, // 70
    {  494, 17,    65536 }, // 71
    {  523, 17,    65536 }, // 72
    {  554, 17,    65536 }, // 73
    {  587, 17,    65536 }, // 74
    {  622, 16,    32768 }, // 75
    {  659, 16,    32768 }, // 76
    {  698, 16,    32768 }, // 77
    {  740, 16,    32768 }, // 78
    {  784, 16,    32768 }, // 79
    {  831, 16,    32768 }, // 80
    {  880, 16,    32768 }, // 81
    {  932, 16,    32768 }, // 82
    {  988, 16,    32768 }, // 83
    { 1047, 16,    32768 }, // 84
    { 1109, 16,    32768 }, // 85
    { 1175, 16,    32768 }, // 86
    { 1245, 15,    16384 }, // 87
    { 1319, 15,    16384 }, // 88
    { 1397, 15,    16384 }, // 89
    { 1480, 15,    16384 }, // 90
    { 1568, 15,    16384 }, // 91
    { 1661, 15,    16384 }, // 92
    { 1760, 15,    16384 }, // 93
    { 1865, 15,    16384 }, // 94
    { 1976, 15,    16384 }  // 95
};


int sound_init(struct sound * this)
{
    for (int i = 0; i < SOUND_CHANNELS; i++)
    {
        uint8_t note = this->params[i].note = 69;

        this->timers[i].speed_mode      = LEDC_HIGH_SPEED_MODE;
        this->timers[i].duty_resolution = notemap[note - 32].rez;

        switch (i)
        {
            case 0:
                this->timers[i].timer_num = LEDC_TIMER_0; break;

            case 1:
                this->timers[i].timer_num = LEDC_TIMER_1; break;

            case 2:
                this->timers[i].timer_num = LEDC_TIMER_2; break;
        }

        this->timers[i].freq_hz         = notemap[note - 32].freq_hz;
        this->timers[i].clk_cfg         = LEDC_AUTO_CLK;
        this->timers[i].deconfigure     = 0;

        if (ledc_timer_config(&this->timers[i]) != ESP_OK)
            return 0;

     /* Keep in mind: {{freq_hz}} cannot be set to 0 (this yields an error).
        Doing so could otherwise be done in order to disable PWM output.
        What works instead: a duty cycle of 0. */

        switch (i)
        {
            case 0:
                this->channels[i].gpio_num   = 13;
                this->channels[i].channel    = 0;
                this->channels[i].timer_sel  = LEDC_TIMER_0;
                break;

            case 1:
                this->channels[i].gpio_num   = 12;
                this->channels[i].channel    = 1;
                this->channels[i].timer_sel  = LEDC_TIMER_1;
                break;

            case 2:
                this->channels[i].gpio_num   = 14;
                this->channels[i].channel    = 2;
                this->channels[i].timer_sel  = LEDC_TIMER_2;
                break;
        }

        this->channels[i].speed_mode    = LEDC_HIGH_SPEED_MODE;
        this->channels[i].intr_type     = LEDC_INTR_DISABLE;
        this->channels[i].duty          = 0;
        this->channels[i].hpoint        = 0;
     // this->channels[i].sleep_mode    = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
     // this->channels[i].output_invert = 0;

        if (ledc_channel_config(&this->channels[i]) != ESP_OK)
            return 0;
    }

    return 1;
}


int sound_update(struct sound * this)
{
    for (int i = 0; i < SOUND_CHANNELS; i++)
    {
        if (!this->params[i].updated_p)
            continue;

        this->params[i].updated_p = 0;

        uint8_t note = this->params[i].note;

        if (note == 255)
        {
            note = 69;

            this->timers[i].freq_hz         = notemap[note - 32].freq_hz;
            this->timers[i].duty_resolution = notemap[note - 32].rez;

            this->channels[i].duty          = 0;
        }
        else if (note < 32 || note > 96)
            continue;
        else
        {
            this->timers[i].freq_hz         = notemap[note - 32].freq_hz;
            this->timers[i].duty_resolution = notemap[note - 32].rez;

            this->channels[i].duty          = notemap[note - 32].duty50;
        }

        if (ledc_timer_config(&this->timers[i]) != ESP_OK)
            return 0;

        if (ledc_channel_config(&this->channels[i]) != ESP_OK)
            return 0;
    }

    return 1;
}


void sound_beep(struct sound * this, uint8_t note, uint16_t duration)
{
    this->params[0].note = note;
    this->params[0].updated_p = 1;

    sound_update(this);

    vTaskDelay(duration / portTICK_PERIOD_MS);

    this->params[0].note = 255;
    this->params[0].updated_p = 1;

    sound_update(this);
}
