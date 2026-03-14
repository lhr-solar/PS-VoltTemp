#pragma once
#include "common.h"
#include "stm32xx_hal.h"
#include "WS2812B.h"
#include "inits.h"

#define NUM_ROW_LEDS 4
#define NUM_ADDRESSABLE_LEDS 8

typedef enum{
    MODULE_0_TEMP_LED = 0,
    MODULE_1_TEMP_LED,
    MODULE_2_TEMP_LED,
    MODULE_3_TEMP_LED,
    MODULE_0_VOLTAGE_LED,
    MODULE_1_VOLTAGE_LED,
    MODULE_2_VOLTAGE_LED,
    MODULE_3_VOLTAGE_LED,
    NUMBER_MODULE_STATUS_LEDs
}addressable_led_idx;

_Static_assert(NUM_ADDRESSABLE_LEDS >= NUMBER_MODULE_STATUS_LEDs, "NUM_ADDRESSABLE_LEDS and addressable_led_idx do not match");


ws2812b_status_t addressableLEDInit();

ws2812b_status_t clearAddressableColors(TickType_t delay_ticks);

ws2812b_status_t setModuleFaultLed(addressable_led_idx moduleLED, ws2812b_color_t color, TickType_t delay_ticks);

ws2812b_status_t setRowFaultLed(ws2812b_color_t colors[], addressable_led_idx moduleLED, TickType_t delay_ticks);