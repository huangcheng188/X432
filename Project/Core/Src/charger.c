#include "charger.h"
#include <stdio.h>

//INOKB  --PA6(unplug or insert typec)
//STBY   --PA4
//CHGENB --PA5
//ENBST  --PA7
//逻辑控制详见 https://www.analog.com/media/en/technical-documentation/data-sheets/max77787.pdf 第33页

max77787_charger_t *max77787_charger = NULL;

static void set_typec_state(typec_enum state)
{
    max77787_charger->typec_state = state;
}

static void charger_enable(uint8_t state)
{
        
}

static uint8_t get_charger_state(void)
{

    return 0;
}

static typec_enum get_typec_is_exist(void)
{
    return max77787_charger->typec_state;
}

max77787_charger_t * charger_create(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_RESET);

    max77787_chager_stby(0);
    max77787_chager_chgenb(0);
    max77787_chager_enbst(0);


    max77787_charger = (max77787_charger_t *)malloc(sizeof(max77787_charger_t));
    if(max77787_charger == NULL){
        MAX77787_PRINTF("malloc faild!!!\r\n");
        return NULL;
    }

    max77787_charger->typec_state = TYPEC_MAX;

    max77787_charger->set_typec_state = set_typec_state;
   
    max77787_charger->init = NULL;
    max77787_charger->charger_enable = charger_enable;
    max77787_charger->get_charger_state  = get_charger_state;
    max77787_charger->get_typec_is_exist = get_typec_is_exist;

    MAX77787_PRINTF("charger init success\r\n");
    return max77787_charger;

}

max77787_charger_t *get_charger_param(void)
{
    return (max77787_charger_t *)max77787_charger;
}

void charger_process(max77787_charger_t * charger, void (* callback)(typec_enum))
{
    if(charger == NULL)
        return ;

    static typec_enum state = TYPEC_MAX;
    static uint8_t first_start = 0;    //first start ,can't get typec status
    
    if(!first_start){
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)){
            charger->typec_state = TYPEC_IS_NOT_EXIST;
        }else{
            charger->typec_state = TYPEC_IS_EXIST;
        }
        first_start != first_start;
    }


    if(state == charger->typec_state){
        return ;
    }

    switch(charger->typec_state){ 
        case TYPEC_IS_EXIST:
            if(callback)
                callback(TYPEC_IS_EXIST);
            MAX77787_PRINTF("TYPEC_IS_EXIST\r\n");
            break;
        case TYPEC_IS_NOT_EXIST:
            if(callback)
                callback(TYPEC_IS_NOT_EXIST);
            MAX77787_PRINTF("TYPEC_IS_NOT_EXIST\r\n");
            break;
        default:
            break;

    }

    state = charger->typec_state;
}