#include "tasks.h"
#include <stm32xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"

StaticTask_t taskInitBuffer;
StackType_t taskInitStack[configMINIMAL_STACK_SIZE];


int main(void) {

  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK)
    Error_Handler();
  
   SystemClock_Config();
   __HAL_RCC_SYSCFG_CLK_ENABLE();
   __HAL_RCC_PWR_CLK_ENABLE();

  xTaskCreateStatic(task_Init,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    taskInitStack,
                    &taskInitBuffer);

    vTaskStartScheduler();

    // should never reach here
    
    while(1){} 
    return 0;
}

