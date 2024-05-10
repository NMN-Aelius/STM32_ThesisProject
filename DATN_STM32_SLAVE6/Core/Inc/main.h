/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define purple_led1_Pin GPIO_PIN_2
#define purple_led1_GPIO_Port GPIOA
#define purple_led2_Pin GPIO_PIN_3
#define purple_led2_GPIO_Port GPIOA
#define a6_Pin GPIO_PIN_6
#define a6_GPIO_Port GPIOA
#define a7_Pin GPIO_PIN_7
#define a7_GPIO_Port GPIOA
#define EN_Pin GPIO_PIN_0
#define EN_GPIO_Port GPIOB
#define STP_Pin GPIO_PIN_1
#define STP_GPIO_Port GPIOB
#define DIR_Pin GPIO_PIN_10
#define DIR_GPIO_Port GPIOB
#define b11_Pin GPIO_PIN_11
#define b11_GPIO_Port GPIOB
#define NG_Pin GPIO_PIN_12
#define NG_GPIO_Port GPIOB
#define NP_Pin GPIO_PIN_13
#define NP_GPIO_Port GPIOB
#define PG_Pin GPIO_PIN_14
#define PG_GPIO_Port GPIOB
#define PP_Pin GPIO_PIN_15
#define PP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
