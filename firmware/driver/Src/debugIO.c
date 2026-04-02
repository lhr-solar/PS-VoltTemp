#include "debugIO.h"

void debugIO_init(){

    // initialize clocks
    gpio_clock_enable((uint32_t)LOGIC_ANALZYER_CH1_PORT);
    gpio_clock_enable((uint32_t)LOGIC_ANALZYER_CH2_PORT);
    gpio_clock_enable((uint32_t)LOGIC_ANALZYER_CH4_PORT);

    GPIO_InitTypeDef debug_io_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = (LOGIC_ANALZYER_CH1_PIN)
    };

    debug_io_config.Pin = LOGIC_ANALZYER_CH1_PIN;
    HAL_GPIO_Init(LOGIC_ANALZYER_CH1_PORT, &debug_io_config);

    debug_io_config.Pin = LOGIC_ANALZYER_CH2_PIN;
    HAL_GPIO_Init(LOGIC_ANALZYER_CH2_PORT, &debug_io_config);

    debug_io_config.Pin = LOGIC_ANALZYER_CH4_PIN;
    HAL_GPIO_Init(LOGIC_ANALZYER_CH4_PORT, &debug_io_config);
}

void debugIO_set(debug_io_t pin, State pin_state){
    switch (pin)
    {
        case logic_analyzer_ch1:
            HAL_GPIO_WritePin(LOGIC_ANALZYER_CH1_PORT, LOGIC_ANALZYER_CH1_PIN, pin_state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case logic_analyzer_ch2:
            HAL_GPIO_WritePin(LOGIC_ANALZYER_CH2_PORT, LOGIC_ANALZYER_CH2_PIN, pin_state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case logic_analyzer_ch4:
            HAL_GPIO_WritePin(LOGIC_ANALZYER_CH4_PORT, LOGIC_ANALZYER_CH4_PIN, pin_state == ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void debugIO_toggle(debug_io_t pin){
    switch (pin)
    {
        case logic_analyzer_ch1:
            HAL_GPIO_TogglePin(LOGIC_ANALZYER_CH1_PORT, LOGIC_ANALZYER_CH1_PIN);
            break;
        case logic_analyzer_ch2:
            HAL_GPIO_TogglePin(LOGIC_ANALZYER_CH2_PORT, LOGIC_ANALZYER_CH2_PIN);
            break;
        case logic_analyzer_ch4:
            HAL_GPIO_TogglePin(LOGIC_ANALZYER_CH4_PORT, LOGIC_ANALZYER_CH4_PIN);
            break;
        default:
            break;
    }

}