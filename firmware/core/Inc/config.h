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

// VoltTemp configurations
// Each VoltTemp configuration defines which battery modules (0 indexed) are connected to that VoltTemp board
// (VoltTemp ID, {Battery Modules connected}, CAN ID)
#define VOLTTEMP_CONFIGS \
    VOLTTEMP_CONFIG(VOLTTEMP_0, {0, 1, 2, 3}, 0x01) \
    VOLTTEMP_CONFIG(VOLTTEMP_1, {4, 5, 6, 7}, 0x02) \
    VOLTTEMP_CONFIG(VOLTTEMP_2, {8, 9, 10, 11}, 0x03) \
    VOLTTEMP_CONFIG(VOLTTEMP_3, {12, 13, 14, 15}, 0x04) \
    VOLTTEMP_CONFIG(VOLTTEMP_4, {16, 17, 18, 19}, 0x05) \
    VOLTTEMP_CONFIG(VOLTTEMP_5, {20, 21, 22, 23}, 0x06) \
    VOLTTEMP_CONFIG(VOLTTEMP_6, {24, 25, 26, 27}, 0x07) \
    VOLTTEMP_CONFIG(VOLTTEMP_7, {28, 29, 30, 31}, 0x08) 
    