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

	Ring_Buffer_t I2C_TX_Buffer; /*I2C TX buffer*/
	Ring_Buffer_t I2C_RX_Buffer; /*I2C RX buffer*/

	GPIO_TypeDef *GPIO_SCL_Port;
	uint16_t GPIO_SCL_Pin;

	GPIO_TypeDef *GPIO_SDA_Port;
	uint16_t GPIO_SDA_Pin;

	uint8_t Address_RW; /* slave address plus read/write bit*/

	uint16_t Bytes_To_Receive; /* no of bytes to receive*/

	uint8_t Bit_Count;
	uint8_t Current_Byte;

	uint8_t Active_Flag;

	Soft_I2C_Master_State_t State;

	Soft_I2C_Master_Flags_t Status_Flag;

	void (*callback)(Soft_I2C_Master_Flags_t flag);

    } Soft_I2C_Master_t;


void Soft_I2C_Master_Init();
void Soft_I2C_Master_Add(Soft_I2C_Master_t *i2c_handle);

void Soft_I2C_Master_Start();

void Soft_I2C_Master_Stop();

Soft_I2C_Master_Flags_t  Soft_I2C_Master_Get_Status(Soft_I2C_Master_t *i2c_handle);

uint16_t Soft_I2C_Master_Read_Available(Soft_I2C_Master_t *i2c_handle);

uint8_t Soft_I2C_Master_Read_Byte(Soft_I2C_Master_t *i2c_handle);

void Soft_I2C_Master_Write_Byte_IT(Soft_I2C_Master_t *i2c_handle,
	                        uint8_t slave_address,
	                        uint16_t register_address,
	                        uint8_t register_address_size,
				uint8_t *byte);

void Soft_I2C_Master_Write_Dummy_IT(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
	                         uint8_t register_address_size);

void Soft_I2C_Master_Write_Bytes_IT(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
				 uint8_t register_address_size,
				 uint8_t* bytes,
				 uint16_t len);

void  Soft_I2C_Master_Read_Request_IT(Soft_I2C_Master_t *i2c_handle,
	                           uint8_t slave_address,
				   uint8_t len);

void Soft_I2C_Master_Scan_IT(Soft_I2C_Master_t *i2c_handle,
	                  uint8_t slave_address);





void Soft_I2C_Master_Write_Byte(Soft_I2C_Master_t *i2c_handle,
	                        uint8_t slave_address,
	                        uint16_t register_address,
	                        uint8_t register_address_size,
				uint8_t *byte);

void Soft_I2C_Master_Write_Dummy(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
	                         uint8_t register_address_size);

uint8_t Soft_I2C_Master_Read_Byte(Soft_I2C_Master_t *i2c_handle);

void Soft_I2C_Master_Write_Bytes(Soft_I2C_Master_t *i2c_handle,
	                         uint8_t slave_address,
	                         uint16_t register_address,
				 uint8_t register_address_size,
				 uint8_t* bytes,
				 uint16_t len);

void  Soft_I2C_Master_Read_Request(Soft_I2C_Master_t *i2c_handle,
	                           uint8_t slave_address,
				   uint8_t len);

void Soft_I2C_Master_Scan(Soft_I2C_Master_t *i2c_handle,
	                  uint8_t slave_address);


#endif /* SOFT_I2C_H_ */
