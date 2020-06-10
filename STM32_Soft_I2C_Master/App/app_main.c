#include "stdio.h"
#include "main.h"
#include "usart.h"
#include "ssd1306.h"
#include "fonts.h"

/* redirect printf to uart */
int __io_putchar(int ch)
{
    huart2.Instance->DR = (ch);
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == 0)
        ;
    return ch;
}

void Sytick_Callback()
{
}

void App_Main()
{
    /* io buffer off*/
    /* redirect printf to uart */
    setvbuf(stdout, NULL, _IONBF, 0);

    extern void Soft_I2C1_Init();
    Soft_I2C1_Init();

    for (uint8_t i = 0; i < 128; i++)
    {
        Soft_I2C_Master_Scan(&Soft_I2C1, i << 1);
        while (Soft_I2C1.Active_Flag)
            ;
        if (Soft_I2C_Master_Get_Status(&Soft_I2C1) == I2C_Ok)
        {
            printf("found i2x device at 0X%X\n", i << 1);
        }
    }

    ssd1306_Init();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("SSD1306 Soft", Font_7x10, White);
    ssd1306_UpdateScreen();

    while (1)
    {
    }
}
