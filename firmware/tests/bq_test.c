#include <stm32f4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>

I2C_HandleTypeDef hi2c1;


uint16_t Write_Data = 0xEE;
uint32_t Read_Data;

uint32_t cell_Data[6];


void Testing_Init(){

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

//==============================================================================================
int main(){
  Testing_Init();
  Init_BQ76920(&hi2c1);
  SystemClock_Config();
  HAL_MspInit();
  
  
  while(1){
    
    get_Voltage_All(cell_Data);
    
    vc1_hi_d = bq76920_Read_1_Reg(VC1_HI);
    vc1_lo_d = bq76920_Read_1_Reg(VC1_LO);

    
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
  }
}
//==============================================================================================