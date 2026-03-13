#include "stm32l4xx_hal.h"
#include "common.h"
#include "stdlib.h"
#include "ADC.h"
#include "temperature.h"
#include "config.h"
#include "canbus.h"
#include "printf.h"
#include "volttemp.h"
#include <string.h>

typedef struct {
    uint8_t BPS_Tap_idx;
    uint8_t BPS_Temperature_Tap_Fault;
    int32_t BPS_Temperature_Tap_Data;
    uint16_t BPS_Temperature_Tap_RawV;
} bps_temperature_arr_t;

#define TEMPERATURE_PRINTF_PERIOD_MS 20000
#define TEMPERATURE_PRINTF_COUNT  (TEMPERATURE_PRINTF_PERIOD_MS/TEMPERATURE_THREAD_PERIOD_MS)

static void initTemperatureMsgHeader(CAN_TxHeaderTypeDef *temperatureMsgHeader){
  temperatureMsgHeader->StdId = CAN_ID_TEMPERATURE_MSG;
  temperatureMsgHeader->RTR = CAN_RTR_DATA;
  temperatureMsgHeader->IDE = CAN_ID_STD;
  temperatureMsgHeader->DLC = CAN_ID_TEMPERATURE_MSG_DLC;
  temperatureMsgHeader->TransmitGlobalTime = DISABLE;
}

static void packTemperatureMessage(bps_temperature_arr_t msg, uint8_t msgArr[8]){
  msgArr[0] = ((msg.BPS_Tap_idx)) & (0x1F);
  msgArr[0] |= ((msg.BPS_Temperature_Tap_Fault & 0x07) << 5);
  memcpy(&msgArr[1], &(msg.BPS_Temperature_Tap_Data), sizeof(int32_t));
  memcpy(&msgArr[5], &(msg.BPS_Temperature_Tap_RawV), sizeof(uint16_t));
}

void task_temp_read(void *pvParameters){

  temp_status_t status;
  uint16_t printDebugCounter = 0;
  Temp_Init();

  TempMsg_t messages[NUM_THERMISTORS_PER_VOLTTEMP] = { 0 };

  CAN_TxHeaderTypeDef tempertaureMsgHeader;
  initTemperatureMsgHeader(&tempertaureMsgHeader);
  bps_temperature_arr_t temperatureMsg= {0};
  uint8_t temperatureMsgData[8] = {0};

  while(1){

    // start sequencing for all temperature channels
    status = Temp_StartAllADC(false);

    if(status == TEMP_ADC_START_FAIL){
      // balls
    }  
    else{
      
      // the delay we wait for should be shorter than the period of the can send task
      if (Temp_GetAllReadings(messages, pdMS_TO_TICKS(100)) != TEMP_OK) {
        // printf("Failed to get all readings\r\n");
        // Error_Handler();
      }

      if(printDebugCounter >= TEMPERATURE_PRINTF_COUNT){
        printf("-------------------------------------------------------------\r\n");
        for (thermistor_t i = TEMP1; i < NUM_THERMISTORS - 1; i++) {
            printf("TEMP %u: %ld mC\r\n", i + 1, messages[i].temperature);
        }
        printf("-------------------------------------------------------------\r\n");
        printDebugCounter = 0;
      }
    }
    printDebugCounter++;

    for(uint8_t i = TEMP1; i <  NUM_THERMISTORS - 1; i++){

      temperatureMsg.BPS_Tap_idx = tapIdxArr[(i)];
      temperatureMsg.BPS_Temperature_Tap_Fault = 0;
      temperatureMsg.BPS_Temperature_Tap_Data = messages[i].temperature;
      temperatureMsg.BPS_Temperature_Tap_RawV = messages[i].raw_voltage;

      packTemperatureMessage(temperatureMsg, temperatureMsgData);
      canbus_send(&tempertaureMsgHeader, temperatureMsgData, pdMS_TO_TICKS(100));
    }

    vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_THREAD_PERIOD_MS));

  }
}
