#include <stm32xx_hal.h>
#include <bq76920.h>
#include <inits.h>

void I2C1_EV_IRQHandler(void){HAL_I2C_EV_IRQHandler(&hi2c1);}

void I2C1_ER_IRQHandler(void){HAL_I2C_ER_IRQHandler(&hi2c1);}


void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c1){
  BaseType_t xBQHigherPriorityTaskWoken = pdFALSE;
  
  xSemaphoreGiveFromISR(I2C_semaphore, &xBQHigherPriorityTaskWoken);
  
  portYIELD_FROM_ISR(xBQHigherPriorityTaskWoken);
}
