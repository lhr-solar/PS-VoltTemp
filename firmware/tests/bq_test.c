#include <stm32l4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include "leds.h"
#include "pinConfig.h"

// I2C pins for PSOM
#define I2C_PORT GPIOB
#define I2C_SCL_PIN GPIO_PIN_6
#define I2C_SDA_PIN GPIO_PIN_7

I2C_HandleTypeDef hi2c1;

#ifdef STM32L431xx
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif

uint8_t sysdat;
uint8_t vc1_lo_d;
uint8_t vc1_hi_d;
uint32_t VC1_data;
uint32_t cell_Data[6];

int main() {
  HAL_Init();
  SystemClock_Config();
  leds_init();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // // Heartbeat LED init
  // GPIO_InitTypeDef hb_init = {
  //     .Mode = GPIO_MODE_OUTPUT_PP,
  //     .Pull = GPIO_NOPULL,
  //     .Pin = GPIO_PIN_11,
  // };
  // HAL_GPIO_Init(GPIOB, &hb_init);
  HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);

  // turn on psom leds to show volttemp number
  volttemp_led_on();

  // init the chip with these i2c pins.
  Init_BQ76920(&hi2c1, I2C_PORT, I2C_SCL_PIN, I2C_SDA_PIN);

  while (1) {
    get_Voltage_All(cell_Data);
    sysdat = sys_Read(OV);

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    HAL_Delay(100);
  }
}
