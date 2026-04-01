#include "tasks.h"
#include "bq76920.h"
#include <stm32xx_hal.h>
#include <bq72920_registers.h>
#include "leds.h"
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "printf.h"
#include "UART.h"
#include "volttemp.h"
#include <string.h>
#include "canbus.h"
#include "debugIO.h"

#define INVALID_VOLTAGE 0xFFFF

typedef struct {
    uint8_t   BPS_Tap_idx;
    uint16_t  BPS_Voltage_Tap_Data;
    uint8_t   BPS_VoltTemp_BQ_Fault;
} bps_voltage_arr_t;

// number of elements in the cell readings array, add 1 for the total stack voltage
#define CELL_READINGS_ARR_SIZE (NUM_VOLTAGES_PER_VOLTTEMP + 1)

// Stores the cell readings in mV, add 1 for the total stack voltage
uint16_t cell_readings[CELL_READINGS_ARR_SIZE];

#define VOLTAGE_PRINT_DEBUG_PERIOD_MS 10000
#define VOLTAGE_PRINT_DEBUG_COUNT (VOLTAGE_PRINT_DEBUG_PERIOD_MS / VOLTTEMP_THREAD_DELAY_MS)

#define BQ_HEARTBEAT_LED_PERIOD_MS 1000
#define BQ_HEARTBEAT_LED_TRIGGER_COUNT (BQ_HEARTBEAT_LED_PERIOD_MS / VOLTTEMP_THREAD_DELAY_MS)

 
static uint8_t getCellRegister(uint8_t index, uint16_t *cellRegister){

  if(index >= CELL_READINGS_ARR_SIZE || cellRegister == NULL){
    return 0;
  }
  
  switch(index){
    case 0:
      *cellRegister = VC1;
      break;
    case 1:
      *cellRegister = VC2;
      break;
    case 2:
      *cellRegister = VC3;
      break;
    case 3:
      *cellRegister = VC5;
      break;
    case 4:
      *cellRegister = BAT;
      break;
    default:
      return 0;
  }  
  return 1;
} 

static void initVoltageMsgHeader(CAN_TxHeaderTypeDef *voltageMsgHeader){
  voltageMsgHeader->StdId = CAN_ID_VOLTAGE_MSG;
  voltageMsgHeader->RTR = CAN_RTR_DATA;
  voltageMsgHeader->IDE = CAN_ID_STD;
  voltageMsgHeader->DLC = CAN_ID_VOLTAGE_MSG_DLC;
  voltageMsgHeader->TransmitGlobalTime = DISABLE;
}

static void packVoltageMessage(bps_voltage_arr_t msg, uint8_t msgArr[8]){
  // 0th byte is the tap ID
  msgArr[0] = (msg.BPS_Tap_idx);

  // 2nd and 3rd(msb) bytes are the voltage data
  memcpy(&msgArr[1], &(msg.BPS_Voltage_Tap_Data), sizeof(uint16_t));

  // 4th byte is the BQ fault
  msgArr[3] =  (msg.BPS_VoltTemp_BQ_Fault);
}

void task_ReadVoltage(void *pvParameters)
{

  // Init I2C, acquire BMS semaphore / mutex
  mx_i2c_init();
  Init_BQ76920();

  // Acquire factory ADC settings
  get_ADC_Info();

  uint16_t printDebugCounter = 0;

  uint16_t cellRegister = 0;
  uint16_t cellVoltageStorage = 0;

  bps_voltage_arr_t voltageMsg = {0};

  CAN_TxHeaderTypeDef voltageMsgHeader;
  initVoltageMsgHeader(&voltageMsgHeader);
  uint8_t voltageMsgData[8];

  uint8_t bqHeartbeatLedCounter = 0;

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1)
  {
    bqHeartbeatLedCounter++;
    // update the voltage for each cell, and send it on CAN
     for(uint8_t i = 0; i < CELL_READINGS_ARR_SIZE; i++){

        // hardcoded lookup for cell_reading index to BQ register, if mappings change then this function needs to change
        uint8_t status = getCellRegister(i, &cellRegister);

        // if the lookup was succesful
        if(status == 1){
          BQ76920_Status_t cellReadStatus = update_Cell_Voltage(cellRegister, &cellVoltageStorage, BQ_TIMEOUT_TICKS);
          if(cellReadStatus == BQ_OK){

            // update cell readings array if we succesfully read
            cell_readings[i] = cellVoltageStorage;

          }
          set_led(BQ_FAULT, cellReadStatus == BQ_OK ? OFF : ON);

          // for triggering the BQ heartbeat led
          if(bqHeartbeatLedCounter > BQ_HEARTBEAT_LED_TRIGGER_COUNT && cellReadStatus == BQ_OK){
            toggle_led(BQ_HEARTBEAT);
            bqHeartbeatLedCounter = 0;
          }

          // no need to send CAN message for total module voltage
          if(cellRegister != BAT){

            voltageMsg.BPS_Tap_idx = tapIdxArr[(i)];

            // 1 indicates a BQ fault (likely a timeout error)
            voltageMsg.BPS_VoltTemp_BQ_Fault = (cellReadStatus == BQ_OK) ? 0 : 1;

            // send the max 16 bit number if BQ fault
            voltageMsg.BPS_Voltage_Tap_Data = (cellReadStatus == BQ_OK) ? cell_readings[i] : (INVALID_VOLTAGE);

            // pack the voltageMsg struct into bytes to send over CAN
            packVoltageMessage(voltageMsg, voltageMsgData);
            
            canbus_send(&voltageMsgHeader, voltageMsgData, BQ_TIMEOUT_TICKS);
          }
        }
        else{
          set_led(BQ_FAULT, ON);
        }
     }

    if(printDebugCounter >= VOLTAGE_PRINT_DEBUG_COUNT){
      printf("-------------------------------------------------------------\r\n");
      printf("Voltage Readings:\r\n");
      printf("Cell 1: %u  \r\n",cell_readings[0]);
      printf("Cell 2: %u  \r\n",cell_readings[1]);
      printf("Cell 3: %u  \r\n",cell_readings[2]);
      printf("Cell 4: %u  \r\n",cell_readings[3]);
      printf("Total : %u  \r\n",cell_readings[4]);
      printf("Voltage can Errors: %lu\r\n ", canbus_getError());
      printf("-------------------------------------------------------------\r\n");
      printDebugCounter = 0;
    }

    printDebugCounter++;

    // Logic analzyer toggle to profile how often the thread runs
    debugIO_toggle(logic_analyzer_ch1);

    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(VOLTTEMP_THREAD_DELAY_MS));
  }
}
