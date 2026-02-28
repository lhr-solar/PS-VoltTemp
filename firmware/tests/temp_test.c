#include "stm32xx_hal.h"
#include "temp_read.h"

StaticTask_t xBlinkyTaskBuffer;
StackType_t xBlinkyStack[ 200 ];
StaticTask_t xQueueTaskBuffer;
StackType_t xQueueStack[ 200 ];

void ADC_Task(void *pvParameters) {
    TempMsg_t message;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        // Start ADC reading
        // Reset queue to prevent race condition (data already in queue and task does not wake up)
        if (VoltTemp_StartADC(true, 1) != TEMP_OK) {
           // Error_Handler();
        };

        // Block until we receive data in queue
        if (VoltTemp_GetReading(1, &message, portMAX_DELAY) == TEMP_OK) {
            // Convert data to current measurent
            message.current_data = VoltTemp_ADCToTemp(message.adc_voltage);
        }
        
        HAL_GPIO_TogglePin(TEMP_GPIO_PORT, TEMP_CHARGE_PIN);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

void Task_Blinky(void *pvParameters) {
    while (1) {
        HAL_GPIO_TogglePin(TEMP_GPIO_PORT, TEMP_HB_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// void Test_Queue(void *pvParameters) {
//     int val = 3000;
//     while (1) {
//         xQueueSend(adc_queue, &val, 0);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

int main() {
    HAL_Init();
    SystemClock_Config();
    
    //if(temp_init() == TEMP_INIT_FAIL) Error_Handler();
    temp_init();

    xTaskCreateStatic(
        ADC_Task,
        "ADC Task",
        512,
        (void*) 1,
        tskIDLE_PRIORITY+2,
        xQueueStack,
        &xQueueTaskBuffer
    );

    xTaskCreateStatic(
        Task_Blinky,
        "Blinky",
        200,
        (void*) 1,
        tskIDLE_PRIORITY+3,
        xBlinkyStack,
        &xBlinkyTaskBuffer
    );

    // xTaskCreateStatic(
    //     Test_Queue,
    //     "Queue Send",
    //     200,
    //     (void*) 1,
    //     tskIDLE_PRIORITY+4,
    //     xQueueStack,
    //     &xQueueTaskBuffer
    // );

    vTaskStartScheduler();

    return 0;
}