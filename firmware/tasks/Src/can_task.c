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
#include "canbus.h"

CAN_struct_t Can_struct;

static void initVoltTempCanHeader(CAN_TxHeaderTypeDef *tx_header){
  tx_header->StdId = VOLTTEMP_CAN_MSG_IG;
  tx_header->RTR = CAN_RTR_DATA;
  tx_header->IDE = CAN_ID_STD;
  tx_header->DLC = VOLTTEMP_CAN_MSG_DLC;
  tx_header->TransmitGlobalTime = DISABLE;
}

static void packVoltTempCanMsg(CAN_struct_t *Can_struct, uint8_t *tx_data){

}

void task_SendMessage(void *pvParameters)
{


  CAN_TxHeaderTypeDef tx_header = {0};

  initVoltTempCanHeader(&tx_header);

  uint8_t tx_data[VOLTTEMP_MSG_DLC] = {0};

  while (1)
  {
    // vt_send_can_msg(&Can_struct, CAN_DELAY);

    packVoltTempCanMsg(&Can_struct, tx_data);

    canbus_send(&tx_header, tx_data, portMAX_DELAY);

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);

    vTaskDelay(pdMS_TO_TICKS(CAN_SEND_TASK_PERIOD_MS));
  }
}


