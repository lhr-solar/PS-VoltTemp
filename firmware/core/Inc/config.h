#pragma once
#include "common.h"

//--------------------------------------------------------------------------------
// Battery Pack layout

#ifndef NUM_VOLTAGES_PER_VOLTTEMP
#define NUM_VOLTAGES_PER_VOLTTEMP           4       // Number of voltages that a volttemp reads
#endif /* NUM_VOLTAGES_PER_VOLTTEMP */

#ifndef NUM_THERMISTORS_PER_VOLTTEMP
#define NUM_THERMISTORS_PER_VOLTTEMP        4       // Number of thermistors that a volttemp reads
#endif /* NUM_THERMISTORS_PER_VOLTTEMP */

//--------------------------------------------------------------------------------

// Voltage Bounds
#ifndef CELL_VOLTAGE_MV_UPPER_BOUND
#define CELL_VOLTAGE_MV_UPPER_BOUND        4500       // Max reasonable voltage we can expect out of a cell
#endif /* CELL_VOLTAGE_MV_UPPER_BOUND */

#ifndef CELL_VOLTAGE_MV_LOWER_BOUND
#define CELL_VOLTAGE_MV_LOWER_BOUND        2000       // Max reasonable voltage we can expect out of a cell
#endif /* CELL_VOLTAGE_MV_UPPER_BOUND */

//--------------------------------------------------------------------------------