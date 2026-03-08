#include "CAN.h"

//Finish this when main is being developed.

typedef enum{ 
    VOLTTEMP_OK,
    VOLTTEMP_READ_FAIL,
    VOLTTEMP_INIT_FAIL,
    VOLTTEMP_ADC_FAIL,
    VOLTTEMP_QUEUE_FULL,
    VOLTTEMP_CAN_SEND_FAIL
} VoltTempStatus_t;



#define VT_TAP_COUNT 4
#define VOLTTEMP_MSG_DLC 7


VoltTempStatus_t vt_send_can_msg(CAN_struct_t* CAN_struct, TickType_t ticksToWait);

VoltTempStatus_t vt_assemble_can_msg(CAN_struct_t* CAN_struct, CAN_TxHeaderTypeDef* tx_header, uint8_t* tx_data);