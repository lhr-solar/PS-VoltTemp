#include <stm32l4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "temp_read.h"
#include "pinConfig.h"
#include "inits.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "UART.h"

StaticTask_t xBlinkyTaskBuffer;
StackType_t xBlinkyStack[configMINIMAL_STACK_SIZE];
StaticTask_t xQueueTaskBuffer;
StackType_t xQueueStack[ 512 ];
StackType_t xInitStack[configMINIMAL_STACK_SIZE];
StaticTask_t initTaskBuffer;

void ADC_Task(void *pvParameters) {
    TempMsg_t message1 = {0};
    TempMsg_t message2 = {0};
    TempMsg_t message3 = {0};
    TempMsg_t message4 = {0};
    TempMsg_t message5 = {0};
 //   TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        // Start ADC reading
        printf("ADC Task started\r\n");
        //Reset queue to prevent race condition (data already in queue and task does not wake up)
        if (VoltTemp_StartADC(false, TEMP1) != TEMP_OK) {
            Error_Handler();
        };
        printf("ADC1 started\r\n");

        if (VoltTemp_StartADC(false, TEMP2) != TEMP_OK) {
            Error_Handler();
        };
        printf("ADC2 started\r\n");

        if (VoltTemp_StartADC(false, TEMP3) != TEMP_OK) {
            Error_Handler();
        };
        printf("ADC3 started\r\n");

        if (VoltTemp_StartADC(false, TEMP4) != TEMP_OK) {
            Error_Handler();
        };
        printf("ADC4 started\r\n");

        if (VoltTemp_StartADC(false, TEMP5) != TEMP_OK) {
            Error_Handler();
        };
        printf("ADC5 started\r\n");

        //Block until we receive data in queue
        
        if (VoltTemp_GetReading(TEMP1, &message1, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 1\r\n");
            Error_Handler();
        } else {
            printf("Reading 1 success\r\n");
            // Convert data to current measurent
            message1.current_data = VoltTemp_ADCToTemp(message1.adc_voltage);
        }
        
        
        printf("Temp: %ld C\r\n", message1.current_data);
        printf("ADC1 Voltage: %d\r\n", message1.adc_voltage);

        if (VoltTemp_GetReading(TEMP2, &message2, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 2\r\n");
            Error_Handler();
        } else {
            printf("Reading 2 success\r\n");
            // Convert data to current measurent
            message2.current_data = VoltTemp_ADCToTemp(message2.adc_voltage);
        }

        printf("Temp2: %ld C\r\n", message2.current_data);
        printf("ADC2 Voltage: %d\r\n", message2.adc_voltage);

        if (VoltTemp_GetReading(TEMP3, &message3, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 3\r\n");
            Error_Handler();
        } else {
            printf("Reading 3 success\r\n");
            // Convert data to current measurent
            message3.current_data = VoltTemp_ADCToTemp(message3.adc_voltage);
        }

        printf("Temp3: %ld C\r\n", message3.current_data);
        printf("ADC3 Voltage: %d\r\n", message3.adc_voltage);

        if (VoltTemp_GetReading(TEMP4, &message4, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 4\r\n");
            Error_Handler();
        } else {
            printf("Reading 4 success\r\n");
            // Convert data to current measurent
            message4.current_data = VoltTemp_ADCToTemp(message4.adc_voltage);
        }

        printf("Temp4: %ld C\r\n", message4.current_data);
        printf("ADC4 Voltage: %d\r\n", message4.adc_voltage);

        if (VoltTemp_GetReading(TEMP5, &message5, pdMS_TO_TICKS(100)) != TEMP_OK) {
            printf("Failed to get reading 5\r\n");
            Error_Handler();
        } else {
            printf("Reading 5 success\r\n");
            // Convert data to current measurent
            message5.current_data = VoltTemp_ADCToTemp(message5.adc_voltage);
        }

        printf("Temp5: %ld C\r\n", message5.current_data);
        printf("ADC5 Voltage: %d\r\n", message5.adc_voltage);



        HAL_GPIO_TogglePin(PSOM_LED1_PORT, PSOM_LED1_PIN);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task_Blinky(void *pvParameters) {
    while (1) {
        HAL_GPIO_TogglePin(PSOM_LED2_PORT, PSOM_LED2_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Init_Task(void *argument)
{
    // Init UART printf
    husart1->Init.BaudRate = 115200;
    husart1->Init.WordLength = UART_WORDLENGTH_8B;
    husart1->Init.StopBits = UART_STOPBITS_1;
    husart1->Init.Parity = UART_PARITY_NONE;
    husart1->Init.Mode = UART_MODE_TX_RX;
    husart1->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    husart1->Init.OverSampling = UART_OVERSAMPLING_16;

    printf_init(husart1);

    temp_init();

    toggle_heartbeat();
    printf("Starting VoltTemp Test\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    toggle_heartbeat();
    



    //   printf("Starting VoltTemp Test\r\n");

    // Task kills itself
    vTaskDelete(NULL);
}


// void Test_Queue(void *pvParameters) {
//     int val = 3000;
//     while (1) {
//         xQueueSend(adc_queue, &val, 0);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

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
    
    
    //if(temp_init() == TEMP_INIT_FAIL) Error_Handler();
    
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
        (void*) 1,
        tskIDLE_PRIORITY+2,
        xQueueStack,
        &xQueueTaskBuffer
    );

    xTaskCreateStatic(
        Task_Blinky,
        "Blinky",
        configMINIMAL_STACK_SIZE,
        (void*) 1,
        tskIDLE_PRIORITY+1,
        xBlinkyStack,
        &xBlinkyTaskBuffer
    );

    vTaskStartScheduler();

    while (1)
    {
    }

    return 0;
}