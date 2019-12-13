/*
 * soft_uart.h
 *
 *  Created on: 20-Apr-2019
 *      Author: medprime
 */

#ifndef SOFT_UART_H_
#define SOFT_UART_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "ring_buffer.h"


typedef struct Soft_Uart_t
    {

        Ring_Buffer_t TX_Ring_Buffer;
        Ring_Buffer_t RX_Ring_Buffer;

	GPIO_TypeDef* GPIO_TX_Port;
	uint16_t GPIO_TX_Pin;

	GPIO_TypeDef* GPIO_RX_Port;
	uint16_t GPIO_RX_Pin;

	uint8_t TX_Bit_Count;
	uint8_t RX_Bit_Count;
	char    TX_Byte;
	char    RX_Byte;

	uint8_t TX_Active_Flag;
	uint8_t RX_Active_Flag;

	uint8_t TC_Flag; //transmission complete flag
	uint8_t RC_Flag;

    } Soft_Uart_t;

    void Soft_Uart_Init(Soft_Uart_t* handle);
    void Soft_Uart_Send_Char(Soft_Uart_t* uart_handle, char data);
    void Soft_Uart_Send_String(Soft_Uart_t* uart_handle, char* str);
    void Soft_Uart_Start();
    void Soft_Uart_Stop();
    uint8_t Soft_Uart_Get_TC_Flag(Soft_Uart_t* uart_handle);
    void Soft_Uart_Clear_TC_Flag(Soft_Uart_t* uart_handle);




#endif /* SOFT_UART_H_ */
