#pragma once

#include "stm32xx_hal.h"
#include "common.h"
#include <BPSCAN_can_msgs.h>
#include "bq76920.h"


// BQ Task
#define configVOLTTEMP_STACK_SIZE 2048
#define VOLTTEMP_PRIO    tskIDLE_PRIORITY + 1
#define VOLTTEMP_DELAY   pdMS_TO_TICKS(300)
#define CAN_DELAY pdMS_TO_TICKS(10)

void task_ReadVoltage(void *pvParameters);
void task_SendMessage(void *pvParameters);

// Init Tasks
#define TASK_INIT_PRIO                  tskIDLE_PRIORITY + 1
#define TASK_INIT_STACK_SIZE            configMINIMAL_STACK_SIZE

void task_Init();

typedef union  {
bps_voltage_temperature_0_t vt1;
bps_voltage_temperature_1_t vt2;
bps_voltage_temperature_2_t vt3;
bps_voltage_temperature_3_t vt4;
bps_voltage_temperature_4_t vt5;
bps_voltage_temperature_5_t vt6;
bps_voltage_temperature_6_t vt7;
bps_voltage_temperature_7_t vt8;
} CAN_struct_t;


extern CAN_struct_t Can_struct;

extern uint16_t cell_readings[5];