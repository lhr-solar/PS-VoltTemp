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
#include "canbus.h"

// Buffers for tasks
StaticTask_t voltage_monitor_TaskBuffer;
StackType_t voltage_monitor_Stack[voltageMonitorStackSize];

StaticTask_t temperature_monitor_TaskBuffer;
StackType_t temperature_monitor_Stack[temperatureMonitorStackSize];


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
  canbus_init();

  /* ================= START TASKS ================= */
  xTaskCreateStatic(task_ReadVoltage,
                  "Voltage Monitor Task",
                  voltageMonitorStackSize,
                  NULL,
                  VOLTAGE_MON_PRIO,
                  voltage_monitor_Stack,
                  &voltage_monitor_TaskBuffer);

  xTaskCreateStatic(task_temp_read,
                  "Temperature Monitor Task",
                  temperatureMonitorStackSize,
                  NULL,
                  TEMPERATURE_MON_PRIO,
                  temperature_monitor_Stack,
                  &temperature_monitor_TaskBuffer);
  
  // Task kills itself :(
  vTaskDelete(NULL);
}
