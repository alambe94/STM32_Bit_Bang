#include "soft_one_wire.h"
#include "delay_us.h"

void Soft_OW_Init(Soft_OW_t* ow_handle)
    {
    /**** GPIO pin configured in cube *********/
    /**** gpoi output open drain *********/
    }

static void Soft_OW_Write_Bit(Soft_OW_t* ow_handle, uint8_t bit)
    {

    HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
	    GPIO_PIN_RESET);

    if (bit)
	{
	/* MASTER WRITE “1” SLOT */
	Delay_Micros(10);

	HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
		GPIO_PIN_SET);

	Delay_Micros(50);

	}
    else
	{
	/* MASTER WRITE “0” SLOT */
	Delay_Micros(50);

	HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
			GPIO_PIN_SET);

	Delay_Micros(10);
	}
    }

static uint8_t Soft_OW_Read_Bit(Soft_OW_t* ow_handle)
    {

    uint8_t xreturn = 0;

    HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
    			GPIO_PIN_RESET);

    Delay_Micros(5);

    HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
       			GPIO_PIN_SET);

    Delay_Micros(10);

    if (HAL_GPIO_ReadPin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin))
	{
	xreturn = 1;
	}

    Delay_Micros(45);

    return xreturn;
    }


uint8_t Soft_OW_Reset(Soft_OW_t* ow_handle)
    {

    uint8_t xreturn = 0;

    HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
	    GPIO_PIN_RESET);

    /* minimum 480 us*/
    Delay_Micros(500);

    HAL_GPIO_WritePin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin,
	    GPIO_PIN_SET);

    /* WAITS 15-60µs*/
    Delay_Micros(60);

    if (!HAL_GPIO_ReadPin(ow_handle->GPIO_OW_Port, ow_handle->GPIO_OW_Pin))
	{
	xreturn = 1;
	}

    Delay_Micros(250);

    return xreturn;
    }


void Soft_OW_Send_Byte(Soft_OW_t* ow_handle, uint8_t byte)
    {
    for (uint8_t i = 0; i < 8; i++)
	{
	Soft_OW_Write_Bit(ow_handle, (byte & 0x01));
	byte >>= 1;
	}
    }


uint8_t Soft_OW_Receive_Byte(Soft_OW_t* ow_handle)
    {

    uint8_t rx_data = 0;

    for (uint8_t i = 0; i < 8; i++)
	{

	rx_data >>= 1;

	if (Soft_OW_Read_Bit(ow_handle))
	    {
	    rx_data |= 0x80;
	    }
	}

    return rx_data;
    }
