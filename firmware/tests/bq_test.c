#include <stm32f4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>

I2C_HandleTypeDef hi2c1;


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

void Testing_Init(void){

  SystemClock_Config();

  HAL_Init();

  // Heartbeat pin on stm32l432kcu is PB3
  #ifdef STM32L432xx
      #define LED_PIN GPIO_PIN_3
      #define LED_PORT GPIOB
  #else
  // Most other nucleos have a heartbeat for A5
      #define LED_PIN GPIO_PIN_5
      #define LED_PORT GPIOA
  #endif

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };
      
    MX_GPIO_Init();

    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
}


uint8_t sysdat;
uint8_t vc1_lo_d;
uint8_t vc1_hi_d;
uint32_t VC1_data;
uint32_t cell_Data[6];

//==============================================================================================
int main(){
  // init misc stuff like sysclk
  Testing_Init();

  HAL_MspInit();

  // init the chip with these i2c pins.
  Init_BQ76920(&hi2c1, GPIOB, GPIO_PIN_6, GPIO_PIN_7);

  while(1){
    
    get_Voltage_All(cell_Data);
    
    sysdat = sys_Read(OV);

    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    HAL_Delay(100);
    
  }
}
//==============================================================================================