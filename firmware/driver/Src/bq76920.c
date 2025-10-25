#include <stm32f4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>

// Variables
//================================
I2C_HandleTypeDef I2C_handler;

// Default I2C pins
// PB6 -> SCL
// PB7 -> SDA
GPIO_TypeDef* I2C_GPIO = GPIOB;
uint16_t I2C_SCL = GPIO_PIN_6;
uint16_t I2C_SDA = GPIO_PIN_7;
//================================

// Init functions
//========================================================================================================
//========================================================================================================

//=======================================================
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1){
    
    // enables the correct gpio clock.
    //======================================================
    if(I2C_GPIO == GPIOA)      __HAL_RCC_GPIOA_CLK_ENABLE();

    else if(I2C_GPIO == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();

    else if(I2C_GPIO == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    //======================================================

    GPIO_InitStruct.Pin = (I2C_SCL)|(I2C_SDA);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

  }
}
//=======================================================

//=======================================================
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c){
  if(hi2c->Instance==I2C1){

    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(I2C_GPIO, I2C_SCL);

    HAL_GPIO_DeInit(I2C_GPIO, I2C_SDA);

  }

}
//=======================================================

//=======================================================
void MX_I2C1_Init(void){

  I2C_handler.Instance = I2C1;
  I2C_handler.Init.ClockSpeed = 100000;
  I2C_handler.Init.DutyCycle = I2C_DUTYCYCLE_2;
  I2C_handler.Init.OwnAddress1 = 0;
  I2C_handler.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  I2C_handler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2C_handler.Init.OwnAddress2 = 0;
  I2C_handler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2C_handler.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&I2C_handler) != HAL_OK) Error_Handler();

}
//=======================================================

// auto-generated not used :-)
//=======================================================
void MX_GPIO_Init(void){

  /* GPIO Ports Clock Enable */
  //======================================================
  if(I2C_GPIO == GPIOA)      __HAL_RCC_GPIOA_CLK_ENABLE();

  else if(I2C_GPIO == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();

  else if(I2C_GPIO == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
  //======================================================

}
//=======================================================

// ADC gain and offset is set by the factory,
// this function asks the chip what it is.
//=================================================
uint16_t ADC_gain = 365; // default 365.
int32_t ADC_offset = 0;  // default 0, offset can be signed.

void get_ADC_Info(void){
  // Gain is a minimum of 365 uV, can be set higher
  uint8_t ADC_g_1 = bq76920_Read_1_Reg(ADCGAIN1);
  uint8_t ADC_g_2 = bq76920_Read_1_Reg(ADCGAIN2);

  // ADC gain upper 2 MSB and lower 3 LSB are in diff regs.
  // They are also offset weirdly,
  // This offsets everything to the correct spot.
  ADC_gain += (((ADC_g_1 << 1) & 0x18) | ((ADC_g_2 >> 5) & 0x7));

  // ADC_offset is stored in mV on the bms chip.
  // Need to convert to uV for using in voltage math.
  ADC_offset = (bq76920_Read_1_Reg(ADCOFFSET)*(1000));
}
//=================================================

// General init that calls the others :-)
// Input desired port and pins.
//===============================================
void Init_BQ76920(I2C_HandleTypeDef* i2c_ptr, GPIO_TypeDef * GPIO_port, uint16_t SCL_pin, uint16_t SDA_pin){
  // sets vars to user input.
  //=========================
  I2C_handler = *i2c_ptr;
  I2C_GPIO = GPIO_port;
  I2C_SCL = SCL_pin;
  I2C_SDA = SDA_pin;
  //=========================

  HAL_I2C_MspInit(&I2C_handler);
  MX_I2C1_Init();

  // Pulls ADC info from the bms chip.
  get_ADC_Info();
}
//===============================================

//========================================================================================================
//========================================================================================================



// Read and write functions.
//========================================================================================================
//========================================================================================================

// Reads & returns data from one register.
// Input is the register.
//=================================================
uint8_t bq76920_Read_1_Reg(uint16_t Mem_Address){
  uint8_t read_Data;
  HAL_I2C_Mem_Read(&I2C_handler, (DEV_ADD << 1), Mem_Address, MEM_SIZE, &read_Data, DATA_SIZE, TIMEOUT);
  return (read_Data);
}
//=================================================

// Reads one bit from a specific register
// Inputs are register and bit to read.
//======================================================
uint8_t bq76920_R_1_bit(uint8_t reg, uint8_t bit_dec){
  return ((bq76920_Read_1_Reg(reg) >> (bit_dec))&(0x1));
}
//======================================================

// Most reads require HI & LO registers,
// This function collects both and combines them.
// Inputs are both registers.
//=================================================
uint16_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2){
  uint16_t data_1 = bq76920_Read_1_Reg(Mem_Add_1);
  uint8_t  data_2 = bq76920_Read_1_Reg(Mem_Add_2);
  // most significant are from reading 1.
  // shift left and then concatenate reading 2.
  return ((data_1 << 8) | (data_2)); // :-)
}
//=================================================

// Writes to the bms chip.
// Inputs are address & data.
//===================================================
void bq76920_Write(uint16_t Mem_Address, uint8_t Write_Data){
  HAL_I2C_Mem_Write(&I2C_handler, (DEV_ADD << 1), Mem_Address, MEM_SIZE, &Write_Data, DATA_SIZE, TIMEOUT);
}
//===================================================

// Writes to one bit in a specified register
// Inputs are register, bit in decimal, state
//==================================================================
void bq76920_W_1_bit(uint8_t reg, uint8_t bit_dec, uint8_t state){
    uint8_t current = bq76920_Read_1_Reg(reg); // acquire current info
    uint8_t new = current; // defaults to rewriting what was read.
    
    // need different masks depending on whether you want to set or clear
    if(state == HIGH)     new = (current |  (1 << bit_dec));  
    else if(state == LOW) new = (current & ~(1 << bit_dec));  
    
    // write to register.
    bq76920_Write(reg, new);
  //==================================================================
}
//==================================================================

//========================================================================================================
//========================================================================================================



// Protection and control. 
//========================================================================================================
//========================================================================================================

// Sys read and write
//=====================================================================
// Input is which "Command" to read from.
// Details on datasheet (pgs. 29-32)
uint8_t sys_Read(SysCommands Command){
  // Return bit corresponding to command.
  // Depending on which SYS command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch(Command){
    // Sys Stat Commands
    //==============================================================
    case CC_READY:      return bq76920_R_1_bit(SYS_STAT, 7);
    case DEVICE_XREADY: return bq76920_R_1_bit(SYS_STAT, 5);
    case OVRD_ALERT:    return bq76920_R_1_bit(SYS_STAT, 4);
    case UV:            return bq76920_R_1_bit(SYS_STAT, 3);
    case OV:            return bq76920_R_1_bit(SYS_STAT, 2);
    case SCD:           return bq76920_R_1_bit(SYS_STAT, 1);
    case OCD:           return bq76920_R_1_bit(SYS_STAT, 0);
    // Sys Control 1 Commands
    //==============================================================
    case LOAD_PRESENT:  return bq76920_R_1_bit(SYS_CTRL1, 7);
    case ADC_EN:        return bq76920_R_1_bit(SYS_CTRL1, 4);
    case TEMP_SEL:      return bq76920_R_1_bit(SYS_CTRL1, 3);
    case SHUT_A:        return bq76920_R_1_bit(SYS_CTRL1, 1);
    case SHUT_B:        return bq76920_R_1_bit(SYS_CTRL1, 0);
    // Sys Control 2 Commands
    //==============================================================
    case DELAY_DIS:     return bq76920_R_1_bit(SYS_CTRL2, 7);
    case CC_EN:         return bq76920_R_1_bit(SYS_CTRL2, 6);
    case CC_ONESHOT:    return bq76920_R_1_bit(SYS_CTRL2, 5);
    case DSG_ON:        return bq76920_R_1_bit(SYS_CTRL2, 1);
    case CHG_ON:        return bq76920_R_1_bit(SYS_CTRL2, 0);
    default: return 0xff;
  }
  return 0xff; // command that doesn't exist chosen :(
}

// Inputs are which "Command" to adjust and what to set the bit to. 
// Details on datasheet (pgs. 29-32)
void sys_Write(SysCommands Command, uint8_t State){
  // Depending on which SYS command you want, a different register may be used.
  // Write to the correct "command"
  switch(Command){
    // Sys Stat Commands
    //==============================================================
    case CC_READY:      bq76920_W_1_bit(SYS_STAT, 7, State); break;
    case DEVICE_XREADY: bq76920_W_1_bit(SYS_STAT, 5, State); break;
    case OVRD_ALERT:    bq76920_W_1_bit(SYS_STAT, 4, State); break;
    case UV:            bq76920_W_1_bit(SYS_STAT, 3, State); break;
    case OV:            bq76920_W_1_bit(SYS_STAT, 2, State); break;
    case SCD:           bq76920_W_1_bit(SYS_STAT, 1, State); break;
    case OCD:           bq76920_W_1_bit(SYS_STAT, 0, State); break;
    // Sys Control 1 Commands
    //==============================================================
    case LOAD_PRESENT:  bq76920_W_1_bit(SYS_CTRL1, 7, State); break;
    case ADC_EN:        bq76920_W_1_bit(SYS_CTRL1, 4, State); break;
    case TEMP_SEL:      bq76920_W_1_bit(SYS_CTRL1, 3, State); break;
    case SHUT_A:        bq76920_W_1_bit(SYS_CTRL1, 1, State); break;
    case SHUT_B:        bq76920_W_1_bit(SYS_CTRL1, 0, State); break;
    // Sys Control 2 Commands
    //==============================================================
    case DELAY_DIS:     bq76920_W_1_bit(SYS_CTRL2, 7, State); break;
    case CC_EN:         bq76920_W_1_bit(SYS_CTRL2, 6, State); break;
    case CC_ONESHOT:    bq76920_W_1_bit(SYS_CTRL2, 5, State); break;
    case DSG_ON:        bq76920_W_1_bit(SYS_CTRL2, 1, State); break;
    case CHG_ON:        bq76920_W_1_bit(SYS_CTRL2, 0, State); break;
    default: break;
  }
}
//=====================================================================

// Protection read / write
//=====================================================================
// Input is which "Command" to read from.
// Details on datasheet (pgs. 32-35)
uint8_t protect_Read(ProtectCommands Command){
  // Return bit corresponding to command.
  // Depending on which PROTECT command you want, a different register may be used.
  // Read the correct bit from the correct register.
  switch(Command){
    // Protect1 Commands
    //==============================================================
    case RSNS:    return bq76920_R_1_bit(PROTECT1, 7);
    case SCD_D1:  return bq76920_R_1_bit(PROTECT1, 6);
    case SCD_D0:  return bq76920_R_1_bit(PROTECT1, 3);
    case SCD_T2:  return bq76920_R_1_bit(PROTECT1, 2);
    case SCD_T1:  return bq76920_R_1_bit(PROTECT1, 1);
    case SCD_T0:  return bq76920_R_1_bit(PROTECT1, 0);
    // Protect2 Commands
    //==============================================================
    case OCD_D2:  return bq76920_R_1_bit(PROTECT2, 6);
    case OCD_D1:  return bq76920_R_1_bit(PROTECT2, 5);
    case OCD_D0:  return bq76920_R_1_bit(PROTECT2, 4);
    case OCD_T3:  return bq76920_R_1_bit(PROTECT2, 3);
    case OCD_T2:  return bq76920_R_1_bit(PROTECT2, 2);
    case OCD_T1:  return bq76920_R_1_bit(PROTECT2, 1);
    case OCD_T0:  return bq76920_R_1_bit(PROTECT2, 0);
    // Protect3 Commands
    //==============================================================
    case UV_D1:   return bq76920_R_1_bit(PROTECT3, 7);
    case UV_D0:   return bq76920_R_1_bit(PROTECT3, 6);
    case OV_D1:   return bq76920_R_1_bit(PROTECT3, 5);
    case OV_D0:   return bq76920_R_1_bit(PROTECT3, 4);
    // OV_Trip Commands
    //==============================================================
    case OV_T7:   return bq76920_R_1_bit(OV_TRIP, 7);
    case OV_T6:   return bq76920_R_1_bit(OV_TRIP, 6);
    case OV_T5:   return bq76920_R_1_bit(OV_TRIP, 5);
    case OV_T4:   return bq76920_R_1_bit(OV_TRIP, 4);
    case OV_T3:   return bq76920_R_1_bit(OV_TRIP, 3);
    case OV_T2:   return bq76920_R_1_bit(OV_TRIP, 2);
    case OV_T1:   return bq76920_R_1_bit(OV_TRIP, 1);
    case OV_T0:   return bq76920_R_1_bit(OV_TRIP, 0);
    // UV_Trip Commands
    //==============================================================
    case UV_T7:   return bq76920_R_1_bit(UV_TRIP, 7);
    case UV_T6:   return bq76920_R_1_bit(UV_TRIP, 6);
    case UV_T5:   return bq76920_R_1_bit(UV_TRIP, 5);
    case UV_T4:   return bq76920_R_1_bit(UV_TRIP, 4);
    case UV_T3:   return bq76920_R_1_bit(UV_TRIP, 3);
    case UV_T2:   return bq76920_R_1_bit(UV_TRIP, 2);
    case UV_T1:   return bq76920_R_1_bit(UV_TRIP, 1);
    case UV_T0:   return bq76920_R_1_bit(UV_TRIP, 0);
    default: return 0xff;
  };
  return 0xff; // :-(
}

// Inputs are which "Command" to adjust and what to set the bit to. 
// Details on datasheet (pgs. 32-35)
void protect_Write(ProtectCommands Command, uint8_t State){
  // Depending on which PROTECT command you want, a different register may be used.
  // Write to the correct "command"
  switch(Command){
    // Protect1 Commands
    //==============================================================
    case RSNS:    bq76920_W_1_bit(PROTECT1, 7, State); break;
    case SCD_D1:  bq76920_W_1_bit(PROTECT1, 4, State); break;
    case SCD_D0:  bq76920_W_1_bit(PROTECT1, 3, State); break;
    case SCD_T2:  bq76920_W_1_bit(PROTECT1, 2, State); break;
    case SCD_T1:  bq76920_W_1_bit(PROTECT1, 1, State); break;
    case SCD_T0:  bq76920_W_1_bit(PROTECT1, 0, State); break;
    // Protect2 Commands
    //==============================================================
    case OCD_D2:  bq76920_W_1_bit(PROTECT2, 6, State); break;
    case OCD_D1:  bq76920_W_1_bit(PROTECT2, 5, State); break;
    case OCD_D0:  bq76920_W_1_bit(PROTECT2, 4, State); break;
    case OCD_T3:  bq76920_W_1_bit(PROTECT2, 3, State); break;
    case OCD_T2:  bq76920_W_1_bit(PROTECT2, 2, State); break;
    case OCD_T1:  bq76920_W_1_bit(PROTECT2, 1, State); break;
    case OCD_T0:  bq76920_W_1_bit(PROTECT2, 0, State); break;
    // Protect3 Commands
    //==============================================================
    case UV_D1:   bq76920_W_1_bit(PROTECT3, 7, State); break;
    case UV_D0:   bq76920_W_1_bit(PROTECT3, 6, State); break;
    case OV_D1:   bq76920_W_1_bit(PROTECT3, 5, State); break;
    case OV_D0:   bq76920_W_1_bit(PROTECT3, 4, State); break;
    // OV_Trip Commands
    //==============================================================
    case OV_T7:   bq76920_W_1_bit(OV_TRIP, 7, State); break;
    case OV_T6:   bq76920_W_1_bit(OV_TRIP, 6, State); break;
    case OV_T5:   bq76920_W_1_bit(OV_TRIP, 5, State); break;
    case OV_T4:   bq76920_W_1_bit(OV_TRIP, 4, State); break;
    case OV_T3:   bq76920_W_1_bit(OV_TRIP, 3, State); break;
    case OV_T2:   bq76920_W_1_bit(OV_TRIP, 2, State); break;
    case OV_T1:   bq76920_W_1_bit(OV_TRIP, 1, State); break;
    case OV_T0:   bq76920_W_1_bit(OV_TRIP, 0, State); break;
    // UV_Trip Commands
    //==============================================================
    case UV_T7:   bq76920_W_1_bit(UV_TRIP, 7, State); break;
    case UV_T6:   bq76920_W_1_bit(UV_TRIP, 6, State); break;
    case UV_T5:   bq76920_W_1_bit(UV_TRIP, 5, State); break;
    case UV_T4:   bq76920_W_1_bit(UV_TRIP, 4, State); break;
    case UV_T3:   bq76920_W_1_bit(UV_TRIP, 3, State); break;
    case UV_T2:   bq76920_W_1_bit(UV_TRIP, 2, State); break;
    case UV_T1:   bq76920_W_1_bit(UV_TRIP, 1, State); break;
    case UV_T0:   bq76920_W_1_bit(UV_TRIP, 0, State); break;
    default: break;
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
uint32_t get_Voltage_1(uint16_t cell){
  // Cell is passed in as 0xabcd, this reads 0xab and 0xcd
  uint16_t ADC_reading = bq76920_Read((cell >> 8), (cell & 0xFF));
  
  // Cell voltage = (ADC READING)*(GAIN) + (ADC OFFSET)       (in uV).
  // (ADC offset is converted from mV to uV during init).
  return (((ADC_reading)*(ADC_gain)) + ((ADC_offset)));
}
//====================================================================

// Populates an array with cell voltages.
//====================================================================
void get_Voltage_All(uint32_t* voltage_array){
  voltage_array[0] = get_Voltage_1(VC1);
  voltage_array[1] = get_Voltage_1(VC2);
  voltage_array[2] = get_Voltage_1(VC3);
  // VC4 is skipped, as it is the shorted cell for our application
  voltage_array[3] = get_Voltage_1(VC5);

  voltage_array[4] = (4*get_Voltage_1(BAT));
}
//====================================================================

//========================================================================================================
//========================================================================================================