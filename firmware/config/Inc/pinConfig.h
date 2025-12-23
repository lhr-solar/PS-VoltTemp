#pragma once

#include "common.h"
#include "stm32xx_hal.h"

//--------------------------------------------------------------------------------
// Pin configuration for LEDs and heartbeat
#define HEARTBEAT_PORT     GPIOB
#define HEARTBEAT_PIN      GPIO_PIN_11

#define PSOM_LED1_PORT     GPIOA
#define PSOM_LED1_PIN      GPIO_PIN_7

#define PSOM_LED2_PORT     GPIOA
#define PSOM_LED2_PIN      GPIO_PIN_8

#define PSOM_LED3_PORT     GPIOA
#define PSOM_LED3_PIN      GPIO_PIN_15
//--------------------------------------------------------------------------------

