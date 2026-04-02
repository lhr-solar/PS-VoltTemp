#include <stm32xx_hal.h>
#include "tasks.h"
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "pinConfig.h"
#include "leds.h"
#include "inits.h"
#include "canbus.h"
#include "printf.h" 
#include "debugIO.h"

// Buffers for tasks
StaticTask_t voltage_monitor_TaskBuffer;
StackType_t voltage_monitor_Stack[voltageMonitorStackSize];

StaticTask_t temperature_monitor_TaskBuffer;
StackType_t temperature_monitor_Stack[temperatureMonitorStackSize];

StaticTask_t slcan_print_TaskBuffer;
StackType_t slcan_print_Stack[slcanPrintStackSize];

void task_Init(){

  // init & turn on psom leds to show volttemp number
  leds_init();
  volttemp_id_led_on();

  // init UART for printing
  mx_uart_init();
  UART_Init();
  printf_init(husart1);

  debugIO_init();

  // Init CAN
  canbus_init();

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


  // mirror all transmitted CAN messages over USB if (CAN_USB_MIRROR_ENABLED = 1)
#if (CAN_USB_MIRROR_ENABLED == 1)
  xTaskCreateStatic(task_printSlcan,
                  "Slcan transmit task",
                  slcanPrintStackSize,
                  NULL,
                  SLCAN_PRINT_PRIO,
                  slcan_print_Stack,
                  &slcan_print_TaskBuffer);
#endif /*(CAN_USB_MIRROR_ENABLED == 1)*/
  
  // Task kills itself :(
  vTaskDelete(NULL);
}
