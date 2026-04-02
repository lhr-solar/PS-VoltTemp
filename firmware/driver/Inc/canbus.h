#pragma once
#include "CAN.h"
#include "tasks.h"
#include "pinConfig.h"

/**
 * @brief Initializes the CAN peripheral.
 *
 * @param none 
 *
 * @return can_status_t Returns CAN_OK on success, CAN_ERR on failure.
 */
can_status_t canbus_init();

/**
 * @brief Sends a FDCAN message.
 *
 * Places a CAN message into the transmit mailbox if available, otherwise
 * queues it in the send queue for later transmission.
 *
 * @param header       Pointer to the CAN transmit header structure.
 * @param data         Array containing the data to send.
 * @param delay_ticks  Maximum delay to wait if queue is full (FreeRTOS ticks).
 *
 * @return can_status_t Returns CAN_OK if message was successfully sent or queued,
 *                      CAN_ERR on failure.
 */
can_status_t canbus_send(CAN_TxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks);

/**
 * @brief Receives a CAN message.
 *
 * Reads a message from the receive queue corresponding to the specified ID.
 * Supports blocking or non-blocking behavior depending on delay_ticks.
 *
 * @param id           CAN identifier of the message to receive.
 * @param header       Pointer to a CAN_RxHeaderTypeDef structure to store the received header.
 * @param data         Array to store the received data.
 * @param delay_ticks  Maximum delay to wait if queue is empty (FreeRTOS ticks).
 *
 * @return can_status_t Returns CAN_OK if a message was received,
 *                      CAN_EMPTY if the queue was empty,
 *                      CAN_ERR on failure or invalid ID.
 */
can_status_t canbus_receive(uint16_t id, CAN_RxHeaderTypeDef* header, uint8_t data[], TickType_t delay_ticks);

/**
 * @brief Returns the HAL CAN Error Status
 *
 * @param none          
 *
 * @return uint32_t containing which error bits were set (defined in stm32l4xx_hal_can.h)
 */
uint32_t canbus_getError();
