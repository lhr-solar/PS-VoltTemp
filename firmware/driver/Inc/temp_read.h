#include <stm32l4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include "pinConfig.h"
#include <ADC.h>

#define QUEUE_LENGTH    2
#define ITEM_SIZE       sizeof( uint16_t )

typedef enum {
    TEMP_OK,
    TEMP_INIT_FAIL,
    TEMP_DEINIT_FAIL,
    TEMP_CHANNEL_CONFIG_FAIL,
    TEMP_ADC_START_FAIL,
    TEMP_ADC_READ_FAIL,
    TEMP_INTERRUPT_BUSY,
    TEMP_INTERRUPT_TIMEOUT,
    TEMP_INTERRUPT_ERROR,
    TEMP_QUEUE_FULL,

} temp_status_t;

enum {
    TEMP1,
    TEMP2,
    TEMP3,
    TEMP4,
    TEMP5
};

typedef struct {
    int32_t current_data;   // signed, 32 bit
    uint16_t adc_voltage;   // unsigned, 12 bit
} TempMsg_t;

/** ================================================================
 *  ADC (12 bit)
 * ================================================================ */
#define TEMP5_ADC_CHANNEL ADC_CHANNEL_5
#define TEMP4_ADC_CHANNEL ADC_CHANNEL_11
#define TEMP3_ADC_CHANNEL ADC_CHANNEL_7
#define TEMP2_ADC_CHANNEL ADC_CHANNEL_9
#define TEMP1_ADC_CHANNEL ADC_CHANNEL_10
#define TEMP_SAMPLE_TIME ADC_SAMPLETIME_47CYCLES_5

// In Theory Values (100% based)

#define ADC_MIN     229
#define ADC_MAX     3988

#define TEMP_MIN_CENTI   1000    // 10.00°C
#define TEMP_RANGE_CENTI 7500    // 75.00°C
#define ADC_RANGE        (ADC_MAX - ADC_MIN)

extern QueueHandle_t temp1_queue;
extern QueueHandle_t temp2_queue;
extern QueueHandle_t temp3_queue;
extern QueueHandle_t temp4_queue;
extern QueueHandle_t temp5_queue;

temp_status_t temp_init();

temp_status_t VoltTemp_StartADC(bool clearQueue, uint8_t temp_select);

temp_status_t VoltTemp_GetReading(uint8_t temp_select, TempMsg_t *message, TickType_t ticksToWait);

int32_t VoltTemp_ADCToTemp(uint16_t adc_val);




