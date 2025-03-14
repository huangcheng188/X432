#include <stdio.h>
#include <stdarg.h> 
#include <string.h>
#include "common.h"
#include "stm32g0xx_hal.h"
#include "charger.h"
//#include "stm32g0xx_hal_usart.h"

hall_t hall;

void hall_set_mode(hall_t *phall, uint8_t mode)
{
    phall->hall_mode = mode;
}

void hall_process(hall_t *phall)
{
    if(phall == NULL)
    return ;

    static uint8_t mode = HALL_UNKNOW;
    static uint8_t first_start = 0;    //first start ,can't get typec status

    if(!first_start){
      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)){
          phall->hall_mode = HALL_FAR;
      }else{
          phall->hall_mode = HALL_NEAR;
      }
      first_start = 1;
    }


    if(mode == phall->hall_mode){
      return ;
    }

    switch(phall->hall_mode){ 
      case HALL_FAR:
          HALL_LOG("HALL_FAR\r\n");
          break;
      case HALL_NEAR:
          HALL_LOG("HALL_NEAR\r\n");
          break;
      default:
          break;

    }
    mode = phall->hall_mode;

}

uart_t uart;

void uart_process_init(void)
{
    memset(&uart, 0, sizeof(uart_t));
}

void uart_process_analysis(uart_t *puart)
{
    if(puart->cnt){
        for(uint8_t i=0; i<puart->cnt; i++){
            printf("-R=0x%02x", puart->buf[i]);
        }
        printf("\r\n");
        puart->cnt = 0;
    }
}

static void uart_process_recive(uart_t *puart, uint8_t data)
{
    puart->buf[puart->cnt] = data;
    puart->cnt++;
    if(puart->cnt >= BUF_LEN){
        printf("*\r\n");
        puart->cnt = 0;
    }
}

// 启动ADC转换并读取结果
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

uint32_t ADC_Read(void)
{

  //HAL_UART_MspDeInit(&huart2);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_Delay(10);

  uint32_t adcValue = 0;
  HAL_ADC_Start(&hadc1);                          // 启动ADC转换
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)  // 等待转换完成
  {
      adcValue = HAL_ADC_GetValue(&hadc1);        // 获取ADC转换结果
  }
  HAL_ADC_Stop(&hadc1);                           // 停止ADC
  //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

  //HAL_UART_MspInit(&huart2);
  return adcValue;                                // 返回ADC值
}


extern UART_HandleTypeDef huart1;

//void UART_Printf(const char *format, ...)
//{
//    char buffer[64];  // 定义缓冲区
//    va_list args;
//    va_start(args, format);
//    vsnprintf(buffer, sizeof(buffer), format, args);  // 格式化字符串
//    va_end(args);
//    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);  // 发送数据
//}

//char data[] = "Hello, UART!";
//HAL_UART_Transmit(&huart1, (uint8_t*)data, sizeof(data) - 1, 1000);

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

#define PROJECT_VERSION "V1.0.0"

void project_version_printf(void)
{
  printf("*****************************************\r\n");
  printf("Firmware Version: %s\r\n", PROJECT_VERSION);
  printf("Compile Date: %s Time: %s\r\n", __DATE__, __TIME__);
  printf("*****************************************\r\n");
  return;
}

timer_t timer = {0};

//中断回调函数（中断下半段都放到此处统一处理）
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(timer.time_1s){
        timer.time_1s--;
    }
}

void stm32_heartrate(void)
{
    static uint8_t cnt = 0;
    if(timer.time_1s == 0){
        timer.time_1s = 100;
        printf("MCU:%d\r\n", cnt++);
    }

}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  max77787_charger_t *charger = (max77787_charger_t *)get_charger_param();
  if(GPIO_Pin == GPIO_PIN_4){
    printf("GPIO_PIN_4 Rising\r\n");
  }else if (GPIO_Pin == GPIO_PIN_6){
    charger->set_typec_state(TYPEC_IS_NOT_EXIST);
    printf("GPIO_PIN_6 Rising\r\n");
  }else if (GPIO_Pin == GPIO_PIN_8){
    hall_set_mode(&hall, HALL_FAR);
    printf("GPIO_PIN_8 Rising\r\n");
  }else{
    printf("Unkown Rising\r\n");
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  max77787_charger_t *charger = (max77787_charger_t *)get_charger_param();
  if(GPIO_Pin == GPIO_PIN_4){
    printf("GPIO_PIN_4 Falling\r\n");
  }else if (GPIO_Pin == GPIO_PIN_6){
    charger->set_typec_state(TYPEC_IS_EXIST);
    printf("GPIO_PIN_6 Falling\r\n");
  }else if(GPIO_Pin == GPIO_PIN_8){
    hall_set_mode(&hall, HALL_NEAR);
    printf("GPIO_PIN_8 Falling\r\n");
  }else{
    printf("Unkown Falling\r\n");
  }

}

extern uint8_t rx_data;
extern UART_HandleTypeDef huart2;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)  // 检查是否是 UART2
  {
      // 处理接收到的数据
      //ProcessReceivedData(rx_data);
      //printf("R:%02x", rx_data);
      uart_process_recive(&uart, rx_data);
      printf("$\r\n");
      // 重新启用接收中断，以接收下一个字节
      HAL_UART_Receive_IT(&huart2, &rx_data, 1);
  }
}


