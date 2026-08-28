#include "stm32xx_hal.h"
#include "addressableLEDs.h"
#include "leds.h"
#include "WS2812B.h"
#include "inits.h"

StaticTask_t TaskBuffer;
StackType_t TaskStack[configMINIMAL_STACK_SIZE];

void task(){

    ws2812b_status_t status = WS2812B_ERROR;
    // status = clearAddressableColors(portMAX_DELAY);

    while(1){

        status = setModuleFaultLed(MODULE_0_TEMP_LED, WS2812B_SOLID_RED, portMAX_DELAY);

        // ws2812b_color_t colors[NUM_ROW_LEDS] = {WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN, WS2812B_SOLID_GREEN};

        // setRowFaultLed(colors, MODULE_0_VOLTAGE_LED, portMAX_DELAY);

        toggle_heartbeat();

        set_led(BQ_HEARTBEAT, status == WS2812B_OK ? ON : OFF);

        vTaskDelay(pdMS_TO_TICKS(1000));    
    }
}

int main(){

    HAL_Init();
    SystemClock_Config();

    leds_init();

    volttemp_id_led_on();

    ws2812b_status_t status = addressableLEDInit();
    if(status == WS2812B_OK){
        set_heartbeat_led(ON);

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
    }


    while(1){
        // should never get here
    }

    return 0;

}