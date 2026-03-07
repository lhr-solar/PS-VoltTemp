#pragma once

#include "common.h"
#include "stm32xx_hal.h"

// Pin configuration for LEDs and heartbeat
//================================================================================
#define HEARTBEAT_PORT      GPIOB
#define HEARTBEAT_PIN       GPIO_PIN_11

#define PSOM_LED1_PORT      GPIOA
#define PSOM_LED1_PIN       GPIO_PIN_7

#define PSOM_LED2_PORT      GPIOA
#define PSOM_LED2_PIN       GPIO_PIN_8

#define PSOM_LED3_PORT      GPIOA
#define PSOM_LED3_PIN       GPIO_PIN_15
//================================================================================


// Pin configuration for BQ I2C (I2C1 on PSOM)
//================================================================================
#define I2C_PORT            GPIOB
#define I2C_SCL_PIN         GPIO_PIN_6
#define I2C_SDA_PIN         GPIO_PIN_7
//================================================================================

// USART pins for PSOM
#define USART_PORT GPIOA
#define USART_TX_PIN GPIO_PIN_9
#define USART_RX_PIN GPIO_PIN_10