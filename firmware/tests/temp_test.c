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
    while (1) {
        // Start ADC reading
        // Reset queue to prevent race condition (data already in queue and task does not wake up)
        temp_status_t status = Temp_StartAllADC(false);
        if (status == TEMP_INVALID_CHANNEL) {
            printf("Invalid Channel\r\n");
        }
        else if (status == TEMP_ADC_START_FAIL) {
            printf("Failed Start ADC Start\r\n");
        }
        
        printf("\r\n");
        printf("All ADC Readings Started...");

        // Block until we receive data in queue
        TempMsg_t messages[4] = { 0 };

        if (Temp_GetAllReadings(messages, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get all readings\r\n");
            Error_Handler();
        }
        else {
            printf("Reading Success\r\n\r\n");
        }

        // Print all temps
        for (thermistor_t i = TEMP1; i < NUM_THERMISTORS - 1; i++) {
            printf("TEMP %u: %ld mC\r\n", i + 1, messages[i].temperature);
            printf("ADC Counts: %d\r\n", messages[i].adc_counts);
            printf("Raw Voltage: %d mV\r\n", messages[i].raw_voltage);
            printf("\r\n");
        }

        printf("-------------------------------------------------------------\r\n");

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
