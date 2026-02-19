#include <stm32l4xx_hal.h>
#include <bq76920.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "UART.h"


// Testing procedure for REV B 
// 
// For the pre-assembled boards, the first test will be seeing if REGOUT is working correctly.
// If it is, we should be getting the steady 3.3, as opposed to random values jumping around.

// Assuming it is steady, the next step is to test each board with the known working code,
// that is the code with no task implementation.
//
// Once each board is verified alone, we need to test them together; in theory they should just work as normal.
//
// After we know multiple volt-temps can work together, we need to finish implementing the task compatible code.
// (I was unable to finish this the other workday sadly)
//
// Upon finishing that code, we should repeat the steps of testing each volt temp and then chaining them together.
//
// After we have several volt-temps working, with the tasks, we should make any final tweaks needed before ordering
// the remaining boards :-)

StaticTask_t xTaskBuffer;
StackType_t xStack[configMINIMAL_STACK_SIZE];

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];

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

  // Task kills itself
  vTaskDelete(NULL);
}

uint32_t cell_Data[6];


void Task_ReadBQ(void *pvParameters)
{

  get_ADC_Info();

  while (1)
  {
    get_Voltage_All(cell_Data);
    
    printf("\033[H");
    printf("Voltage Readings:\r\n");
    printf("Cell 1: %ld\r\n",cell_Data[0]);
    printf("Cell 2: %ld\r\n",cell_Data[1]);
    printf("Cell 3: %ld\r\n",cell_Data[2]);
    printf("Cell 4: %ld\r\n",cell_Data[3]);
    printf("Total_: %ld\r\n",cell_Data[4]);

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}



int main()
{
  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK)
    Error_Handler();
  SystemClock_Config();

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  leds_init();
  mx_i2c_init();
  // turn on psom leds to show volttemp number
  volttemp_led_on();

  mx_uart_init();

  // init the chip with these i2c pins.
  Init_BQ76920();

  xTaskCreateStatic(Init_Task,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    initTaskStack,
                    &initTaskBuffer);

  xTaskCreateStatic(Task_ReadBQ,
                    "BQ Test",
                    512,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    xStack,
                    &xTaskBuffer);

  vTaskStartScheduler();

  while (1)
  {
  }

  return 0;
}
