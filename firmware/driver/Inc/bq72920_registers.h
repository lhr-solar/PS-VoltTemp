// Registers for the BQ76920

#pragma once
// I2C thingies 
//======================
#define DEV_ADD   0x08
#define MEM_SIZE  0x01
#define DATA_SIZE 0x01
#define DATA_SIZE_DOUBLE 0x02
#define TIMEOUT   0x64
//======================

// System Status
//======================
#define SYS_STAT  0x00
//======================


// Cell Balancing
//======================
#define CELLBAL1  0x01

//not on bq72920
#define CELLBAL2  0x02
#define CELLBAL3  0x03
//======================


// Sys Control
//======================
#define SYS_CTRL1 0x04
#define SYS_CTRL2 0x05
//======================

// Protection
//======================
#define PROTECT1  0x06
#define PROTECT2  0x07
#define PROTECT3  0x08
#define OV_TRIP   0x09
#define UV_TRIP   0x0A
#define CC_CFG    0x0B
//======================


// Cell Addresses
//======================
#define VC1_HI    0x0C
#define VC1_LO    0x0D
#define VC1       0x0C0D

#define VC2_HI    0x0E
#define VC2_LO    0x0F
#define VC2       0x0E0F

#define VC3_HI    0x10
#define VC3_LO    0x11
#define VC3       0x1011

#define VC4_HI    0x12
#define VC4_LO    0x13
#define VC4       0x1213

#define VC5_HI    0x14
#define VC5_LO    0x15
#define VC5       0x1415

#define VC6_HI    0x16
#define VC6_LO    0x17
#define VC6       0x1617

// Entire module voltage
#define BAT_HI    0x2A
#define BAT_LO    0x2B
#define BAT       0x2A2B
//======================


// Thermistors
// We do not use :(
//======================
#define TS1_HI    0x2C
#define TS1_LO    0x2D

// Not part of 76920
#define TS2_HI    0x2E
#define TS2_LO    0x2F
#define TS3_HI    0x30
#define TS3_LO    0x31
//======================


// Coulomb Counter
//======================
#define CC_HI     0x32
#define CC_LO     0x33
//======================


// ADC stuffs
// Factory presets
//======================
#define ADCGAIN1  0x50
#define ADCOFFSET 0x51
#define ADCGAIN2  0x59
//======================

#define END_OF_REGS 0x60

