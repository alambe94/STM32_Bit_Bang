#include "soft_i2c_slave.h"
#include "stm32f1xx_hal.h"
#include "delay_us.h"

/* add a new instance of soft i2c slave to state machine*/
void Soft_I2C_Slave_Init(Soft_I2C_Slave_t *i2c_handle)
    {

    i2c_handle->Active_Flag = 0;
    i2c_handle->State = Detect_Start;
    i2c_handle->Buffer_Index = 0;
    i2c_handle->Current_Byte = 0;

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    //__HAL_RCC_GPIOA_CLK_ENABLE();
    //__HAL_RCC_GPIOB_CLK_ENABLE();
    //__HAL_RCC_GPIOC_CLK_ENABLE();
    //__HAL_RCC_GPIOD_CLK_ENABLE();
    //__HAL_RCC_GPIOE_CLK_ENABLE();
    //__HAL_RCC_GPIOF_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port, i2c_handle->GPIO_SCL_Pin,
	    GPIO_PIN_SET);
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,
	    GPIO_PIN_SET);

    GPIO_InitStruct.Pin = i2c_handle->GPIO_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(i2c_handle->GPIO_SCL_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = i2c_handle->GPIO_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(i2c_handle->GPIO_SDA_Port, &GPIO_InitStruct);

    }

static void Soft_I2C_SDA_High(Soft_I2C_Slave_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,
	    GPIO_PIN_SET);
    }

static void Soft_I2C_SDA_Low(Soft_I2C_Slave_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,
	    GPIO_PIN_RESET);
    }

static uint8_t Soft_I2C_SDA_Read(Soft_I2C_Slave_t *i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin);
    }

static uint8_t Soft_I2C_ACK_Read(Soft_I2C_Slave_t *i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port,
	    i2c_handle->GPIO_SDA_Pin) ? 0 : 1;
    }

static uint8_t Soft_I2C_SCL_Read(Soft_I2C_Slave_t *i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SCL_Port, i2c_handle->GPIO_SCL_Pin);
    }

static void Soft_I2C_Slave_State_Machine_SDA_ISR(Soft_I2C_Slave_t *i2c_handle)
    {

    if (i2c_handle->State == Detect_Start)
	{
	if (!Soft_I2C_SDA_Read(i2c_handle) && Soft_I2C_SCL_Read(i2c_handle))
	    {
	    i2c_handle->State = Address_In;
	    i2c_handle->Bit_Count = 0;
	    i2c_handle->Current_Byte = 0;
	    i2c_handle->Active_Flag = 1;

	    i2c_handle->Event = I2C_Start_Detected;

	    if (i2c_handle->callback != NULL)
		{
		//i2c_handle->callback(i2c_handle->Event);
		}

	    }
	}

    if (i2c_handle->State == Detect_Stop)
	{
	if (Soft_I2C_SDA_Read(i2c_handle) && Soft_I2C_SCL_Read(i2c_handle))
	    {
	    i2c_handle->State = Detect_Start;
	    i2c_handle->Bit_Count = 0;
	    i2c_handle->Current_Byte = 0;
	    i2c_handle->Active_Flag = 0;

	    i2c_handle->Event = I2C_Stop_Detected;

	    if (i2c_handle->callback != NULL)
		{
		//i2c_handle->callback(i2c_handle->Event);
		}
	    }
	}

    }

static void Soft_I2C_Slave_State_Machine_SCL_ISR(Soft_I2C_Slave_t *i2c_handle)
    {

    if (Soft_I2C_SCL_Read(i2c_handle))
	{
	/*things to do when clock is high*/

	if (i2c_handle->State == Detect_Start
		|| i2c_handle->State == Detect_Stop)
	    {
	    /*detected in sda isr*/
	    }
	else if (i2c_handle->State == Address_In)
	    {

	    i2c_handle->Current_Byte <<= 1;

	    if (Soft_I2C_SDA_Read(i2c_handle))
		{
		i2c_handle->Current_Byte |= 0x01;
		}

	    i2c_handle->Bit_Count++;

	    if (i2c_handle->Bit_Count == 8)
		{

		i2c_handle->Bit_Count = 0;

		i2c_handle->Received_Address = i2c_handle->Current_Byte;

		if (i2c_handle->Received_Address == i2c_handle->Own_Address)
		    {

		    i2c_handle->Event = I2C_Write_Request;

		    if (i2c_handle->callback != NULL)
			{
			i2c_handle->callback(i2c_handle->Event);
			}
		    }

		if (i2c_handle->Received_Address
			== (i2c_handle->Own_Address + 1))
		    {

		    i2c_handle->Event = I2C_Read_Request;

		    if (i2c_handle->callback != NULL)
			{
			i2c_handle->callback(i2c_handle->Event);
			}
		    }

		if (i2c_handle->Buffer_Ready)
		    {
		    i2c_handle->State = Give_ACK;
		    }
		else
		    {
		    i2c_handle->State = Detect_Stop;
		    }

		i2c_handle->Current_Byte = 0;

		}

	    }
	else if (i2c_handle->State == Give_ACK)
	    {

	    if (i2c_handle->Received_Address == i2c_handle->Own_Address)
		{
		i2c_handle->State = Data_In;
		}
	    else
		{/*(Own_Address + 1)*/
		i2c_handle->State = Data_Out;
		}

	    }
	else if (i2c_handle->State == Data_In)
	    {

	    if (i2c_handle->Buffer_Ready)
		{

		i2c_handle->Current_Byte <<= 1;

		if (Soft_I2C_SDA_Read(i2c_handle) == GPIO_PIN_SET)
		    {
		    i2c_handle->Current_Byte |= 0x01;
		    }

		i2c_handle->Bit_Count++;

		if (i2c_handle->Bit_Count == 8)
		    {

		    i2c_handle->Bit_Count = 0;

		    i2c_handle->Buffer_Ready--;

		    i2c_handle->State = Give_ACK;

		    i2c_handle->I2C_Buffer[i2c_handle->Buffer_Index] =
			    i2c_handle->Current_Byte;

		    i2c_handle->Current_Byte = 0;

		    i2c_handle->Buffer_Index++;

		    }

		}
	    else
		{
		i2c_handle->State = Detect_Stop;
		}

	    }
	else if (i2c_handle->State == Data_Out)
	    {

	    if (i2c_handle->Bit_Count == 8)
		{

		i2c_handle->Bit_Count = 0;

		i2c_handle->Current_Byte = 0;

		i2c_handle->State = Read_ACK;
		}

	    }
	else if (i2c_handle->State == Read_ACK)
	    {

	    if (Soft_I2C_ACK_Read(i2c_handle))
		{

		i2c_handle->Event = I2C_ACK_Received;

		if (i2c_handle->callback != NULL)
		    {
		    //i2c_handle->callback(i2c_handle->Event);
		    }

		if (i2c_handle->Received_Address == i2c_handle->Own_Address)
		    {
		    i2c_handle->State = Data_In;
		    }
		else
		    {
		    /*(Own_Address + 1)*/

		    if (i2c_handle->Buffer_Ready)
			{

			i2c_handle->State = Data_Out;

			i2c_handle->Buffer_Ready--;

			/*grab byte from buffer to send*/
			i2c_handle->Current_Byte =
				i2c_handle->I2C_Buffer[i2c_handle->Buffer_Index];

			i2c_handle->Buffer_Index++;

			}
		    else
			{
			i2c_handle->State = Detect_Stop;
			}

		    }
		}
	    else
		{
		i2c_handle->Event = I2C_NACK_Received;

		if (i2c_handle->callback != NULL)
		    {
		    //i2c_handle->callback(i2c_handle->Event);
		    }
		i2c_handle->State = Detect_Stop;
		}

	    }

	}

    else
	{

	/*things to do when clock is low*/
	if (i2c_handle->State == Detect_Start
		|| i2c_handle->State == Detect_Stop)
	    {
	    /*detected in sda isr*/
	    }
	else if (i2c_handle->State == Address_In || i2c_handle->State == Data_In
		|| i2c_handle->State == Read_ACK)
	    {
	    /*leave sda high*/
	    Soft_I2C_SDA_High(i2c_handle);
	    }
	else if (i2c_handle->State == Give_ACK)
	    {
	    Soft_I2C_SDA_Low(i2c_handle);
	    }
	else if (i2c_handle->State == Data_Out)
	    {
	    /*send Current_Byte*/
	    if (i2c_handle->Current_Byte & 0x80)
		{
		Soft_I2C_SDA_High(i2c_handle);
		}
	    else
		{
		Soft_I2C_SDA_Low(i2c_handle);
		}

	    i2c_handle->Current_Byte <<= 1;

	    i2c_handle->Bit_Count++;
	    }

	}

    }
