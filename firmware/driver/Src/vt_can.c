#include "tasks.h"
#include "bq76920.h"
#include <stm32xx_hal.h>
#include <bq76920.h>
#include <bq72920_registers.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "printf.h"
#include "UART.h"
#include <BPSCAN_can_msgs.h>
#include <volttemp.h>
#include <CAN.h>

#ifdef VOLTTEMP_0
// No pins set
#elif defined(VOLTTEMP_1)
  #define CURRENT_VT vt1
  #define VOLTTEMP_MSG_ID 0x3
#elif defined(VOLTTEMP_2)
  #define CURRENT_VT vt2
  #define 
#endif


void vt_CAN_start(){
  if (can_start(hcan1) != CAN_OK)
    Error_Handler();
}

static void vt_fill_CAN_struct(CAN_struct_t* CAN_struct, uint8_t tap_index){
    CAN_struct->CURRENT_VT.BPS_Tap_ID = tap_index;
    CAN_struct->CURRENT_VT.BPS_VoltTemp_BQ_Fault = BQ_STATUS;
    CAN_struct->CURRENT_VT.BPS_Temperature_Tap_Fault = 0;
    CAN_struct->CURRENT_VT.BPS_Voltage_Tap_Data = cell_readings[tap_index];
    CAN_struct->CURRENT_VT.BPS_Temperature_Tap_Data = 0;
    CAN_struct->CURRENT_VT.BPS_Temperature_Tap_RawV = 0;
}

static CAN_TxHeaderTypeDef tx_header = {0};
static uint8_t tx_data[VOLTTEMP_MSG_DLC] = {0};

VoltTempStatus_t vt_assemble_can_msg(CAN_struct_t* CAN_struct, CAN_TxHeaderTypeDef* TX_header, uint8_t* TX_data){
  // Generate the header
  TX_header->StdId = VOLTTEMP_MSG_ID;
  TX_header->RTR = CAN_RTR_DATA;
  TX_header->IDE = CAN_ID_STD;
  TX_header->DLC = VOLTTEMP_MSG_DLC;
  TX_header->TransmitGlobalTime = DISABLE;

  // Generate the data array
  TX_data[0] = (uint8_t) CAN_struct->CURRENT_VT.BPS_Tap_ID;
  TX_data[1] = (uint8_t) CAN_struct->CURRENT_VT.BPS_VoltTemp_BQ_Fault;
  TX_data[2] = (uint8_t) CAN_struct->CURRENT_VT.BPS_Temperature_Tap_Fault;
  TX_data[3] = (uint8_t) CAN_struct->CURRENT_VT.BPS_Voltage_Tap_Data;
  TX_data[4] = (uint8_t) CAN_struct->CURRENT_VT.BPS_Temperature_Tap_Data;
  TX_data[5] = (uint8_t) CAN_struct->CURRENT_VT.BPS_Temperature_Tap_RawV;
  TX_data[6] = (uint8_t) 1;
  TX_data[7] = (uint8_t) 1;
  return VOLTTEMP_OK;
}


VoltTempStatus_t vt_send_can_msg(CAN_struct_t* CAN_struct, TickType_t ticksToWait){
  // Send data from each tap
  for(uint8_t tap_index = 0; tap_index < VT_TAP_COUNT; tap_index++){
    vt_fill_CAN_struct(CAN_struct, tap_index);
    vt_assemble_can_msg(CAN_struct, &tx_header, tx_data);

    if (can_send(hcan1, &tx_header, tx_data, ticksToWait) != CAN_OK) {
      return VOLTTEMP_CAN_SEND_FAIL;
    }
  }
  return VOLTTEMP_OK;
}

