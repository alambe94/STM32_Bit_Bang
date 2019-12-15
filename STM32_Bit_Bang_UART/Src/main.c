/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "soft_uart.h"
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

/* USER CODE BEGIN PV */
Soft_Uart_TX_t UART1;
char UART1_TX_Buffer[32];
char UART1_RX_Buffer[32];

Soft_Uart_TX_t UART2;
char UART2_TX_Buffer[32];
char UART2_RX_Buffer[32];

Soft_Uart_TX_t UART3;
char UART3_TX_Buffer[32];
char UART3_RX_Buffer[32];

Soft_Uart_TX_t UART4;
char UART4_TX_Buffer[32];
char UART4_RX_Buffer[32];

Soft_Uart_TX_t UART5;
char UART5_TX_Buffer[32];
char UART5_RX_Buffer[32];

Soft_Uart_TX_t UART6;
char UART6_TX_Buffer[32];
char UART6_RX_Buffer[32];

Soft_Uart_TX_t UART7;
char UART7_TX_Buffer[32];
char UART7_RX_Buffer[32];

Soft_Uart_TX_t UART8;
char UART8_TX_Buffer[32];
char UART8_RX_Buffer[32];

Soft_Uart_TX_t UART9;
char UART9_TX_Buffer[32];
char UART9_RX_Buffer[32];

Soft_Uart_TX_t UART10;
char UART10_TX_Buffer[32];
char UART10_RX_Buffer[32];




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  UART1.GPIO_TX_Pin           = UART_TX1_Pin;
  UART1.GPIO_TX_Port          = UART_TX1_GPIO_Port;
  UART1.TX_Ring_Buffer.Buffer = UART1_TX_Buffer;
  UART1.TX_Ring_Buffer.Size   = sizeof(UART1_TX_Buffer);


  UART2.GPIO_TX_Pin           = UART_TX2_Pin;
  UART2.GPIO_TX_Port          = UART_TX2_GPIO_Port;
  UART2.TX_Ring_Buffer.Buffer = UART2_TX_Buffer;
  UART2.TX_Ring_Buffer.Size   = sizeof(UART2_TX_Buffer);


  UART3.GPIO_TX_Pin           = UART_TX3_Pin;
  UART3.GPIO_TX_Port          = UART_TX3_GPIO_Port;
  UART3.TX_Ring_Buffer.Buffer = UART3_TX_Buffer;
  UART3.TX_Ring_Buffer.Size   = sizeof(UART3_TX_Buffer);


  UART4.GPIO_TX_Pin           = UART_TX4_Pin;
  UART4.GPIO_TX_Port          = UART_TX4_GPIO_Port;
  UART4.TX_Ring_Buffer.Buffer = UART4_TX_Buffer;
  UART4.TX_Ring_Buffer.Size   = sizeof(UART4_TX_Buffer);


  UART5.GPIO_TX_Pin           = UART_TX5_Pin;
  UART5.GPIO_TX_Port          = UART_TX5_GPIO_Port;
  UART5.TX_Ring_Buffer.Buffer = UART5_TX_Buffer;
  UART5.TX_Ring_Buffer.Size   = sizeof(UART5_TX_Buffer);


  UART6.GPIO_TX_Pin           = UART_TX6_Pin;
  UART6.GPIO_TX_Port          = UART_TX6_GPIO_Port;
  UART6.TX_Ring_Buffer.Buffer = UART6_TX_Buffer;
  UART6.TX_Ring_Buffer.Size   = sizeof(UART6_TX_Buffer);


  UART7.GPIO_TX_Pin           = UART_TX7_Pin;
  UART7.GPIO_TX_Port          = UART_TX7_GPIO_Port;
  UART7.TX_Ring_Buffer.Buffer = UART7_TX_Buffer;
  UART7.TX_Ring_Buffer.Size   = sizeof(UART7_TX_Buffer);


  UART8.GPIO_TX_Pin           = UART_TX8_Pin;
  UART8.GPIO_TX_Port          = UART_TX8_GPIO_Port;
  UART8.TX_Ring_Buffer.Buffer = UART8_TX_Buffer;
  UART8.TX_Ring_Buffer.Size   = sizeof(UART8_TX_Buffer);


  UART9.GPIO_TX_Pin           = UART_TX9_Pin;
  UART9.GPIO_TX_Port          = UART_TX9_GPIO_Port;
  UART9.TX_Ring_Buffer.Buffer = UART9_TX_Buffer;
  UART9.TX_Ring_Buffer.Size   = sizeof(UART9_TX_Buffer);


  UART10.GPIO_TX_Pin           = UART_TX10_Pin;
  UART10.GPIO_TX_Port          = UART_TX10_GPIO_Port;
  UART10.TX_Ring_Buffer.Buffer = UART10_TX_Buffer;
  UART10.TX_Ring_Buffer.Size   = sizeof(UART10_TX_Buffer);



  Soft_Uart_TX_Add(&UART1);
  Soft_Uart_TX_Add(&UART2);
  Soft_Uart_TX_Add(&UART3);
  Soft_Uart_TX_Add(&UART4);
  Soft_Uart_TX_Add(&UART5);
  Soft_Uart_TX_Add(&UART6);
  Soft_Uart_TX_Add(&UART7);
  Soft_Uart_TX_Add(&UART8);
  Soft_Uart_TX_Add(&UART9);
  Soft_Uart_TX_Add(&UART10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      Soft_Uart_Send_String(&UART1, "Hello World! from UART1\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART2, "Hello World! from UART2\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART3, "Hello World! from UART3\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART4, "Hello World! from UART4\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART5, "Hello World! from UART5\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART6, "Hello World! from UART6\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART7, "Hello World! from UART7\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART8, "Hello World! from UART8\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART9, "Hello World! from UART9\r\n");
      //HAL_Delay(1);
      Soft_Uart_Send_String(&UART10, "Hello World! from UART10\r\n");

    /* wait for transmission to complete*/
      while(!Soft_Uart_Get_TC_Flag(&UART10))
          {
          }

      Soft_Uart_Clear_TC_Flag(&UART1);
      Soft_Uart_Clear_TC_Flag(&UART2);
      Soft_Uart_Clear_TC_Flag(&UART3);
      Soft_Uart_Clear_TC_Flag(&UART4);
      Soft_Uart_Clear_TC_Flag(&UART5);
      Soft_Uart_Clear_TC_Flag(&UART6);
      Soft_Uart_Clear_TC_Flag(&UART7);
      Soft_Uart_Clear_TC_Flag(&UART8);
      Soft_Uart_Clear_TC_Flag(&UART9);
      Soft_Uart_Clear_TC_Flag(&UART10);


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

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
