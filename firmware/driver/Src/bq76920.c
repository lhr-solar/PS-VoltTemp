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


//=======================================================
void Init_BQ76920(I2C_HandleTypeDef* i2c_ptr){
  
  I2C_handler = *i2c_ptr;

  HAL_Init();


  HAL_I2C_MspInit(&I2C_handler);
  MX_I2C1_Init();
    
}
//=======================================================


// Read and write functions.
//========================================================================================================
uint8_t bq76920_Read_1(uint16_t Mem_Address){
  uint8_t Read_Data;
  HAL_I2C_Mem_Read(&I2C_handler, DEV_ADD << 1, Mem_Address, MEM_SIZE, &Read_Data, DATA_SIZE, TIMEOUT);
  return Read_Data;
}

uint16_t bq76920_Read(uint16_t Mem_Add_1, uint16_t Mem_Add_2){
  uint16_t Data_1 = bq76920_Read_1(Mem_Add_1);
  uint8_t Data_2 = bq76920_Read_1(Mem_Add_2);
  return ((Data_1 << 8) | (Data_2)); // :-)
}

void bq76920_Write(uint16_t Mem_Address, uint8_t Write_Data){
  HAL_I2C_Mem_Write(&I2C_handler, DEV_ADD << 1, Mem_Address, MEM_SIZE, &Write_Data, DATA_SIZE, TIMEOUT);
}
//========================================================================================================


// Cell Voltage Commands
//========================================================================================================

uint16_t ADC_gain;
int8_t ADC_offset;

void get_ADC_Info(void){
  ADC_gain = bq76920_Read_1(ADCGAIN1);

  ADC_gain += (365);

  ADC_offset = bq76920_Read_1(ADCOFFSET);
}


uint32_t get_Cell_Voltage(uint16_t cell){
  uint16_t cell_Data = bq76920_Read((cell >> 8), (cell & 0xFF));

  return (cell_Data)*(ADC_gain) + ADC_offset;

}

//========================================================================================================