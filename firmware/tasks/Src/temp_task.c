#include "stm32l4xx_hal.h"
#include "common.h"
#include "stdlib.h"
#include "ADC.h"
#include "temperature.h"
#include "config.h"
#include "canbus.h"
#include "printf.h"

#define TEMPERATURE_PRINTF_PERIOD_MS 20000
#define TEMPERATURE_PRINTF_COUNT  (TEMPERATURE_PRINTF_PERIOD_MS/TEMPERATURE_THREAD_PERIOD_MS)

void task_temp_read(void *pvParameters){

  temp_status_t status;
  uint8_t printDebugCounter = 0;
  Temp_Init();

  TempMsg_t messages[NUM_THERMISTORS_PER_VOLTTEMP] = { 0 };

  while(1){

    // start sequencing for all temperature channels
    status = Temp_StartAllADC(false);

    if(status == TEMP_ADC_START_FAIL){
      // balls
    }  
    else{
      
      // the delay we wait for should be shorter than the period of the can send task
      if (Temp_GetAllReadings(messages, pdMS_TO_TICKS(100)) != TEMP_OK) {
        // printf("Failed to get all readings\r\n");
        // Error_Handler();
      }

      if(printDebugCounter >= TEMPERATURE_PRINTF_COUNT){
        printf("-------------------------------------------------------------\r\n");
        for (thermistor_t i = TEMP1; i < NUM_THERMISTORS - 1; i++) {
            printf("TEMP %u: %ld mC\r\n", i + 1, messages[i].temperature);
        }
        printf("-------------------------------------------------------------\r\n");
        printDebugCounter = 0;
      }
    }
    printDebugCounter++;
    vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_THREAD_PERIOD_MS));

  }
}
