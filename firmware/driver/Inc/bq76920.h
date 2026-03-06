/**
 ******************************************************************************
 * @file    bq76920.h
 * @author  LHRs BPS - Clark Rucker
 * @brief   Contains register maps for the bq76920 chip.
 * @date    10/11/2025
 ******************************************************************************
 */

#pragma once

#include <stm32xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <stdint.h>

// Task based I2C things

// Semaphore and mutex def.
extern SemaphoreHandle_t I2C_semaphore;
extern StaticSemaphore_t I2C_semaphore_buffer_pool;

extern SemaphoreHandle_t I2C_mutex;
extern StaticSemaphore_t I2C_mutex_buffer_pool;

// Device status
typedef enum
{
  BQ_OK,
  BQ_ERR,
} BQ76920_Status_t;

// Commands

// sys commands
typedef enum
{
  // stat
  CC_READY,
  DEVICE_XREADY,
  OVRD_ALERT,
  UV,
  OV,
  SCD,
  OCD,
  // ctrl1
  LOAD_PRESENT,
  ADC_EN,
  TEMP_SEL,
  SHUT_A,
  SHUT_B,
  // ctrl2
  DELAY_DIS,
  CC_EN,
  CC_ONESHOT,
  DSG_ON,
  CHG_ON
} SysCommands_t;

// protect commands
typedef enum
{
  // protect1
  RSNS,
  SCD_D1,
  SCD_D0,
  SCD_T2,
  SCD_T1,
  SCD_T0,
  // protect2
  OCD_D2,
  OCD_D1,
  OCD_D0,
  OCD_T3,
  OCD_T2,
  OCD_T1,
  OCD_T0,
  // protect3
  UV_D1,
  UV_D0,
  OV_D1,
  OV_D0,
  // OV_Trip
  OV_T7,
  OV_T6,
  OV_T5,
  OV_T4,
  OV_T3,
  OV_T2,
  OV_T1,
  OV_T0,
  // UV_Trip
  UV_T7,
  UV_T6,
  UV_T5,
  UV_T4,
  UV_T3,
  UV_T2,
  UV_T1,
  UV_T0
} ProtectCommands_t;


// Init things

// Initialization function
void Init_BQ76920();
// gets ADC info from chip.
void get_ADC_Info();


// Reading & Writing functions
#define HIGH 0x1
#define LOW 0x0
#define BQ_READ_ERR 0xff
#define CRC_MSB 0x80
#define CRC_SMBUS 0x07
#define CRC_MSG_LENGTH 3
#define CRC_INIT_VAL 0
// returns data from the bms,
// input is the address.

/**
 * @brief  Reads from a BQ76920 Register.
 * @param  Mem Address on BMS chip
 * @param  Pointer to where data will be stored
 * @return OK If successful, ERR otherwise
 */
BQ76920_Status_t bq76920_Read_1_Reg(uint16_t Mem_Address, uint8_t *read_Data, TickType_t delay_ticks);

/**
 * @brief  Returns combined data from two mem addresses, many require a HI & LO
 * @param  First mem address
 * @param  Second mem address
 * @param  Pointer to where data will be stored
 * @return OK If successful, ERR otherwise
 */
BQ76920_Status_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2, uint16_t *data, TickType_t delay_ticks);

/**
 * @brief  Read from Sys registers, abstracts which register out to only require command.
 * @param  Command to read status of
 * @param  delay for read mutex acquisition
 * @return val of register if successfull, 0xff otherwise
 */
uint8_t sys_Read(SysCommands_t Command, TickType_t delay_ticks);

/**
 * @brief  Write to Sys registers, abstracts which register out to only require command.
 * @param  Command to write to
 * @param  State to set command to
 * @param  delay for write mutex acquisition
 */
void sys_Write(SysCommands_t Command, uint8_t State, TickType_t delay_ticks);

/**
 * @brief  Read from Protect registers, abstracts which register out to only require command.
 * @param  Command to read status of
 * @param  delay for read mutex acquisition
 * @return val of register if successfull, 0xff otherwise
 */
uint8_t protect_Read(ProtectCommands_t Command, TickType_t delay_ticks);

/**
 * @brief  Write to Protect registers, abstracts which register out to only require command.
 * @param  Command to write to
 * @param  State to set command to
 * @param  delay for write mutex acquisition
 */
void protect_Write(ProtectCommands_t Command, uint8_t State, TickType_t delay_ticks);


/**
 * @brief  Acquire the voltage of one cell.
 * @param  Which cell to read
 * @param  delay for write mutex acquisition
 * @return Reading from the cell
 */
uint32_t get_Voltage_Cell(uint16_t cell, TickType_t delay_ticks);

/**
 * @brief  Acquire the voltage of all cells.
 * @param  Pointer to array that holds cell readings
 * @param  delay for write mutex acquisition
 * @return OK if successful, ERR otherwise
 */
BQ76920_Status_t get_Voltage_All(uint32_t *voltage_array, TickType_t delay_ticks);

