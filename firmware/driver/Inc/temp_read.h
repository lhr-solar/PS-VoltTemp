#include <stm32f4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <ADC.h>


#define STACK_SIZE 200

#define QUEUE_LENGTH    10
#define ITEM_SIZE       sizeof( uint32_t )



#define ADC_PIN   GPIO_PIN_3
#define ADC_PORT  GPIOA

void temp_init(ADC_InitTypeDef, ADC_HandleTypeDef*);


void task_temp_read(void *pvParameters);
