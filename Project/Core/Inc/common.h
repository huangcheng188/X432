#ifndef __COMMON__
#define __COMMON__

#include "stm32g0xx_hal.h"

typedef struct timer_struct{
    uint8_t time_1s;
}timer_t;

extern timer_t timer;
void project_version_printf(void);

#endif //__COMMON__
