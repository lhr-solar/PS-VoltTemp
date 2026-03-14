#include "stm32xx_hal.h"
#include "leds.h"


int main(){
    HAL_Init();

    leds_init();

    while(1){

        toggle_led(PSOM_HEARTBEAT);
        toggle_led(BQ_HEARTBEAT);
        toggle_led(BQ_FAULT);
        HAL_Delay(500);
    }

    return 0;
}
