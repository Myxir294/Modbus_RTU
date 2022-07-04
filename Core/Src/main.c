/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


typedef enum{S_IDLE, S_ADDRESS, S_FUNCTION_DATA, S_FUNCTION_OK, S_FUNCTION_FAIL, S_WAIT_EOF, S_UNKNOWN_FUNCTION, S_CRC_CHECK1, S_CRC_CHECK2, S_CRC_ERROR} SM_STATE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

char test_read_payload[4] = {0xAA, 0xAA, 0xAA, 0xAA};
char test_write_payload[6] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char function_to_execute;
int expected_length;
uint16_t self_crc = 0xFFFF;
char pom_komunikat[] = "Wykryto nieoczekiwany koniec ramki";
//char pom_komunikat2[] = "Niezgodnosc odebranego CRC z wyliczonym";
void maszyna_stanu(char ch, bool koniec_ramki)
{
	static SM_STATE stan = S_IDLE;
	static char payload[252];
	static int payload_counter = 0;
	static char self_address = 0x11;
	static char pom_crc1;
	if(koniec_ramki && stan != S_WAIT_EOF && stan != S_CRC_CHECK2)
	{
		stan = S_WAIT_EOF;
		HAL_UART_Transmit(&huart2, (uint8_t*)pom_komunikat, strlen(pom_komunikat), HAL_MAX_DELAY);
		payload_counter = 0;
		memset(payload, 0, 255 * sizeof(int));
		function_to_execute = 0x00;
	}
	switch(stan)
	{
		case S_IDLE:
			if(ch == self_address)
			{
				self_crc = 0xFFFF;
				stan = S_ADDRESS;
				crc16_update(&self_crc, ch);
			}
			else
			{
				stan = S_WAIT_EOF;
			}
			break;

		case S_WAIT_EOF:
			if(koniec_ramki)
			{
				stan = S_IDLE;
			}
			break;
		case S_ADDRESS:
			if(ch == 0x02)
			{
				function_to_execute = 0x02;
				expected_length = 4;
				stan = S_FUNCTION_DATA;
			}
			else if(ch == 0x05)
			{
				function_to_execute = 0x05;
				expected_length = 6;
				stan = S_FUNCTION_DATA;
			}
			else
			{
				stan = S_UNKNOWN_FUNCTION;
			}
			crc16_update(&self_crc, ch);
			break;

		case S_UNKNOWN_FUNCTION:
			self_crc = 0xFFFF;
			put_tx(self_address);
			crc16_update(&self_crc, self_address);

			put_tx(0x82);
			crc16_update(&self_crc, 0x82);

			put_tx(0x01);
			crc16_update(&self_crc, 0x01);

			uint8_t crc2  = (uint8_t) (self_crc & 0x00FF);
			uint8_t crc1  = (uint8_t) (self_crc >> 8 & 0x00FF);

			put_tx(crc1);
			put_tx(crc2);
			stan = S_WAIT_EOF;
			break;

		case S_FUNCTION_DATA:
			payload[payload_counter++] = ch;
			crc16_update(&self_crc, ch);
			if(payload_counter == expected_length)
			{
				stan = S_CRC_CHECK1;
				payload_counter = 0;
			}
			break;
		case S_CRC_CHECK1:
			pom_crc1 = ch;
			stan = S_CRC_CHECK2;
			break;
		case S_CRC_CHECK2:
			if (self_crc != ((pom_crc1 << 8) | (ch & 0xFF)))
			{
				stan = S_FUNCTION_FAIL;
			}
			else
			{
				if(function_to_execute == 0x02)
				{
					for(int i = 0; i < expected_length; i++)
					{
						if(payload[i] != test_read_payload[i])
						{
							stan = S_FUNCTION_FAIL;
						}
					}
					if(stan != S_FUNCTION_FAIL)
					{
						self_crc = 0xFFFF;
						put_tx(self_address);
						crc16_update(&self_crc, self_address);

						put_tx(0x02);
						crc16_update(&self_crc, 0x82);

						put_tx(0xAA);
						crc16_update(&self_crc, 0xAA);

						put_tx(0xAA);
						crc16_update(&self_crc, 0xAA);

						uint8_t crc2  = (uint8_t) (self_crc & 0x00FF);
						uint8_t crc1  = (uint8_t) (self_crc >> 8 & 0x00FF);

						put_tx(crc1);
						put_tx(crc2);
						//stan = S_FUNCTION_OK;
					}
					//stan = S_IDLE;
				}
				else if(function_to_execute == 0x05)
				{
					for(int i = 0; i < expected_length; i++)
					{
						if(payload[i] != test_write_payload[i])
						{
							stan = S_FUNCTION_FAIL;
						}
					}
					if(stan != S_FUNCTION_FAIL)
					{
						self_crc = 0xFFFF;
						put_tx(self_address);
						crc16_update(&self_crc, self_address);

						put_tx(0x02);
						crc16_update(&self_crc, 0x82);

						put_tx(0xBB);
						crc16_update(&self_crc, 0xBB);

						put_tx(0xBB);
						crc16_update(&self_crc, 0xBB);

						uint8_t crc2  = (uint8_t) (self_crc & 0x00FF);
						uint8_t crc1  = (uint8_t) (self_crc >> 8 & 0x00FF);

						put_tx(crc1);
						put_tx(crc2);
						//stan = S_FUNCTION_OK;
					}
					//stan = S_IDLE;
				}
			}
			if(stan == S_FUNCTION_FAIL)
			{
				self_crc = 0xFFFF;
				put_tx(self_address);
				crc16_update(&self_crc, self_address);

				put_tx(0x82);
				crc16_update(&self_crc, 0x82);

				put_tx(function_to_execute);
				crc16_update(&self_crc, function_to_execute);

				uint8_t crc2  = (uint8_t) (self_crc & 0x00FF);
				uint8_t crc1  = (uint8_t) (self_crc >> 8 & 0x00FF);

				put_tx(crc1);
				put_tx(crc2);
			}
			stan = S_IDLE;
			break;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  	char znak;
	serial_init(&huart2);
	int pom_dlugosc_ramki = 0;
	int max = 7;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while(serial_available())
	  {
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  znak = serial_read();
		  if(znak == 0x02){max = 7;}
		  if(znak == 0x05){max = 9;}
		  if(pom_dlugosc_ramki >= max)
		  {
			  maszyna_stanu(znak, true);
			  pom_dlugosc_ramki = 0;
		  }
		  else
		  {
			  maszyna_stanu(znak, false);
			  pom_dlugosc_ramki++;
		  }
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_Pin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

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
