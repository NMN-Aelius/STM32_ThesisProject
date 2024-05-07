/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../../thu_vien/Nhut.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
CAN_TxHeaderTypeDef TX_CAN_HEADER;
CAN_RxHeaderTypeDef RX_CAN_HEADER;
CAN_FilterTypeDef canfilterconfig;

uint32_t TxMailBox;
uint8_t Data_CAN_Rx[6];

// ================ CREATE OBJECT MOTOR
struct Parameter_motor
{
	uint8_t name;
	uint8_t data[7];
	bool flag_motor;
}motor1, motor2, motor3, motor4, motor5, motor6;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// ================ DATA UART
#define DATA_UART_AVAILABLE  1
#define DATA_UART_UNAVAILABLE 0
// ================ FLAG TIMER 1
#define FLAG_TIMER1_ON  1
#define FLAG_TIMER1_OFF 0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

//================ DECLARE UART VALUE
uint8_t Data_UART_Receive[43] ;
uint8_t Data_Received;
uint8_t Data_UART_Receive_Lenght = 0;
uint8_t Data_UART_Saved[43];

bool StateUART = DATA_UART_UNAVAILABLE;
bool Check_OKE = false;
bool Flag_Reset = false;

// ================ DECLARE TIMER1 VALUE
bool flag_timer1 = FLAG_TIMER1_OFF;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//===============FUNCTION INTERRUPT TIMER1 (checked)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		flag_timer1 = true;
	}
}
//===============FUNCTION INTERRUPT TIMER2 (checked)
void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
}

//===============FUNCTION INTERRUPT UART (checked)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart4, Data_UART_Receive, 43);
	if(Data_UART_Receive[42] == 's')
	{
		HAL_GPIO_TogglePin(cam_GPIO_Port, cam_Pin);
		StateUART = DATA_UART_AVAILABLE;
		Data_UART_Receive_Lenght = 43;
	}
	else if(Data_UART_Receive[42] == 'r') // Lenh Reset he thong
	{
		HAL_GPIO_TogglePin(do_GPIO_Port, do_Pin);
		Flag_Reset = true;
	}
	else
	{
		StateUART = DATA_UART_UNAVAILABLE;
		for(int i = 0; i < 43; i++)
			Data_UART_Receive[i] = ' ';
	}
}
//===============FUNCTION INTERRUPT CAN MESSAGE (checked)
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RX_CAN_HEADER, Data_CAN_Rx) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		switch(RX_CAN_HEADER.StdId)
		{
		case 0x001:
			motor1.data[0] = Data_CAN_Rx[0];  motor1.data[1] = Data_CAN_Rx[1]; motor1.data[2] = Data_CAN_Rx[2];
			motor1.data[3] = Data_CAN_Rx[3];  motor1.data[4] = Data_CAN_Rx[4]; motor1.data[5] = Data_CAN_Rx[5];
			motor1.flag_motor = true;
			break;
		case 0x002:
			motor2.data[0] = Data_CAN_Rx[0];  motor2.data[1] = Data_CAN_Rx[1]; motor2.data[2] = Data_CAN_Rx[2];
			motor2.data[3] = Data_CAN_Rx[3];  motor2.data[4] = Data_CAN_Rx[4]; motor2.data[5] = Data_CAN_Rx[5];
			motor2.flag_motor= true;
			break;
		case 0x004:
			motor3.data[0] = Data_CAN_Rx[0];  motor3.data[1] = Data_CAN_Rx[1]; motor3.data[2] = Data_CAN_Rx[2];
			motor3.data[3] = Data_CAN_Rx[3];  motor3.data[4] = Data_CAN_Rx[4]; motor3.data[5] = Data_CAN_Rx[5];
			motor3.flag_motor= true;
			break;
		case 0x008:
			motor4.data[0] = Data_CAN_Rx[0];  motor4.data[1] = Data_CAN_Rx[1]; motor4.data[2] = Data_CAN_Rx[2];
			motor4.data[3] = Data_CAN_Rx[3];  motor4.data[4] = Data_CAN_Rx[4]; motor4.data[5] = Data_CAN_Rx[5];
			motor4.flag_motor = true;
			break;
		case 0x010:
			motor5.data[0] = Data_CAN_Rx[0];  motor5.data[1] = Data_CAN_Rx[1]; motor5.data[2] = Data_CAN_Rx[2];
			motor5.data[3] = Data_CAN_Rx[3];  motor5.data[4] = Data_CAN_Rx[4]; motor5.data[5] = Data_CAN_Rx[5];
			motor5.flag_motor= true;
			break;
		case 0x020:
			motor6.data[0] = Data_CAN_Rx[0];  motor6.data[1] = Data_CAN_Rx[1]; motor6.data[2] = Data_CAN_Rx[2];
			motor6.data[3] = Data_CAN_Rx[3];  motor6.data[4] = Data_CAN_Rx[4]; motor6.data[5] = Data_CAN_Rx[5];
			motor6.flag_motor = true;
			break;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

	//========================UART RECEIVE IT
	HAL_UART_Init(&huart4);
	HAL_UART_Receive_IT(&huart4, Data_UART_Receive, 43);

	//========================CAN BUS CONFIG
	/*----------------- CAN START TX AND RX ----------*/
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // Enable interrupts
	/*----------------- ENABLE TIMER 1----------*/
	HAL_TIM_Base_Start_IT(&htim1);
	/*----------------- ENABLE TIMER 2----------*/
	HAL_TIM_Base_Start(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	motor1.flag_motor = false;motor1.name = 'a';
	motor2.flag_motor = false; 	motor2.name = 'b';
	motor3.flag_motor = false;motor3.name = 'c';
	motor4.flag_motor = false; motor4.name = 'd';
	motor5.flag_motor = false; motor5.name = 'e';
	motor6.flag_motor = false;motor6.name = 'f';
	while(1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(flag_timer1 == true)
		{
			uint8_t length = 0;
			flag_timer1 = false;
			if(StateUART == DATA_UART_AVAILABLE)
			{
				length = Data_UART_Receive_Lenght;
				Data_UART_Receive_Lenght = 0; // Not Reset - > failed
				for(int i = 0; i <= length; i++)
				{
					Data_UART_Saved[i] = Data_UART_Receive[i];
				}
				StateUART = DATA_UART_UNAVAILABLE;
				Check_OKE = true;
				// this function return length
			}
			if(Check_OKE == true && Flag_Reset == false)
			{
				Check_OKE = false;
				uint8_t theta1[6] = {' ', ' ', ' ', ' ', ' ', ' '},
						theta2[6] = {' ', ' ', ' ', ' ', ' ', ' '},
						theta3[6] = {' ', ' ', ' ', ' ', ' ', ' '},
						theta4[6] = {' ', ' ', ' ', ' ', ' ', ' '},
						theta5[6] = {' ', ' ', ' ', ' ', ' ', ' '},
						theta6[6] = {' ', ' ', ' ', ' ', ' ', ' '};
				// Function handles (checked)
				Split_String_UART(Data_UART_Saved, length, theta1, theta2, theta3, theta4, theta5, theta6);

				// Send 6 massage CAN (checked)
				TX_CAN_HEADER.StdId = 0x001; // Node 1
				if(HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta1, &TxMailBox) == HAL_OK)
				{
					HAL_GPIO_TogglePin(xanhl_GPIO_Port, xanhl_Pin);
				}
				delay_us(300);

				TX_CAN_HEADER.StdId = 0x002;// Node 2
				if(HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta2, &TxMailBox) == HAL_OK)
				{
					HAL_GPIO_TogglePin(G_GPIO_Port, G_Pin);
				}
				delay_us(300);

				TX_CAN_HEADER.StdId = 0x004;// Node 3
				if(HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta3, &TxMailBox) == HAL_OK)
				{
					HAL_GPIO_TogglePin(R_GPIO_Port, R_Pin);
				}
				delay_us(300);

				TX_CAN_HEADER.StdId = 0x008;// Node 3
				if(HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta4, &TxMailBox) == HAL_OK)
				{
					HAL_GPIO_TogglePin(xanhd_GPIO_Port, xanhd_Pin);
				}
				delay_us(300);

				TX_CAN_HEADER.StdId = 0x010;// Node 3
				HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta5, &TxMailBox);
				delay_us(300);

				TX_CAN_HEADER.StdId = 0x020;// Node 3
				HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, theta6, &TxMailBox);
				delay_us(500);

				//=======================SET RUN SYNCHRONOUS

				TX_CAN_HEADER.StdId = 0x000;
				uint8_t Start_6_motor[8] = {'s', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
				HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, Start_6_motor, &TxMailBox);
			}
			else if (Flag_Reset == true) //(checked)
			{
				Flag_Reset = false;
				//----------------------------- Message reset----------------------
				TX_CAN_HEADER.StdId = 0x000;
				uint8_t Start_6_motor[8] = {'r', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
				HAL_CAN_AddTxMessage(&hcan1, &TX_CAN_HEADER, Start_6_motor, &TxMailBox);
			}

		}
		if(motor1.flag_motor == true && motor2.flag_motor == true && motor3.flag_motor == true && motor4.flag_motor == true&& motor5.flag_motor == true && motor6.flag_motor == true)
//		if(motor1.flag_motor == true )
		{
			motor1.flag_motor = false;
			motor2.flag_motor = false;
			motor3.flag_motor = false;
			motor4.flag_motor = false;
			motor5.flag_motor = false;
			motor6.flag_motor = false;

			uint8_t Data_encoder[43];
			Handles_UART_6_motor(motor1.data, motor2.data, motor3.data, motor4.data, motor5.data, motor6.data, Data_encoder);

			HAL_UART_Transmit(&huart4, Data_encoder, 43, 5);
		}
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */
  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */
  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 7;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
	/*----------------------- Configuration CAN Filter---------------
	 * */
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 1; // which filter bank to use from the assigned ones
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canfilterconfig.FilterIdHigh = 0x0000 << 5;
	canfilterconfig.FilterIdLow = 0x0000;
	canfilterconfig.FilterMaskIdHigh = 0xff00 << 5;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 14;
	if(HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig) != HAL_OK) // add
	{
		Error_Handler();
	}
	/* -----------------------------CAN Transmit and Interrrupts ----------------------------- */
	TX_CAN_HEADER.RTR=CAN_RTR_DATA; //Remote transmission request = Data frame
	TX_CAN_HEADER.IDE=CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TX_CAN_HEADER.DLC=6;
  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 83;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 19999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 167;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, xanhl_Pin|cam_Pin|do_Pin|xanhd_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, R_Pin|G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : xanhl_Pin cam_Pin do_Pin xanhd_Pin */
  GPIO_InitStruct.Pin = xanhl_Pin|cam_Pin|do_Pin|xanhd_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : R_Pin G_Pin */
  GPIO_InitStruct.Pin = R_Pin|G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
