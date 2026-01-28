#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h" /* Must come first. */
#include "stm32xx_hal.h"
#include "common.h"


// BQ Task
extern StaticTask_t volttempt_task_buffer;
extern StackType_t  volttemp_task_stack[configMINIMAL_STACK_SIZE];
#define VOLTTEMP_PRIO    1
#define VOLTTEMP_DELAY   pdMS_TO_TICKS(200)

void volttemp_task(void *pvParameters);


// Init Tasks
#define TASK_INIT_PRIO                  tskIDLE_PRIORITY + 1
#define TASK_INIT_STACK_SIZE            configMINIMAL_STACK_SIZE

void Task_Init();

#endif
