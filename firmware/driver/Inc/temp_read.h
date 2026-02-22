#include <stm32l4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <ADC.h>


#define STACK_SIZE 200

#define QUEUE_LENGTH    10
#define ITEM_SIZE       sizeof( uint32_t )

typedef enum {
    TEMP_OK,
    TEMP_INIT_FAIL,
    TEMP_DEINIT_FAIL,
    TEMP_CHANNEL_CONFIG_FAIL,
    TEMP_INTERRUPT_BUSY,
    TEMP_INTERRUPT_TIMEOUT,
    TEMP_INTERRUPT_ERROR,
    TEMP_QUEUE_FULL,

} temp_status_t;

#define ADC_PIN   GPIO_PIN_3
#define ADC_PORT  GPIOA

temp_status_t temp_init(ADC_InitTypeDef, ADC_HandleTypeDef*);


temp_status_t task_temp_read(void *pvParameters);
