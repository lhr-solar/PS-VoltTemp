#pragma once
#include "CAN.h"
#include "tasks.h"
#include "pinConfig.h"

can_status_t canbus_init();

can_status_t canbus_send(CAN_TxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks);

can_status_t canbus_receive(uint16_t id, CAN_RxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks);

uint32_t canbus_getError();
