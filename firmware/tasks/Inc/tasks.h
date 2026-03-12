#pragma once

#include "stm32xx_hal.h"
#include "common.h"
#include <BPSCAN_can_msgs.h>
#include "bq76920.h"


// BQ Task
#define configVOLTTEMP_STACK_SIZE 2048
#define VOLTTEMP_PRIO    tskIDLE_PRIORITY + 1
#define CAN_DELAY pdMS_TO_TICKS(10)

void task_ReadVoltage(void *pvParameters);
void task_SendMessage(void *pvParameters);

// Init Tasks
#define TASK_INIT_PRIO                  tskIDLE_PRIORITY + 1
#define TASK_INIT_STACK_SIZE            configMINIMAL_STACK_SIZE

#define CAN_SEND_TASK_PERIOD_MS             500
#define TEMPERATURE_THREAD_PERIOD_MS        250
#define VOLTTEMP_THREAD_DELAY_MS            300



void task_Init();

typedef union  {
    bps_voltage_temperature_arr_0_t vt0;
    bps_voltage_temperature_arr_1_t vt1;
    bps_voltage_temperature_arr_2_t vt2;
    bps_voltage_temperature_arr_3_t vt3;
    bps_voltage_temperature_arr_4_t vt4;
    bps_voltage_temperature_arr_5_t vt5;
    bps_voltage_temperature_arr_6_t vt6;
    bps_voltage_temperature_arr_7_t vt7;
} CAN_struct_t;

extern SemaphoreHandle_t can_msg_mutex;
extern StaticSemaphore_t can_msg_mutex_buffer; 


extern CAN_struct_t Can_struct;

extern uint16_t cell_readings[5];