#include "soft_uart_tx.h"
#include "tim.h"

/*
 * TIM period value at 1us resolution
 * 9600   -> (104 - 1) minimum CPU Clock >= 8MHz
 * 19200  -> (52 - 1)  minimum CPU Clock >= 16MHz
 * 38400  -> (26 - 1)  minimum CPU Clock >= 48MHz
 * 57600  -> (18 - 1)  minimum CPU Clock >= 48MHz
 * 115200 -> (9 - 1)   minimum CPU Clock >= 72MHz
 */

void Soft_UART_TX_TIM_Init()
{
    /* timer configured in cube @see tim.c */
}

void Soft_UART_TX_TIM_Start()
{
    HAL_TIM_Base_Start_IT(&htim11);
}

void Soft_UART_TX_TIM_Stop()
{
    HAL_TIM_Base_Stop_IT(&htim11);
}

void TIM11_ISR()
{
    Soft_UART_Timer_ISR();
}
