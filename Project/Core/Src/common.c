#include <stdio.h>
#include <stdarg.h> 
#include <string.h>
#include "common.h"
#include "stm32g0xx_hal.h"
#include "charger.h"
//#include "stm32g0xx_hal_usart.h"


// 启动ADC转换并读取结果
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

uint32_t ADC_Read(void)
{

  HAL_UART_MspDeInit(&huart2);

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

  HAL_UART_MspInit(&huart2);
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



//中断回调函数（中断下半段都放到此处统一处理）
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  printf("T\r\n");
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
      printf("R:%02x", rx_data);
      // 重新启用接收中断，以接收下一个字节
      HAL_UART_Receive_IT(&huart2, &rx_data, 1);
  }
}


