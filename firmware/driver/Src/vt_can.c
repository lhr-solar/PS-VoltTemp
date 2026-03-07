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
#endif


void vt_CAN_start(){
  if (can_start(hcan1) != CAN_OK)
    Error_Handler();
}

static void vt_fill_CAN_struct(CAN_struct_t* CAN_struct){
  for(uint8_t tap_index = 0; tap_index < VT_TAP_COUNT; tap_index++){
    CAN_struct->CURRENT_VT.BPS_Voltage_Tap_Data = cell_readings[tap_index];

  }
}

static CAN_TxHeaderTypeDef tx_header = {0};
static uint8_t tx_data[VOLTTEMP_MSG_DLC] = {0};

VoltTempStatus_t vt_assemble_can_msg(CAN_struct_t* CAN_struct, CAN_TxHeaderTypeDef* TX_header, uint8_t* TX_data){

}


VoltTempStatus_t vt_send_can_msg(CAN_struct_t* CAN_struct){
  vt_fill_CAN_struct(CAN_struct);
  vt_assemble_can_msg(CAN_struct, &tx_header, tx_data);

}

