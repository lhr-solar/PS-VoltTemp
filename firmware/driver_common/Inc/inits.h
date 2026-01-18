
void SystemClock_Config(void);

void mx_i2c_init(void);

void HAL_UART_MspGPIOInit(UART_HandleTypeDef*);

void UART_Init(void);

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef I2C_handler;
extern UART_HandleTypeDef huart1;