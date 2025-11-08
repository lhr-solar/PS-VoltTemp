#include <stm32f4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <ADC.h>
#include <temp_read.h>


void task_temp_read(void *pvParameters){
  uint32_t reading = 0;
  // read once
    HAL_Delay(100);
    adc_status_t stat = adc_read(ADC_CHANNEL_1, ADC_SAMPLETIME_3CYCLES, ADC_Handle, &xReadings);
    if (stat != ADC_OK)Error_Handler();


    xQueueReceive(xReadings, &reading, 0);
  
}