#pragma once

#include "stm32xx_hal.h"
#include "common.h"
#include "pinConfig.h"
#include "config.h"

typedef enum{
    PSOM_HEARTBEAT,
    BQ_HEARTBEAT,
    BQ_FAULT
}voltemp_leds_t;

void leds_init(void);

void set_led(voltemp_leds_t led, State pin_state);

void toggle_led(voltemp_leds_t led);

void set_heartbeat_led(State pin_state);

void volttemp_id_led_on(void);

void toggle_heartbeat(void);
