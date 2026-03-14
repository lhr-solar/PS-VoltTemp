#pragma once
// Configure the system clock for the STM32
void SystemClock_Config(void);

// Enable & Configure the I2C for BMS chip
void mx_i2c_init(void);
extern I2C_HandleTypeDef hi2c1;


// Enable UART for printf support
extern UART_HandleTypeDef* husart1;
void mx_uart_init(void);
void UART_Init(void);
