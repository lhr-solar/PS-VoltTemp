#include <stm32f4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <ADC.h>
#include "temp_read.h"


uint8_t qStorage[QUEUE_LENGTH * ITEM_SIZE];
static StaticQueue_t xStaticQueue;
StaticTask_t xTaskBuffer;
StackType_t xStack[ STACK_SIZE ];
QueueHandle_t xReadings;
ADC_HandleTypeDef* ADC_Handle;


void HAL_ADC_MspGPIOInit() {
  /* GPIO Ports Clock Enable */
  //======================================================
  if(ADC_PORT == GPIOA)      __HAL_RCC_GPIOA_CLK_ENABLE();

  else if(ADC_PORT == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();

  else if(ADC_PORT == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
  //======================================================

  GPIO_InitTypeDef input =  {
    .Pin = ADC_PIN,
    .Mode = GPIO_MODE_ANALOG,
    .Pull = GPIO_NOPULL,
  };

  HAL_GPIO_Init(ADC_PORT, &input);
}



void temp_init(ADC_InitTypeDef TEMP_ADC, ADC_HandleTypeDef* handle){

  ADC_Handle = handle;

  xReadings = xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, qStorage, &xStaticQueue);

  // init ADC
  ADC_InitTypeDef adc_init_1 = {0};

  adc_init_1.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  adc_init_1.Resolution = ADC_RESOLUTION_12B;
  adc_init_1.ScanConvMode = DISABLE;
  adc_init_1.ContinuousConvMode = DISABLE;
  adc_init_1.DiscontinuousConvMode = DISABLE;
  adc_init_1.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc_init_1.ExternalTrigConv = ADC_SOFTWARE_START;
  adc_init_1.DataAlign = ADC_DATAALIGN_RIGHT;
  adc_init_1.NbrOfConversion = 1;
  adc_init_1.DMAContinuousRequests = DISABLE;
  adc_init_1.EOCSelection = ADC_EOC_SINGLE_CONV;

  volatile adc_status_t s = adc_init(adc_init_1, ADC_Handle);
  s+=0;
  if (s != ADC_OK) Error_Handler();

  xTaskCreateStatic(task_temp_read,
                  "Read the temperature :^)",
                  configMINIMAL_STACK_SIZE,
                  (void*) 1,
                  tskIDLE_PRIORITY+4,
                  xStack,
                  &xTaskBuffer);

  vTaskStartScheduler();  
  
  // should never reach

}




