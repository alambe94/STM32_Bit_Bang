#include "soft_i2c_master.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"

#define I2C2_RX_BUFFER_SIZE 512
#define I2C2_TX_BUFFER_SIZE 512

uint8_t I2C2_RX_Buffer[I2C2_RX_BUFFER_SIZE];
uint8_t I2C2_TX_Buffer[I2C2_TX_BUFFER_SIZE];

Ring_Buffer_t I2C2_RX_RB;
Ring_Buffer_t I2C2_TX_RB;

Soft_I2C_Master_t Soft_I2C2;

void Soft_I2C2_GPIO_Init()
{
}

void Soft_I2C2_SDA_High()
{
}

void Soft_I2C2_SDA_Low()
{
}

void Soft_I2C2_SCL_High()
{
}

void Soft_I2C2_SCL_Low()
{
}

uint8_t Soft_I2C2_SDA_Read()
{
    return 0;
}

void Soft_I2C2_Callback(Soft_I2C_Master_Flags_t flag)
{
}

void Soft_I2C2_Init()
{
    Ring_Buffer_Init(&I2C2_RX_RB, I2C2_RX_Buffer, 1, I2C2_RX_BUFFER_SIZE);
    Ring_Buffer_Init(&I2C2_TX_RB, I2C2_TX_Buffer, 1, I2C2_TX_BUFFER_SIZE);

    Soft_I2C2.I2C_RX_Ring_Buffer = &I2C2_RX_RB;
    Soft_I2C2.I2C_TX_Ring_Buffer = &I2C2_TX_RB;

    Soft_I2C2.Soft_I2C_GPIO_Init = Soft_I2C2_GPIO_Init;

    Soft_I2C2.Soft_I2C_SCL_High = Soft_I2C2_SCL_High;
    Soft_I2C2.Soft_I2C_SCL_Low = Soft_I2C2_SCL_Low;

    Soft_I2C2.Soft_I2C_SDA_High = Soft_I2C2_SDA_High;
    Soft_I2C2.Soft_I2C_SDA_Low = Soft_I2C2_SDA_Low;

    Soft_I2C2.Soft_I2C_SDA_Read = Soft_I2C2_SDA_Read;
    Soft_I2C2.callback = Soft_I2C2_Callback;

#if (I2C_USE_INTERRUPT_MODE)
    Soft_I2C_Master_Add(&Soft_I2C2);
#else
    Soft_I2C_Master_Init(&Soft_I2C2);
#endif
}
