#include <stm32xx_hal.h>
#include <bq76920.h>
#include <bq72920_registers.h>
#include <common.h>
#include <stdlib.h>
#include <stdio.h>
#include "leds.h"
#include "pinConfig.h"
#include "inits.h"
#include "printf.h"
#include "UART.h"


// Testing procedure for REV B boards
// 
// In order to test voltage readings:
//  Define BQ_PRINT_READINGS
//  Flash to a VoltTemp PSOM
//  Connect VoltTemp to the volttemp test pcb (resistor ladder)
//    INPUT VOLTAGE SHOULD BE ~13-14 V
//  Use screen or putty to observe readings (make sure to use 115200 speed)
//  Should print out individual cell readings & total module voltage.

StaticTask_t xTaskBuffer;
StackType_t xStack[2048];

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[512];

// Initialize UART
void Init_Task(void *argument)
{
  // Init UART printf
  UART_Init();

  // Task kills itself :(
  vTaskDelete(NULL);
}

uint32_t cell_Data[6];
uint8_t reg_data;

void Task_ReadBQ(void *pvParameters)
{

  get_ADC_Info();

  while (1)
  {
    
    #define BQ_PRINT_READINGS

    #ifdef BQ_PRINT_READINGS
    get_Voltage_All(cell_Data, BQ_DELAY);
    
    printf("\033[H");
    printf("Voltage Readings:\r\n");
    printf("Cell 1: %ld.%.3ld  [V]\r\n",cell_Data[0]/1000000,(cell_Data[0]%1000000)/1000);
    printf("Cell 2: %ld.%.3ld  [V]\r\n",cell_Data[1]/1000000,(cell_Data[1]%1000000)/1000);
    printf("Cell 3: %ld.%.3ld  [V]\r\n",cell_Data[2]/1000000,(cell_Data[2]%1000000)/1000);
    printf("Cell 4: %ld.%.3ld  [V]\r\n",cell_Data[3]/1000000,(cell_Data[3]%1000000)/1000);
    printf("Total : %ld.%.3ld [V]\r\n",cell_Data[4]/1000000,(cell_Data[4]%1000000)/1000);
    #endif



    #ifdef BQ_PRINT_REG_TEST
    bq76920_Read_1_Reg(SYS_CTRL1,&reg_data,pdMS_TO_TICKS(100));

    printf("RegData  %d\r\n",reg_data);
    
    uint8_t write;
    if(reg_data==24)write = 0;
    else write = 1;
    //sys_Write(TEMP_SEL,write);
    bq76920_W_1_bit(SYS_CTRL1,3,write, BQ_DELAY);
    #endif
	

    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    vTaskDelay(pdMS_TO_TICKS(200));
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
  volttemp_id_led_on();

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
                    2048,
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
