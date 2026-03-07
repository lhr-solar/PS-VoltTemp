#include <stm32l4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include "pinConfig.h"
#include <ADC.h>

#define QUEUE_LENGTH    3
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
    TEMP1 = 0,
    TEMP2,
    TEMP3,
    TEMP4,
    TEMP5,
};

typedef struct {
    int32_t temperature;   // signed, 32 bit, millicelsius
    uint16_t adc_counts;   // unsigned, 12 bit, counts
} TempMsg_t;

/** ================================================================
 *  ADC (12 bit)
 * ================================================================ */
#define TEMP5_ADC_CHANNEL ADC_CHANNEL_5
#define TEMP4_ADC_CHANNEL ADC_CHANNEL_11
#define TEMP3_ADC_CHANNEL ADC_CHANNEL_7
#define TEMP2_ADC_CHANNEL ADC_CHANNEL_9
#define TEMP1_ADC_CHANNEL ADC_CHANNEL_10
#define TEMP_SAMPLE_TIME ADC_SAMPLETIME_2CYCLES_5

extern QueueHandle_t temp1_queue;
extern QueueHandle_t temp2_queue;
extern QueueHandle_t temp3_queue;
extern QueueHandle_t temp4_queue;
extern QueueHandle_t temp5_queue;

// TODO: doc comments

temp_status_t temp_init();

temp_status_t VoltTemp_StartADC(bool clearQueue, uint8_t temp_select);

temp_status_t VoltTemp_GetReading(uint8_t temp_select, TempMsg_t* message, TickType_t ticksToWait);
