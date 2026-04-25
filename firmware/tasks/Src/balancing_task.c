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
#include <string.h>
#include "canbus.h"
#include "volttemp.h"
#include "debugIO.h"
#include "balancing.h"

#define BALANCING_PRINT_DEBUG_PERIOD_MS 30000
#define BALANCING_PRINT_DEBUG_COUNT (BALANCING_PRINT_DEBUG_PERIOD_MS / BALANCING_THREAD_DELAY_MS)

uint8_t bleh = 0; 

uint16_t balanceMsgID;
static void initBalanceMsgHeader(CAN_RxHeaderTypeDef *balanceMsgHeader){
  balanceMsgID = CAN_ID_BALANCE_MSG;
  balanceMsgHeader->RTR = CAN_RTR_DATA;
  balanceMsgHeader->IDE = CAN_ID_STD;
  balanceMsgHeader->DLC = CAN_ID_BALANCE_MSG_DLC;
}

cell_num_t cell_to_bal = BAL_OFF;
uint8_t segment_idx = 0;
void unpack_balance_data(uint8_t* balanceMsgData){
  // seg id is first 3 bits
  segment_idx = balanceMsgData[0] && (0x07);
  // cell to balance is last 5 bits
  cell_to_bal =  balanceMsgData[0] && (0xf8);
}

void task_balance(void *pvParameters)
{
  uint16_t printDebugCounter = 0;

  bleh = tapIdxArr[0]; //dumb

  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  CAN_RxHeaderTypeDef balanceMsgHeader;
  initBalanceMsgHeader(&balanceMsgHeader);
  uint8_t balanceMsgData[1];

  while (1)
  {
    
    canbus_receive(balanceMsgID, &balanceMsgHeader, balanceMsgData, BALANCING_THREAD_DELAY_MS);
    
    unpack_balance_data(balanceMsgData);

    // only send cell to balance if segment id is correct
    if(segment_idx == SEGMENT_ID)
      balance_cell(cell_to_bal);

    uint8_t cellball1_read;
    bq76920_Read_1_Reg(CELLBAL1,&cellball1_read,BALANCE_DELAY);

    if(printDebugCounter >= BALANCING_PRINT_DEBUG_COUNT){
      printf("----------------------\r\n");
      printf("seg idx: %u \r\n", segment_idx);
      printf("cell to bal (from can): %u \r\n", cell_to_bal); // num is 1 higher than idx
      printf("can reading: %u \r\n", balanceMsgData[0]);
      printf("----------------------\r\n");
      printDebugCounter = 0;
    } printDebugCounter++;

    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(BALANCING_THREAD_DELAY_MS));
  }
}
