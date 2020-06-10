#include "soft_i2c_master.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"

#define I2C5_RX_BUFFER_SIZE 512
#define I2C5_TX_BUFFER_SIZE 512

uint8_t I2C5_RX_Buffer[I2C5_RX_BUFFER_SIZE];
uint8_t I2C5_TX_Buffer[I2C5_TX_BUFFER_SIZE];

Ring_Buffer_t I2C5_RX_RB;
Ring_Buffer_t I2C5_TX_RB;

Soft_I2C_Master_t Soft_I2C5;

void Soft_I2C5_GPIO_Init()
{
}

void Soft_I2C5_SDA_High()
{
}

void Soft_I2C5_SDA_Low()
{
}

void Soft_I2C5_SCL_High()
{
}

void Soft_I2C5_SCL_Low()
{
}

uint8_t Soft_I2C5_SDA_Read()
{
    return 0;
}

void Soft_I2C5_Callback(Soft_I2C_Master_Flags_t flag)
{
}

void Soft_I2C5_Init()
{
    Ring_Buffer_Init(&I2C5_RX_RB, I2C5_RX_Buffer, 1, I2C5_RX_BUFFER_SIZE);
    Ring_Buffer_Init(&I2C5_TX_RB, I2C5_TX_Buffer, 1, I2C5_TX_BUFFER_SIZE);

    Soft_I2C5.I2C_RX_Ring_Buffer = &I2C5_RX_RB;
    Soft_I2C5.I2C_TX_Ring_Buffer = &I2C5_TX_RB;

    Soft_I2C5.Soft_I2C_GPIO_Init = Soft_I2C5_GPIO_Init;

    Soft_I2C5.Soft_I2C_SCL_High = Soft_I2C5_SCL_High;
    Soft_I2C5.Soft_I2C_SCL_Low = Soft_I2C5_SCL_Low;

    Soft_I2C5.Soft_I2C_SDA_High = Soft_I2C5_SDA_High;
    Soft_I2C5.Soft_I2C_SDA_Low = Soft_I2C5_SDA_Low;

    Soft_I2C5.Soft_I2C_SDA_Read = Soft_I2C5_SDA_Read;
    Soft_I2C5.callback = Soft_I2C5_Callback;

#if (I2C_USE_INTERRUPT_MODE)
    Soft_I2C_Master_Add(&Soft_I2C5);
#else
    Soft_I2C_Master_Init(&Soft_I2C5);
#endif
}
