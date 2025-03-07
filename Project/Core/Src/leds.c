#include "leds.h"
#include <stdio.h>

#define LED_R_SET(PinState)  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, PinState)
#define LED_G_SET(PinState)  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, PinState)
#define LED_B_SET(PinState)  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, PinState)

#define LED_W_SET(PinState)  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, PinState)
#define LED_W1_SET(PinState) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, PinState)

#define LED_ALL_SET(PinState) LED_R_SET(PinState);LED_G_SET(PinState);LED_B_SET(PinState);LED_W_SET(PinState);LED_W1_SET(PinState);

void led_control(led_enum led, GPIO_PinState on_off)
{
  switch (led) {
    case LED_ALL_OFF:
        LED_ALL_SET(on_off);
        printf("All LEDs are OFF\n");
        break;
    case LED_R:
        LED_R_SET(on_off);
        printf("Red LED is ON\n");
        break;
    case LED_G:
        LED_G_SET(on_off);
        printf("Green LED is ON\n");
        break;
    case LED_B:
        LED_B_SET(on_off);
        printf("Blue LED is ON\n");
        break;
    case LED_W:
        LED_W_SET(on_off);
        printf("White LED is ON\n");
        break;
    case LED_W1:
        LED_W1_SET(on_off);
        printf("White LED 1 is ON\n");
        break;
    case LED_ALL_ON:
        LED_ALL_SET(on_off);
        printf("All LEDs are ON\n");
        break;
    default:
        // 处理组合情况,暂不处理组合
        break;
    }

}

