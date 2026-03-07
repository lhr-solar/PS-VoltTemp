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

CAN_struct_t Can_struct;


void task_SendMessage(void *pvParameters)
{
  while (1)
  {
    vt_send_can_msg(&Can_struct);

    vTaskDelay(VOLTTEMP_DELAY);
  }
}


