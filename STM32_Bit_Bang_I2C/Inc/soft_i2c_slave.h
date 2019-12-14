#include "stm32f1xx_hal.h"

#ifndef SOFT_I2C_SLAVE_H_
#define SOFT_I2C_SLAVE_H_

#define SOFT_I2C_OK		0
#define SOFT_I2C_ERR		1

typedef enum Soft_I2C_Slave_State_t
    {
    Detect_Start,
    Address_In,
    Data_In,
    Give_ACK,
    Data_Out,
    Read_ACK,
    Detect_Stop
    } Soft_I2C_Slave_State_t;

typedef enum Soft_I2C_Slave_Events_t
    {
    I2C_Start_Detected,
    I2C_Read_Request,
    I2C_Write_Request,
    I2C_ACK_Received,
    I2C_NACK_Received,
    I2C_Stop_Detected,
    } Soft_I2C_Slave_Events_t;

typedef struct Soft_I2C_Slave_t
    {

	uint8_t *I2C_Buffer;   /* I2C working buffer*/
	uint16_t Buffer_Index; /* I2C_Buffer index*/

	/* no of bytes slave is ready to receive or send if set 0 nack nack will be send*/
	/* callback will be called when address is matched with I2C_Read_Request or I2C_Write_Request*/
	/* in callback set the buffer where the data will be written to or sen from*/
	/* set Buffer_Ready bytes, slave is able to receive or transmit*/
	uint16_t Buffer_Ready;

	GPIO_TypeDef *GPIO_SCL_Port;
	uint16_t GPIO_SCL_Pin;

	GPIO_TypeDef *GPIO_SDA_Port;
	uint16_t GPIO_SDA_Pin;

	uint8_t Own_Address;

	uint8_t Received_Address;


	uint8_t Bit_Count;
	uint8_t Current_Byte;

	uint8_t Active_Flag;

	Soft_I2C_Slave_Events_t Event;

	Soft_I2C_Slave_State_t State;

	void (*callback)(Soft_I2C_Slave_Events_t event);

    } Soft_I2C_Slave_t;

#endif /* SOFT_I2C_SLAVE_H_ */
