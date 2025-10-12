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
int8_t ADC_offset;  // ranges from -128 to 127

void get_ADC_Info(void){
  // Gain is a minimum of 365 uV, can be set higher
  ADC_gain = bq76920_Read_1(ADCGAIN1) + (365);

  ADC_offset = bq76920_Read_1(ADCOFFSET);
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
uint8_t bq76920_Read_1(uint16_t Mem_Address){
  uint8_t read_Data;
  HAL_I2C_Mem_Read(&I2C_handler, (DEV_ADD << 1), Mem_Address, MEM_SIZE, &read_Data, DATA_SIZE, TIMEOUT);
  return (read_Data);
}
//=================================================

// Most reads require HI & LO,
// This function collects both and combines them.
//=================================================
uint16_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2){
  uint16_t data_1 = bq76920_Read_1(Mem_Add_1);
  uint8_t  data_2 = bq76920_Read_1(Mem_Add_2);
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

//========================================================================================================

// System Control interface.
//=====================================================================
// Input R/W preference, desired sysctrl bit and state
// Returns information if you read, FF if you write F0 if fail.
uint8_t sys(uint8_t R_W, SysCommands Command, uint8_t state){
  // default values, LOAD_PRESENT, read command.
  uint8_t  bit  = 0x80;
  uint16_t reg = SYS_CTRL1;
  // Choose correct bit & register to interact with.
  switch(Command){
    // Stat commands.
    //==============================================================
    case CC_READY:      reg = SYS_STAT;  bit = 0x80; break; // bit 7
    case DEVICE_XREADY: reg = SYS_STAT;  bit = 0x20; break; // bit 5
    case OVRD_ALERT:    reg = SYS_STAT;  bit = 0x10; break; // bit 4
    case UV:            reg = SYS_STAT;  bit = 0x08; break; // bit 3
    case OV:            reg = SYS_STAT;  bit = 0x04; break; // bit 2
    case SCD:           reg = SYS_STAT;  bit = 0x02; break; // bit 1
    case OCD:           reg = SYS_STAT;  bit = 0x01; break; // bit 0
    // Ctrl1 commands.
    //==============================================================
    case LOAD_PRESENT:  reg = SYS_CTRL1; bit = 0x80; break; // bit 7
    case ADC_EN:        reg = SYS_CTRL1; bit = 0x10; break; // bit 4
    case TEMP_SEL:      reg = SYS_CTRL1; bit = 0x08; break; // bit 3
    case SHUT_A:        reg = SYS_CTRL1; bit = 0x02; break; // bit 1
    case SHUT_B:        reg = SYS_CTRL1; bit = 0x01; break; // bit 0
    // Ctrl2 commands.
    //==============================================================
    case DELAY_DIS:     reg = SYS_CTRL2; bit = 0x80; break; // bit 7
    case CC_EN:         reg = SYS_CTRL2; bit = 0x40; break; // bit 6
    case CC_ONESHOT:    reg = SYS_CTRL2; bit = 0x20; break; // bit 5
    case DSG_ON:        reg = SYS_CTRL2; bit = 0x02; break; // bit 1
    case CHG_ON:        reg = SYS_CTRL2; bit = 0x01; break; // bit 0
  };
  

  // Read or write to sys.
  if(state == READ){
    // we only care about the chosen bit
    // return its value to deal with in main code.
    return (bq76920_Read_1(reg) & bit);
  }else
  if(state == WRITE){
    // if not reading, write.
    bq76920_Write(reg, bit);
    return 0xFF; // we wrote :-)
  }else return 0xF0; // if you return this, invalid input likely.
}
//=====================================================================



// Cell Voltage Commands
//========================================================================================================

// This function pulls chip reading and calculates actual voltage.
//====================================================================
uint32_t get_Voltage_1(uint16_t cell){
  uint16_t cell_Data = bq76920_Read((cell >> 8), (cell & 0xFF));

  // Cell voltage = (ADC READING)*(GAIN) (in uV) + (ADC OFFSET) (in mV)
  // Convert offset to uV
  return (((cell_Data)*(ADC_gain)) + ((ADC_offset)*(1000)));
}
//====================================================================

// Populates an array with cell voltages.
//====================================================================
void get_Voltage_All(uint32_t* voltage_array){
  voltage_array[0] = get_Voltage_1(VC1);
  voltage_array[1] = get_Voltage_1(VC2);
  // VC3 is skipped, as it is the shorted cell for our application.
  voltage_array[2] = get_Voltage_1(VC4);
  voltage_array[3] = get_Voltage_1(VC5);
  
  // Total voltage stored at end of array.
  voltage_array[4] = get_Voltage_1(BAT);
}
//====================================================================

//========================================================================================================