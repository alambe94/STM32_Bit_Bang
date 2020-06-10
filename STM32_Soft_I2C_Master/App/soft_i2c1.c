#include "soft_i2c_master.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"

#define I2C1_SDA_PIN GPIO_PIN_9
#define I2C1_SDA_PIN_PORT GPIOB

#define I2C1_SCL_PIN GPIO_PIN_8
#define I2C1_SCL_PIN_PORT GPIOB

#define I2C1_RX_BUFFER_SIZE 512
#define I2C1_TX_BUFFER_SIZE 512

uint8_t I2C1_RX_Buffer[I2C1_RX_BUFFER_SIZE];
uint8_t I2C1_TX_Buffer[I2C1_TX_BUFFER_SIZE];

Ring_Buffer_t I2C1_RX_RB;
Ring_Buffer_t I2C1_TX_RB;

Soft_I2C_Master_t Soft_I2C1;

void Soft_I2C1_GPIO_Init()
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(I2C1_SDA_PIN_PORT, I2C1_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C1_SCL_PIN_PORT, I2C1_SCL_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = I2C1_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(I2C1_SDA_PIN_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = I2C1_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(I2C1_SCL_PIN_PORT, &GPIO_InitStruct);
    */

    /* configured in cube @see gpio.c */
}

void Soft_I2C1_SDA_High()
{
    HAL_GPIO_WritePin(I2C1_SDA_PIN_PORT, I2C1_SDA_PIN, GPIO_PIN_SET);
}

void Soft_I2C1_SDA_Low()
{
    HAL_GPIO_WritePin(I2C1_SDA_PIN_PORT, I2C1_SDA_PIN, GPIO_PIN_RESET);
}

void Soft_I2C1_SCL_High()
{
    HAL_GPIO_WritePin(I2C1_SCL_PIN_PORT, I2C1_SCL_PIN, GPIO_PIN_SET);
}

void Soft_I2C1_SCL_Low()
{
    HAL_GPIO_WritePin(I2C1_SCL_PIN_PORT, I2C1_SCL_PIN, GPIO_PIN_RESET);
}

uint8_t Soft_I2C1_SDA_Read()
{
    return HAL_GPIO_ReadPin(I2C1_SDA_PIN_PORT, I2C1_SDA_PIN);
}

void Soft_I2C1_Callback(Soft_I2C_Master_Flags_t flag)
{
}

void Soft_I2C1_Init()
{
    Ring_Buffer_Init(&I2C1_RX_RB, I2C1_RX_Buffer, 1, I2C1_RX_BUFFER_SIZE);
    Ring_Buffer_Init(&I2C1_TX_RB, I2C1_TX_Buffer, 1, I2C1_TX_BUFFER_SIZE);

    Soft_I2C1.I2C_RX_Ring_Buffer = &I2C1_RX_RB;
    Soft_I2C1.I2C_TX_Ring_Buffer = &I2C1_TX_RB;

    Soft_I2C1.Soft_I2C_GPIO_Init = Soft_I2C1_GPIO_Init;

    Soft_I2C1.Soft_I2C_SCL_High = Soft_I2C1_SCL_High;
    Soft_I2C1.Soft_I2C_SCL_Low = Soft_I2C1_SCL_Low;

    Soft_I2C1.Soft_I2C_SDA_High = Soft_I2C1_SDA_High;
    Soft_I2C1.Soft_I2C_SDA_Low = Soft_I2C1_SDA_Low;

    Soft_I2C1.Soft_I2C_SDA_Read = Soft_I2C1_SDA_Read;
    Soft_I2C1.callback = Soft_I2C1_Callback;

#if (I2C_USE_INTERRUPT_MODE)
    Soft_I2C_Master_Add(&Soft_I2C1);
#else
    Soft_I2C_Master_Init(&Soft_I2C1);
#endif
}
