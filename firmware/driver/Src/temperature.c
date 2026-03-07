#include "temperature.h"
#include "inits.h"
#include "thermistor_lut.h"

/** ================================================================
 *  Local Variables
 * ================================================================ */
 /** Temp Queues to store ADC conversions
  * - Can hold 10 elements for on the fly averaging.
  */
QueueHandle_t temp1_queue;
QueueHandle_t temp2_queue;
QueueHandle_t temp3_queue;
QueueHandle_t temp4_queue;
QueueHandle_t temp5_queue;

uint8_t temp1_qStorage[QUEUE_LENGTH * ITEM_SIZE];
uint8_t temp2_qStorage[QUEUE_LENGTH * ITEM_SIZE];
uint8_t temp3_qStorage[QUEUE_LENGTH * ITEM_SIZE];
uint8_t temp4_qStorage[QUEUE_LENGTH * ITEM_SIZE];
uint8_t temp5_qStorage[QUEUE_LENGTH * ITEM_SIZE];

static StaticQueue_t xStaticQueue_temp1;
static StaticQueue_t xStaticQueue_temp2;
static StaticQueue_t xStaticQueue_temp3;
static StaticQueue_t xStaticQueue_temp4;
static StaticQueue_t xStaticQueue_temp5;
// uint8_t adc_qStorage[ADC_QUEUE_LENGTH * ADC_ITEM_SIZE];
// static StaticQueue_t xStaticQueue_adc;

/** ================================================================
 *  Local Init Functions
 * ================================================================ */

 /**
  * @brief Initialize ADC pin, clock, and interrupt.
  * - Fails if clock config fails.
  * - Called by HAL_ADC_Init().
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (hadc->Instance == ADC1)
    {
        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
        PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
        PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
        PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
        PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
        PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_ADC_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PA0     ------> ADC1_IN5
        PA2     ------> ADC1_IN7
        PA4     ------> ADC1_IN9
        PA5     ------> ADC1_IN10
        PA6     ------> ADC1_IN11
        PB1     ------> ADC1_IN16
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC1_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
    }
}

temp_status_t Temp_Init() {
    /* Initialize queue */
    temp1_queue = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        temp1_qStorage,
        &xStaticQueue_temp1);
    temp2_queue = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        temp2_qStorage,
        &xStaticQueue_temp2);
    temp3_queue = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        temp3_qStorage,
        &xStaticQueue_temp3);
    temp4_queue = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        temp4_qStorage,
        &xStaticQueue_temp4);
    temp5_queue = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        temp5_qStorage,
        &xStaticQueue_temp5);

    /* ================ ADC Init Struct ================ */
    ADC_InitTypeDef init = { 0 };

    /** Common config
     */

    init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    init.Resolution = ADC_RESOLUTION_12B;
    init.DataAlign = ADC_DATAALIGN_RIGHT;
    init.ScanConvMode = ADC_SCAN_DISABLE;
    init.EOCSelection = ADC_EOC_SINGLE_CONV;
    init.LowPowerAutoWait = DISABLE;
    init.ContinuousConvMode = DISABLE;
    init.NbrOfConversion = 1;
    init.DiscontinuousConvMode = DISABLE;
    init.ExternalTrigConv = ADC_SOFTWARE_START;
    init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    init.DMAContinuousRequests = DISABLE;
    init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    init.OversamplingMode = DISABLE;

    /* Initialize ADC */
    volatile adc_status_t s = adc_init(&init, hadc1);
    s += 0;
    if (s != ADC_OK)
        return TEMP_INIT_FAIL;

    /* Calibrate after initialization (must be after clock setup)*/
    HAL_ADCEx_Calibration_Start(hadc1, ADC_SINGLE_ENDED);

    return TEMP_OK;
}

// Returns temperature in millicelsius from thermistor lookup table
// LUT generated by generateTempLUT.py using data in ERTJ1VR.csv
static int32_t ADCToTemp(uint16_t adc_val) {
    return thermistor_lut[adc_val];
}

temp_status_t Temp_StartADC(bool clearQueue, thermistor_t temp_select) {
    // Clear queue if requested
    if (clearQueue)
    {
        switch (temp_select)
        {
        case TEMP1:
            xQueueReset(temp1_queue);
            break;
        case TEMP2:
            xQueueReset(temp2_queue);
            break;
        case TEMP3:
            xQueueReset(temp3_queue);
            break;
        case TEMP4:
            xQueueReset(temp4_queue);
            break;
        case TEMP5:
            xQueueReset(temp5_queue);
            break;
        default:
            return TEMP_INVALID_CHANNEL;
        }
    }

    ADC_ChannelConfTypeDef sConfig = { 0 };

    switch (temp_select)
    {
    case TEMP1:
        // Create sConfig struct
        sConfig.Channel = TEMP1_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = TEMP_SAMPLE_TIME;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp1_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    case TEMP2:
        // Create sConfig struct
        sConfig.Channel = TEMP2_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = TEMP_SAMPLE_TIME;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp2_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    case TEMP3:
        // Create sConfig struct
        sConfig.Channel = TEMP3_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = TEMP_SAMPLE_TIME;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp3_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    case TEMP4:
        // Create sConfig struct
        sConfig.Channel = TEMP4_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = TEMP_SAMPLE_TIME;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp4_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    case TEMP5:
        // Create sConfig struct
        sConfig.Channel = TEMP5_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = TEMP_SAMPLE_TIME;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp5_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    default:
        return TEMP_INVALID_CHANNEL;
    }
    return TEMP_OK;
}

temp_status_t Temp_GetSingleReading(thermistor_t temp_select, TempMsg_t* message, TickType_t ticksToWait) {
    // Receieve adc reading (counts) from queue
    // TODO: error handle queue recv failure
    switch (temp_select)
    {
    case TEMP1:
        if (xQueueReceive(temp1_queue, &(message->adc_counts), ticksToWait) != pdPASS) {
            return TEMP_ADC_READ_FAIL;
        };
        break;
    case TEMP2:
        if (xQueueReceive(temp2_queue, &(message->adc_counts), ticksToWait) != pdPASS) {
            return TEMP_ADC_READ_FAIL;
        };
        break;
    case TEMP3:
        if (xQueueReceive(temp3_queue, &(message->adc_counts), ticksToWait) != pdPASS) {
            return TEMP_ADC_READ_FAIL;
        };
        break;
    case TEMP4:
        if (xQueueReceive(temp4_queue, &(message->adc_counts), ticksToWait) != pdPASS) {
            return TEMP_ADC_READ_FAIL;
        };
        break;
    case TEMP5:
        if (xQueueReceive(temp5_queue, &(message->adc_counts), ticksToWait) != pdPASS) {
            return TEMP_ADC_READ_FAIL;
        };
        break;
    default:
        return TEMP_INVALID_CHANNEL;
    }

    // Store converted millicelsius value in message struct
    message->temperature = ADCToTemp(message->adc_counts);

    return TEMP_OK;
}

temp_status_t Temp_GetAllReadings(TempMsg_t* messages, TickType_t ticksToWait) {
    // Read first 4 thermistors (only 4 on the segment, 5th is extra sooo)
    for (thermistor_t i = TEMP1; i < NUM_THERMISTORS - 1; i++) {
        temp_status_t status = Temp_GetSingleReading(i, &messages[i], ticksToWait);
        if (status != TEMP_OK) {
            return status;
        }
    }

    return TEMP_OK;
}
