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
#include "volttemp.h"

// number of elements in the cell readings array, add 1 for the total stack voltage
#define CELL_READINGS_ARR_SIZE (NUM_VOLTAGES_PER_VOLTTEMP + 1)
// // Stores the cell readings in mV, add 1 for the total stack voltage
// uint16_t cell_readings[CELL_READINGS_ARR_SIZE];

uint16_t cell_readings[5];

BQ76920_Status_t BQ_STATUS = BQ_OK;

#define VOLTAGE_PRINT_DEBUG_PERIOD_MS 5000
#define VOLTAGE_PRINT_DEBUG_COUNT (VOLTAGE_PRINT_DEBUG_PERIOD_MS / VOLTTEMP_THREAD_DELAY_MS)
 
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
      break;
  }

  
  return 1;

} 

void task_ReadVoltage(void *pvParameters)
{
  // Acquire factory ADC settings
  get_ADC_Info();

  uint8_t printDebugCounter = 0;

  uint16_t cellRegister = 0;
  uint16_t cellVoltageStorage = 0;

  while (1)
  {
     for(uint8_t i = 0; i < CELL_READINGS_ARR_SIZE; i++){
        uint8_t status = getCellRegister(i, &cellRegister);
        if(status == 1){
          BQ76920_Status_t cellReadStatus = update_Cell_Voltage(cellRegister, &cellVoltageStorage, BQ_TIMEOUT_TICKS);
          if(cellReadStatus == BQ_OK){
            // update cell readings array if we succesfully read
            cell_readings[i] = cellVoltageStorage;
          } 

          // no need to send CAN message for top module voltage
          if(cellRegister != BAT && cellReadStatus == BQ_OK){
            // send CAN message if we succesfully read voltage
          }
        }
     }

    // if(get_Voltage_All(cell_readings, BQ_TIMEOUT_TICKS) != BQ_OK) {
    //   BQ_STATUS = BQ_ERR;
    // }

    if(printDebugCounter >= VOLTAGE_PRINT_DEBUG_COUNT){
      printf("Voltage Readings:\r\n");
      printf("Cell 1: %u  \r\n",cell_readings[0]);
      printf("Cell 2: %u  \r\n",cell_readings[1]);
      printf("Cell 3: %u  \r\n",cell_readings[2]);
      printf("Cell 4: %u  \r\n",cell_readings[3]);
      printf("Total : %u  \r\n",cell_readings[4]);
      printDebugCounter = 0;
    }

    printDebugCounter++;

    vTaskDelay(pdMS_TO_TICKS(VOLTTEMP_THREAD_DELAY_MS));
  }
}
