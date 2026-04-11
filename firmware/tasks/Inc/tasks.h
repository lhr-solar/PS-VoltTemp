#pragma once

#include "stm32xx_hal.h"
#include "common.h"
#include <BPSCAN_can_msgs.h>
#include "bq76920.h"

// Init Tasks
#define TASK_INIT_PRIO                  tskIDLE_PRIORITY + 1
#define TASK_INIT_STACK_SIZE            configMINIMAL_STACK_SIZE

// Voltage Monitor Task
#define voltageMonitorStackSize         configMINIMAL_STACK_SIZE*2
#define VOLTAGE_MON_PRIO                tskIDLE_PRIORITY + 2

// Temperature Monitor Task
#define temperatureMonitorStackSize     configMINIMAL_STACK_SIZE*2
#define TEMPERATURE_MON_PRIO            tskIDLE_PRIORITY + 3

// Balance Cells Task
#define BalanceCellsStackSize           configMINIMAL_STACK_SIZE*2
#define BALANCE_CELLS_PRIO              tskIDLE_PRIORITY + 4

// Slcan Print Task
#define slcanPrintStackSize             configMINIMAL_STACK_SIZE*8
#define SLCAN_PRINT_PRIO                tskIDLE_PRIORITY + 1

void task_ReadVoltage(void *pvParameters);
void task_temp_read(void *pvParameters);
void task_printSlcan(void *pvParameters);
void task_balance(void *pvParameters);

#define TEMPERATURE_THREAD_PERIOD_MS        250
#define VOLTTEMP_THREAD_DELAY_MS            300
#define BALANCING_THREAD_DELAY_MS           500



void task_Init();

extern uint16_t cell_readings[5];