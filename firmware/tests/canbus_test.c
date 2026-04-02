#include "stm32xx_hal.h"
#include "leds.h"
#include "canbus.h"

StaticTask_t task_TaskBuffer;
StackType_t task_Stack[configMINIMAL_STACK_SIZE];

static void task_1(void *pvParameters) {

    can_status_t stat = canbus_init();
    if(stat != CAN_OK){
        set_led(BQ_FAULT, ON);
    }

    int test_id = 0x321;

    // create payload to send
    CAN_TxHeaderTypeDef tx_header = {0};   
    tx_header.StdId = test_id;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.IDE = CAN_ID_STD;
    tx_header.DLC = 2;
    tx_header.TransmitGlobalTime = DISABLE;
    
    // send two payloads to 0x1
    uint8_t tx_data[8] = {0};
    tx_data[0] = 0x01;
    tx_data[1] = 0x00;

    while(1){
        toggle_heartbeat();
        can_status_t stat = canbus_send(&tx_header, tx_data, portMAX_DELAY);
        if(stat == CAN_OK){
            set_led(BQ_HEARTBEAT, ON);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(){
    HAL_Init();

    SystemClock_Config();

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    leds_init();

    volttemp_id_led_on();


    xTaskCreateStatic(
            task_1,
            "tx_task 1",
            configMINIMAL_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + 2,
            task_Stack,
            &task_TaskBuffer);
            
    vTaskStartScheduler();

    while(1){

    }

}