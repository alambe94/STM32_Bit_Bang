#include "stdio.h"
#include "soft_uart_tx.h"

/* redirect printf to uart */
int __io_putchar(int ch)
{
    extern Soft_UART_TX_t Soft_UART1_TX;
    Soft_UART_Send_Char(&Soft_UART1_TX, ch);
    while (!Soft_UART_Get_TC_Flag(&Soft_UART1_TX))
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

    extern void Soft_UART_TX_TIM_Init();
    Soft_UART_TX_TIM_Init();

    extern void Soft_UART1_TX_Init();
    Soft_UART1_TX_Init();

    printf("hello from soft uart1\n");

    while (1)
    {
    }
}
