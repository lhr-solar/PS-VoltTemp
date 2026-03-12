#include "stm32l4xx_hal.h"
#include "common.h"
#include "stdlib.h"
#include "ADC.h"
#include "temperature.h"

/*
void task_temp_read(void *pvParameters){
  uint32_t reading = 0;
  // read once
    HAL_Delay(100);
    adc_status_t stat = adc_read(ADC_CHANNEL_1, ADC_SAMPLETIME_2CYCLE_5, ADC_Handle, &readings);
    if (stat != ADC_OK)Error_Handler();


    xQueueReceive(xReadings, &reading, 0);
  
}
    */
