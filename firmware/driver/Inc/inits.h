
// Configure the system clock for the STM32
void SystemClock_Config(void);

// Enable & Configure the I2C for BMS chip
void mx_i2c_init(void);
extern I2C_HandleTypeDef hi2c1;


// Enable UART for printf support
void mx_uart_init(void);
void UART_Init(void);
extern UART_HandleTypeDef* husart1;

// Enable CAN for leader communication
void mx_CAN_init(void);
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan);
extern CAN_HandleTypeDef* hcan1;
void vt_CAN_start(void);