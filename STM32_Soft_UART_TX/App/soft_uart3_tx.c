#include "soft_uart_tx.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"

#define UART3_TX_PIN GPIO_PIN_9
#define UART3_TX_PIN_PORT GPIOB

#define UART3_TX_BUFFER_SIZE 64

uint8_t UART3_TX_Buffer[UART3_TX_BUFFER_SIZE];

Ring_Buffer_t UART3_TX_RB;

Soft_UART_TX_t Soft_UART3_TX;

void Soft_UART3_TX_GPIO_Init()
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(UART3_TX_PIN_PORT, UART3_TX_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = UART3_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(UART3_TX_PIN_PORT, &GPIO_InitStruct);
    */
}

void Soft_UART3_TX_High()
{
    HAL_GPIO_WritePin(UART3_TX_PIN_PORT, UART3_TX_PIN, GPIO_PIN_SET);
}

void Soft_UART3_TX_Low()
{
    HAL_GPIO_WritePin(UART3_TX_PIN_PORT, UART3_TX_PIN, GPIO_PIN_RESET);
}

void Soft_UART3_TX_Init()
{
    Ring_Buffer_Init(&UART3_TX_RB, UART3_TX_Buffer, 1, UART3_TX_BUFFER_SIZE);

    Soft_UART3_TX.TX_Ring_Buffer = &UART3_TX_RB;

    Soft_UART3_TX.TX_GPIO_Init = Soft_UART3_TX_GPIO_Init;
    Soft_UART3_TX.TX_High = Soft_UART3_TX_High;
    Soft_UART3_TX.TX_Low = Soft_UART3_TX_Low;

    Soft_UART_TX_Add(&Soft_UART3_TX);
}
