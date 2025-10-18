#include <stm32f4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>

I2C_HandleTypeDef I2C_handler;



//=======================================================
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1){
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

  }
}
//=======================================================

//=======================================================
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c){
  if(hi2c->Instance==I2C1){

    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  }

}
//=======================================================

//=======================================================
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure the main internal regulator output voltage
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }

  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
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

//=======================================================
void MX_GPIO_Init(void){

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}
//=======================================================


// ADC gain and offset is set by the factory,
// this function asks the chip what it is.
//=================================================
uint16_t ADC_gain = 365; // default 365.
int32_t ADC_offset = 0;   // default 0, signed int8.

void get_ADC_Info(void){
  // Gain is a minimum of 365 uV, can be set higher
  uint8_t ADC_g_1 = bq76920_Read_1_Reg(ADCGAIN1);
  uint8_t ADC_g_2 = bq76920_Read_1_Reg(ADCGAIN2);

  ADC_gain += (((ADC_g_1 << 1) & 0x18) | ((ADC_g_2 >> 5) & 0x7));

  // ADC_offset is stored in mV on the bms chip.
  // need to convert to uV for using in math.
  ADC_offset = (bq76920_Read_1_Reg(ADCOFFSET)*(1000));
}
//=================================================

//=======================================================
void Init_BQ76920(I2C_HandleTypeDef* i2c_ptr){
  
  I2C_handler = *i2c_ptr;

  HAL_I2C_MspInit(&I2C_handler);
  MX_I2C1_Init();
  get_ADC_Info();
}
//=======================================================


// Read and write functions.
//========================================================================================================

// Reads one address from the bms.
//=================================================
uint8_t bq76920_Read_1_Reg(uint16_t Mem_Address){
  uint8_t read_Data;
  HAL_I2C_Mem_Read(&I2C_handler, (DEV_ADD << 1), Mem_Address, MEM_SIZE, &read_Data, DATA_SIZE, TIMEOUT);
  return (read_Data);
}
//=================================================

// Most reads require HI & LO,
// This function collects both and combines them.
//=================================================
uint16_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2){
  uint16_t data_1 = bq76920_Read_1_Reg(Mem_Add_1);
  uint8_t  data_2 = bq76920_Read_1_Reg(Mem_Add_2);
  // MSB is from first reading, LSB from second
  // Data_2 is uint8 to prevent garbage.
  return ((data_1 << 8) | (data_2)); // :-)
}
//=================================================

// Writes to the bms chip, input is address & data.
//===================================================
void bq76920_Write(uint16_t Mem_Address, uint8_t Write_Data){
  HAL_I2C_Mem_Write(&I2C_handler, (DEV_ADD << 1), Mem_Address, MEM_SIZE, &Write_Data, DATA_SIZE, TIMEOUT);
}
//===================================================

// Reads or writes one bit from a specific register
// Input is r/w preference, register, bit in decimal, state
// Returns information if you read, FF if you write
//======================================================================
void bq76920_W_1_bit(uint8_t reg, uint8_t bit_dec, uint8_t state){
  // write to register
  //=================================================
    // need different masks depending on whether you want to set or clear
    uint8_t current = bq76920_Read_1_Reg(reg); // acquire current info
    uint8_t new = current; // defaults to rewriting what was read.
    // edit the read information.
    if(state == HIGH)     new = (current |  (1 << bit_dec));  
    else if(state == LOW) new = (current & ~(1 << bit_dec));  
    // write new information.
    bq76920_Write(reg, new);
  //=================================================
}

uint8_t bq76920_R_1_bit(uint8_t reg, uint8_t bit_dec){
  return ((bq76920_Read_1_Reg(reg) >> (bit_dec))&(0x1));
}
//======================================================================


// Protection and control. 
//========================================================================================================

// Sys read and write
//=====================================================================
uint8_t sys_Read(SysCommands Command){
  // Return bit corresponding to command. 
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
  }
  return 0xff; // command that doesn't exist chosen :(
}

void sys_Write(SysCommands Command, uint8_t State){
  switch(Command){
    // Sys Stat Commands
    //==============================================================
    case CC_READY:      bq76920_W_1_bit(SYS_STAT, 7, State);
    case DEVICE_XREADY: bq76920_W_1_bit(SYS_STAT, 5, State);
    case OVRD_ALERT:    bq76920_W_1_bit(SYS_STAT, 4, State);
    case UV:            bq76920_W_1_bit(SYS_STAT, 3, State);
    case OV:            bq76920_W_1_bit(SYS_STAT, 2, State);
    case SCD:           bq76920_W_1_bit(SYS_STAT, 1, State);
    case OCD:           bq76920_W_1_bit(SYS_STAT, 0, State);
    // Sys Control 1 Commands
    //==============================================================
    case LOAD_PRESENT:  bq76920_W_1_bit(SYS_CTRL1, 7, State);
    case ADC_EN:        bq76920_W_1_bit(SYS_CTRL1, 4, State);
    case TEMP_SEL:      bq76920_W_1_bit(SYS_CTRL1, 3, State);
    case SHUT_A:        bq76920_W_1_bit(SYS_CTRL1, 1, State);
    case SHUT_B:        bq76920_W_1_bit(SYS_CTRL1, 0, State);
    // Sys Control 2 Commands
    //==============================================================
    case DELAY_DIS:     bq76920_W_1_bit(SYS_CTRL2, 7, State);
    case CC_EN:         bq76920_W_1_bit(SYS_CTRL2, 6, State);
    case CC_ONESHOT:    bq76920_W_1_bit(SYS_CTRL2, 5, State);
    case DSG_ON:        bq76920_W_1_bit(SYS_CTRL2, 1, State);
    case CHG_ON:        bq76920_W_1_bit(SYS_CTRL2, 0, State);
  }
}
//=====================================================================

// Protection read / write
//=====================================================================

uint8_t protect_Read(ProtectCommands Command){

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
  };
  return 0xff; // :-(
}

void protect_Write(ProtectCommands Command, uint8_t State){
  
  switch(Command){
    // Protect1 Commands
    //==============================================================
    case RSNS:    bq76920_W_1_bit(PROTECT1, 7, State);
    case SCD_D1:  bq76920_W_1_bit(PROTECT1, 4, State);
    case SCD_D0:  bq76920_W_1_bit(PROTECT1, 3, State);
    case SCD_T2:  bq76920_W_1_bit(PROTECT1, 2, State);
    case SCD_T1:  bq76920_W_1_bit(PROTECT1, 1, State);
    case SCD_T0:  bq76920_W_1_bit(PROTECT1, 0, State);
    // Protect2 Commands
    //==============================================================
    case OCD_D2:  bq76920_W_1_bit(PROTECT2, 6, State);
    case OCD_D1:  bq76920_W_1_bit(PROTECT2, 5, State);
    case OCD_D0:  bq76920_W_1_bit(PROTECT2, 4, State);
    case OCD_T3:  bq76920_W_1_bit(PROTECT2, 3, State);
    case OCD_T2:  bq76920_W_1_bit(PROTECT2, 2, State);
    case OCD_T1:  bq76920_W_1_bit(PROTECT2, 1, State);
    case OCD_T0:  bq76920_W_1_bit(PROTECT2, 0, State);
    // Protect3 Commands
    //==============================================================
    case UV_D1:   bq76920_W_1_bit(PROTECT3, 7, State);
    case UV_D0:   bq76920_W_1_bit(PROTECT3, 6, State);
    case OV_D1:   bq76920_W_1_bit(PROTECT3, 5, State);
    case OV_D0:   bq76920_W_1_bit(PROTECT3, 4, State);
    // OV_Trip Commands
    //==============================================================
    case OV_T7:   bq76920_W_1_bit(OV_TRIP, 7, State);
    case OV_T6:   bq76920_W_1_bit(OV_TRIP, 6, State);
    case OV_T5:   bq76920_W_1_bit(OV_TRIP, 5, State);
    case OV_T4:   bq76920_W_1_bit(OV_TRIP, 4, State);
    case OV_T3:   bq76920_W_1_bit(OV_TRIP, 3, State);
    case OV_T2:   bq76920_W_1_bit(OV_TRIP, 2, State);
    case OV_T1:   bq76920_W_1_bit(OV_TRIP, 1, State);
    case OV_T0:   bq76920_W_1_bit(OV_TRIP, 0, State);
    // UV_Trip Commands
    //==============================================================
    case UV_T7:   bq76920_W_1_bit(UV_TRIP, 7, State);
    case UV_T6:   bq76920_W_1_bit(UV_TRIP, 6, State);
    case UV_T5:   bq76920_W_1_bit(UV_TRIP, 5, State);
    case UV_T4:   bq76920_W_1_bit(UV_TRIP, 4, State);
    case UV_T3:   bq76920_W_1_bit(UV_TRIP, 3, State);
    case UV_T2:   bq76920_W_1_bit(UV_TRIP, 2, State);
    case UV_T1:   bq76920_W_1_bit(UV_TRIP, 1, State);
    case UV_T0:   bq76920_W_1_bit(UV_TRIP, 0, State);
  };
}
//=====================================================================
//========================================================================================================

// Cell Voltage Commands
//========================================================================================================

// This function pulls chip reading and calculates actual voltage.
//====================================================================
uint32_t get_Voltage_1(uint16_t cell){
  // Cell is passed in as 0xabcd, this reads 0xab and 0xcd
  uint16_t cell_Data = bq76920_Read((cell >> 8), (cell & 0xFF));
  // Cell voltage = (ADC READING)*(GAIN) (in uV) + (ADC OFFSET) (in uV).
  // ADC OFFSET CONVERTED TO uV WHEN READ DURING INIT.
  return (((cell_Data)*(ADC_gain)) + ((ADC_offset)));
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

  voltage_array[5] = (4*get_Voltage_1(BAT));
}
//====================================================================

//========================================================================================================