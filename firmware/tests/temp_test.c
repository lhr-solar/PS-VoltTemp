#include <stdlib.h>
#include <stdio.h>

#include "stm32l4xx_hal.h"
#include "common.h"
#include "leds.h"
#include "temperature.h"
#include "pinConfig.h"
#include "inits.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "UART.h"

StaticTask_t xBlinkyTaskBuffer;
StackType_t xBlinkyStack[configMINIMAL_STACK_SIZE];
StaticTask_t xQueueTaskBuffer;
StackType_t xQueueStack[512];
StackType_t xInitStack[configMINIMAL_STACK_SIZE];
StaticTask_t initTaskBuffer;

void ADC_Task(void* pvParameters) {
    TempMsg_t message1 = { 0 };
    // TempMsg_t message2 = { 0 };
    // TempMsg_t message3 = { 0 };
    // TempMsg_t message4 = { 0 };
    // TempMsg_t message5 = { 0 };

    while (1) {
        // Start ADC reading
        printf("ADC Task started\r\n");
        // Reset queue to prevent race condition (data already in queue and task does not wake up)
        temp_status_t status = Temp_StartADC(false, TEMP1);
        if (status == TEMP_INVALID_CHANNEL) {
            printf("Invalid channel\r\n");
        }
        else if (status == TEMP_ADC_START_FAIL) {
            printf("Failed Start ADC Start\r\n");
        }
        printf("ADC1 started\r\n");

        // if (Temp_StartADC(false, TEMP2) != TEMP_OK) {
        //     Error_Handler();
        // };
        // printf("ADC2 started\r\n");

        // if (Temp_StartADC(false, TEMP3) != TEMP_OK) {
        //     Error_Handler();
        // };
        // printf("ADC3 started\r\n");

        // if (Temp_StartADC(false, TEMP4) != TEMP_OK) {
        //     Error_Handler();
        // };
        // printf("ADC4 started\r\n");

        // if (Temp_StartADC(false, TEMP5) != TEMP_OK) {
        //     Error_Handler();
        // };
        // printf("ADC5 started\r\n");

        // Block until we receive data in queue

        if (Temp_GetSingleReading(TEMP1, &message1, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 1\r\n");
            Error_Handler();
        }
        else {
            printf("Reading 1 success\r\n");
        }


        printf("Temp: %ld C\r\n", message1.temperature);
        printf("ADC1 Counts: %d\r\n", message1.adc_counts);

        // if (Temp_GetSingleReading(TEMP2, &message2, pdMS_TO_TICKS(100)) != TEMP_OK) {
        //     printf("Failed to get reading 2\r\n");
        //     Error_Handler();
        // }
        // else {
        //     printf("Reading 2 success\r\n");
        // }

        // printf("Temp2: %ld C\r\n", message2.temperature);
        // printf("ADC2 Counts: %d\r\n", message2.adc_counts);

        // if (Temp_GetSingleReading(TEMP3, &message3, pdMS_TO_TICKS(100)) != TEMP_OK) {
        //     printf("Failed to get reading 3\r\n");
        //     Error_Handler();
        // }
        // else {
        //     printf("Reading 3 success\r\n");
        // }

        // printf("Temp3: %ld C\r\n", message3.temperature);
        // printf("ADC3 Counts: %d\r\n", message3.adc_counts);

        // if (Temp_GetSingleReading(TEMP4, &message4, pdMS_TO_TICKS(100)) != TEMP_OK) {
        //     printf("Failed to get reading 4\r\n");
        //     Error_Handler();
        // }
        // else {
        //     printf("Reading 4 success\r\n");
        // }

        // printf("Temp4: %ld C\r\n", message4.temperature);
        // printf("ADC4 Counts: %d\r\n", message4.adc_counts);

        // if (Temp_GetSingleReading(TEMP5, &message5, pdMS_TO_TICKS(100)) != TEMP_OK) {
        //     printf("Failed to get reading 5\r\n");
        //     Error_Handler();
        // }
        // else {
        //     printf("Reading 5 success\r\n");
        // }

        // printf("Temp5: %ld C\r\n", message5.temperature);
        // printf("ADC5 Counts: %d\r\n", message5.adc_counts);

        HAL_GPIO_TogglePin(PSOM_LED1_PORT, PSOM_LED1_PIN);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task_Blinky(void* pvParameters) {
    while (1) {
        // HAL_GPIO_TogglePin(PSOM_LED2_PORT, PSOM_LED2_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Init_Task(void* argument) {
    // Init UART printf
    UART_Init();

    Temp_Init();

    toggle_heartbeat();
    printf("Starting Temperature Test\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    toggle_heartbeat();

    // Task kills itself
    vTaskDelete(NULL);
}

int main() {
    // initialize the HAL and system clock
    if (HAL_Init() != HAL_OK)
        Error_Handler();
    SystemClock_Config();

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    leds_init();
    // turn on psom leds to show volttemp number
    volttemp_led_on();

    mx_uart_init();

    xTaskCreateStatic(
        Init_Task,
        "Init Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 3,
        xInitStack,
        &initTaskBuffer
    );

    xTaskCreateStatic(
        ADC_Task,
        "ADC Task",
        512,
        (void*)1,
        tskIDLE_PRIORITY + 2,
        xQueueStack,
        &xQueueTaskBuffer
    );

    xTaskCreateStatic(
        Task_Blinky,
        "Blinky",
        configMINIMAL_STACK_SIZE,
        (void*)1,
        tskIDLE_PRIORITY + 1,
        xBlinkyStack,
        &xBlinkyTaskBuffer
    );

    vTaskStartScheduler();

    while (1)
    {
    }

    return 0;
}
