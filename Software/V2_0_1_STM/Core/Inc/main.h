/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HSE_16MHZ_Pin GPIO_PIN_0
#define HSE_16MHZ_GPIO_Port GPIOA
#define LDR_sense_Pin GPIO_PIN_1
#define LDR_sense_GPIO_Port GPIOA
#define Therm_sense_Pin GPIO_PIN_4
#define Therm_sense_GPIO_Port GPIOA
#define Display_SCK_Pin GPIO_PIN_5
#define Display_SCK_GPIO_Port GPIOA
#define Display_RCK_Pin GPIO_PIN_6
#define Display_RCK_GPIO_Port GPIOA
#define Display_MOSI_Pin GPIO_PIN_7
#define Display_MOSI_GPIO_Port GPIOA
#define Display_PWM_Pin GPIO_PIN_0
#define Display_PWM_GPIO_Port GPIOB
#define Heater_PWM_Pin GPIO_PIN_1
#define Heater_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
