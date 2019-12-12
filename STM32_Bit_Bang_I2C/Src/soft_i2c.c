#include "stm32f1xx_hal.h"
#include "soft_i2c.h"
#include "delay_us.h"


void Soft_I2C_Init(Soft_I2C_t *i2c_handle)
    {

    }

void Soft_I2C_Delay()
    {

    }

/*generate start condition */

uint8_t Soft_I2C_Start(Soft_I2C_t *i2c_handle)
    {

    }

/*generate stop condition */

void Soft_I2C_Stop(Soft_I2C_t *i2c_handle)
    {

    }

uint8_t Soft_I2C_Wait_ACK(Soft_I2C_t *i2c_handle)
    {

    }

void Soft_I2C_ACK(Soft_I2C_t *i2c_handle)
    {

    }

void Soft_I2C_NACK(Soft_I2C_t *i2c_handle)
    {



    }

uint8_t Soft_I2C_Send_Byte(Soft_I2C_t *i2c_handle, uint8_t byte)
    {


    }

uint8_t Soft_I2C_Receive_Byte(Soft_I2C_t *i2c_handle)
    {



    }

uint8_t Soft_I2C_Write_Byte(Soft_I2C_t *i2c_handle, uint8_t slave_address,
	uint8_t register_address, uint8_t *byte)
    {



    }

uint8_t Soft_I2C_Read_Byte(Soft_I2C_t *i2c_handle, uint8_t slave_address,
	uint8_t register_address, uint8_t *val)
    {


    }

uint8_t Soft_I2C_Write_Bytes(Soft_I2C_t *i2c_handle, uint8_t slave_address,
	uint8_t register_address, uint8_t *buf, uint8_t num)
    {



    }

uint8_t Soft_I2C_Read_Bytes(Soft_I2C_t *i2c_handle, uint8_t slave_address,
	uint8_t register_address, uint8_t *buf, uint8_t num)
    {



    }

uint8_t Soft_I2C_Scan(Soft_I2C_t *i2c_handle, uint8_t slave_address)
    {


    }

#define MAX_SOFT_I2C  10

static uint8_t Soft_I2C_Count = 0;

Soft_I2C_Master_t *Soft_I2C_List[MAX_SOFT_I2C];

/**** TIM configured in cube to generate interrupt every 1/clk_freq rate *********/
/*100khz freq*/
void Soft_I2C_Master_TIM_ISR()
    {

    Soft_I2C_Master_t *i2c_handle = NULL;

    uint8_t i2c_active_count = 0;

    for (uint8_t i = 0; i < Soft_I2C_Count; i++)
	{

	i2c_active_count++;

	/* grab i2c handle from list*/
	i2c_handle = Soft_I2C_List[i];

	if (i2c_handle->Active_Flag)
	    {

	    if (i2c_handle->CLK_State == 1)
		{

		/*things to do when clock is high*/
		HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
			i2c_handle->GPIO_SCL_Pin, GPIO_PIN_SET);

		i2c_handle->CLK_State = 0;

		if (i2c_handle->State == Generate_Start)
		    {

		    /*generate start contion*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_SET);

		    i2c_handle->Current_Byte = i2c_handle->Address_RW;

		    i2c_handle->State = Addressing_RW;

		    /*clear all error flags*/
		    i2c_handle->Error_Flag = 0;

		    }

		else if (i2c_handle->State == Addressing_ACK)
		    {

		    /*check for ACK*/
		    if (HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin) == GPIO_PIN_RESET)
			{

			if ((i2c_handle->Address_RW) & 0x01)
			    {
			    i2c_handle->State = Data_In;
			    }
			else
			    {
			    i2c_handle->State = Data_Out;
			    }
			}

		    else
			{
			i2c_handle->State = Generate_Stop;
			i2c_handle->Error_Flag = 1;
			}

		    }

		else if (i2c_handle->State == Data_In)
		    {

		    i2c_handle->Current_Byte <<= 1;

		    if (HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin) == GPIO_PIN_SET)
			{
			i2c_handle->Current_Byte |= 0x01;
			}

		    i2c_handle->Bit_Count++;
		    if (i2c_handle->Bit_Count == 8)
			{
			i2c_handle->Bit_Count = 0;
			Ring_Buffer_Put_Char(&i2c_handle->Ring_Buffer,
				i2c_handle->Current_Byte);
			i2c_handle->Current_Byte = 0;
			i2c_handle->Bytes_To_Read--;

			if (i2c_handle->Bytes_To_Read == 0)
			    {
			    /*all bytes received, generate stop*/
			    i2c_handle->State = Generate_Stop;
			    }
			else
			    {
			    /*give ack to receive next byte*/
			    i2c_handle->State = Data_In_ACK;
			    }
			}

		    }

		if (i2c_handle->State == Generate_Stop)
		    {

		    /*generate stop contion*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);

		    i2c_handle->Active_Flag = 0;

		    }

		}
	    else
		{
		/*things to do when clock is low*/
		HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
			i2c_handle->GPIO_SCL_Pin, GPIO_PIN_RESET);

		if (i2c_handle->State == Addressing_RW
			|| i2c_handle->State == Data_Out)
		    {

		    /*send Current_Byte*/
		    if (i2c_handle->Current_Byte & 0x80)
			{
			HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
				i2c_handle->GPIO_SDA_Pin, GPIO_PIN_SET);
			}
		    else
			{
			HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
				i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);
			}

		    i2c_handle->Current_Byte <<= 1;

		    i2c_handle->Bit_Count++;

		    if (i2c_handle->Bit_Count == 8)
			{
			/*urrent_Byte sent*/
			i2c_handle->Bit_Count = 0;
			i2c_handle->Current_Byte = 0;

			if (i2c_handle->State == Addressing_RW)
			    {
			    i2c_handle->State = Addressing_ACK;
			    }
			else
			    {
			    /*grab new Current_Byte if available*/
			    if (Ring_Buffer_Get_Count(&i2c_handle->Ring_Buffer))
				{
				Ring_Buffer_Get_Char(&i2c_handle->Ring_Buffer,
					(char*) &i2c_handle->Current_Byte);
				}
			    else
				{
				/*all bytes tranfered generate stop*/
				i2c_handle->State = Generate_Stop;
				}
			    }

			}

		    }

		if (i2c_handle->State == Data_In_ACK)
		    {
		    i2c_handle->State = Data_In;
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);
		    }

		}

	    }

	}

    // if all soft_i2c are idle, disable tim interrupt
    if (!i2c_active_count)
	{
	}

    }
