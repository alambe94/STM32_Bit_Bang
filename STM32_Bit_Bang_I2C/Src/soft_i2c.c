#include "stm32f1xx_hal.h"
#include "soft_i2c.h"
#include "delay_us.h"
#include "tim.h"

/* transaction in polling or interrupt mode*/
#define INTERRUPT_MODE 1
#define MAX_SOFT_I2C_MASTER  10

static Soft_I2C_Master_t *Soft_I2C_Master_List[MAX_SOFT_I2C_MASTER];
static uint8_t Soft_I2C_Master_Count = 0;

/* custom callback funtion prototype define in stm32f1xx_hal_tim.h */
static void Soft_I2C_Master_State_Machine(Soft_I2C_Master_t *i2c_handle);

#if (INTERRUPT_MODE)
#define SOFT_I2C_MASTER_TIM  &htim4
static void Soft_I2C_Master_TIM_Start();
static void Soft_I2C_Master_TIM_Stop();
static void Soft_I2C_Master_TIM_ISR();
#else
static void Soft_I2C_Delay()
    {
    Delay_us(5); /*adjust i2c clk frequency */
    }
#endif

void Soft_I2C_Master_Init()
    {
    /*configure TIM*/
    /*configure GPIO*/
    /*configure Clocks*/

    /* all the i2c instances shares the the same tim*/

#if (INTERRUPT_MODE)
    /*tim configured in cube see tim.c*/
    /*attach custom callback function to tim*/
    /* USE_HAL_TIM_REGISTER_CALLBACKS is set to 1 in stm32f1xx_hal_conf.h*/
    HAL_TIM_RegisterCallback(SOFT_I2C_MASTER_TIM, HAL_TIM_PERIOD_ELAPSED_CB_ID,
	    Soft_I2C_Master_TIM_ISR);
#endif
    }

/* add a new instance of soft i2c master to state machine*/
void Soft_I2C_Master_Add(Soft_I2C_Master_t *i2c_handle)
    {

    if (Soft_I2C_Master_Count < MAX_SOFT_I2C_MASTER)
	{

	i2c_handle->Active_Flag = 0;
	i2c_handle->State = Generate_Start;
	i2c_handle->I2C_RX_Ring_Buffer.Read_Index = 0;
	i2c_handle->I2C_RX_Ring_Buffer.Write_Index = 0;
	i2c_handle->I2C_TX_Ring_Buffer.Read_Index = 0;
	i2c_handle->I2C_TX_Ring_Buffer.Write_Index = 0;

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
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(i2c_handle->GPIO_SCL_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = i2c_handle->GPIO_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(i2c_handle->GPIO_SDA_Port, &GPIO_InitStruct);

	Soft_I2C_Master_List[Soft_I2C_Master_Count] = i2c_handle;
	Soft_I2C_Master_Count++;

	}

    }

#if (INTERRUPT_MODE)
static void Soft_I2C_Master_TIM_Start()
    {
    HAL_TIM_Base_Start_IT(SOFT_I2C_MASTER_TIM);
    }

static void Soft_I2C_Master_TIM_Stop()
    {
    HAL_TIM_Base_Stop_IT(SOFT_I2C_MASTER_TIM);
    }

/**** TIM configured in cube to generate interrupt *********/
static void Soft_I2C_Master_TIM_ISR()
    {

    Soft_I2C_Master_t *i2c_handle = NULL;

    uint8_t i2c_active_count = 0;

    for (uint8_t i = 0; i < Soft_I2C_Master_Count; i++)
	{

	/* grab i2c handle from list*/
	i2c_handle = Soft_I2C_Master_List[i];

	if (i2c_handle->Active_Flag)
	    {
	    i2c_active_count++;
	    Soft_I2C_Master_State_Machine(i2c_handle);
	    }

	}
    // if all soft_i2c are idle, disable tim interrupt
    if (!i2c_active_count)
	{
	Soft_I2C_Master_TIM_Stop();
	}

    }
#endif

static void Soft_I2C_SDA_High(Soft_I2C_Master_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,
	    GPIO_PIN_SET);
    }

static void Soft_I2C_SDA_Low(Soft_I2C_Master_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin,
	    GPIO_PIN_RESET);
    }

static void Soft_I2C_SCL_High(Soft_I2C_Master_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port, i2c_handle->GPIO_SCL_Pin,
	    GPIO_PIN_SET);
    }

static void Soft_I2C_SCL_Low(Soft_I2C_Master_t *i2c_handle)
    {
    HAL_GPIO_WritePin(i2c_handle->GPIO_SCL_Port, i2c_handle->GPIO_SCL_Pin,
	    GPIO_PIN_RESET);
    }

static uint8_t Soft_I2C_SDA_Read(Soft_I2C_Master_t *i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port, i2c_handle->GPIO_SDA_Pin);
    }

static uint8_t Soft_I2C_ACK_Read(Soft_I2C_Master_t *i2c_handle)
    {
    return HAL_GPIO_ReadPin(i2c_handle->GPIO_SDA_Port,
	    i2c_handle->GPIO_SDA_Pin) ? 0 : 1;
    }

static void Soft_I2C_Master_State_Machine(Soft_I2C_Master_t *i2c_handle)
    {

    static uint8_t clk_state = 0;

    if (clk_state == 0)
	{

	clk_state = 1;

	Soft_I2C_SCL_High(i2c_handle);

	/*things to do when clock is high*/

	if (i2c_handle->State == Generate_Start)
	    {

	    /*generate start contion*/
	    Soft_I2C_SDA_Low(i2c_handle);

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
	    if (Soft_I2C_ACK_Read(i2c_handle))
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
		    if (Ring_Buffer_Get_Count(&i2c_handle->I2C_TX_Ring_Buffer))
			{
			Ring_Buffer_Get_Char(&i2c_handle->I2C_TX_Ring_Buffer,
				&i2c_handle->Current_Byte);
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

	    if (Soft_I2C_SDA_Read(i2c_handle) == GPIO_PIN_SET)
		{
		i2c_handle->Current_Byte |= 0x01;
		}

	    i2c_handle->Bit_Count++;

	    if (i2c_handle->Bit_Count == 8)
		{

		i2c_handle->Bit_Count = 0;

		Ring_Buffer_Put_Char(&i2c_handle->I2C_RX_Ring_Buffer,
			i2c_handle->Current_Byte);

		i2c_handle->Current_Byte = 0;

		i2c_handle->Bytes_To_Receive--;

		if (i2c_handle->Bytes_To_Receive == 0)
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
	    /*next stage*/
	    i2c_handle->State = Data_In;
	    }
	else if (i2c_handle->State == Generate_Stop)
	    {

	    /*generate stop contion*/
	    Soft_I2C_SDA_High(i2c_handle);

	    i2c_handle->Active_Flag = 0;
	    i2c_handle->State = Generate_Start;
	    clk_state = 0;

	    if (i2c_handle->Status_Flag != I2C_Error)
		{
		i2c_handle->Status_Flag = I2C_Ok;
		}

	    if (i2c_handle->callback != NULL)
		{
		i2c_handle->callback(i2c_handle->Status_Flag);
		}
	    }

	}

    else if (clk_state == 1) /*i2c clock frequency is half of interrupt frequency but symmetric clock waveform*/
	//if (clk_state == 1) /*i2c clock frequency is same as interrupt frequency but asymmetric clock waveform*/
	{

	clk_state = 0;

	Soft_I2C_SCL_Low(i2c_handle);

	/*things to do when clock is low*/

	if (i2c_handle->State == Generate_Start)
	    {
	    /**/
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
	else if (i2c_handle->State == Read_ACK || i2c_handle->State == Data_In)
	    {
	    /* release sda line to read*/
	    Soft_I2C_SDA_High(i2c_handle);
	    }
	else if (i2c_handle->State == Give_ACK
		|| i2c_handle->State == Generate_Stop)
	    {
	    /* set sda line to low*/
	    Soft_I2C_SDA_Low(i2c_handle);
	    }

	}

    }

/* available bytes to read in rx buffer*/
uint16_t Soft_I2C_Master_Available(Soft_I2C_Master_t *i2c_handle)
    {
    return Ring_Buffer_Get_Count(&i2c_handle->I2C_RX_Ring_Buffer);
    }

/* read byte from rx buffer*/
uint8_t Soft_I2C_Master_Read_Byte(Soft_I2C_Master_t *i2c_handle)
    {
    uint8_t temp;
    Ring_Buffer_Get_Char(&i2c_handle->I2C_RX_Ring_Buffer, &temp);
    return temp;
    }

Soft_I2C_Master_Flags_t Soft_I2C_Master_Get_Status(Soft_I2C_Master_t *i2c_handle)
    {
    return i2c_handle->Status_Flag;
    }

void Soft_I2C_Master_Transmit(Soft_I2C_Master_t *i2c_handle,
	                      uint8_t slave_address,
			      uint16_t reg_addr,
	                      uint8_t reg_addr_size,
			      uint8_t *bytes,
			      uint16_t len)
    {

    i2c_handle->Address_RW = slave_address;

    if (reg_addr_size == 2)
	{
	/* msb of register address if address is of two bytes*/
	Ring_Buffer_Put_Char(&i2c_handle->I2C_TX_Ring_Buffer,
		(reg_addr >> 8));
	}

    Ring_Buffer_Put_Char(&i2c_handle->I2C_TX_Ring_Buffer, reg_addr);

    while (len--)
	{
	Ring_Buffer_Put_Char(&i2c_handle->I2C_TX_Ring_Buffer, *bytes++);
	}

    i2c_handle->Bit_Count = 0;

    i2c_handle->Current_Byte = 0;

    i2c_handle->State = Generate_Start;

    i2c_handle->Status_Flag = I2C_Busy;

    i2c_handle->Active_Flag = 1;

#if (INTERRUPT_MODE)
    /*enable TIM to start transaction */
    Soft_I2C_Master_TIM_Start();
#else
    while(i2c_handle->Active_Flag)
	{
	Soft_I2C_Master_State_Machine(i2c_handle);
	Soft_I2C_Delay();
	}
#endif
    }

void Soft_I2C_Master_Receive_Request(Soft_I2C_Master_t *i2c_handle,
	                             uint8_t slave_address,
				     uint16_t reg_addr,
				     uint8_t reg_addr_size,
			             uint8_t *bytes,
				     uint16_t len)
    {

    /* send and wait for register address to read from*/
    Soft_I2C_Master_Transmit(i2c_handle,
	                     slave_address,
			     reg_addr,
			     reg_addr_size,
			     NULL,
			     0);

    while(i2c_handle->Active_Flag)
	{
	}

    i2c_handle->Address_RW = slave_address + 1;

    i2c_handle->Bytes_To_Receive = len;

    i2c_handle->Bit_Count = 0;

    i2c_handle->Current_Byte = 0;

    i2c_handle->State = Generate_Start;

    i2c_handle->Status_Flag = I2C_Busy;

    i2c_handle->Active_Flag = 1;

#if (INTERRUPT_MODE)
    /*enable TIM to start transaction */
    Soft_I2C_Master_TIM_Start();
#else
    while(i2c_handle->Active_Flag)
	{
	Soft_I2C_Master_State_Machine(i2c_handle);
	Soft_I2C_Delay();
	}
#endif
    }

/*check if slave responding*/
void Soft_I2C_Master_Scan(Soft_I2C_Master_t *i2c_handle,
	uint8_t slave_address)
    {

    i2c_handle->Address_RW = slave_address;

    i2c_handle->Bit_Count = 0;

    i2c_handle->Current_Byte = 0;

    i2c_handle->State = Generate_Start;

    i2c_handle->Status_Flag = I2C_Busy;

    i2c_handle->Active_Flag = 1;

#if (INTERRUPT_MODE)
    /*enable TIM to start transaction */
    Soft_I2C_Master_TIM_Start();
#else
    while(i2c_handle->Active_Flag)
	{
	Soft_I2C_Master_State_Machine(i2c_handle);
	Soft_I2C_Delay();
	}
#endif
    }
