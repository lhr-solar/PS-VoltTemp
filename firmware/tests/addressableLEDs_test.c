#include "stm32xx_hal.h"
#include "addressableLEDs.h"
#include "leds.h"
#include "WS2812B.h"

StaticTask_t TaskBuffer;
StackType_t TaskStack[configMINIMAL_STACK_SIZE];

void task(){

    clearAddressableColors(portMAX_DELAY);

    setModuleFaultLed(MODULE_1_TEMP_LED, FAULT_COLOR, portMAX_DELAY);

    ws2812b_color_t colors[NUM_ROW_LEDS] = {WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN};
    setRowFaultLed(colors, MODULE_0_VOLTAGE_LED, portMAX_DELAY);
    
    while(1){


        toggle_heartbeat();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(){

    HAL_Init();
    SystemClock_Config();

    leds_init();

    ws2812b_status_t status = addressableLEDInit();
    if(status == WS2812B_OK){
        set_heartbeat_led(ON);
    }


    // Create the tasks statically
    xTaskCreateStatic(task, 
                     "TX",
                     configMINIMAL_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY + 2,
                     TaskStack,
                     &TaskBuffer);

    // Start the scheduler
    vTaskStartScheduler();

    while(1){
        // should never get here
    }

    return 0;

}