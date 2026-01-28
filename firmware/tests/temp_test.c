#include <stm32f4xx_hal.h>
#include <temp_read.h>
#include <common.h>
#include <stdlib.h>

ADC_InitTypeDef TEMP_ADC;


int main(){

  temp_init(TEMP_ADC, hadc1);
 
  while(1){}
  return 1;
}