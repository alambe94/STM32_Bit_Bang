#include "soft_i2c_slave.h"
#include "stm32f1xx_hal.h"

#define SOFT_I2C_SLAVE_SCL_PIN  GPIO_PIN_6
#define SOFT_I2C_SLAVE_SCL_PORT GPIOD

#define SOFT_I2C_SLAVE_SDA_PIN  GPIO_PIN_7
#define SOFT_I2C_SLAVE_SDA_PORT GPIOD

typedef enum Soft_I2C_Slave_State_t
    {
    Slave_Detect_Start,
    Slave_Address_In,
    Slave_Data_In,
    Slave_Give_ACK,
    Slave_Data_Out,
    Slave_Read_ACK,
    Slave_Detect_Stop
    } Soft_I2C_Slave_State_t;


static struct Soft_I2C_Slave_t
    {
	uint8_t *Buffer; /* I2C working buffer*/
	uint16_t Buffer_Index; /* Buffer index*/

	/* no of bytes slave is ready to receive or send if set 0 NACK will be send*/
	/* callback will be called when address is matched with I2C_Read_Request or I2C_Write_Request*/
	/* in callback set the buffer where the data will be written to or send from*/
	/* set Byte_Count , slave is able to receive or transmit*/
	uint16_t Byte_Count;

	uint8_t Own_Address;

	uint8_t Received_Address;

	uint8_t Bit_Count;
	uint8_t Current_Byte;

	uint8_t Active_Flag;

	Soft_I2C_Slave_State_t state;

	void (*callback)(Soft_I2C_Slave_Event_t event);

    } Soft_I2C_Slave;

static void Soft_I2C_Slave_SDA_Enable_INT()
    {
    SET_BIT(EXTI->IMR, SOFT_I2C_SLAVE_SDA_PIN);
    }

static void Soft_I2C_Slave_SDA_Disable_INT()
    {
    CLEAR_BIT(EXTI->IMR, SOFT_I2C_SLAVE_SDA_PIN);
    }

void Soft_I2C_Slave_Init(void (*Event_Callback)(Soft_I2C_Slave_Event_t event))
    {

    Soft_I2C_Slave.Active_Flag = 0;
    Soft_I2C_Slave.Buffer_Index = 0;
    Soft_I2C_Slave.Current_Byte = 0;

    Soft_I2C_Slave.callback = Event_Callback;

    Soft_I2C_Slave.state = Slave_Detect_Start;

    /* GPIO Ports Clock Enable */
    //__HAL_RCC_GPIOA_CLK_ENABLE();
    //__HAL_RCC_GPIOB_CLK_ENABLE();
    //__HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    //__HAL_RCC_GPIOE_CLK_ENABLE();
    //__HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    HAL_GPIO_WritePin(SOFT_I2C_SLAVE_SCL_PORT, SOFT_I2C_SLAVE_SCL_PIN,
	    GPIO_PIN_SET);
    HAL_GPIO_WritePin(SOFT_I2C_SLAVE_SDA_PORT, SOFT_I2C_SLAVE_SDA_PIN,
	    GPIO_PIN_SET);

    GPIO_InitStruct.Pin = SOFT_I2C_SLAVE_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(SOFT_I2C_SLAVE_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SOFT_I2C_SLAVE_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(SOFT_I2C_SLAVE_SDA_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    }

static void Soft_I2C_Slave_SDA_High()
    {
    HAL_GPIO_WritePin(SOFT_I2C_SLAVE_SDA_PORT, SOFT_I2C_SLAVE_SDA_PIN,
	    GPIO_PIN_SET);
    }

static void Soft_I2C_Slave_SDA_Low()
    {
    HAL_GPIO_WritePin(SOFT_I2C_SLAVE_SDA_PORT, SOFT_I2C_SLAVE_SDA_PIN,
	    GPIO_PIN_RESET);
    }

static uint8_t Soft_I2C_Slave_SDA_Read()
    {
    return HAL_GPIO_ReadPin(SOFT_I2C_SLAVE_SDA_PORT, SOFT_I2C_SLAVE_SDA_PIN);
    }

static uint8_t Soft_I2C_Slave_ACK_Read()
    {
    return HAL_GPIO_ReadPin(SOFT_I2C_SLAVE_SDA_PORT,
    SOFT_I2C_SLAVE_SDA_PIN) ? 0 : 1;
    }

static uint8_t Soft_I2C_Slave_SCL_Read()
    {
    return HAL_GPIO_ReadPin(SOFT_I2C_SLAVE_SCL_PORT, SOFT_I2C_SLAVE_SCL_PIN);
    }

static void Soft_I2C_Slave_State_Machine_SDA_ISR()
    {

    if (Soft_I2C_Slave.state == Slave_Detect_Start)
	{
	if (!Soft_I2C_Slave_SDA_Read() && Soft_I2C_Slave_SCL_Read())
	    {

	    Soft_I2C_Slave_SDA_Disable_INT();

	    Soft_I2C_Slave.state = Slave_Address_In;
	    Soft_I2C_Slave.Bit_Count = 0;
	    Soft_I2C_Slave.Current_Byte = 0;
	    Soft_I2C_Slave.Buffer_Index = 0;
	    Soft_I2C_Slave.Byte_Count = 0;
	    Soft_I2C_Slave.Active_Flag = 1;

	    if (Soft_I2C_Slave.callback != NULL)
		{
		//Soft_I2C_Slave.callback(Slave_Start_Detected);
		}

	    }
	}

    else if (Soft_I2C_Slave.state == Slave_Detect_Stop)
	{
	if (Soft_I2C_Slave_SDA_Read() && Soft_I2C_Slave_SCL_Read())
	    {
	    Soft_I2C_Slave.state = Slave_Detect_Start;

	    Soft_I2C_Slave.Active_Flag = 0;

	    if (Soft_I2C_Slave.callback != NULL)
		{
		//Soft_I2C_Slave.callback(Slave_Stop_Detected);
		}
	    }
	}

    }

static void Soft_I2C_Slave_State_Machine_SCL_ISR()
    {

    /*rising edge*/
    if (Soft_I2C_Slave_SCL_Read())
	{
	    /*to detect start or stop contion enable interrupt on sda line*/
	if (Soft_I2C_Slave.state == Slave_Detect_Start
		|| Soft_I2C_Slave.state == Slave_Detect_Stop)
	    {
	    Soft_I2C_Slave_SDA_Enable_INT();
	    }
	else if (Soft_I2C_Slave.state == Slave_Address_In)
	    {

	    Soft_I2C_Slave.Current_Byte <<= 1;

	    if (Soft_I2C_Slave_SDA_Read())
		{
		Soft_I2C_Slave.Current_Byte |= 0x01;
		}

	    Soft_I2C_Slave.Bit_Count++;

	    if (Soft_I2C_Slave.Bit_Count == 8)
		{

		Soft_I2C_Slave.Bit_Count = 0;

		Soft_I2C_Slave.Received_Address = Soft_I2C_Slave.Current_Byte;

		Soft_I2C_Slave.Current_Byte = 0;

		if (Soft_I2C_Slave.Received_Address
			== Soft_I2C_Slave.Own_Address)
		    {

		    if (Soft_I2C_Slave.callback != NULL)
			{
			Soft_I2C_Slave.callback(Slave_Write_Request);
			}

		    if (Soft_I2C_Slave.Byte_Count)
			{
			Soft_I2C_Slave.state = Slave_Give_ACK;
			}
		    }

		else if (Soft_I2C_Slave.Received_Address
			== (Soft_I2C_Slave.Own_Address + 1))
		    {

		    if (Soft_I2C_Slave.callback != NULL)
			{
			Soft_I2C_Slave.callback(Slave_Read_Request);
			}

		    if (Soft_I2C_Slave.Byte_Count)
			{
			Soft_I2C_Slave.state = Slave_Give_ACK;
			}
		    }
		else
		    {
		    Soft_I2C_Slave.state = Slave_Detect_Stop;
		    }

		}

	    }
	else if (Soft_I2C_Slave.state == Slave_Give_ACK)
	    {

	    if (Soft_I2C_Slave.Received_Address == Soft_I2C_Slave.Own_Address)
		{
		Soft_I2C_Slave.state = Slave_Data_In;
		}
	    else
		{/*(Own_Address + 1)*/
		Soft_I2C_Slave.state = Slave_Data_Out;
		}

	    }
	else if (Soft_I2C_Slave.state == Slave_Data_In)
	    {

	    if (Soft_I2C_Slave.Byte_Count)
		{

		Soft_I2C_Slave.Current_Byte <<= 1;

		if (Soft_I2C_Slave_SDA_Read())
		    {
		    Soft_I2C_Slave.Current_Byte |= 0x01;
		    }

		Soft_I2C_Slave.Bit_Count++;

		if (Soft_I2C_Slave.Bit_Count == 8)
		    {

		    Soft_I2C_Slave.Bit_Count = 0;

		    Soft_I2C_Slave.Byte_Count--;

		    Soft_I2C_Slave.state = Slave_Give_ACK;

		    Soft_I2C_Slave.Buffer[Soft_I2C_Slave.Buffer_Index] =
			    Soft_I2C_Slave.Current_Byte;

		    Soft_I2C_Slave.Current_Byte = 0;

		    Soft_I2C_Slave.Buffer_Index++;

		    }

		}
	    else
		{
		/* same as giving NACK*/
		Soft_I2C_Slave.state = Slave_Detect_Stop;
		}

	    }
	else if (Soft_I2C_Slave.state == Slave_Data_Out)
	    {

	    if (Soft_I2C_Slave.Bit_Count == 8)
		{

		Soft_I2C_Slave.Bit_Count = 0;

		Soft_I2C_Slave.Current_Byte = 0;

		Soft_I2C_Slave.state = Slave_Read_ACK;
		}

	    }
	else if (Soft_I2C_Slave.state == Slave_Read_ACK)
	    {

	    if (Soft_I2C_Slave_ACK_Read())
		{

		Soft_I2C_Slave.state = Slave_Data_Out;

		if (Soft_I2C_Slave.callback != NULL)
		    {
		    //Soft_I2C_Slave.callback(Slave_ACK_Received);
		    }

		/*grab byte from buffer to send*/
		Soft_I2C_Slave.Current_Byte =
			Soft_I2C_Slave.Buffer[Soft_I2C_Slave.Buffer_Index];

		Soft_I2C_Slave.Buffer_Index++;

		if (Soft_I2C_Slave.Buffer_Index == Soft_I2C_Slave.Byte_Count)
		    {
		    /*roll over*/
		    Soft_I2C_Slave.Buffer_Index = 0;
		    }

		}
	    else
		{
		if (Soft_I2C_Slave.callback != NULL)
		    {
		    //Soft_I2C_Slave.callback(Slave_NACK_Receivedt);
		    }
		Soft_I2C_Slave.state = Slave_Detect_Stop;
		}

	    }

	}

    else
	{
	/*falling edge edge*/
	if (Soft_I2C_Slave.state == Slave_Detect_Start
		|| Soft_I2C_Slave.state == Slave_Detect_Stop)
	    {
        // error? clk falling edege in star/stop detection
	    }
	else if (Soft_I2C_Slave.state == Slave_Address_In
		|| Soft_I2C_Slave.state == Slave_Data_In
		|| Soft_I2C_Slave.state == Slave_Read_ACK)
	    {
	    /*leave sda high*/
	    Soft_I2C_Slave_SDA_High();
	    }
	else if (Soft_I2C_Slave.state == Slave_Give_ACK)
	    {
	    Soft_I2C_Slave_SDA_Low();
	    }
	else if (Soft_I2C_Slave.state == Slave_Data_Out)
	    {
	    /*send Current_Byte*/
	    if (Soft_I2C_Slave.Current_Byte & 0x80)
		{
		Soft_I2C_Slave_SDA_High();
		}
	    else
		{
		Soft_I2C_Slave_SDA_Low();
		}

	    Soft_I2C_Slave.Current_Byte <<= 1;

	    Soft_I2C_Slave.Bit_Count++;
	    }

	}

    }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {

    if (GPIO_Pin == SOFT_I2C_SLAVE_SDA_PIN)
	{
	Soft_I2C_Slave_State_Machine_SDA_ISR();
	}

    if (GPIO_Pin == SOFT_I2C_SLAVE_SCL_PIN)
	{
	Soft_I2C_Slave_State_Machine_SCL_ISR();
	}

    }
