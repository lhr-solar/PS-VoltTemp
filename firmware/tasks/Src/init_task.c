#include <stm32xx_hal.h>
#include <bq76920.h>
#include <bq72920_registers.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "printf.h"
#include "UART.h"
#include "tasks.h"

// Buffers for tasks
StaticTask_t volt_temp_TaskBuffer;
StackType_t volt_temp_Stack[configVOLTTEMP_STACK_SIZE];

StaticTask_t volt_temp_can_TaskBuffer;
StackType_t volt_temp_can_Stack[configVOLTTEMP_STACK_SIZE];

void task_Init(){

  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK)
    Error_Handler();
  
  SystemClock_Config();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  // init & turn on psom leds to show volttemp number
  leds_init();
  volttemp_led_on();

  // init UART for printing
  mx_uart_init();
  UART_Init();
  printf_init(husart1);

  // Init I2C, acquire BMS semaphore / mutex
  mx_i2c_init();
  Init_BQ76920();

  // Init CAN
  HAL_CAN_MspInit(hcan1);
  mx_CAN_init();
  vt_CAN_start();

  /* ================= START TASKS ================= */
  xTaskCreateStatic(task_ReadVoltage,
                  "BQ Test",
                  configVOLTTEMP_STACK_SIZE,
                  NULL,
                  tskIDLE_PRIORITY + 3,
                  volt_temp_Stack,
                  &volt_temp_TaskBuffer);

    xTaskCreateStatic(task_SendMessage,
                  "BQ Test",
                  configVOLTTEMP_STACK_SIZE,
                  NULL,
                  tskIDLE_PRIORITY + 2,
                  volt_temp_can_Stack,
                  &volt_temp_can_TaskBuffer);
  
  
  // Task kills itself :(
  vTaskDelete(NULL);
}
