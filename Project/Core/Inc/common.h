#ifndef __COMMON__
#define __COMMON__

#include "stm32g0xx_hal.h"

#define HALL_LOG

#ifdef HALL_LOG
#define HALL_LOG(fmt, ...) printf("hall:"fmt, ##__VA_ARGS__)
#else
#define HALL_LOG 
#endif 

typedef struct timer_struct{
    uint8_t time_1s;
}timer_t;

extern timer_t timer;


#define HALL_NEAR   0
#define HALL_FAR    1
#define HALL_UNKNOW 2
typedef struct hall_struct{
    uint8_t hall_mode;
}hall_t;

extern hall_t hall;

void project_version_printf(void);
void hall_process(hall_t *phall);

#endif //__COMMON__
