#ifndef __LED__
#define __LED__

#include "stm32g0xx_hal.h"

typedef enum led {
    LED_ALL_OFF = 0,
    LED_ALL_ON,
    LED_R,
    LED_G,
    LED_B,
    LED_W,
    LED_W1
} led_enum;

#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

void led_control(led_enum led, GPIO_PinState on_off);

#endif //__LED__
