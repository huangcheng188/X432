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

#define BUF_LEN 8

typedef struct uart_struct{
    uint8_t cnt;
    uint8_t buf[BUF_LEN];
}uart_t;

extern uart_t uart;

void project_version_printf(void);
void hall_process(hall_t *phall);
void uart_process_init(void);
#define uart_process_buf_clean uart_process_init        //when receive uart data must clean buf
void uart_process_analysis(uart_t *puart);

#endif //__COMMON__
