#include "tasks.h"
#include "bq76920.h"

// For volttemp
StaticTask_t volttempt_task_buffer;
StackType_t  volttemp_task_stack[configMINIMAL_STACK_SIZE];
/*
void Task_Init(){
    
    //Init BQ chip.
    Init_BQ76920();

    xTaskCreateStatic(
        volttemp_task,
        "VoltTemp Task",
        configMINIMAL_STACK_SIZE,
        (void*) 1,
        VOLTTEMP_PRIO,
        volttemp_task_stack,
        &volttempt_task_buffer
    );

   // Delete Init Task
    vTaskDelete(NULL);
}
    */