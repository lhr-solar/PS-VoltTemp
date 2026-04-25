#pragma once

#include "common.h"
#include "stm32xx_hal.h"

// Pin configuration for LEDs and heartbeat
//================================================================================
#define HEARTBEAT_PORT      GPIOB
#define HEARTBEAT_PIN       GPIO_PIN_11

#define BQ_HEARTBEAT_PORT   GPIOA
#define BQ_HEARTBEAT_PIN    GPIO_PIN_12

#define BQ_FAULT_PORT       GPIOA
#define BQ_FAULT_PIN        GPIO_PIN_11

#define BQ_BOOT_PORT        GPIOB
#define BQ_BOOT_PIN         GPIO_PIN_0

#define PSOM_LED1_PORT      GPIOA
#define PSOM_LED1_PIN       GPIO_PIN_7

#define PSOM_LED2_PORT      GPIOA
#define PSOM_LED2_PIN       GPIO_PIN_8

#define PSOM_LED3_PORT      GPIOA
#define PSOM_LED3_PIN       GPIO_PIN_15
//================================================================================

// Pin configuration for the broken out logic analzyer pins
//================================================================================
#define LOGIC_ANALZYER_CH1_PORT GPIOB
#define LOGIC_ANALZYER_CH1_PIN GPIO_PIN_3

#define LOGIC_ANALZYER_CH2_PORT GPIOB
#define LOGIC_ANALZYER_CH2_PIN GPIO_PIN_15

#define LOGIC_ANALZYER_CH4_PORT GPIOB
#define LOGIC_ANALZYER_CH4_PIN GPIO_PIN_14
//================================================================================



// Pin configuration for BQ I2C
//================================================================================
#define I2C_PORT            GPIOB
#define I2C_SCL_PIN         GPIO_PIN_6
#define I2C_SDA_PIN         GPIO_PIN_7
//================================================================================

// Pin configuration for ADC 
//================================================================================
// Temp 1 = ADC1_IN10 = PA5 
#define TEMP1_PORT            GPIOA
#define TEMP1_PIN             GPIO_PIN_5
// Temp 2 = ADC1_IN9 = PA4
#define TEMP2_PORT            GPIOA
#define TEMP2_PIN             GPIO_PIN_4
// Temp 3 = ADC1_IN7 = PA2
#define TEMP3_PORT            GPIOA
#define TEMP3_PIN             GPIO_PIN_2
// Temp 4 = ADC1_IN11 = PA6
#define TEMP4_PORT            GPIOA
#define TEMP4_PIN             GPIO_PIN_6
// Temp 5 = OPAMP1_VINP / ADC1_IN5 = PA0
#define TEMP5_PORT            GPIOA
#define TEMP5_PIN             GPIO_PIN_0
//================================================================================

// Pin configuration for BQ CAN 
//================================================================================
#define CAN_PORT            GPIOB
#define CAN_RX              GPIO_PIN_8
#define CAN_TX              GPIO_PIN_9
//================================================================================

// USART pins for PSOM
#define USART_PORT GPIOA
#define USART_TX_PIN GPIO_PIN_9
#define USART_RX_PIN GPIO_PIN_10
