#pragma once
#include "common.h"

//--------------------------------------------------------------------------------
// Battery Pack layout
#ifndef NUM_BATTERY_MODULES
#define NUM_BATTERY_MODULES                 32      // Number of battery modules
#endif /* NUM_BATTERY_MODULES */

#ifndef NUM_TOTAL_VOLTTEMP
#define NUM_TOTAL_VOLTTEMP                  8      // Total number of VoltTemp Boards
#endif /* NUM_TOTAL_VOLTTEMP */

#ifndef NUM_VOLTAGES_PER_VOLTTEMP
#define NUM_VOLTAGES_PER_VOLTTEMP           4       // Number of voltages that a volttemp reads
#endif /* NUM_VOLTAGES_PER_VOLTTEMP */

#ifndef NUM_THERMISTORS_PER_VOLTTEMP
#define NUM_THERMISTORS_PER_VOLTTEMP        4       // Number of thermistors that a volttemp reads
#endif /* NUM_THERMISTORS_PER_VOLTTEMP */

//--------------------------------------------------------------------------------