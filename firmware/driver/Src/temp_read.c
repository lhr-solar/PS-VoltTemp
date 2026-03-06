#include "temp_read.h"
#include "inits.h"

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
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if (hadc->Instance == ADC1)
    {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */

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
        HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
        // GPIO_InitTypeDef ADC1_5_InitStruct = {0};
        // GPIO_InitTypeDef ADC1_7_InitStruct = {0};
        // GPIO_InitTypeDef ADC1_9_InitStruct = {0};
        // GPIO_InitTypeDef ADC1_10_InitStruct = {0};
        // GPIO_InitTypeDef ADC1_11_InitStruct = {0};
        // RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

        // if(hadc->Instance==ADC1) {
        // /** Initializes the peripherals clock
        //  */
        // PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        // PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
        // PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
        // PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
        // PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
        // PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
        // PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
        // PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
        // PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
        // if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        //   Error_Handler();
        // }

        // /* Peripheral clock enable */
        // __HAL_RCC_ADC_CLK_ENABLE();

        // /**ADC1 GPIO Configuration
        // PA0     ------> ADC1_IN5
        // PA2     ------> ADC1_IN7
        // PA4     ------> ADC1_IN9
        // PA5     ------> ADC1_IN10
        // PA6     ------> ADC1_IN11
        // */
        // __HAL_RCC_GPIOA_CLK_ENABLE();
        // ADC1_5_InitStruct.Pin = TEMP5_PIN;
        // ADC1_5_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        // ADC1_5_InitStruct.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(TEMP5_PORT, &ADC1_5_InitStruct);

        // ADC1_7_InitStruct.Pin = TEMP3_PIN;
        // ADC1_7_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        // ADC1_7_InitStruct.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(TEMP3_PORT, &ADC1_7_InitStruct);

        // ADC1_9_InitStruct.Pin = TEMP2_PIN;
        // ADC1_9_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        // ADC1_9_InitStruct.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(TEMP2_PORT, &ADC1_9_InitStruct);

        // ADC1_10_InitStruct.Pin = TEMP1_PIN;
        // ADC1_10_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        // ADC1_10_InitStruct.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(TEMP1_PORT, &ADC1_10_InitStruct);

        // ADC1_11_InitStruct.Pin = TEMP4_PIN;
        // ADC1_11_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
        // ADC1_11_InitStruct.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(TEMP4_PORT, &ADC1_11_InitStruct);

        // /* ADC1 interrupt Init: PRIO MUST BE AT LEAST 5 */
        // HAL_NVIC_SetPriority(ADC1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
        // HAL_NVIC_EnableIRQ(ADC1_IRQn);
    }
}

// void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   if(hcan->Instance==CAN1) {
//     /* Peripheral clock enable */
//     __HAL_RCC_CAN1_CLK_ENABLE();

//     __HAL_RCC_GPIOB_CLK_ENABLE();
//     /**CAN1 GPIO Configuration
//     PB8     ------> CAN1_RX
//     PB9     ------> CAN1_TX
//     */
//     GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//     GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//   }
// }

/**
 * @brief Initializes ADC queue and hardware.
 * - Fails if adc_init() fails.
 * - Called by Amperes_Init().
 */
static bool VoltTemp_ADC_Init()
{
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
    ADC_InitTypeDef init = {0};

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
        return false;

    /* Calibrate after initialization (must be after clock setup)*/
    HAL_ADCEx_Calibration_Start(hadc1, ADC_SINGLE_ENDED);

    return true;
}

/** ================================================================
 *  Temperature Functions
 * ================================================================ */

temp_status_t temp_init()
{
    /* HAL_Init should be run before this is called */

    // Initialize GPIO
    // Temp_GPIO_Init();

    // Init ADC
    if (!VoltTemp_ADC_Init())
        return TEMP_INIT_FAIL;

    return TEMP_OK;
}

int32_t VoltTemp_ADCToTemp(uint16_t adc_val)
{

    // 0.2V - 3.2V corresponds to 10C - 85C

    // 0-4095 ADC value corresponds to

    // Returns temperature in CENTIDEGREES C for fixed point precision (e.g. 2534 = 25.34C)

    // Get signed ADC value in terms of reference point; scale for fixed point math
    if (adc_val <= ADC_MIN)
        return TEMP_MIN_CENTI;

    if (adc_val >= ADC_MAX)
        return TEMP_MIN_CENTI + TEMP_RANGE_CENTI;

    int32_t delta_adc = adc_val - ADC_MIN;

    // Linear interpolation:
    // T = Tmin + (delta_adc * TempRange) / ADCrange
    int32_t temp = TEMP_MIN_CENTI +
                   (delta_adc * TEMP_RANGE_CENTI) / ADC_RANGE;

    return temp;
}

temp_status_t VoltTemp_StartADC(bool clearQueue, uint8_t temp_select)
{
    // Clear queue if requested
    if (clearQueue)
    {
        switch (temp_select)
        {
        case 1:
            xQueueReset(temp1_queue);
            break;
        case 2:
            xQueueReset(temp2_queue);
            break;
        case 3:
            xQueueReset(temp3_queue);
            break;
        case 4:
            xQueueReset(temp4_queue);
            break;
        case 5:
            xQueueReset(temp5_queue);
            break;
        }
    }

    switch (temp_select)
    {
    case TEMP1:
        // Create sConfig struct
        ADC_ChannelConfTypeDef sConfig = {0};
        sConfig.Channel = TEMP1_ADC_CHANNEL;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (adc_read(hadc1, &sConfig, temp1_queue) != ADC_OK)
            return TEMP_ADC_START_FAIL;
        break;
    // case 2:
    //     if (adc_read(TEMP2_ADC_CHANNEL, TEMP_SAMPLE_TIME, hadc1, temp2_queue) != ADC_OK)
    //         return TEMP_ADC_START_FAIL;
    //     break;
    // case 3:
    //     if (adc_read(TEMP3_ADC_CHANNEL, TEMP_SAMPLE_TIME, hadc1, temp3_queue) != ADC_OK)
    //         return TEMP_ADC_START_FAIL;
    //     break;
    // case 4:
    //     if (adc_read(TEMP4_ADC_CHANNEL, TEMP_SAMPLE_TIME, hadc1, temp4_queue) != ADC_OK)
    //         return TEMP_ADC_START_FAIL;
    //     break;
    // case 5:
    //     if (adc_read(TEMP5_ADC_CHANNEL, TEMP_SAMPLE_TIME, hadc1, temp5_queue) != ADC_OK)
    //         return TEMP_ADC_START_FAIL;
    //     break;
    default:
        // TODO return proper error
        break;
    }
    return TEMP_OK;
}

temp_status_t VoltTemp_GetReading(uint8_t temp_select, TempMsg_t *message, TickType_t ticksToWait)
{
    // Get ADC value from queue
    // QueueHandle_t* queue = NULL;
    // switch(temp_select) {
    //     case 1: queue = &temp1_queue; break;
    //     case 2: queue = &temp2_queue; break;
    //     case 3: queue = &temp3_queue; break;
    //     case 4: queue = &temp4_queue; break;
    //     case 5: queue = &temp5_queue; break;
    // }
    switch (temp_select)
    {
    case TEMP1:
        xQueueReceive(temp1_queue, &(message->adc_voltage), ticksToWait);
        break;
    case TEMP2:
        xQueueReceive(temp2_queue, &(message->adc_voltage), ticksToWait);
        break;
    case TEMP3:
        xQueueReceive(temp3_queue, &(message->adc_voltage), ticksToWait);
        break;
    case TEMP4:
        xQueueReceive(temp4_queue, &(message->adc_voltage), ticksToWait);
        break;
    case TEMP5:
        xQueueReceive(temp5_queue, &(message->adc_voltage), ticksToWait);
        break;
    }

    // if (xQueueReceive(temp1_queue, &(message->adc_voltage), ticksToWait) != pdPASS) {
    //     return TEMP_ADC_READ_FAIL;
    // }
    // message->current_data = VoltTemp_ADCToTemp(message->adc_voltage);
    return TEMP_OK;
}

// void Amperes_UpdateLEDs(int32_t currentValue) {
//     if (currentValue < 0) {
//         HAL_GPIO_WritePin(AMPERES_GPIO_PORT, AMPERES_DISCHARGE_PIN, 1);
//         HAL_GPIO_WritePin(AMPERES_GPIO_PORT, AMPERES_CHARGE_PIN, 0);
//     } else if (currentValue > 0) {
//         HAL_GPIO_WritePin(AMPERES_GPIO_PORT, AMPERES_CHARGE_PIN, 1);
//         HAL_GPIO_WritePin(AMPERES_GPIO_PORT, AMPERES_DISCHARGE_PIN, 0);
//     }
// }
