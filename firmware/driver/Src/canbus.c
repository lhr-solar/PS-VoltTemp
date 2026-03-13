#include "canbus.h"

CAN_HandleTypeDef* voltTempCAN;

can_status_t canbus_init(){

    voltTempCAN = hcan1;

    /* CAN1 Init Struct */
    // Baud rate is 250 kbit/s
    voltTempCAN->Init.Prescaler = 20;
    voltTempCAN->Init.SyncJumpWidth = CAN_SJW_1TQ;
    voltTempCAN->Init.TimeSeg1 = CAN_BS1_13TQ;
    voltTempCAN->Init.TimeSeg2 = CAN_BS2_2TQ;
    voltTempCAN->Init.Mode = CAN_MODE_NORMAL; 
    voltTempCAN->Init.TimeTriggeredMode = DISABLE;
    voltTempCAN->Init.AutoBusOff = DISABLE;
    voltTempCAN->Init.AutoWakeUp = DISABLE;
    voltTempCAN->Init.AutoRetransmission = DISABLE;
    voltTempCAN->Init.ReceiveFifoLocked = DISABLE;

    // If TransmitFifoPriority is disabled, the hardware selects the mailbox based on the message ID priority. 
    // If enabled, the hardware uses a FIFO mechanism to select the mailbox based on the order of transmission requests.
    voltTempCAN->Init.TransmitFifoPriority = ENABLE;

    /* Create CAN filter */
    /* For production, reject all incoming IDs */
    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterActivation = DISABLE;

    /* Initialize CAN1 */
    if (can_init(voltTempCAN, &sFilterConfig) != CAN_OK) {
        return CAN_ERR;
    }
    if((can_start(voltTempCAN) != CAN_OK)){
        return CAN_ERR;
    }

    return CAN_OK;
}

can_status_t canbus_send(CAN_TxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks){

    return can_send(voltTempCAN, header, data, delay_ticks);
}

can_status_t canbus_receive(uint16_t id, CAN_RxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks){
  
  return can_recv(voltTempCAN, id, header, data, delay_ticks);

}


void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)   {
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = CAN_RX | CAN_TX;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(CAN_PORT, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  }
}