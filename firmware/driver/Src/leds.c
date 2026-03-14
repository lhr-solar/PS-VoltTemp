#include "leds.h"

static void volttemp_id_leds_init(void);
static void heartbeat_init(void);
static void bq_status_init();


void leds_init(void){
    volttemp_id_leds_init();
    heartbeat_init();
    bq_status_init();
}

void set_led(voltemp_leds_t led, State pin_state){
    switch (led){
        case PSOM_HEARTBEAT:
            HAL_GPIO_WritePin(HEARTBEAT_PORT, HEARTBEAT_PIN, (pin_state == ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case BQ_HEARTBEAT:
            HAL_GPIO_WritePin(HEARTBEAT_PORT, HEARTBEAT_PIN, (pin_state == ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case BQ_FAULT:
            HAL_GPIO_WritePin(HEARTBEAT_PORT, HEARTBEAT_PIN, (pin_state == ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void toggle_led(voltemp_leds_t led){
    switch (led){
        case PSOM_HEARTBEAT:
            HAL_GPIO_TogglePin(HEARTBEAT_PORT, HEARTBEAT_PIN);
            break;
        case BQ_HEARTBEAT:
            HAL_GPIO_TogglePin(BQ_HEARTBEAT_PORT, BQ_HEARTBEAT_PIN);
            break;
        case BQ_FAULT:
            HAL_GPIO_TogglePin(BQ_FAULT_PORT, BQ_FAULT_PIN);
            break;
        default:
            break;
    }
}

static void bq_status_init(void){
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = (BQ_HEARTBEAT_PIN)
    };
    gpio_clock_enable((uint32_t)BQ_HEARTBEAT_PORT);
    HAL_GPIO_Init(BQ_HEARTBEAT_PORT, &led_config);

    led_config.Pin = BQ_FAULT_PIN;
    gpio_clock_enable((uint32_t)BQ_FAULT_PORT);
    HAL_GPIO_Init(BQ_FAULT_PORT, &led_config);
}

static void volttemp_id_leds_init(void){
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

void volttemp_id_led_on(void){
    // Turn off all LEDs first
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_RESET);

    #ifdef VOLTTEMP_0
    // No pins set
    #elif defined(VOLTTEMP_1)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_2)
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_3)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_4)
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_5)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_6)
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);

    #elif defined(VOLTTEMP_7)
    HAL_GPIO_WritePin(PSOM_LED1_PORT, PSOM_LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSOM_LED2_PORT, PSOM_LED2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSOM_LED3_PORT, PSOM_LED3_PIN, GPIO_PIN_SET);

    #endif
}