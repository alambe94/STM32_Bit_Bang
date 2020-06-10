#include "soft_i2c_master.h"
#include "tim.h"

#if (I2C_USE_INTERRUPT_MODE)
void Soft_I2C_Master_TIM_Init()
{
    /* timer configured in cube @see tim.c */
}

void Soft_I2C_Master_TIM_Start()
{
    HAL_TIM_Base_Start_IT(&htim11);
}

void Soft_I2C_Master_TIM_Stop()
{
    HAL_TIM_Base_Stop_IT(&htim11);
}
#else
void Delay_Micros(uint16_t micros)
{
    uint16_t count = 0XFF;
    while (count--)
        ;
}
#endif

void TIM11_ISR(void)
{
#if (I2C_USE_INTERRUPT_MODE)
    Soft_I2C_Master_TIM_ISR();
#endif
}
