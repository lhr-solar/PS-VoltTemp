#include <stm32l4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "inits.h"

I2C_HandleTypeDef I2C_handler;


// I2C Interrupts 
//========================================================================================================
//========================================================================================================

// Semaphore and mutex decl.
//========================================================================================================
SemaphoreHandle_t I2C_semaphore;
StaticSemaphore_t I2C_semaphore_buffer_pool;

SemaphoreHandle_t I2C_mutex;
StaticSemaphore_t I2C_mutex_buffer_pool;
//========================================================================================================


void I2C1_EV_IRQHandler(void){HAL_I2C_EV_IRQHandler(&hi2c1);}

void I2C1_ER_IRQHandler(void){HAL_I2C_ER_IRQHandler(&hi2c1);}


void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;
  
  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);
  
  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}
//========================================================================================================
//========================================================================================================


// Read and write functions.
//========================================================================================================
//========================================================================================================

// Reads & returns data from one register.
// Input is the register.
//=================================================
BQ76920_Status bq76920_Read_1_Reg(uint16_t Mem_Address, uint8_t* read_Data){
  HAL_StatusTypeDef rx_status;

  // acquire mutex
  if(xSemaphoreTake(I2C_mutex, pdMS_TO_TICKS(100)) != pdTRUE) return BQ_ERR;
  
  // ask politely to read
  rx_status =  HAL_I2C_Mem_Read_IT(&hi2c1, (DEV_ADD << 1),
		 			 Mem_Address, I2C_MEMADD_SIZE_8BIT,
					 read_Data, DATA_SIZE);
 
  // make sure read was ok
  if(rx_status == HAL_OK){
    // check for timeout
    if(xSemaphoreTake(I2C_semaphore, pdMS_TO_TICKS(100)) != pdTRUE){
      xSemaphoreGive(I2C_mutex);
      HAL_I2C_Master_Abort_IT(&hi2c1,(DEV_ADD << 1));
      return BQ_ERR;
    }
  }else{
    xSemaphoreGive(I2C_mutex);
    return BQ_ERR;
  } 

  // give up mutex & return
  xSemaphoreGive(I2C_mutex);
  return BQ_OK;
}
//=================================================

// Reads one bit from a specific register
// Inputs are register and bit to read.
//======================================================
uint8_t bq76920_R_1_bit(uint8_t reg, uint8_t bit_dec){
  uint8_t read_val; bq76920_Read_1_Reg(reg, &read_val);
  return ((read_val >> (bit_dec)) & (0x1));
}
//======================================================

// Most reads require HI & LO registers,
// This function collects both and combines them.
// Inputs are both registers.
//=================================================
BQ76920_Status bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2, uint16_t* data)
{
  uint8_t data_1; bq76920_Read_1_Reg(Mem_Add_1, &data_1);
  uint8_t data_2; bq76920_Read_1_Reg(Mem_Add_2, &data_2);
  // most significant are from reading 1.
  // shift left and then concatenate reading 2.
  *data = (data_1 << 8) | (data_2);
  return BQ_OK; // :-)
}
//=================================================

// Writes to the bms chip.
// Inputs are address & data.
//===================================================
// CRC function for writing
unsigned char CRC8(unsigned char *ptr, unsigned char len,unsigned char key){
	unsigned char i;
	unsigned char crc=0;
	while(len--!=0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= key;
			}
			else
				crc *= 2;

			if((*ptr & i)!=0)
				crc ^= key;
		}
		ptr++;
	}
	return(crc);
}


 
BQ76920_Status bq76920_Write(uint16_t Mem_Address, uint8_t new_data){
  HAL_StatusTypeDef tx_status;

  // acquire semaphore
  if(xSemaphoreTake(I2C_mutex, pdMS_TO_TICKS(100)) != pdTRUE) return BQ_ERR;

  // perform the CRC things
  uint8_t crc_input[3];
  uint8_t write_data[3];

  crc_input[0] = (DEV_ADD << 1); 
  crc_input[1] = Mem_Address;
  crc_input[2] = new_data;

  write_data[0] = new_data;
  write_data[1] = CRC8(crc_input, 3, 0x07);

  // politely ask to write
  tx_status =  HAL_I2C_Mem_Write_IT(&hi2c1, (DEV_ADD << 1),
		 		    Mem_Address, I2C_MEMADD_SIZE_8BIT,
			      write_data, 2);

  // make sure write was ok
  if(tx_status == HAL_OK){
    // check for timeout
    if(xSemaphoreTake(I2C_semaphore, pdMS_TO_TICKS(100)) != pdTRUE){
      xSemaphoreGive(I2C_mutex);
      HAL_I2C_Master_Abort_IT(&hi2c1,(DEV_ADD << 1));
      return BQ_ERR;
    }
  }else{
    xSemaphoreGive(I2C_mutex);
    return BQ_ERR;
  } 

  // return mutex and status
  xSemaphoreGive(I2C_mutex);
  return BQ_OK;
}
//===================================================


// Writes to one bit in a specified register
// Inputs are register, bit in decimal, state
//==================================================================
BQ76920_Status bq76920_W_1_bit(uint8_t reg, uint8_t bit_dec, uint8_t state){
  uint8_t current; 
  if(bq76920_Read_1_Reg(reg, &current) != BQ_OK) return BQ_ERR; // acquire current info
  uint8_t new = current;                     // defaults to rewriting what was read.

  // need different masks depending on whether you want to set or clear
  if (state == HIGH)
    new = (current | (1 << bit_dec));
  else if (state == LOW)
    new = (current & ~(1 << bit_dec));

  // write to register.
  bq76920_Write(reg, new);
  return BQ_OK;
  //==================================================================
}
//==================================================================

//========================================================================================================
//========================================================================================================


// ADC gain and offset is set by the factory,
// this function asks the chip what it is.
//=================================================
uint16_t ADC_gain = 365; // default 365.
int32_t ADC_offset = 0;  // default 0, offset can be signed.
uint8_t ADC_of_reading = 0;

void get_ADC_Info(void)
{
  // Gain is a minimum of 365 uV, can be set higher
  uint8_t ADC_g_1; bq76920_Read_1_Reg(ADCGAIN1, &ADC_g_1);
  uint8_t ADC_g_2; bq76920_Read_1_Reg(ADCGAIN2, &ADC_g_2);

  // ADC gain upper 2 MSB and lower 3 LSB are in diff regs.
  // They are also offset weirdly,
  // This offsets everything to the correct spot.
  ADC_gain = 365 + (((ADC_g_1 << 1) & 0x18) | ((ADC_g_2 >> 5) & 0x7));

  // ADC_offset is stored in mV on the bms chip.
  // Need to convert to uV for using in voltage math.
  bq76920_Read_1_Reg(ADCOFFSET, &ADC_of_reading);
  
  ADC_offset = (int8_t)ADC_of_reading * (1000);
}
//=================================================

// Gets chip info
//===============================================
void Init_BQ76920()
{
  I2C_handler = hi2c1;
  // Pulls ADC info from the bms chip.

  I2C_semaphore = xSemaphoreCreateBinaryStatic(&I2C_semaphore_buffer_pool);
  configASSERT(I2C_semaphore != NULL);

  I2C_mutex = xSemaphoreCreateMutexStatic(&I2C_mutex_buffer_pool);
  
}
//===============================================



// Protection and control.
//========================================================================================================
//========================================================================================================

// Sys read and write
//=====================================================================
// Input is which "Command" to read from.
// Details on datasheet (pgs. 29-32)
uint8_t sys_Read(SysCommands Command)
{
  // Return bit corresponding to command.
  // Depending on which SYS command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch (Command)
  {
  // Sys Stat Commands
  //==============================================================
  case CC_READY:
    return bq76920_R_1_bit(SYS_STAT, 7);
  case DEVICE_XREADY:
    return bq76920_R_1_bit(SYS_STAT, 5);
  case OVRD_ALERT:
    return bq76920_R_1_bit(SYS_STAT, 4);
  case UV:
    return bq76920_R_1_bit(SYS_STAT, 3);
  case OV:
    return bq76920_R_1_bit(SYS_STAT, 2);
  case SCD:
    return bq76920_R_1_bit(SYS_STAT, 1);
  case OCD:
    return bq76920_R_1_bit(SYS_STAT, 0);
  // Sys Control 1 Commands
  //==============================================================
  case LOAD_PRESENT:
    return bq76920_R_1_bit(SYS_CTRL1, 7);
  case ADC_EN:
    return bq76920_R_1_bit(SYS_CTRL1, 4);
  case TEMP_SEL:
    return bq76920_R_1_bit(SYS_CTRL1, 3);
  case SHUT_A:
    return bq76920_R_1_bit(SYS_CTRL1, 1);
  case SHUT_B:
    return bq76920_R_1_bit(SYS_CTRL1, 0);
  // Sys Control 2 Commands
  //==============================================================
  case DELAY_DIS:
    return bq76920_R_1_bit(SYS_CTRL2, 7);
  case CC_EN:
    return bq76920_R_1_bit(SYS_CTRL2, 6);
  case CC_ONESHOT:
    return bq76920_R_1_bit(SYS_CTRL2, 5);
  case DSG_ON:
    return bq76920_R_1_bit(SYS_CTRL2, 1);
  case CHG_ON:
    return bq76920_R_1_bit(SYS_CTRL2, 0);
  default:
    return 0xff;
  }
  return 0xff; // command that doesn't exist chosen :(
}

// Inputs are which "Command" to adjust and what to set the bit to.
// Details on datasheet (pgs. 29-32)
void sys_Write(SysCommands Command, uint8_t State)
{
  // Depending on which SYS command you want, a different register may be used.
  // Write to the correct "command"
  switch (Command)
  {
  // Sys Stat Commands
  //==============================================================
  case CC_READY:
    bq76920_W_1_bit(SYS_STAT, 7, State);
    break;
  case DEVICE_XREADY:
    bq76920_W_1_bit(SYS_STAT, 5, State);
    break;
  case OVRD_ALERT:
    bq76920_W_1_bit(SYS_STAT, 4, State);
    break;
  case UV:
    bq76920_W_1_bit(SYS_STAT, 3, State);
    break;
  case OV:
    bq76920_W_1_bit(SYS_STAT, 2, State);
    break;
  case SCD:
    bq76920_W_1_bit(SYS_STAT, 1, State);
    break;
  case OCD:
    bq76920_W_1_bit(SYS_STAT, 0, State);
    break;
  // Sys Control 1 Commands
  //==============================================================
  case LOAD_PRESENT:
    bq76920_W_1_bit(SYS_CTRL1, 7, State);
    break;
  case ADC_EN:
    bq76920_W_1_bit(SYS_CTRL1, 4, State);
    break;
  case TEMP_SEL:
    bq76920_W_1_bit(SYS_CTRL1, 3, State);
    break;
  case SHUT_A:
    bq76920_W_1_bit(SYS_CTRL1, 1, State);
    break;
  case SHUT_B:
    bq76920_W_1_bit(SYS_CTRL1, 0, State);
    break;
  // Sys Control 2 Commands
  //==============================================================
  case DELAY_DIS:
    bq76920_W_1_bit(SYS_CTRL2, 7, State);
    break;
  case CC_EN:
    bq76920_W_1_bit(SYS_CTRL2, 6, State);
    break;
  case CC_ONESHOT:
    bq76920_W_1_bit(SYS_CTRL2, 5, State);
    break;
  case DSG_ON:
    bq76920_W_1_bit(SYS_CTRL2, 1, State);
    break;
  case CHG_ON:
    bq76920_W_1_bit(SYS_CTRL2, 0, State);
    break;
  default:
    break;
  }
}
//=====================================================================

// Protection read / write
//=====================================================================
// Input is which "Command" to read from.
// Details on datasheet (pgs. 32-35)
uint8_t protect_Read(ProtectCommands Command)
{
  // Return bit corresponding to command.
  // Depending on which PROTECT command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch (Command)
  {
  // Protect1 Commands
  //==============================================================
  case RSNS:
    return bq76920_R_1_bit(PROTECT1, 7);
  case SCD_D1:
    return bq76920_R_1_bit(PROTECT1, 4);
  case SCD_D0:
    return bq76920_R_1_bit(PROTECT1, 3);
  case SCD_T2:
    return bq76920_R_1_bit(PROTECT1, 2);
  case SCD_T1:
    return bq76920_R_1_bit(PROTECT1, 1);
  case SCD_T0:
    return bq76920_R_1_bit(PROTECT1, 0);
  // Protect2 Commands
  //==============================================================
  case OCD_D2:
    return bq76920_R_1_bit(PROTECT2, 6);
  case OCD_D1:
    return bq76920_R_1_bit(PROTECT2, 5);
  case OCD_D0:
    return bq76920_R_1_bit(PROTECT2, 4);
  case OCD_T3:
    return bq76920_R_1_bit(PROTECT2, 3);
  case OCD_T2:
    return bq76920_R_1_bit(PROTECT2, 2);
  case OCD_T1:
    return bq76920_R_1_bit(PROTECT2, 1);
  case OCD_T0:
    return bq76920_R_1_bit(PROTECT2, 0);
  // Protect3 Commands
  //==============================================================
  case UV_D1:
    return bq76920_R_1_bit(PROTECT3, 7);
  case UV_D0:
    return bq76920_R_1_bit(PROTECT3, 6);
  case OV_D1:
    return bq76920_R_1_bit(PROTECT3, 5);
  case OV_D0:
    return bq76920_R_1_bit(PROTECT3, 4);
  // OV_Trip Commands
  //==============================================================
  case OV_T7:
    return bq76920_R_1_bit(OV_TRIP, 7);
  case OV_T6:
    return bq76920_R_1_bit(OV_TRIP, 6);
  case OV_T5:
    return bq76920_R_1_bit(OV_TRIP, 5);
  case OV_T4:
    return bq76920_R_1_bit(OV_TRIP, 4);
  case OV_T3:
    return bq76920_R_1_bit(OV_TRIP, 3);
  case OV_T2:
    return bq76920_R_1_bit(OV_TRIP, 2);
  case OV_T1:
    return bq76920_R_1_bit(OV_TRIP, 1);
  case OV_T0:
    return bq76920_R_1_bit(OV_TRIP, 0);
  // UV_Trip Commands
  //==============================================================
  case UV_T7:
    return bq76920_R_1_bit(UV_TRIP, 7);
  case UV_T6:
    return bq76920_R_1_bit(UV_TRIP, 6);
  case UV_T5:
    return bq76920_R_1_bit(UV_TRIP, 5);
  case UV_T4:
    return bq76920_R_1_bit(UV_TRIP, 4);
  case UV_T3:
    return bq76920_R_1_bit(UV_TRIP, 3);
  case UV_T2:
    return bq76920_R_1_bit(UV_TRIP, 2);
  case UV_T1:
    return bq76920_R_1_bit(UV_TRIP, 1);
  case UV_T0:
    return bq76920_R_1_bit(UV_TRIP, 0);
  default:
    return 0xff;
  };
  return 0xff; // :-(
}

// Inputs are which "Command" to adjust and what to set the bit to.
// Details on datasheet (pgs. 32-35)
void protect_Write(ProtectCommands Command, uint8_t State)
{
  // Depending on which PROTECT command you want, a different register may be used.
  // Write to the correct "command"
  switch (Command)
  {
  // Protect1 Commands
  //==============================================================
  case RSNS:
    bq76920_W_1_bit(PROTECT1, 7, State);
    break;
  case SCD_D1:
    bq76920_W_1_bit(PROTECT1, 4, State);
    break;
  case SCD_D0:
    bq76920_W_1_bit(PROTECT1, 3, State);
    break;
  case SCD_T2:
    bq76920_W_1_bit(PROTECT1, 2, State);
    break;
  case SCD_T1:
    bq76920_W_1_bit(PROTECT1, 1, State);
    break;
  case SCD_T0:
    bq76920_W_1_bit(PROTECT1, 0, State);
    break;
  // Protect2 Commands
  //==============================================================
  case OCD_D2:
    bq76920_W_1_bit(PROTECT2, 6, State);
    break;
  case OCD_D1:
    bq76920_W_1_bit(PROTECT2, 5, State);
    break;
  case OCD_D0:
    bq76920_W_1_bit(PROTECT2, 4, State);
    break;
  case OCD_T3:
    bq76920_W_1_bit(PROTECT2, 3, State);
    break;
  case OCD_T2:
    bq76920_W_1_bit(PROTECT2, 2, State);
    break;
  case OCD_T1:
    bq76920_W_1_bit(PROTECT2, 1, State);
    break;
  case OCD_T0:
    bq76920_W_1_bit(PROTECT2, 0, State);
    break;
  // Protect3 Commands
  //==============================================================
  case UV_D1:
    bq76920_W_1_bit(PROTECT3, 7, State);
    break;
  case UV_D0:
    bq76920_W_1_bit(PROTECT3, 6, State);
    break;
  case OV_D1:
    bq76920_W_1_bit(PROTECT3, 5, State);
    break;
  case OV_D0:
    bq76920_W_1_bit(PROTECT3, 4, State);
    break;
  // OV_Trip Commands
  //==============================================================
  case OV_T7:
    bq76920_W_1_bit(OV_TRIP, 7, State);
    break;
  case OV_T6:
    bq76920_W_1_bit(OV_TRIP, 6, State);
    break;
  case OV_T5:
    bq76920_W_1_bit(OV_TRIP, 5, State);
    break;
  case OV_T4:
    bq76920_W_1_bit(OV_TRIP, 4, State);
    break;
  case OV_T3:
    bq76920_W_1_bit(OV_TRIP, 3, State);
    break;
  case OV_T2:
    bq76920_W_1_bit(OV_TRIP, 2, State);
    break;
  case OV_T1:
    bq76920_W_1_bit(OV_TRIP, 1, State);
    break;
  case OV_T0:
    bq76920_W_1_bit(OV_TRIP, 0, State);
    break;
  // UV_Trip Commands
  //==============================================================
  case UV_T7:
    bq76920_W_1_bit(UV_TRIP, 7, State);
    break;
  case UV_T6:
    bq76920_W_1_bit(UV_TRIP, 6, State);
    break;
  case UV_T5:
    bq76920_W_1_bit(UV_TRIP, 5, State);
    break;
  case UV_T4:
    bq76920_W_1_bit(UV_TRIP, 4, State);
    break;
  case UV_T3:
    bq76920_W_1_bit(UV_TRIP, 3, State);
    break;
  case UV_T2:
    bq76920_W_1_bit(UV_TRIP, 2, State);
    break;
  case UV_T1:
    bq76920_W_1_bit(UV_TRIP, 1, State);
    break;
  case UV_T0:
    bq76920_W_1_bit(UV_TRIP, 0, State);
    break;
  default:
    break;
  };
}
//=====================================================================

//========================================================================================================
//========================================================================================================

// Cell Voltage Commands
//========================================================================================================
//========================================================================================================

// This function pulls cell reading and calculates actual voltage.
//====================================================================
uint32_t get_Voltage_1(uint16_t cell)
{
  // Cell is passed in as 0xabcd, this reads 0xab and 0xcd
  uint16_t ADC_reading; bq76920_Read((cell >> 8), (cell & 0xFF), &ADC_reading);

  // Cell voltage = (ADC READING)*(GAIN) + (ADC OFFSET)       (in uV).
  // (ADC offset is converted from mV to uV during init).
  return (((ADC_reading) * (ADC_gain)) + ((ADC_offset)));
}
//====================================================================

// Populates an array with cell voltages.
//====================================================================
void get_Voltage_All(uint32_t *voltage_array)
{
  voltage_array[0] = get_Voltage_1(VC1);
  voltage_array[1] = get_Voltage_1(VC2);
  voltage_array[2] = get_Voltage_1(VC3);
  // VC4 is skipped, as it is the shorted cell for our application
  voltage_array[3] = get_Voltage_1(VC5);

  voltage_array[4] = (4 * get_Voltage_1(BAT));
}
//====================================================================

//========================================================================================================
//========================================================================================================
