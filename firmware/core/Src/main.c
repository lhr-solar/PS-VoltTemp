#include "tasks.h"
#include <stm32xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"

StackType_t Task_Init_Stack_Array[ TASK_INIT_STACK_SIZE ];
StaticTask_t Task_Init_Buffer;

/*
int main(void) {

  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK) Error_Handler();
  SystemClock_Config();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  leds_init();
  mx_i2c_init();
  mx_uart_init();
  

   xTaskCreateStatic(
                    Task_Init,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    (void*)NULL,
                    TASK_INIT_PRIO,
                    Task_Init_Stack_Array,
                    &Task_Init_Buffer
   );

    // Start the scheduler
    vTaskStartScheduler();

    while(1){}
    
    return 0;
}

*/