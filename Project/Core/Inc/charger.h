#ifndef __CHAEGER__
#define __CHAEGER__
#include "stm32g0xx_hal.h"

#define MAX77787_LOG

#ifdef MAX77787_LOG
#define MAX77787_PRINTF(fmt, ...) printf("MAX77787:"fmt, ##__VA_ARGS__)
#else
#define MAX77787_PRINTF 
#endif 

#define max77787_chager_stby(state)   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, !!state)
#define max77787_chager_chgenb(state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, !!state)
#define max77787_chager_enbst(state)  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, !!state)


typedef enum {
    TYPEC_IS_EXIST=0,
    TYPEC_IS_NOT_EXIST,
    TYPEC_MAX
}typec_enum;

typedef struct max77787_charger{
    typec_enum typec_state;                     //get type-c insert or unplug state
    void (*set_typec_state)(typec_enum state);  //set type-c status 

    void (*init)(void);
    void (*charger_enable)(uint8_t);
    uint8_t (*get_charger_state)(void);
    typec_enum (*get_typec_is_exist)(void);     //get typec status
    
}max77787_charger_t;

max77787_charger_t * charger_create(void);
max77787_charger_t *get_charger_param(void);
void charger_process(max77787_charger_t * charger, void (* callback)(typec_enum));

#endif //__CHAEGER__
