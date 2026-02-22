#include <stm32l4xx_hal.h>
#include <temp_read.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "UART.h"

ADC_InitTypeDef TEMP_ADC;

StaticTask_t tempTaskBuffer;
StackType_t tempStack[2048];

StaticTask_t blinkyTaskBuffer;
StackType_t blinkyStack[512];

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[512];

// Initialize UART and EMC2305
void Init_Task(void *argument)
{
  // Init UART printf
  husart1->Init.BaudRate = 115200;
  husart1->Init.WordLength = UART_WORDLENGTH_8B;
  husart1->Init.StopBits = UART_STOPBITS_1;
  husart1->Init.Parity = UART_PARITY_NONE;
  husart1->Init.Mode = UART_MODE_TX_RX;
  husart1->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart1->Init.OverSampling = UART_OVERSAMPLING_16;

  printf_init(husart1);

  GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };
    
  Heartbeat_Clock_Init(); // enable clock for LED_PORT
  HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config

  // Task kills itself
  vTaskDelete(NULL);
}



uint32_t cell_Data[6];
uint8_t reg_data;

void Task_ReadTemp(void *pvParameters)
{

  get_ADC_Info();

  while (1)
  {
    
    get_Voltage_All(cell_Data);
    /*
    printf("\033[H");
    printf("Voltage Readings:\r\n");
    printf("Cell 1: %ld.%.3ld  [V]\r\n",cell_Data[0]/1000000,(cell_Data[0]%1000000)/1000);
    printf("Cell 2: %ld.%.3ld  [V]\r\n",cell_Data[1]/1000000,(cell_Data[1]%1000000)/1000);
    printf("Cell 3: %ld.%.3ld  [V]\r\n",cell_Data[2]/1000000,(cell_Data[2]%1000000)/1000);
    printf("Cell 4: %ld.%.3ld  [V]\r\n",cell_Data[3]/1000000,(cell_Data[3]%1000000)/1000);
    printf("Total : %ld.%.3ld [V]\r\n",cell_Data[4]/1000000,(cell_Data[4]%1000000)/1000);
    */

    bq76920_Read_1_Reg(SYS_CTRL1,&reg_data);

    printf("RegData  %d\r\n",reg_data);
    
    uint8_t write;
    if(reg_data==24)write = 0;
    else write = 1;
    //sys_Write(TEMP_SEL,write);
    bq76920_W_1_bit(SYS_CTRL1,3,write);

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


void Task_Blinky(void *argument)
{
  while(1){
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

  return 0;
}


int main(){

  temp_init(TEMP_ADC, hadc1);
 

  if (HAL_Init() != HAL_OK)
    Error_Handler();

  SystemClock_Config();

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  leds_init();
  // turn on psom leds to show volttemp number
  volttemp_led_on();

  mx_uart_init();


  xTaskCreateStatic(Init_Task,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    initTaskStack,
                    &initTaskBuffer);

  xTaskCreateStatic(Task_ReadTemp,
                    "Temp Test",
                    2048,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    tempStack,
                    &tempTaskBuffer);
                    
  xTaskCreateStatic(Task_Blinky,
                    "Blinky",
                    512,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    blinkyStack,
                    &blinkyTaskBuffer);

  vTaskStartScheduler();

  while (1)
  {
  }

  return 0;
}