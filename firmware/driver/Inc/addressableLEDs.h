#pragma once
#include "common.h"
#include "stm32xx_hal.h"
#include "WS2812B.h"

#define FAULT_COLOR WS2812B_SOLID_RED
#define NO_FAULT_COLOR WS2812B_SOLID_GREEN

#define NUM_ROW_LEDS 4

typedef enum{
    MODULE_0_TEMP_LED = 0,
    MODULE_1_TEMP_LED,
    MODULE_2_TEMP_LED,
    MODULE_3_TEMP_LED,
    MODULE_0_VOLTAGE_LED,
    MODULE_1_VOLTAGE_LED,
    MODULE_2_VOLTAGE_LED,
    MODULE_3_VOLTAGE_LED,
}addressable_led_idx;


ws2812b_status_t addressableLEDInit();

ws2812b_status_t clearAddressableColors(TickType_t delay_ticks);

ws2812b_status_t setModuleFaultLed(addressable_led_idx moduleLED, ws2812b_color_t color, TickType_t delay_ticks);

ws2812b_status_t setRowFaultLed(ws2812b_color_t colors[], addressable_led_idx moduleLED, TickType_t delay_ticks);