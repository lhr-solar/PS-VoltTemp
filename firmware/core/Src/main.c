#include "tasks.h"
#include <stm32xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"

// TODO
// Implement actual main for temp & bq
StaticTask_t taskInitBuffer;
StackType_t taskInitStack[configMINIMAL_STACK_SIZE];

int main(void) {
  xTaskCreateStatic(task_Init,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    taskInitStack,
                    &taskInitBuffer);

    vTaskStartScheduler();
    
    while(1){} 
    return 0;
}