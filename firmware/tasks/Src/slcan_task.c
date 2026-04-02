#include "tasks.h"
#include <stm32xx_hal.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "pinConfig.h"
#include "inits.h"
#include "printf.h"
#include <string.h>
#include "queue_ex.h"
#include "CAN.h"
#include "canbus.h"
#include "debugIO.h"

#define CAN_TX_TELEMETRY_QUEUE_SIZE    32

static StaticQueue_t canTxTelemetryQueueBuffer;
static uint8_t canTxTelemetryQueueStorage[CAN_TX_TELEMETRY_QUEUE_SIZE * sizeof(can_tx_payload_t)];
static QueueHandle_t canTxTelemetryQueue;

void can_tx_print_slcan(const can_tx_payload_t *payload)
{

    uint32_t id  = payload->header.StdId;
    uint8_t  len = (payload->header.DLC);

    /* SLCAN supports max 8 bytes */
    if (len > 8)
        len = 8;

    if (payload->header.IDE == CAN_ID_STD)
    {
        /* tIII DLC DATA... */
        printf("t%03lX%1X", id & 0x7FF, len);
    }
    else
    {
        /* TIIIIIIII DLC DATA... */
        printf("T%08lX%1X", id & 0x1FFFFFFF, len);
    }

    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02X", payload->data[i]);
    }

    printf("\r\n");
}

void can_tx_callback_hook(CAN_HandleTypeDef* hcan, const can_tx_payload_t* payload) {
#if (CAN_USB_MIRROR_ENABLED == 1)
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if(canTxTelemetryQueue != NULL){
        xQueueSendCircularBufferFromISR(
            canTxTelemetryQueue,
            payload,
            &higherPriorityTaskWoken,
            sizeof(can_tx_payload_t)
        );

    }
    // don't yield at the end of this since the rest of the ISR needs to run
#endif /*(CAN_USB_MIRROR_ENABLED == 1)*/

}


void task_printSlcan(void *pvParameters){
    
    // canbus MUST be initialized by now
    
    can_tx_payload_t payload;

    canTxTelemetryQueue = xQueueCreateStatic(
        CAN_TX_TELEMETRY_QUEUE_SIZE,
        sizeof(can_tx_payload_t),
        canTxTelemetryQueueStorage,
        &canTxTelemetryQueueBuffer
    );

    while(1){
        // forward all transmitted can messages to USB
        if (xQueueReceive(canTxTelemetryQueue, &payload, portMAX_DELAY) == pdTRUE){
            can_tx_print_slcan(&payload);

            taskYIELD();
        }
        
        // Logic analzyer toggle to profile how often the thread runs
        debugIO_toggle(logic_analyzer_ch4);
    }

}