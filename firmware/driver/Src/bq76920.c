#include <stm32xx_hal.h>
#include <bq76920.h>
#include <bq72920_registers.h>
#include <common.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "inits.h"

// Semaphore and mutex decl.
SemaphoreHandle_t I2C_semaphore;
StaticSemaphore_t I2C_semaphore_buffer_pool;

SemaphoreHandle_t I2C_mutex;
StaticSemaphore_t I2C_mutex_buffer_pool;

// Reads & returns data from one register.
// Input is the register.
BQ76920_Status_t bq76920_Read_1_Reg(uint16_t Mem_Address, uint8_t *read_Data, TickType_t delay_ticks)
{
  // check for valid pointer
  if (read_Data == NULL)
    return BQ_ERR;

  HAL_StatusTypeDef rx_status;

  // acquire mutex
  if (xSemaphoreTake(I2C_mutex, delay_ticks) != pdTRUE)
    return BQ_ERR;

  // ask politely to read
  rx_status = HAL_I2C_Mem_Read_IT(&hi2c1, (DEV_ADD << 1),
                                  Mem_Address, I2C_MEMADD_SIZE_8BIT,
                                  read_Data, DATA_SIZE);

  // check if ok
  if (rx_status != HAL_OK)
  {
    xSemaphoreGive(I2C_mutex);
    return BQ_ERR;
  }

  // take mutex, if fail abort
  if (xSemaphoreTake(I2C_semaphore, delay_ticks) != pdTRUE)
  {
    xSemaphoreGive(I2C_mutex);
    HAL_I2C_Master_Abort_IT(&hi2c1, (DEV_ADD << 1));
    return BQ_ERR;
  }

  // give up mutex & return
  xSemaphoreGive(I2C_mutex);
  return BQ_OK;
}

// Reads one bit from a specific register
// Inputs are register and bit to read.
uint8_t bq76920_R_1_bit(uint8_t reg, uint8_t bit_dec, TickType_t delay_ticks)
{
  if (reg >= END_OF_REGS)
    return BQ_READ_ERR;
  uint8_t read_val;
  if (bq76920_Read_1_Reg(reg, &read_val, delay_ticks) != BQ_OK)
    return BQ_READ_ERR;
  return ((read_val >> (bit_dec)) & (0x1));
}

// Most reads require HI & LO registers,
// This function collects both and combines them.
// Inputs are both registers.
BQ76920_Status_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2, uint16_t *data, TickType_t delay_ticks)
{
  if (data == NULL)
    return BQ_ERR;
  uint8_t data_1;
  if (bq76920_Read_1_Reg(Mem_Add_1, &data_1, delay_ticks) != BQ_OK)
    return BQ_ERR;
  uint8_t data_2;
  if (bq76920_Read_1_Reg(Mem_Add_2, &data_2, delay_ticks) != BQ_OK)
    return BQ_ERR;
  // most significant are from reading 1.
  // shift left and then concatenate reading 2.
  *data = (data_1 << 8) | (data_2);
  return BQ_OK; // :-)
}

// CRC function for writing (From BQ76920 example drivers)
// General CRC-8 Formula: return = (Message[x] * x^8) % key
// Shifts message left 8 bits, divides by key, returns remainder.
static uint8_t CRC8(uint8_t *message, uint8_t len, uint8_t key)
{
  uint8_t index;
  uint8_t crc = CRC_INIT_VAL;
  while (len-- != 0)
  {
    for (index = CRC_MSB; index != 0; index /= 2)
    {
      if ((crc & CRC_MSB) != 0)
      {
        crc *= 2;
        crc ^= key;
      }
      else
        crc *= 2;

      if ((*message & index) != 0)
        crc ^= key;
    }
    message++;
  }
  return (crc);
}

static BQ76920_Status_t bq76920_Write(uint16_t Mem_Address, uint8_t new_data, TickType_t delay_ticks)
{
  HAL_StatusTypeDef tx_status;

  // acquire semaphore
  if (xSemaphoreTake(I2C_mutex, delay_ticks) != pdTRUE)
    return BQ_ERR;

  // perform the CRC things
  uint8_t crc_input[3];
  uint8_t write_data[3];

  crc_input[0] = (DEV_ADD << 1);
  crc_input[1] = Mem_Address;
  crc_input[2] = new_data;

  write_data[0] = new_data;
  write_data[1] = CRC8(crc_input, CRC_MSG_LENGTH, CRC_SMBUS);

  // politely ask to write
  tx_status = HAL_I2C_Mem_Write_IT(&hi2c1, (DEV_ADD << 1),
                                   Mem_Address, I2C_MEMADD_SIZE_8BIT,
                                   write_data, DATA_SIZE_DOUBLE);

  // check if ok
  if (tx_status != HAL_OK)
  {
    xSemaphoreGive(I2C_mutex);
    return BQ_ERR;
  }

  // take mutex, if fail abort
  if (xSemaphoreTake(I2C_semaphore, delay_ticks) != pdTRUE)
  {
    xSemaphoreGive(I2C_mutex);
    HAL_I2C_Master_Abort_IT(&hi2c1, (DEV_ADD << 1));
    return BQ_ERR;
  }

  // return mutex and status
  xSemaphoreGive(I2C_mutex);
  return BQ_OK;
}

// Writes to one bit in a specified register
// Inputs are register, bit in decimal, state
static BQ76920_Status_t bq76920_W_1_bit(uint8_t reg, uint8_t bit_dec, uint8_t state, TickType_t delay_ticks)
{
  uint8_t current;
  if (bq76920_Read_1_Reg(reg, &current, delay_ticks) != BQ_OK)
    return BQ_ERR;       // acquire current info
  uint8_t new = current; // defaults to rewriting what was read.

  // need different masks depending on whether you want to set or clear
  if (state == HIGH)
    new = (current | (1 << bit_dec));
  else if (state == LOW)
    new = (current & ~(1 << bit_dec));

  // write to register.
  if (bq76920_Write(reg, new, delay_ticks) != BQ_OK)
    return BQ_ERR;
  return BQ_OK;
}

// ADC gain and offset is set by the factory,
// this function asks the chip what it is.
uint16_t ADC_gain = 365; // default 365.
int32_t ADC_offset = 0;  // default 0, offset can be signed.
uint8_t ADC_of_reading = 0;

void get_ADC_Info(void)
{
  TickType_t delay_ticks = BQ_DELAY;
  // Gain is a minimum of 365 uV, can be set higher
  uint8_t ADC_g_1;
  if (bq76920_Read_1_Reg(ADCGAIN1, &ADC_g_1, delay_ticks) != BQ_OK)
    Error_Handler();
  uint8_t ADC_g_2;
  if (bq76920_Read_1_Reg(ADCGAIN2, &ADC_g_2, delay_ticks) != BQ_OK)
    Error_Handler();

  // ADC gain upper 2 MSB and lower 3 LSB are in diff regs.
  // They are also offset weirdly,
  // This offsets everything to the correct spot.
  ADC_gain = ADCGAIN_DEFAULT + (((ADC_g_1 << ADC_G1_OFFSET) & ADC_G1_MASK) | ((ADC_g_2 >> ADC_G2_OFFSET) & ADC_G2_MASK));

  // ADC_offset is stored in mV on the bms chip.
  // Need to convert to uV for using in voltage math.
  if (bq76920_Read_1_Reg(ADCOFFSET, &ADC_of_reading, delay_ticks) != BQ_OK)
    Error_Handler();

  ADC_offset = (int8_t)ADC_of_reading * (ADC_TO_UV);
}

// Gets chip info
void Init_BQ76920()
{
  // create the semaphore & mutex
  I2C_semaphore = xSemaphoreCreateBinaryStatic(&I2C_semaphore_buffer_pool);
  configASSERT(I2C_semaphore != NULL);

  I2C_mutex = xSemaphoreCreateMutexStatic(&I2C_mutex_buffer_pool);
  configASSERT(I2C_mutex != NULL);
}

// Protection and control.

// Sys read and write

// Input is which "Command" to read from.
// Details on datasheet (pgs. 29-32)
uint8_t sys_Read(SysCommands_t Command, TickType_t delay_ticks)
{
  // Return bit corresponding to command.
  // Depending on which SYS command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch (Command)
  {
  // Sys Stat Commands
  //==============================================================
  case CC_READY:
    return bq76920_R_1_bit(SYS_STAT, 7, delay_ticks);
  case DEVICE_XREADY:
    return bq76920_R_1_bit(SYS_STAT, 5, delay_ticks);
  case OVRD_ALERT:
    return bq76920_R_1_bit(SYS_STAT, 4, delay_ticks);
  case UV:
    return bq76920_R_1_bit(SYS_STAT, 3, delay_ticks);
  case OV:
    return bq76920_R_1_bit(SYS_STAT, 2, delay_ticks);
  case SCD:
    return bq76920_R_1_bit(SYS_STAT, 1, delay_ticks);
  case OCD:
    return bq76920_R_1_bit(SYS_STAT, 0, delay_ticks);
  // Sys Control 1 Commands
  //==============================================================
  case LOAD_PRESENT:
    return bq76920_R_1_bit(SYS_CTRL1, 7, delay_ticks);
  case ADC_EN:
    return bq76920_R_1_bit(SYS_CTRL1, 4, delay_ticks);
  case TEMP_SEL:
    return bq76920_R_1_bit(SYS_CTRL1, 3, delay_ticks);
  case SHUT_A:
    return bq76920_R_1_bit(SYS_CTRL1, 1, delay_ticks);
  case SHUT_B:
    return bq76920_R_1_bit(SYS_CTRL1, 0, delay_ticks);
  // Sys Control 2 Commands
  //==============================================================
  case DELAY_DIS:
    return bq76920_R_1_bit(SYS_CTRL2, 7, delay_ticks);
  case CC_EN:
    return bq76920_R_1_bit(SYS_CTRL2, 6, delay_ticks);
  case CC_ONESHOT:
    return bq76920_R_1_bit(SYS_CTRL2, 5, delay_ticks);
  case DSG_ON:
    return bq76920_R_1_bit(SYS_CTRL2, 1, delay_ticks);
  case CHG_ON:
    return bq76920_R_1_bit(SYS_CTRL2, 0, delay_ticks);
  default:
    return BQ_READ_ERR;
  }
  return BQ_READ_ERR; // command that doesn't exist chosen :(
}

// Inputs are which "Command" to adjust and what to set the bit to.
// Details on datasheet (pgs. 29-32)
void sys_Write(SysCommands_t Command, uint8_t State, TickType_t delay_ticks)
{
  // Depending on which SYS command you want, a different register may be used.
  // Write to the correct "command"
  switch (Command)
  {
  // Sys Stat Commands
  case CC_READY:
    bq76920_W_1_bit(SYS_STAT, 7, State, delay_ticks);
    break;
  case DEVICE_XREADY:
    bq76920_W_1_bit(SYS_STAT, 5, State, delay_ticks);
    break;
  case OVRD_ALERT:
    bq76920_W_1_bit(SYS_STAT, 4, State, delay_ticks);
    break;
  case UV:
    bq76920_W_1_bit(SYS_STAT, 3, State, delay_ticks);
    break;
  case OV:
    bq76920_W_1_bit(SYS_STAT, 2, State, delay_ticks);
    break;
  case SCD:
    bq76920_W_1_bit(SYS_STAT, 1, State, delay_ticks);
    break;
  case OCD:
    bq76920_W_1_bit(SYS_STAT, 0, State, delay_ticks);
    break;
  // Sys Control 1 Commands
  case LOAD_PRESENT:
    bq76920_W_1_bit(SYS_CTRL1, 7, State, delay_ticks);
    break;
  case ADC_EN:
    bq76920_W_1_bit(SYS_CTRL1, 4, State, delay_ticks);
    break;
  case TEMP_SEL:
    bq76920_W_1_bit(SYS_CTRL1, 3, State, delay_ticks);
    break;
  case SHUT_A:
    bq76920_W_1_bit(SYS_CTRL1, 1, State, delay_ticks);
    break;
  case SHUT_B:
    bq76920_W_1_bit(SYS_CTRL1, 0, State, delay_ticks);
    break;
  // Sys Control 2 Commands
  case DELAY_DIS:
    bq76920_W_1_bit(SYS_CTRL2, 7, State, delay_ticks);
    break;
  case CC_EN:
    bq76920_W_1_bit(SYS_CTRL2, 6, State, delay_ticks);
    break;
  case CC_ONESHOT:
    bq76920_W_1_bit(SYS_CTRL2, 5, State, delay_ticks);
    break;
  case DSG_ON:
    bq76920_W_1_bit(SYS_CTRL2, 1, State, delay_ticks);
    break;
  case CHG_ON:
    bq76920_W_1_bit(SYS_CTRL2, 0, State, delay_ticks);
    break;
  default:
    break;
  }
}

// Protection read / write
// Input is which "Command" to read from.
// Details on datasheet (pgs. 32-35)
uint8_t protect_Read(ProtectCommands_t Command, TickType_t delay_ticks)
{
  // Return bit corresponding to command.
  // Depending on which PROTECT command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch (Command)
  {
  // Protect1 Commands
  case RSNS:
    return bq76920_R_1_bit(PROTECT1, 7, delay_ticks);
  case SCD_D1:
    return bq76920_R_1_bit(PROTECT1, 4, delay_ticks);
  case SCD_D0:
    return bq76920_R_1_bit(PROTECT1, 3, delay_ticks);
  case SCD_T2:
    return bq76920_R_1_bit(PROTECT1, 2, delay_ticks);
  case SCD_T1:
    return bq76920_R_1_bit(PROTECT1, 1, delay_ticks);
  case SCD_T0:
    return bq76920_R_1_bit(PROTECT1, 0, delay_ticks);
  // Protect2 Commands
  case OCD_D2:
    return bq76920_R_1_bit(PROTECT2, 6, delay_ticks);
  case OCD_D1:
    return bq76920_R_1_bit(PROTECT2, 5, delay_ticks);
  case OCD_D0:
    return bq76920_R_1_bit(PROTECT2, 4, delay_ticks);
  case OCD_T3:
    return bq76920_R_1_bit(PROTECT2, 3, delay_ticks);
  case OCD_T2:
    return bq76920_R_1_bit(PROTECT2, 2, delay_ticks);
  case OCD_T1:
    return bq76920_R_1_bit(PROTECT2, 1, delay_ticks);
  case OCD_T0:
    return bq76920_R_1_bit(PROTECT2, 0, delay_ticks);
  // Protect3 Commands
  case UV_D1:
    return bq76920_R_1_bit(PROTECT3, 7, delay_ticks);
  case UV_D0:
    return bq76920_R_1_bit(PROTECT3, 6, delay_ticks);
  case OV_D1:
    return bq76920_R_1_bit(PROTECT3, 5, delay_ticks);
  case OV_D0:
    return bq76920_R_1_bit(PROTECT3, 4, delay_ticks);
  // OV_Trip Commands
  case OV_T7:
    return bq76920_R_1_bit(OV_TRIP, 7, delay_ticks);
  case OV_T6:
    return bq76920_R_1_bit(OV_TRIP, 6, delay_ticks);
  case OV_T5:
    return bq76920_R_1_bit(OV_TRIP, 5, delay_ticks);
  case OV_T4:
    return bq76920_R_1_bit(OV_TRIP, 4, delay_ticks);
  case OV_T3:
    return bq76920_R_1_bit(OV_TRIP, 3, delay_ticks);
  case OV_T2:
    return bq76920_R_1_bit(OV_TRIP, 2, delay_ticks);
  case OV_T1:
    return bq76920_R_1_bit(OV_TRIP, 1, delay_ticks);
  case OV_T0:
    return bq76920_R_1_bit(OV_TRIP, 0, delay_ticks);
  // UV_Trip Commands
  case UV_T7:
    return bq76920_R_1_bit(UV_TRIP, 7, delay_ticks);
  case UV_T6:
    return bq76920_R_1_bit(UV_TRIP, 6, delay_ticks);
  case UV_T5:
    return bq76920_R_1_bit(UV_TRIP, 5, delay_ticks);
  case UV_T4:
    return bq76920_R_1_bit(UV_TRIP, 4, delay_ticks);
  case UV_T3:
    return bq76920_R_1_bit(UV_TRIP, 3, delay_ticks);
  case UV_T2:
    return bq76920_R_1_bit(UV_TRIP, 2, delay_ticks);
  case UV_T1:
    return bq76920_R_1_bit(UV_TRIP, 1, delay_ticks);
  case UV_T0:
    return bq76920_R_1_bit(UV_TRIP, 0, delay_ticks);
  default:
    return BQ_READ_ERR;
  };
  return BQ_READ_ERR; // :-(
}

// Inputs are which "Command" to adjust and what to set the bit to.
// Details on datasheet (pgs. 32-35)
void protect_Write(ProtectCommands_t Command, uint8_t State, TickType_t delay_ticks)
{
  // Depending on which PROTECT command you want, a different register may be used.
  // Write to the correct "command"
  switch (Command)
  {
  // Protect1 Commands
  case RSNS:
    bq76920_W_1_bit(PROTECT1, 7, State, delay_ticks);
    break;
  case SCD_D1:
    bq76920_W_1_bit(PROTECT1, 4, State, delay_ticks);
    break;
  case SCD_D0:
    bq76920_W_1_bit(PROTECT1, 3, State, delay_ticks);
    break;
  case SCD_T2:
    bq76920_W_1_bit(PROTECT1, 2, State, delay_ticks);
    break;
  case SCD_T1:
    bq76920_W_1_bit(PROTECT1, 1, State, delay_ticks);
    break;
  case SCD_T0:
    bq76920_W_1_bit(PROTECT1, 0, State, delay_ticks);
    break;
  // Protect2 Commands
  case OCD_D2:
    bq76920_W_1_bit(PROTECT2, 6, State, delay_ticks);
    break;
  case OCD_D1:
    bq76920_W_1_bit(PROTECT2, 5, State, delay_ticks);
    break;
  case OCD_D0:
    bq76920_W_1_bit(PROTECT2, 4, State, delay_ticks);
    break;
  case OCD_T3:
    bq76920_W_1_bit(PROTECT2, 3, State, delay_ticks);
    break;
  case OCD_T2:
    bq76920_W_1_bit(PROTECT2, 2, State, delay_ticks);
    break;
  case OCD_T1:
    bq76920_W_1_bit(PROTECT2, 1, State, delay_ticks);
    break;
  case OCD_T0:
    bq76920_W_1_bit(PROTECT2, 0, State, delay_ticks);
    break;
  // Protect3 Commands
  case UV_D1:
    bq76920_W_1_bit(PROTECT3, 7, State, delay_ticks);
    break;
  case UV_D0:
    bq76920_W_1_bit(PROTECT3, 6, State, delay_ticks);
    break;
  case OV_D1:
    bq76920_W_1_bit(PROTECT3, 5, State, delay_ticks);
    break;
  case OV_D0:
    bq76920_W_1_bit(PROTECT3, 4, State, delay_ticks);
    break;
  // OV_Trip Commands
  case OV_T7:
    bq76920_W_1_bit(OV_TRIP, 7, State, delay_ticks);
    break;
  case OV_T6:
    bq76920_W_1_bit(OV_TRIP, 6, State, delay_ticks);
    break;
  case OV_T5:
    bq76920_W_1_bit(OV_TRIP, 5, State, delay_ticks);
    break;
  case OV_T4:
    bq76920_W_1_bit(OV_TRIP, 4, State, delay_ticks);
    break;
  case OV_T3:
    bq76920_W_1_bit(OV_TRIP, 3, State, delay_ticks);
    break;
  case OV_T2:
    bq76920_W_1_bit(OV_TRIP, 2, State, delay_ticks);
    break;
  case OV_T1:
    bq76920_W_1_bit(OV_TRIP, 1, State, delay_ticks);
    break;
  case OV_T0:
    bq76920_W_1_bit(OV_TRIP, 0, State, delay_ticks);
    break;
  // UV_Trip Commands
  case UV_T7:
    bq76920_W_1_bit(UV_TRIP, 7, State, delay_ticks);
    break;
  case UV_T6:
    bq76920_W_1_bit(UV_TRIP, 6, State, delay_ticks);
    break;
  case UV_T5:
    bq76920_W_1_bit(UV_TRIP, 5, State, delay_ticks);
    break;
  case UV_T4:
    bq76920_W_1_bit(UV_TRIP, 4, State, delay_ticks);
    break;
  case UV_T3:
    bq76920_W_1_bit(UV_TRIP, 3, State, delay_ticks);
    break;
  case UV_T2:
    bq76920_W_1_bit(UV_TRIP, 2, State, delay_ticks);
    break;
  case UV_T1:
    bq76920_W_1_bit(UV_TRIP, 1, State, delay_ticks);
    break;
  case UV_T0:
    bq76920_W_1_bit(UV_TRIP, 0, State, delay_ticks);
    break;
  default:
    break;
  };
}

// Cell Voltage Commands

// This function pulls cell reading and calculates actual voltage.
uint32_t get_Voltage_Cell(uint16_t cell, TickType_t delay_ticks)
{
  // Cell is passed in as 0xabcd, this reads 0xab and 0xcd
  uint16_t ADC_reading;
  bq76920_Read((cell >> 8), (cell & 0xFF), &ADC_reading, delay_ticks);

  // Cell voltage = (ADC READING)*(GAIN) + (ADC OFFSET)       (in uV).
  // (ADC offset is converted from mV to uV during init).
  return (((ADC_reading) * (ADC_gain)) + ((ADC_offset)));
}

// Populates an array with cell voltages.
BQ76920_Status_t get_Voltage_All(uint32_t *voltage_array, TickType_t delay_ticks)
{
  // make sure valid pointer
  if (voltage_array == NULL)
    return BQ_ERR;

  voltage_array[0] = get_Voltage_Cell(VC1, delay_ticks);
  voltage_array[1] = get_Voltage_Cell(VC2, delay_ticks);
  voltage_array[2] = get_Voltage_Cell(VC3, delay_ticks);
  // VC4 is skipped, as it is the shorted cell for our application
  voltage_array[3] = get_Voltage_Cell(VC5, delay_ticks);

  voltage_array[4] = (4 * get_Voltage_Cell(BAT, delay_ticks));

  return BQ_OK;
}
