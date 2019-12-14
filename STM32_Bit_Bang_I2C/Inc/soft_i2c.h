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

typedef enum Soft_I2C_Master_State_t
    {
    Generate_Start,
    Data_Out,
    Read_ACK,
    Data_In,
    Give_ACK,
    Generate_Stop
    } Soft_I2C_Master_State_t;

typedef enum Soft_I2C_Master_Flags_t
    {
    I2C_Ok,
    I2C_Error,
    I2C_Busy
    } Soft_I2C_Master_Flags_t;

typedef struct Soft_I2C_Master_t
    {

	uint8_t *I2C_Buffer; /*I2C working buffer*/
	uint16_t Buffer_Index; /* I2C_Buffer index*/

	GPIO_TypeDef *GPIO_SCL_Port;
	uint16_t GPIO_SCL_Pin;

	GPIO_TypeDef *GPIO_SDA_Port;
	uint16_t GPIO_SDA_Pin;

	uint8_t Address_RW; /* slave address plus read/write bit*/

	/**
	 * send the msb and lsb of memory address of slave
	 * slave memory address is stored in MEM_Address[]
	 * if memory address is two byte then MEM_Address[1] contains msb and MEM_Address[0] contains lsb.
	 * And and MEM_Address_Count is set to two.
	 * first decrement MEM_Address_Count grabs the msb (MEM_Address_Count is 1 now)
	 * second decrement MEM_Address_Count grabs the lsb (MEM_Address_Count is 0 now)
	 *
	 * if memory address is 1 byte then MEM_Address[0] contains lsb.
	 * And and MEM_Address_Count is set to 1.
	 * first decrement grab the lsb (MEM_Address_Count is 0 now)
	 */
	uint8_t MEM_Address[2]; /* memory address in slave to read from and write to 1byte or 2byte*/
	uint8_t MEM_Address_Count; /*1byte or 2byte*/

	uint16_t Byte_Count; /* no of bytes to transmit or receive*/

	uint8_t Bit_Count;
	uint8_t Current_Byte;

	uint8_t Active_Flag;

	Soft_I2C_Master_State_t State;

	Soft_I2C_Master_Flags_t Status_Flag;

	void (*callback)(Soft_I2C_Master_Flags_t flag);

    } Soft_I2C_Master_t;

void Soft_I2C_Master_Init();
void Soft_I2C_Master_Add(Soft_I2C_Master_t *i2c_handle);

void Soft_I2C_Master_Write_Byte(Soft_I2C_Master_t *i2c_handle,
	                        uint8_t slave_address,
	                        uint16_t register_address,
	                        uint8_t register_address_size,
				uint8_t *byte);

void Soft_I2C_Master_Write_Dummy(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
	                         uint8_t register_address_size);

void Soft_I2C_Master_Read_Byte(Soft_I2C_Master_t *i2c_handle,
	                       uint8_t slave_address,
			       uint8_t *data);

void Soft_I2C_Master_Write_Bytes(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
				 uint8_t register_address_size,
				 uint8_t* bytes,
				 uint16_t len);

void  Soft_I2C_Master_Read_Bytes(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
				 uint8_t *data,
				 uint8_t len);

void Soft_I2C_Master_Scan(Soft_I2C_Master_t *i2c_handle,
	                  uint8_t slave_address);

Soft_I2C_Master_Flags_t  Soft_I2C_Master_Get_Status(Soft_I2C_Master_t *i2c_handle);

void Soft_I2C_Master_Start();

void Soft_I2C_Master_Stop();

#endif /* SOFT_I2C_H_ */
