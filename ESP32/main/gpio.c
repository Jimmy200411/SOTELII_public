#include "gpio.h"

int gpio_init(void)
{
    gpio_config_t gpio = {};

    gpio.intr_type    = GPIO_INTR_DISABLE;
    gpio.mode         = GPIO_MODE_OUTPUT;
    gpio.pin_bit_mask = (1LL << LED_1)
                      | (1LL << LED_2)
                      | (1LL << TFT_CS)
                      | (1LL << TFT_DC)
                      | (1LL << TFT_RESET)
                      | (1LL << TFT_LED);

    gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio.pull_up_en   = GPIO_PULLUP_DISABLE;

    if (gpio_config(&gpio) != ESP_OK)
        return 0;


    gpio.mode         = GPIO_MODE_INPUT;
    gpio.pin_bit_mask = (1LL << KEYPAD_LT)
                      | (1LL << KEYPAD_RT)
                      | (1LL << KEYPAD_UP)
                      | (1LL << KEYPAD_DN)
                      | (1LL << KEYPAD_OK)
                      | (1LL << BUTTON_1)
                      | (1LL << BUTTON_2);

    if (gpio_config(&gpio) != ESP_OK)
        return 0;

    return 1;
}
