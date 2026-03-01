
void SystemClock_Config(void);

void mx_i2c_init(void);

void mx_uart_init(void);


void UART_Init(void);

void MX_GPIO_Init(void);

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef I2C_handler;
extern UART_HandleTypeDef* husart1;