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
#include "debugIO.h"
#include "balancing.h"


// get largest reading from array 
// THE BPS LEADER WILL DO THIS IN FINAL IMPLEMENTAION 
cell_num_t get_highest_cell(uint16_t* val_arr){
  cell_num_t highest_cell = 0;
  uint16_t highest_reading = val_arr[0];
  for(uint8_t i = 0; i < BQ_CELL_COUNT; i++){
    if(val_arr[i] > highest_reading){
      highest_cell = i;
      highest_reading = val_arr[i];
    }
  }
  return highest_cell;
}

// tell bq chip which cell to balance
BQ76920_Status_t balance_cell(cell_num_t cellnum){
  uint8_t bal_msg = 0x0;
  // check for valid cell
  // bal_msg only nonzero if valid cell passed.
  if(cellnum < BAL_OFF && cellnum >= CELL_1)
    bal_msg = (1 << cellnum);

  // write msg to cell register
  if(bq76920_Write(CELLBAL1,(bal_msg),BALANCE_DELAY) != BQ_OK)
    return BQ_ERR;

  // if cell to turn off was invalid, return err
  // turns of balancing if entry invalid 
  if(cellnum > BAL_OFF)
    return BQ_ERR;
  else return BQ_OK;
}