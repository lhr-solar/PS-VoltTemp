#pragma once

#include <stm32xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
  CELL_1,
  CELL_2,
  CELL_3,
  CELL_4,
  BAL_OFF
} cell_num_t;


/**
 * @brief  Finds which entry of the voltage reading array is largest
 * @param  which array to check 
 * @return which cell is highest, send to balance cell
 */
cell_num_t get_highest_cell(uint16_t* val_arr);

/**
 * @brief  Tells the BQ chip which cell to balance
 * @param  Which cell to balance / turn off balancing
 * @return OK If successful, ERR otherwise
 */
BQ76920_Status_t balance_cell(cell_num_t cellnum);
