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

#define VOLTTEMP_MSG_DLC 7

#ifdef VOLTTEMP_0
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_0
    #define CURRENT_VT              vt0
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_0
    // static uint8_t tapIdxArr[NUM_VOLTAGES_PER_VOLTTEMP] = {0, 1, 2, 3};
#elif defined(VOLTTEMP_1)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_1
    #define CURRENT_VT              vt1
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_1
#elif defined(VOLTTEMP_2)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_2
    #define CURRENT_VT              vt2
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_2
#elif defined(VOLTTEMP_3)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_3
    #define CURRENT_VT              vt3
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_3
#elif defined(VOLTTEMP_4)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_4
    #define CURRENT_VT              vt4
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_4
#elif defined(VOLTTEMP_5)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_5
    #define CURRENT_VT              vt5
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_5
#elif defined(VOLTTEMP_6)
    #define VOLTTEMP_CAN_MSG_IG     CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_6
    #define CURRENT_VT              vt6
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_6

#elif defined(VOLTTEMP_7)
    #define VOLTTEMP_CAN_MSG_IG CAN_ID_BPS_VOLTAGE_TEMPERATURE_ARR_7
    #define CURRENT_VT vt7
    #define VOLTTEMP_CAN_MSG_DLC    CAN_DLC_BPS_VOLTAGE_TEMPERATURE_ARR_7
#else
    #error "Invalid volttemp defined, must be within range of 0-7"
#endif


VoltTempStatus_t vt_send_can_msg(CAN_struct_t* CAN_struct, TickType_t ticksToWait);

VoltTempStatus_t vt_assemble_can_msg(CAN_struct_t* CAN_struct, CAN_TxHeaderTypeDef* tx_header, uint8_t* tx_data);