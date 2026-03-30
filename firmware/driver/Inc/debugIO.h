#pragma once

#include "pinConfig.h"
#include "config.h"

// if you want to add new channels, you need to update the init, set, and toggle functions
typedef enum{
    logic_analyzer_ch1,
    logic_analyzer_ch2,
    logic_analyzer_ch4,
}debug_io_t;

/**
 * @brief Initializes Logic analzyer IO defined in pinDefs.h
 * *
 * @param none
 * @return none
 */
void debugIO_init();

/**
 * @brief Sets a Logic analzyer pin
 * *
 * @param pin enum of type debug_io_t that defines what pin is to be set
 * @param pin_state enum of type State that is the new state of the pin
 * @return none
 */
void debugIO_set(debug_io_t pin,  State pin_state);

/**
 * @brief Toggles a Logic analzyer pin
 * *
 * @param pin enum of type debug_io_t that defines what pin is to be set
 * @return none
 */
void debugIO_toggle(debug_io_t pin);