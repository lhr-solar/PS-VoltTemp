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

uint16_t cell_readings[5];
BQ76920_Status_t BQ_STATUS = BQ_OK;

#define VOLTAGE_PRINT_DEBUG_PERIOD_MS 5000
#define VOLTAGE_PRINT_DEBUG_COUNT (VOLTAGE_PRINT_DEBUG_PERIOD_MS / VOLTTEMP_THREAD_DELAY_MS)

void task_ReadVoltage(void *pvParameters)
{
  // Acquire factory ADC settings
  get_ADC_Info();

  uint8_t printDebugCounter = 0;

  while (1)
  {

    if(get_Voltage_All(cell_readings, BQ_DELAY) != BQ_OK) {
      BQ_STATUS = BQ_ERR;
    }

    if(printDebugCounter >= VOLTAGE_PRINT_DEBUG_COUNT){
      printf("Voltage Readings:\r\n");
      printf("Cell 1: %u  \r\n",cell_readings[0]);
      printf("Cell 2: %u  \r\n",cell_readings[1]);
      printf("Cell 3: %u  \r\n",cell_readings[2]);
      printf("Cell 4: %u  \r\n",cell_readings[3]);
      printf("Total : %u  \r\n",cell_readings[4]);
      printDebugCounter = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(VOLTTEMP_THREAD_DELAY_MS));
  }
}
