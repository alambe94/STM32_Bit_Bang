#include "soft_uart.h"

#define SOFT_UART_TIM htim4
#define MAX_SOFT_UART  10

extern TIM_HandleTypeDef htim4;

static uint8_t Soft_Uart_Count = 0;

Soft_Uart_t *Soft_Uart_List[MAX_SOFT_UART];

void Soft_Uart_Init(Soft_Uart_t *uart_handle)
    {
    /**** TIM configured in cube to generate interrupt every 1/baud rate *********/
    /**** GPIO pin configured in cube *********/

    if (Soft_Uart_Count < MAX_SOFT_UART)
	{

	uart_handle->RC_Flag = 0;
	uart_handle->RX_Active_Flag = 0;
	uart_handle->RX_Bit_Count = 0;
	uart_handle->RX_Byte = 0x00;
	uart_handle->RX_Ring_Buffer.Read_Index = 0;
	uart_handle->RX_Ring_Buffer.Write_Index = 0;

	uart_handle->TC_Flag = 0;
	uart_handle->TX_Active_Flag = 0;
	uart_handle->TX_Bit_Count = 0;
	uart_handle->TX_Byte = 0;
	uart_handle->TX_Ring_Buffer.Read_Index = 0;
	uart_handle->TX_Ring_Buffer.Write_Index = 0;

	Soft_Uart_List[Soft_Uart_Count] = uart_handle;
	Soft_Uart_Count++;
	}
    }

/**** TIM configured in cube to generate interrupt every 1/baud rate *********/
/*9600 baud*/
void Soft_Uart_TIM_ISR()
    {

    Soft_Uart_t *uart_handle = NULL;
    uint8_t uart_active_count = 0;

    for (uint8_t i = 0; i < Soft_Uart_Count; i++)
	{

	/* grab uart handle from list*/
	uart_handle = Soft_Uart_List[i];

	if (!uart_handle->TX_Active_Flag)
	    {
	    if (Ring_Buffer_Get_Count(&uart_handle->TX_Ring_Buffer))
		{
		uart_handle->TX_Active_Flag = 1;
		}
	    else
		{
		uart_handle->TC_Flag = 1;
		}
	    }

	if (uart_handle->TX_Active_Flag)
	    {

	    uart_active_count++;

	    if (!(uart_handle->TX_Bit_Count)) //start bit
		{

		Ring_Buffer_Get_Char(&uart_handle->TX_Ring_Buffer,
			&uart_handle->TX_Byte);

		HAL_GPIO_WritePin(uart_handle->GPIO_TX_Port,
			uart_handle->GPIO_TX_Pin, GPIO_PIN_RESET);

		uart_handle->TX_Bit_Count++;
		}
	    else if (uart_handle->TX_Bit_Count < 9) //data frame
		{

		if ((uart_handle->TX_Byte >> (uart_handle->TX_Bit_Count - 1))
			& 0x01)
		    {

		    HAL_GPIO_WritePin(uart_handle->GPIO_TX_Port,
			    uart_handle->GPIO_TX_Pin, GPIO_PIN_SET);

		    }
		else
		    {

		    HAL_GPIO_WritePin(uart_handle->GPIO_TX_Port,
			    uart_handle->GPIO_TX_Pin, GPIO_PIN_RESET);

		    }
		uart_handle->TX_Bit_Count++;

		}
	    else //stop bit
		{
		HAL_GPIO_WritePin(uart_handle->GPIO_TX_Port,
			uart_handle->GPIO_TX_Pin, GPIO_PIN_SET);
		uart_handle->TX_Bit_Count = 0;
		uart_handle->TX_Active_Flag = 0;
		}
	    }

	}

    // if all soft_uart are idle, disable tim interrupt
    if (!uart_active_count)
	{
	//HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM);
	}

    }

void Soft_Uart_Send_Char(Soft_Uart_t *uart_handle, char data)
    {
    Ring_Buffer_Put_Char(&uart_handle->TX_Ring_Buffer, data);
    //HAL_TIM_Base_Start_IT(&SOFT_UART_TIM);
    }

void Soft_Uart_Send_String(Soft_Uart_t *uart_handle, char *str)
    {
    while (*str != '\0')
	{
	Ring_Buffer_Put_Char(&uart_handle->TX_Ring_Buffer, *str++);
	}
    //HAL_TIM_Base_Start_IT(&SOFT_UART_TIM);
    }

uint8_t Soft_Uart_Get_TC_Flag(Soft_Uart_t *uart_handle)
    {
    return uart_handle->TC_Flag;
    }

void Soft_Uart_Clear_TC_Flag(Soft_Uart_t *uart_handle)
    {
    uart_handle->TC_Flag = 0;
    }

void Soft_Uart_Start()
    {
    HAL_TIM_Base_Start_IT(&SOFT_UART_TIM);
    }

void Soft_Uart_Stop()
    {
    HAL_TIM_Base_Stop_IT(&SOFT_UART_TIM);
    }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
    if (htim == &SOFT_UART_TIM)
	{
	Soft_Uart_TIM_ISR();
	}
    }
