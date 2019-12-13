/*
* Soft_I2C.h
*
*  Created on: 10-Jul-2017
*      Author: medprime
*/

#include "stm32f1xx_hal.h"

#include "ring_buffer.h"


#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_

#define SOFT_I2C_OK		0
#define SOFT_I2C_ERR		1

typedef struct Soft_I2C_t
    {
	GPIO_TypeDef *GPIO_SCL_Port;
	uint16_t GPIO_SCL_Pin;

	GPIO_TypeDef *GPIO_SDA_Port;
	uint16_t GPIO_SDA_Pin;

    } Soft_I2C_t;


enum Soft_I2C_Master_State_t
    {
    Generate_Start,
    Data_Out,
    Read_ACK,
    Data_In,
    Give_ACK,
    Generate_Stop
    };

enum Soft_I2C_Master_Flags_t
    {
    I2C_Time_Out,
    I2C_Ok,
    I2C_Error,
    I2C_Busy
    };

typedef struct Soft_I2C_Master_t
    {

	Ring_Buffer_t Ring_Buffer;

	GPIO_TypeDef *GPIO_SCL_Port;
	uint16_t GPIO_SCL_Pin;

	GPIO_TypeDef *GPIO_SDA_Port;
	uint16_t GPIO_SDA_Pin;

	uint8_t CLK_State;

	uint8_t Address_RW;
	uint16_t Bytes_To_Read;

	uint8_t Bit_Count;
	uint8_t Current_Byte;

	uint8_t Active_Flag;

	enum Soft_I2C_Master_State_t State;

	enum Soft_I2C_Master_Flags_t Status_Flag;

    } Soft_I2C_Master_t;


void Soft_I2C_Init(Soft_I2C_t* i2c_handle);

uint8_t Soft_I2C_Write_Byte(Soft_I2C_t* i2c_handle,
	                   uint8_t slave_address,
			   uint8_t register_address,
			   uint8_t register_address_size,
                           uint8_t *byte);

uint8_t Soft_I2C_Read_Byte(Soft_I2C_t* i2c_handle,
	                   uint8_t slave_address,
			   uint8_t register_address,
			   uint8_t register_address_size,
                           uint8_t *val);

uint8_t Soft_I2C_Write_Bytes(Soft_I2C_t* i2c_handle,
	                     uint8_t slave_address,
			     uint8_t register_address,
			     uint8_t register_address_size,
                             uint8_t *buf,
			     uint8_t num);

uint8_t Soft_I2C_Read_Bytes(Soft_I2C_t* i2c_handle,
	                    uint8_t slave_address,
	                    uint8_t register_address,
			    uint8_t register_address_size,
                            uint8_t *buf,
			    uint8_t num);


uint8_t Soft_I2C_Scan(Soft_I2C_t* i2c_handle, uint8_t slave_address);

#endif /* SOFT_I2C_H_ */
