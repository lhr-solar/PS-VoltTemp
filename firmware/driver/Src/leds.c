#include "leds.h"

static void volttemp_leds_init(void);
static void heartbeat_init(void);

void leds_init(void){
    volttemp_leds_init();
    heartbeat_init();
}

static void volttemp_leds_init(void){
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = (PSOM_LED1_PIN)
    };
    gpio_clock_enable((uint32_t)PSOM_LED1_PORT);
    HAL_GPIO_Init(PSOM_LED1_PORT, &led_config);
    
    led_config.Pin = PSOM_LED2_PIN;
    HAL_GPIO_Init(PSOM_LED2_PORT, &led_config);
    
    led_config.Pin = PSOM_LED3_PIN;
    gpio_clock_enable((uint32_t)PSOM_LED3_PORT);
    HAL_GPIO_Init(PSOM_LED3_PORT, &led_config);

}

static void heartbeat_init(void){
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = (HEARTBEAT_PIN)
    };
    gpio_clock_enable((uint32_t)HEARTBEAT_PORT);
    HAL_GPIO_Init(HEARTBEAT_PORT, &led_config);
}

void toggle_heartbeat(void){
    HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
}

void set_heartbeat_led(State pin_state){
    HAL_GPIO_WritePin(HEARTBEAT_PORT, HEARTBEAT_PIN, (pin_state == ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void volttemp_led_on(void){
    // Turn off all LEDs first
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0

    #ifdef VOLTTEMP_0
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0

    #elif defined(VOLTTEMP_1)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0

    #elif defined(VOLTTEMP_2)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0
    
    #elif defined(VOLTTEMP_3)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0

    #elif defined(VOLTTEMP_4)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);     // 1

    #elif defined(VOLTTEMP_5)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);   // 0
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);     // 1

    #elif defined(VOLTTEMP_6)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);   // 0

    #elif defined(VOLTTEMP_7)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);     // 1
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);     // 1

    #endif
}