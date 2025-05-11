/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

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
#define OV_SENSE_HV_Pin GPIO_PIN_13
#define OV_SENSE_HV_GPIO_Port GPIOC
#define OC_SENSE_HV_Pin GPIO_PIN_14
#define OC_SENSE_HV_GPIO_Port GPIOC
#define FAULT_12_Pin GPIO_PIN_15
#define FAULT_12_GPIO_Port GPIOC
#define V_SENSE_HV_Pin GPIO_PIN_0
#define V_SENSE_HV_GPIO_Port GPIOA
#define V_SENSE_12_Pin GPIO_PIN_1
#define V_SENSE_12_GPIO_Port GPIOA
#define V_SENSE_HV_IN_Pin GPIO_PIN_2
#define V_SENSE_HV_IN_GPIO_Port GPIOA
#define HV_RELAY_Pin GPIO_PIN_3
#define HV_RELAY_GPIO_Port GPIOA
#define SHDN_12_Pin GPIO_PIN_4
#define SHDN_12_GPIO_Port GPIOA
#define V_SENSE_12_IN_Pin GPIO_PIN_5
#define V_SENSE_12_IN_GPIO_Port GPIOA
#define V_SENSE_5_Pin GPIO_PIN_6
#define V_SENSE_5_GPIO_Port GPIOA
#define I_SENSE_12_Pin GPIO_PIN_7
#define I_SENSE_12_GPIO_Port GPIOA
#define I_SENSE_5_Pin GPIO_PIN_0
#define I_SENSE_5_GPIO_Port GPIOB
#define I_SENSE_HV_Pin GPIO_PIN_1
#define I_SENSE_HV_GPIO_Port GPIOB
#define IND_G_Pin GPIO_PIN_8
#define IND_G_GPIO_Port GPIOA
#define IND_R_Pin GPIO_PIN_9
#define IND_R_GPIO_Port GPIOA
#define IND_B_Pin GPIO_PIN_10
#define IND_B_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define BUTTON_Pin GPIO_PIN_5
#define BUTTON_GPIO_Port GPIOB
#define PRECHG_SSR_Pin GPIO_PIN_6
#define PRECHG_SSR_GPIO_Port GPIOB
#define BOOT0_SENSE_Pin GPIO_PIN_7
#define BOOT0_SENSE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
