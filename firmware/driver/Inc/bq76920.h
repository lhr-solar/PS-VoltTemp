/** 
  ******************************************************************************
  * @file    bq76920.h
  * @author  UTSVT BPS - Clark Rucker
  * @brief   Contains register maps for the bq76920 chip.
  * @date    10/11/2025
  ******************************************************************************
  */
#include <stm32l4xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"
  
#ifndef __BQ76920_
#define __BQ76920_


// Task based I2C things
//========================================
typedef struct {
    I2C_HandleTypeDef* hi2c;        // STM32 HAL I2C handle
    uint16_t dev_addr;              // HAL convention: 7-bit address << 1
    SemaphoreHandle_t i2c_complete; // Semaphore to signal I2C transaction complete
} BQ76920_HandleTypeDef;

// I2C Response Timeout
#ifndef BQ76920_I2C_TIMEOUT
#define BQ76920_I2C_TIMEOUT 100u // 100ms default
#endif

// I2C Operation Types
typedef enum {
    BQ76920_OP_WRITE,
    BQ76920_OP_READ
} BQ76920_I2C_OP;

// I2C Message Struct
typedef struct {
    BQ76920_HandleTypeDef* chip;    // Chip to send to
    BQ76920_I2C_OP operation;       // Read/Write operation
    uint8_t reg_addr;               // Register address
    uint8_t write_data;             // Data to write (only used for write operations)
    uint8_t* read_data;             // Pointer for storing read data (only used for read operations)
    uint8_t semaphore_index;        // Index of semaphore handle in pool
} BQ76920_I2C_Message;

#ifndef BQ76920_QUEUE_LENGTH
#define BQ76920_QUEUE_LENGTH 10                             // Message queue length
#endif

#define BQ76920_QUEUE_ITEM_SIZE sizeof(BQ76920_I2C_Message) // Size of queue item (message)
#define BQ76920_SEMAPHORE_POOL_SIZE BQ76920_QUEUE_LENGTH    // Number of caller semaphores (max concurrent I2C requests)

// Device status
typedef enum {
    BQ76920_OK,
    BQ76920_ERR,
} BQ76920_Status;


// Commands.
//========================================
// sys commands
typedef enum{
  //stat
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
} SysCommands;

// protect commands
typedef enum{
  //protect1
  RSNS,
  SCD_D1,
  SCD_D0,
  SCD_T2,
  SCD_T1,
  SCD_T0,
  //protect2
  OCD_D2,
  OCD_D1,
  OCD_D0,
  OCD_T3,
  OCD_T2,
  OCD_T1,
  OCD_T0,
  //protect3
  UV_D1,
  UV_D0,
  OV_D1,
  OV_D0,
  //OV_Trip
  OV_T7,
  OV_T6,
  OV_T5,
  OV_T4,
  OV_T3,
  OV_T2,
  OV_T1,
  OV_T0,
  //UV_Trip
  UV_T7,
  UV_T6,
  UV_T5,
  UV_T4,
  UV_T3,
  UV_T2,
  UV_T1,
  UV_T0
} ProtectCommands;
//========================================


// Init things
//========================================
// Initialization function
void Init_BQ76920();
// gets ADC info from chip.
void get_ADC_Info();

typedef enum{ 
    VOLTTEMP_OK,
    VOLTTEMP_INIT_FAIL,
    VOLTTEMP_ADC_FAIL,
    VOLTTEMP_QUEUE_FULL,
    VOLTTEMP_CAN_SEND_FAIL
} VoltTempStatus;

//========================================

// Reading & Writing functions
//========================================
#define HIGH 0x1
#define LOW  0x0
// returns data from the bms,
// input is the address.
uint8_t bq76920_Read_1_Reg(uint16_t);
// returns combined data from two, input is both.
uint16_t bq76920_Read(uint16_t,uint16_t);
// Writes to bms, input is address.
void bq76920_Write(uint16_t, uint8_t);
// read or write to one bit of the bms


// Interact with Sys Registers
uint8_t sys_Read(SysCommands);
void sys_Write(SysCommands, uint8_t);

// Interact with Protect Registers
uint8_t protect_Read(ProtectCommands);
void protect_Write(ProtectCommands, uint8_t);


void cell_Bal_Tgl(uint8_t);
//========================================

// Voltage related functions.
//========================================
// returns voltage of 1 cell, input is cell.
uint32_t get_Voltage_1(uint16_t);
// populates array with all voltages.
void get_Voltage_All(uint32_t*);
//========================================


// I2C thingies 
//======================
#define DEV_ADD   0x08
#define MEM_SIZE  0x01
#define DATA_SIZE 0x01
#define TIMEOUT   0x64
//======================

// System Status
//======================
#define SYS_STAT  0x00
//======================


// Cell Balancing
//======================
#define CELLBAL1  0x01

//not on bq72920
#define CELLBAL2  0x02
#define CELLBAL3  0x03
//======================


// Sys Control
//======================
#define SYS_CTRL1 0x04
#define SYS_CTRL2 0x05
//======================

// Protection
//======================
#define PROTECT1  0x06
#define PROTECT2  0x07
#define PROTECT3  0x08
#define OV_TRIP   0x09
#define UV_TRIP   0x0A
#define CC_CFG    0x0B
//======================


// Cell Addresses
//======================
#define VC1_HI    0x0C
#define VC1_LO    0x0D
#define VC1       0x0C0D

#define VC2_HI    0x0E
#define VC2_LO    0x0F
#define VC2       0x0E0F

#define VC3_HI    0x10
#define VC3_LO    0x11
#define VC3       0x1011

#define VC4_HI    0x12
#define VC4_LO    0x13
#define VC4       0x1213

#define VC5_HI    0x14
#define VC5_LO    0x15
#define VC5       0x1415

#define VC6_HI    0x16
#define VC6_LO    0x17
#define VC6       0x1617

// Entire module voltage
#define BAT_HI    0x2A
#define BAT_LO    0x2B
#define BAT       0x2A2B
//======================


// Thermistors
// We do not use :(
//======================
#define TS1_HI    0x2C
#define TS1_LO    0x2D

// Not part of 76920
#define TS2_HI    0x2E
#define TS2_LO    0x2F
#define TS3_HI    0x30
#define TS3_LO    0x31
//======================


// Coulomb Counter
//======================
#define CC_HI     0x32
#define CC_LO     0x33
//======================


// ADC stuffs
// Factory presets
//======================
#define ADCGAIN1  0x50
#define ADCOFFSET 0x51
#define ADCGAIN2  0x59
//======================

#endif
