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
#include "stm32l4xx_hal.h"
#include "cmsis_os2.h"

#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dma.h"

#include "stm32l4xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nu_uart.h"
#include "ar2_data.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern osThreadId_t meterTaskHandle;
extern osThreadId_t nrfTaskHandle;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define METER_TX_Pin GPIO_PIN_2
#define METER_TX_GPIO_Port GPIOA
#define METER_RX_Pin GPIO_PIN_3
#define METER_RX_GPIO_Port GPIOA
#define METER_EN_Pin GPIO_PIN_4
#define METER_EN_GPIO_Port GPIOA
#define RESERVED3_Pin GPIO_PIN_13
#define RESERVED3_GPIO_Port GPIOB
#define NRF_REV2_Pin GPIO_PIN_14
#define NRF_REV2_GPIO_Port GPIOB
#define NRF_REV1_Pin GPIO_PIN_15
#define NRF_REV1_GPIO_Port GPIOB
#define NRF_TX_Pin GPIO_PIN_9
#define NRF_TX_GPIO_Port GPIOA
#define NRF_RX_Pin GPIO_PIN_10
#define NRF_RX_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_7
#define LED3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define LED1_ON()				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_OFF()				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED1_TOGGLE()  			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)
#define LED2_ON()				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF()				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED2_TOGGLE()  			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)
#define LED3_ON()				HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_OFF()				HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)
#define LED3_TOGGLE()   		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin)

/* RS-485 direction control:
 * METER_EN drives the transceiver DE/RE (implementation-specific wiring).
 * TX_ENABLE = drive bus, RX_ENABLE = listen.
 */
#define RS485_TX_ENABLE()		HAL_GPIO_WritePin(METER_EN_GPIO_Port, METER_EN_Pin, GPIO_PIN_SET)
#define RS485_RX_ENABLE()		HAL_GPIO_WritePin(METER_EN_GPIO_Port, METER_EN_Pin, GPIO_PIN_RESET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
