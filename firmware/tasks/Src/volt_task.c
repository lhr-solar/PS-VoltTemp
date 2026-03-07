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

uint16_t cell_readings[5];

void task_ReadVoltage(void *pvParameters)
{
  // Acquire factory ADC settings
  get_ADC_Info();

  while (1)
  {

    get_Voltage_All(cell_readings, BQ_DELAY);
    
    //#define BQ_PRINT_READINGS
    #ifdef BQ_PRINT_READINGS
    printf("\033[H");
    printf("Voltage Readings:\r\n");
    printf("Cell 1: %u  \r\n",cell_readings[0]);
    printf("Cell 2: %u  \r\n",cell_readings[1]);
    printf("Cell 3: %u  \r\n",cell_readings[2]);
    printf("Cell 4: %u  \r\n",cell_readings[3]);
    printf("Total : %u  \r\n",cell_readings[4]);
    #endif

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    vTaskDelay(VOLTTEMP_DELAY);
  }
}
