#include "stm32f1xx_hal.h"
#include "soft_i2c.h"
#include "delay_us.h"


#define MAX_SOFT_I2C_MASTER  10

static Soft_I2C_Master_t *Soft_I2C_Master_List[MAX_SOFT_I2C_MASTER];
static uint8_t Soft_I2C_Master_Count = 0;

extern TIM_HandleTypeDef htim4;
#define SOFT_I2C_MASTER_TIM  &htim4

/* custom callback funtion prototype define in stm32f1xx_hal_tim.h */
void Soft_I2C_Master_TIM_ISR(TIM_HandleTypeDef *htim);

void Soft_I2C_Master_Init()
    {
    /*configure TIM*/
    /*configure GPIO*/
    /*configure Clocks*/

    /* all the i2c instances shares the the same tim*/

    /*tim configured in cube see tim.c*/
    /*attach custom callback function to tim*/
    /* USE_HAL_TIM_REGISTER_CALLBACKS is set to 1 in stm32f1xx_hal_conf.h*/
    HAL_TIM_RegisterCallback(SOFT_I2C_MASTER_TIM, HAL_TIM_PERIOD_ELAPSED_CB_ID, Soft_I2C_Master_TIM_ISR);
    }

/* add a new instance of soft i2c master to state machine*/
void Soft_I2C_Master_Add(Soft_I2C_Master_t *i2c_handle)
    {

    if (Soft_I2C_Master_Count < MAX_SOFT_I2C_MASTER)
	{

	i2c_handle->Active_Flag = 0;
	i2c_handle->State = Generate_Start;
	i2c_handle->I2C_Buffer.Read_Index = 0;
	i2c_handle->I2C_Buffer.Write_Index = 0;

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	//__HAL_RCC_GPIOA_CLK_ENABLE();
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	//__HAL_RCC_GPIOC_CLK_ENABLE();
	//__HAL_RCC_GPIOD_CLK_ENABLE();
	//__HAL_RCC_GPIOE_CLK_ENABLE();
	//__HAL_RCC_GPIOF_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port, i2c_handle->GPIO_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,GPIO_PIN_SET);

	GPIO_InitStruct.Pin = i2c_handle->GPIO_SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(i2c_handle->GPIO_SCL_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = i2c_handle->GPIO_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(i2c_handle->GPIO_SDA_Port, &GPIO_InitStruct);

	}

    Soft_I2C_Master_List[Soft_I2C_Master_Count] = i2c_handle;
    Soft_I2C_Master_Count++;

    }

void Soft_I2C_Master_Write_Byte(Soft_I2C_Master_t *i2c_handle,
	                        uint8_t slave_address,
	                        uint16_t register_address,
	                        uint8_t register_address_size,
				uint8_t byte)
    {
    i2c_handle->Address_RW = slave_address;

    if (register_address_size == 2)
	{
	/* msb of register address if address is of two bytes*/
	Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, (register_address >> 8));
	}

    Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, register_address);

    Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, byte);

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

/*set the read pointer in slave to read from*/
void Soft_I2C_Master_Write_Dummy(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
	                         uint8_t register_address_size)
    {
    i2c_handle->Address_RW = slave_address;

    if (register_address_size == 2)
	{
	/* msb of register address if address is of two bytes*/
	Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, (register_address >> 8));
	}

    Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, register_address);

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);

    }

void Soft_I2C_Master_Read_Byte(Soft_I2C_Master_t *i2c_handle,
	                       uint8_t slave_address)
    {

    i2c_handle->Address_RW = slave_address + 1;

    i2c_handle->Bytes_To_Read = 1;

    Ring_Buffer_Flush(&i2c_handle->I2C_Buffer);

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

void Soft_I2C_Master_Write_Bytes(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
				 uint8_t register_address_size,
				 uint8_t* bytes,
				 uint16_t len)
    {

    i2c_handle->Address_RW = slave_address;

    if (register_address_size == 2)
	{
	/* msb of register address if address is of two bytes*/
	Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, (register_address >> 8));
	}

    Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, register_address);

    while (len--)
	{
	Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer, *bytes++);
	}

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

void  Soft_I2C_Master_Read_Bytes(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
				 uint8_t len)
    {
    i2c_handle->Address_RW = slave_address + 1;

    i2c_handle->Bytes_To_Read = len;

    Ring_Buffer_Flush(&i2c_handle->I2C_Buffer);

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

/*check if slave respondung*/
void Soft_I2C_Master_Scan(Soft_I2C_Master_t *i2c_handle,
	                  uint8_t slave_address)
    {

    i2c_handle->Address_RW = slave_address;

    i2c_handle->Active_Flag = 1;

    i2c_handle->Status_Flag = I2C_Busy;

    /*enable TIM to start transaction */
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);

    }

enum Soft_I2C_Master_Flags_t  Soft_I2C_Master_Get_Status(Soft_I2C_Master_t *i2c_handle)
    {
    return i2c_handle->Status_Flag;
    }

void Soft_I2C_Master_Start()
    {
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

void Soft_I2C_Master_Stop()
    {
    HAL_TIM_Base_Stop_IT(SOFT_I2C_MASTER_TIM);
    }

/**** TIM configured in cube to generate interrupt every 2/clk_freq rate *********/
/*200khz tim freq for 100khz i2c*/
void Soft_I2C_Master_TIM_ISR(TIM_HandleTypeDef *htim)
    {

    Soft_I2C_Master_t *i2c_handle = NULL;

    uint8_t i2c_active_count = 0;

    static uint8_t clk_state = 0;

    for (uint8_t i = 0; i < Soft_I2C_Master_Count; i++)
	{

	/* grab i2c handle from list*/
	i2c_handle = Soft_I2C_Master_List[i];

	if (i2c_handle->Active_Flag)
	    {

	    i2c_active_count++;

	    if (clk_state == 0)
		{

		clk_state = 1;

		HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
			i2c_handle->GPIO_SCL_Pin, GPIO_PIN_SET);

		/*things to do when clock is high*/

		if (i2c_handle->State == Generate_Start)
		    {

		    /*generate start contion*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);

		    i2c_handle->Current_Byte = i2c_handle->Address_RW;

		    i2c_handle->State = Data_Out;

		    i2c_handle->Bit_Count = 0;

		    }
		else if (i2c_handle->State == Data_Out)
		    {

		    if (i2c_handle->Bit_Count == 8)
			{
			/* Current_Byte sent*/
			i2c_handle->Bit_Count = 0;
			i2c_handle->Current_Byte = 0;
			i2c_handle->State = Read_ACK;
			}

		    }
		else if (i2c_handle->State == Read_ACK)
		    {

		    /*check for ACK*/
		    if (HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin) == GPIO_PIN_RESET)
			{
			/*ACK Received*/

			if ((i2c_handle->Address_RW) & 0x01)
			    {
			    /*read from slave*/
			    i2c_handle->State = Data_In;
			    }
			else
			    {
			    /*write to slave*/
			    i2c_handle->State = Data_Out;

			    /*grab new Current_Byte if available*/
			    if (Ring_Buffer_Get_Count(&i2c_handle->I2C_Buffer))
				{
				Ring_Buffer_Get_Char(&i2c_handle->I2C_Buffer,
					(char*) &i2c_handle->Current_Byte);
				}
			    else
				{
				/*all bytes tranfered generate stop*/
				i2c_handle->State = Generate_Stop;
				}
			    }

			}
		    else
			{
			/*NACK Received*/
			i2c_handle->State = Generate_Stop;
			i2c_handle->Status_Flag = I2C_Error;
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

			Ring_Buffer_Put_Char(&i2c_handle->I2C_Buffer,
				i2c_handle->Current_Byte);

			i2c_handle->Current_Byte = 0;

			i2c_handle->Bytes_To_Read--;

			if (i2c_handle->Bytes_To_Read == 0)
			    {
			    /*all bytes received, generate stop*/
			    /*or give NACK then stop? */
			    i2c_handle->State = Generate_Stop;
			    }
			else
			    {
			    /*give ack to receive next byte*/
			    i2c_handle->State = Give_ACK;
			    }
			}

		    }
		else if (i2c_handle->State == Give_ACK)
		    {
		    /*give ACK*/
		    i2c_handle->State = Data_In;
		    }
		else if (i2c_handle->State == Generate_Stop)
		    {

		    /*generate stop contion*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_SET);

		    i2c_handle->Active_Flag = 0;
		    i2c_handle->State = Generate_Start;

		    if (i2c_handle->Status_Flag != I2C_Error)
			{
			i2c_handle->Status_Flag = I2C_Ok;
			}
		    }

		}

	    //else if (clk_state == 1) /*i2c clock frequency is half of interrupt frequency but symmetric clock waveform*/
	    if (clk_state == 1) /*i2c clock frequency same as interrupt frequency but asymmetric clock waveform*/
		{

		clk_state = 0;

		HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
			i2c_handle->GPIO_SCL_Pin, GPIO_PIN_RESET);

		/*things to do when clock is low*/

		if (i2c_handle->State == Data_Out)
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

		    }
		else if (i2c_handle->State == Read_ACK
			|| i2c_handle->State == Data_In)
		    {

		    /* release sda line to read*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_SET);
		    }
		else if (i2c_handle->State == Give_ACK
			|| i2c_handle->State == Generate_Stop)
		    {

		    /*give ACK*/
		    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);
		    }

		}

	    }

	}
    // if all soft_i2c are idle, disable tim interrupt
    if (!i2c_active_count)
	{
	HAL_TIM_Base_Stop_IT(SOFT_I2C_MASTER_TIM);
	clk_state = 0;
	}

    }



void Soft_I2C_Delay()
    {
    }


/*generate start condition */

static void Soft_I2C_SDA_High(Soft_I2C_Master_t* i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
    			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_SET);
    }


static void Soft_I2C_SDA_Low(Soft_I2C_Master_t* i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
    			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);
    }

static void Soft_I2C_SCL_High(Soft_I2C_Master_t* i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
    			    i2c_handle->GPIO_SCL_Pin, GPIO_PIN_SET);
    }


static void Soft_I2C_SCL_Low(Soft_I2C_Master_t* i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port,
    			    i2c_handle->GPIO_SCL_Pin, GPIO_PIN_RESET);
    }

static uint8_t Soft_I2C_SDA_Read(Soft_I2C_Master_t* i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin);
    }

static uint8_t Soft_I2C_Read_ACK(Soft_I2C_Master_t* i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin);
    }

static void Soft_I2C_ACK(Soft_I2C_Master_t* i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port,
        			    i2c_handle->GPIO_SDA_Pin, GPIO_PIN_RESET);
    }

static uint8_t Soft_I2C_Send_Byte(Soft_I2C_Master_t* i2c_handle, uint8_t byte)
    {

    uint8_t count = 8;

    while (count--)
	{
	if (byte & 0x80)
	    {
	    Soft_I2C_SDA_High(i2c_handle);
	    }
	else
	    {
	    Soft_I2C_SDA_Low(i2c_handle);
	    }

	byte <<= 1;

	Soft_I2C_SCL_High(i2c_handle);
	Soft_I2C_Delay();
	Soft_I2C_SCL_Low(i2c_handle);
	Soft_I2C_Delay();
	}

    if (Soft_I2C_Read_ACK(i2c_handle) == GPIO_PIN_SET)
	{
	return SOFT_I2C_ERR;
	}
    return SOFT_I2C_OK;
    }

static uint8_t Soft_I2C_Receive_Byte(Soft_I2C_Master_t *i2c_handle)
    {

    uint8_t i = 8;
    uint8_t ReceivedByte = 0;

    while (i--)
	{
	ReceivedByte <<= 1;
	Soft_I2C_SCL_Low(i2c_handle);
	Soft_I2C_Delay();
	Soft_I2C_SCL_High(i2c_handle);
	Soft_I2C_Delay();
	if (Soft_I2C_SDA_Read(i2c_handle))
	    {
	    ReceivedByte |= 0x01;
	    }
	}
    return ReceivedByte;

    }

uint8_t Soft_I2C_Master_Write_Byte_Block(Soft_I2C_Master_t* i2c_handle,
	                    uint8_t     slave_address,
	                    uint16_t     register_address,
			    uint8_t      register_address_size,
			    uint8_t      byte)
    {

    Soft_I2C_SCL_High(i2c_handle);
    Soft_I2C_SDA_High(i2c_handle);
    Soft_I2C_Delay();

    /*start condtion*/
    Soft_I2C_SDA_Low(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SCL_Low(i2c_handle);


    if (Soft_I2C_Send_Byte(i2c_handle, slave_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (register_address_size == 2)
	{
	if(Soft_I2C_Send_Byte(i2c_handle, (register_address >> 8)) == SOFT_I2C_ERR)
	    {
	    return SOFT_I2C_ERR;
	    }
	}

    if (Soft_I2C_Send_Byte(i2c_handle, register_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (byte)
	{
	if (Soft_I2C_Send_Byte(i2c_handle, byte) == SOFT_I2C_ERR)
	    {
	    return SOFT_I2C_ERR;
	    }
	}

    /*stop condtion*/
    Soft_I2C_SDA_Low(i2c_handle);
    Soft_I2C_SCL_High(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SDA_High(i2c_handle);

    return SOFT_I2C_OK;

    }

uint8_t Soft_I2C_Master_Read_Byte_Block(Soft_I2C_Master_t* i2c_handle,
	                   uint8_t     slave_address,
	                   uint8_t     register_address,
			   uint8_t     register_address_size,
			   uint8_t     *val)
    {

    Soft_I2C_SCL_High(i2c_handle);
    Soft_I2C_SDA_High(i2c_handle);
    Soft_I2C_Delay();

    /*start condtion*/
    Soft_I2C_SDA_Low(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SCL_Low(i2c_handle);

    if (Soft_I2C_Send_Byte(i2c_handle, slave_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (register_address_size == 2)
	{
	if(Soft_I2C_Send_Byte(i2c_handle, (register_address >> 8)) == SOFT_I2C_ERR)
	    {
	    return SOFT_I2C_ERR;
	    }
	}

    if (Soft_I2C_Send_Byte(i2c_handle, register_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    //repeated start
    Soft_I2C_SDA_High(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SDA_Low(i2c_handle);


    if (Soft_I2C_Send_Byte(i2c_handle, slave_address + 1) == SOFT_I2C_ERR) //read bit
	{
	return SOFT_I2C_ERR;
	}

    *val = Soft_I2C_Receive_Byte(i2c_handle);

    /*stop condtion*/
    Soft_I2C_SCL_Low(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SDA_Low(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SCL_High(i2c_handle);
    Soft_I2C_Delay();
    Soft_I2C_SDA_High(i2c_handle);

    return SOFT_I2C_OK;

    }

uint8_t Soft_I2C_Master_Write_Bytes_Block(Soft_I2C_Master_t* i2c_handle,
	                     uint8_t     slave_address,
	                     uint16_t     register_address,
			     uint8_t     register_address_size,
			     uint8_t     *buf,
			     uint8_t     num)
    {

    if (Soft_I2C_Start(i2c_handle) == SOFT_I2C_ERR)
	{
	Soft_I2C_Stop(i2c_handle);
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, slave_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, register_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    while (num--)
	{
	if (Soft_I2C_Send_Byte(i2c_handle, *buf++) == SOFT_I2C_ERR)
	    {
	    return SOFT_I2C_ERR;
	    }

	}

    Soft_I2C_Stop(i2c_handle);

    return SOFT_I2C_OK;

    }

uint8_t Soft_I2C_Master_Read_Bytes_Block(Soft_I2C_Master_t* i2c_handle,
	                    uint8_t     slave_address,
	                    uint8_t     register_address,
			    uint8_t     *buf,
			    uint8_t     num)
    {

    if (Soft_I2C_Start(i2c_handle) == SOFT_I2C_ERR)
	{
	Soft_I2C_Stop(i2c_handle);
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, slave_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, register_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Start(i2c_handle) == SOFT_I2C_ERR) //repeated start
	{
	Soft_I2C_Stop(i2c_handle);
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, slave_address + 1) == SOFT_I2C_ERR) //read bit
	{
	return SOFT_I2C_ERR;
	}

    while (num--)
	{
	*buf++ = Soft_I2C_Receive_Byte(i2c_handle);

	if (num == 0)
	    {
	    Soft_I2C_NACK(i2c_handle);
	    }
	else
	    {
	    Soft_I2C_ACK(i2c_handle);
	    }
	}

    Soft_I2C_Stop(i2c_handle);

    return SOFT_I2C_OK;

    }

uint8_t Soft_I2C_Master_Scan_Block(Soft_I2C_Master_t* i2c_handle, uint8_t slave_address)
    {

    if (Soft_I2C_Start(i2c_handle) == SOFT_I2C_ERR)
	{
	Soft_I2C_Stop(i2c_handle);
	return SOFT_I2C_ERR;
	}

    if (Soft_I2C_Send_Byte(i2c_handle, slave_address) == SOFT_I2C_ERR)
	{
	return SOFT_I2C_ERR;
	}

    Soft_I2C_Stop(i2c_handle);

    return SOFT_I2C_OK;

    }
