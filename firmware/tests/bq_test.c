#include <stm32l4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "UART.h"

uint32_t* cell_data;

StaticTask_t xTaskBuffer;
StackType_t xStack[ 200 ];

void Task_ReadBQ(void *pvParameters) {
  printf_init(husart1);
  while(1) {
    get_Voltage_All(cell_data);
    
    printf("T");
    
    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main() {
  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK) Error_Handler();
  SystemClock_Config();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  leds_init();
  mx_i2c_init();
  // turn on psom leds to show volttemp number
  volttemp_led_on();

  HAL_UART_MspGPIOInit(husart1);
  UART_Init();

  // init the chip with these i2c pins.
  Init_BQ76920();

  xTaskCreateStatic(Task_ReadBQ,
                  "BQ Test",
                  configMINIMAL_STACK_SIZE,
                  (void*) 1,
                  tskIDLE_PRIORITY+1,
                  xStack,
                  &xTaskBuffer);

  vTaskStartScheduler();

  while(1){}

  return 0;
}
