#include "soft_uart_tx.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"

#define UART1_TX_PIN GPIO_PIN_2
#define UART1_TX_PIN_PORT GPIOA

#define UART1_TX_BUFFER_SIZE 64

uint8_t UART1_TX_Buffer[UART1_TX_BUFFER_SIZE];

Ring_Buffer_t UART1_TX_RB;

Soft_UART_TX_t Soft_UART1_TX;

void Soft_UART1_TX_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(UART1_TX_PIN_PORT, UART1_TX_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = UART1_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(UART1_TX_PIN_PORT, &GPIO_InitStruct);
}

void Soft_UART1_TX_High()
{
    HAL_GPIO_WritePin(UART1_TX_PIN_PORT, UART1_TX_PIN, GPIO_PIN_SET);
}

void Soft_UART1_TX_Low()
{
    HAL_GPIO_WritePin(UART1_TX_PIN_PORT, UART1_TX_PIN, GPIO_PIN_RESET);
}

void Soft_UART1_TX_Init()
{
    Ring_Buffer_Init(&UART1_TX_RB, UART1_TX_Buffer, 1, UART1_TX_BUFFER_SIZE);

    Soft_UART1_TX.TX_Ring_Buffer = &UART1_TX_RB;

    Soft_UART1_TX.TX_GPIO_Init = Soft_UART1_TX_GPIO_Init;
    Soft_UART1_TX.TX_High = Soft_UART1_TX_High;
    Soft_UART1_TX.TX_Low = Soft_UART1_TX_Low;

    Soft_UART_TX_Add(&Soft_UART1_TX);
}
