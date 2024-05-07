/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

CAN_TxHeaderTypeDef TX_CAN_HEADER;
CAN_RxHeaderTypeDef RX_CAN_HEADER;
CAN_FilterTypeDef canfilterconfig;

//=============MODE
#define AC_SERVO 0
#define DC_MOTOR 1

bool Mode = AC_SERVO;

//=============DEBUG
struct DebugV
{
	uint16_t count; // for debug
	uint32_t position; // for debug
};

//=============MOTOR
#define GearboxAC_DtoP 4000*1/360 //Manual setup
#define GearboxStep_DtoP 816/90 // gear box of step motor
#define PosToDeg 1/45.56424581005592 // convert position to angle

int Angle; //Current Angle
float previousAngle = 0; // Update angle
bool Run = false;
//=============UART
#define TxBufferSize 4  //Transmit Data
#define RxBufferSize 10 //Receive Data Position

#define UART_OK 1
#define UART_NOT_OK 0

uint8_t TxDataUart[TxBufferSize];
uint8_t RxDataUart[RxBufferSize];

int sign; // xet dau cho goc tu step encoder

bool Flag_Uart = UART_OK;

//=============CAN
uint8_t CAN_Data_Rx[6]; // du lieu nhan tu can
uint32_t TxMailBox;

float pulseEnd = 0; //luu xung cho lan tiep
bool Check_Data = 0; //kiem tra du lie

uint8_t Data_Decode[6]; // Array temp for data send to master
char Data_Saved[8]; //du lieu duoc luu vao sau khi nhan tu CAN
bool flag_send = false; // cho phep gui du lieu
bool flag_run = false;

//=============TIMER1
bool flag_timer1 = false;
bool flag_enable_send = false;

uint8_t countSampleTime = 0; // time for sampling
uint8_t countSendData = 0; // time for send data to CAN

//=============END=============

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
volatile short ExternalPulse;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//=================STEP MOTOR ENCODER (Checked)
uint8_t getCheckSum(uint8_t *buffer,uint8_t size)
{
	uint16_t sum=0;
	for(uint8_t i = 0 ;i < size;i++)
	{
		sum += buffer[i];
	}
	return(sum&0xFF);
}
void ReadUart(uint8_t l_sAddress)
{
	TxDataUart[0] = 0xFA;
	TxDataUart[1] = l_sAddress;
	TxDataUart[2] = 0x31; // Position read mode int32 (4 bytes data last)
	TxDataUart[3] = getCheckSum(TxDataUart,TxBufferSize-1);

	HAL_UART_Transmit_IT(&huart1, TxDataUart, TxBufferSize);
}
uint32_t DecodeData(uint8_t *input)
{
	uint32_t readValue = (int32_t)(
			((uint32_t)input[0] << 24)    |
			((uint32_t)input[1] << 16)    |
			((uint32_t)input[2] << 8)     |
			((uint32_t)input[3] << 0));
	return readValue;
}

//=================ENCODER DATA (Checked)
void EncodeDataDC(uint8_t dataSend[])
{
	if(Check_Data == 1) // du lieu hop le ?
	{
		float Theta = (float)(PosToDeg*DecodeData(&RxDataUart[5]));
		Angle = Theta*100; // lam tron goc thanh kieu INT
		Check_Data = 0;
	}
	int IntValue = abs(Angle/100);
	int DecValue = abs(Angle%100);

	dataSend[0] = DecValue/1%10;
	dataSend[1] = DecValue/10%10;

	dataSend[2] = IntValue/1%10;
	dataSend[3] = IntValue/10%10;
	dataSend[4] = IntValue/100%10;

	if(sign < 0) dataSend[5] = '-';
	else 		  dataSend[5] = '+';
}
void EncodeDataAC(uint8_t dataSend[])
{
	ExternalPulse = __HAL_TIM_GET_COUNTER(&htim2);
	Angle = ExternalPulse*GearboxAC_DtoP;// AC- x100: 4000 Pulse / Cycle

	int IntValue = abs(Angle/100);
	int DecValue = abs(Angle%100);

	dataSend[0] = DecValue/1%10;
	dataSend[1] = DecValue/10%10;

	dataSend[2] = IntValue/1%10;
	dataSend[3] = IntValue/10%10;
	dataSend[4] = IntValue/100%10;

	if(Angle < 0) dataSend[5] = '-';
	else	      dataSend[5] = '+';
}

//=================TIMER4 COUNTER MODE DELAY (checked)
void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim4,0);
	while (__HAL_TIM_GET_COUNTER(&htim4) < us);
}

//=================CONTROL DC MOTOR (checked tempt)
void ForwardDC(uint16_t l_pulseIn, uint16_t timeDelay)
{
	HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
	for(uint16_t i =0; i < l_pulseIn; i++)
	{
		HAL_GPIO_WritePin(STP_GPIO_Port, STP_Pin, GPIO_PIN_RESET);
		delay_us(timeDelay);
		HAL_GPIO_WritePin(STP_GPIO_Port, STP_Pin, GPIO_PIN_SET);
		delay_us(timeDelay);
	}
}
void InverseDC(uint16_t l_pulseIn, uint16_t timeDelay)
{
	HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
	for(uint16_t i = 0; i < l_pulseIn; i++)
	{
		HAL_GPIO_WritePin(STP_GPIO_Port, STP_Pin, GPIO_PIN_RESET);
		delay_us(timeDelay);
		HAL_GPIO_WritePin(STP_GPIO_Port, STP_Pin, GPIO_PIN_SET);
		delay_us(timeDelay);
	}
}

//=================CONTROL AC SERVO (checked tempt)
void Create_pulse_Forward_AC(uint32_t pulse_in, uint32_t time_delay)
{
	HAL_GPIO_WritePin(GPIOB, NG_Pin, GPIO_PIN_RESET);
	for (int i = 0; i < pulse_in; i++)
	{
		HAL_GPIO_WritePin(GPIOB, PG_Pin, GPIO_PIN_RESET);
		delay_us(time_delay);
		HAL_GPIO_WritePin(GPIOB, PG_Pin, GPIO_PIN_SET);
		delay_us(time_delay);
	}
}
void Create_pulse_Inverse_AC(uint32_t pulse_in, uint32_t time_delay)
{
	HAL_GPIO_WritePin(GPIOB, NG_Pin, GPIO_PIN_SET);
	for (int i = 0; i < pulse_in; i++)
	{
		HAL_GPIO_WritePin(GPIOB, PG_Pin, GPIO_PIN_RESET);
		delay_us(time_delay);
		HAL_GPIO_WritePin(GPIOB, PG_Pin, GPIO_PIN_SET);
		delay_us(time_delay);
	}
}

//=================MODE CONTROL AC SERVO/DC MOTOR (Checked)
void Control_Motor(float DELTA)
{
	if(Mode == AC_SERVO)
	{
		float deltaPulse = DELTA*GearboxAC_DtoP;
		pulseEnd += deltaPulse - (int32_t)deltaPulse;
		int32_t pulseSupply = deltaPulse + (int32_t)pulseEnd/1;

		pulseEnd -= (int32_t)pulseEnd/1;
		if(pulseSupply > 0)
		{
			Create_pulse_Forward_AC(abs(pulseSupply), 100);
		}
		else
		{
			Create_pulse_Inverse_AC(abs(pulseSupply), 100);
		}
	}
	else // DC Motor
	{
		float deltaPulse = DELTA*GearboxStep_DtoP;
		pulseEnd += deltaPulse - (int32_t)deltaPulse;
		int32_t pulseSupply = deltaPulse + (int32_t)pulseEnd/1;

		pulseEnd -= (int32_t)pulseEnd/1;
		if(deltaPulse > 0)
		{
			ForwardDC(abs(pulseSupply), 100);
		}
		else
		{
			InverseDC(abs(pulseSupply), 100);
		}
	}
}

//=================TIMER1 TIMER MODE SAMPLE TIME (checked)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(HAL_UART_Receive_IT(&huart1, RxDataUart, RxBufferSize) == HAL_OK)
	{
		Flag_Uart = UART_OK;
		if(RxDataUart[0] == 251 && RxDataUart[1] == 1 && RxDataUart [2] == 49)
		{
			Check_Data = 1;
			if(RxDataUart[3] == 255)
			{
				RxDataUart[3] = (~RxDataUart[3]) & 0xFF;
				RxDataUart[4] = (~RxDataUart[4]) & 0xFF;
				RxDataUart[5] = (~RxDataUart[5]) & 0xFF;
				RxDataUart[6] = (~RxDataUart[6]) & 0xFF;
				RxDataUart[7] = (~RxDataUart[7]) & 0xFF;
				RxDataUart[8] = (~RxDataUart[8]) & 0xFF;
				RxDataUart[9] = getCheckSum(RxDataUart, RxBufferSize-1);
				sign = -1;
			}
			else
			{
				sign = 1;
			}
		}
		else
		{
			memset(RxDataUart, 0x00,  RxBufferSize);
			ReadUart(1);
			Flag_Uart = UART_NOT_OK;
		}
	}
}

//=================TIMER1 TIMER MODE SAMPLE TIME (checked)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		countSampleTime++;
		countSendData++;
		if(countSampleTime == 3)
		{
			flag_timer1 = true;
			countSampleTime = 0;
		}
		if(countSendData == 1)
		{
			flag_enable_send = true;
			countSendData = 0;
		}
	}
}
//=================TIMER2 EXTERNAL COUNTER MODE ENCODER (Checked)
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		ExternalPulse = __HAL_TIM_GET_COUNTER(&htim2);
	}
}

//=================INTERRUPT CAN RECEIVE MESSAGE (Checked)
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_GPIO_TogglePin(purple_led1_GPIO_Port, purple_led1_Pin);
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RX_CAN_HEADER, CAN_Data_Rx) == HAL_OK)
	{
		switch(RX_CAN_HEADER.StdId)
		{
		case 0x001:
			Data_Saved[0] = CAN_Data_Rx[0];
			Data_Saved[1] = CAN_Data_Rx[1];
			Data_Saved[2] = CAN_Data_Rx[2];
			Data_Saved[3] = CAN_Data_Rx[3];
			Data_Saved[4] = CAN_Data_Rx[4];
			Data_Saved[5] = CAN_Data_Rx[5];
			Data_Saved[6] = CAN_Data_Rx[6];
			Data_Saved[7] = CAN_Data_Rx[7];
			break;
		case 0x000:
			if(CAN_Data_Rx[0] == 's')
			{
				flag_run = true;
			}
			if(CAN_Data_Rx[0] == 'r')
			{
				TIM2->CNT=0;
				ExternalPulse = 0;
				Angle = 0;
				previousAngle = 0;
			}
			break;
		}
	}
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void updateEncoder()
{
	if(Mode == AC_SERVO)
	{
		// Update the value of counter encoder
		ExternalPulse = __HAL_TIM_GET_COUNTER(&htim2);
	}
	else
	{
		if(Flag_Uart == UART_OK)
		{
			ReadUart(1);
			Flag_Uart = UART_NOT_OK;
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
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_TIM4_Init();
	MX_CAN_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	//========CAN
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); // Enable interrupts

	//========TIMER AND COUNTER
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_Base_Start_IT(&htim1);

	//========UART
	HAL_UART_Receive_IT(&huart1, RxDataUart, RxBufferSize);
	HAL_Delay(100);

	//========ENCODER
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_1|TIM_CHANNEL_2);

	//========SET DEFAULT PIN MODE
	HAL_GPIO_WritePin(GPIOB, PG_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, NG_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(purple_led1_GPIO_Port, purple_led1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(purple_led2_GPIO_Port, purple_led2_Pin, GPIO_PIN_SET);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if(flag_timer1 == true)
		{
			flag_timer1 = false;
			if(flag_run == true)
			{
				flag_run = false;
				float Theta_temp = atoi(Data_Saved);
				float deltaAngle = Theta_temp/100 - previousAngle;
				Control_Motor(deltaAngle);
				previousAngle += deltaAngle;
				flag_send = true;
			}
		}

		// UPDATE ENCODER AC SERVO OR STEP MOTOR
		updateEncoder();

		//==========UPDATE INFO FOR MASTER AND SLAVE 2
		if(flag_send == true && flag_enable_send == true)
		{
			//Down-flag for next time.
			flag_send = false;
			flag_enable_send = false;


			if(Mode == AC_SERVO) EncodeDataAC(Data_Decode);
			else EncodeDataDC(Data_Decode);

			TX_CAN_HEADER.StdId = 0x001; //Send to Master
			if(HAL_CAN_AddTxMessage(&hcan, &TX_CAN_HEADER, Data_Decode, &TxMailBox) == HAL_OK)
			{
				HAL_GPIO_TogglePin(purple_led2_GPIO_Port, purple_led2_Pin);
			}
			delay_us(300);

			TX_CAN_HEADER.StdId = 0x000; //Send to all
			uint8_t RunCode;
			RunCode = '2'; // RunCode for Motor2
			HAL_CAN_AddTxMessage(&hcan, &TX_CAN_HEADER, &RunCode, &TxMailBox);
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

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief CAN Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN_Init(void)
{

	/* USER CODE BEGIN CAN_Init 0 */

	/* USER CODE END CAN_Init 0 */

	/* USER CODE BEGIN CAN_Init 1 */

	/* USER CODE END CAN_Init 1 */
	hcan.Instance = CAN1;
	hcan.Init.Prescaler = 6;
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan.Init.TimeSeg1 = CAN_BS1_2TQ;
	hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
	hcan.Init.TimeTriggeredMode = DISABLE;
	hcan.Init.AutoBusOff = DISABLE;
	hcan.Init.AutoWakeUp = DISABLE;
	hcan.Init.AutoRetransmission = DISABLE;
	hcan.Init.ReceiveFifoLocked = DISABLE;
	hcan.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CAN_Init 2 */
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 1; // which filter bank to use from the assigned ones
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; //
	canfilterconfig.FilterIdHigh = 0x001 << 5;
	canfilterconfig.FilterIdLow = 0x000;
	canfilterconfig.FilterMaskIdHigh = 0xffe << 5;
	canfilterconfig.FilterMaskIdLow = 0x000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 1;

	if(HAL_CAN_ConfigFilter(&hcan, &canfilterconfig) != HAL_OK) // add
	{
		Error_Handler();
	}
	/* -----------------------------CAN Transmit and Interrrupts ----------------------------- */
	TX_CAN_HEADER.RTR= CAN_RTR_DATA; //Remote transmission request = Data frame
	TX_CAN_HEADER.IDE= CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TX_CAN_HEADER.DLC= 6;
	/* USER CODE END CAN_Init 2 */
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
	htim1.Init.Prescaler = 71;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 999;
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

	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void)
{

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 71;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 65535;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

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
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, purple_led1_Pin|purple_led2_Pin|a6_Pin|a7_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, EN_Pin|STP_Pin|DIR_Pin|b11_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, NG_Pin|NP_Pin|PG_Pin|PP_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : purple_led1_Pin purple_led2_Pin a6_Pin a7_Pin */
	GPIO_InitStruct.Pin = purple_led1_Pin|purple_led2_Pin|a6_Pin|a7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : EN_Pin STP_Pin DIR_Pin b11_Pin */
	GPIO_InitStruct.Pin = EN_Pin|STP_Pin|DIR_Pin|b11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : NG_Pin NP_Pin PG_Pin PP_Pin */
	GPIO_InitStruct.Pin = NG_Pin|NP_Pin|PG_Pin|PP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
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
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
