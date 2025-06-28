#ifndef gpio_h
#define gpio_h

#include "soc/gpio_reg.h"
#include "driver/gpio.h"

#define KEYPAD_LT GPIO_NUM_36
#define KEYPAD_RT GPIO_NUM_39
#define KEYPAD_UP GPIO_NUM_34
#define KEYPAD_DN GPIO_NUM_35
#define KEYPAD_OK GPIO_NUM_32

#define BUTTON_1  GPIO_NUM_33
#define BUTTON_2  GPIO_NUM_25

#define LED_1     GPIO_NUM_26
#define LED_2     GPIO_NUM_27

#define TFT_CS    GPIO_NUM_5
#define TFT_DC    GPIO_NUM_2
#define TFT_MOSI  GPIO_NUM_23
#define TFT_CLK   GPIO_NUM_18
#define TFT_RESET GPIO_NUM_15
#define TFT_MISO  GPIO_NUM_19
#define TFT_LED   GPIO_NUM_4

#define TFT_CS_LO() gpio_set_level(TFT_CS, 0)
#define TFT_CS_HI() gpio_set_level(TFT_CS, 1)

#define TFT_RESET_LO() gpio_set_level(TFT_RESET, 0)
#define TFT_RESET_HI() gpio_set_level(TFT_RESET, 1)

#define TFT_DC_LO() gpio_set_level(TFT_DC, 0)
#define TFT_DC_HI() gpio_set_level(TFT_DC, 1)

int gpio_init(void);

#endif
