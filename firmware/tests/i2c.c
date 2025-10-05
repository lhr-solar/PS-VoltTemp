#include <stm32f4xx_hal.h>
#include <stdlib.h>

#define devAddress 0x08
#define BAT_HI 0x2A
#define BAT_LO 0x2B
#define memSize 1
#define size 1
#define timeout 100

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

// Heartbeat pin on stm32l432kcu is PB3
#ifdef STM32L432xx
    #define LED_PIN GPIO_PIN_3
    #define LED_PORT GPIOB
#else
// Most other nucleos have a heartbeat for A5
    #define LED_PIN GPIO_PIN_5
    #define LED_PORT GPIOA
#endif


uint8_t Write_Data = 0xEE;
uint8_t Read_Data;


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


//==============================================================================================
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
//==============================================================================================

//==============================================================================================
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
//==============================================================================================

//==============================================================================================
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
//==============================================================================================

//==============================================================================================
static void MX_I2C1_Init(void){

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK){
    Error_Handler();
  }


}
//==============================================================================================

//==============================================================================================
static void MX_GPIO_Init(void){

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}
//==============================================================================================

//==============================================================================================
int main(){
  HAL_Init();

  SystemClock_Config();
  HAL_MspInit();
  HAL_I2C_MspInit(&hi2c1);
  MX_GPIO_Init();
  MX_I2C1_Init();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef led_config = {
      .Mode = GPIO_MODE_OUTPUT_PP,
      .Pull = GPIO_NOPULL,
      .Pin = LED_PIN
  };
  
  HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config

  
  while(1){
    HAL_I2C_Mem_Read(&hi2c1, devAddress << 1, BAT_HI, memSize, &Read_Data, size, timeout);
    HAL_Delay(1);
    HAL_I2C_Mem_Read(&hi2c1, devAddress << 1, BAT_LO, memSize, &Read_Data, size, timeout);
    HAL_Delay(1);

    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    

  }
}
//==============================================================================================