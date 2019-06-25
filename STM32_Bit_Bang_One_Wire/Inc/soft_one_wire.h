/*
 * soft_one_wire.h
 *
 *  Created on: 20-Apr-2019
 *      Author: medprime
 */

#ifndef SOFT_OW_H_
#define SOFT_OW_H_

#include "stm32f1xx_hal.h"
#include "main.h"

typedef struct Soft_OW_t
    {

	GPIO_TypeDef* GPIO_OW_Port;
	uint16_t GPIO_OW_Pin;

    } Soft_OW_t;

void Soft_OW_Init(Soft_OW_t* ow_handle);
uint8_t Soft_OW_Reset(Soft_OW_t* ow_handle);
void Soft_OW_Send_Byte(Soft_OW_t* ow_handle, uint8_t byte);
uint8_t Soft_OW_Receive_Byte(Soft_OW_t* ow_handle);

#endif /* SOFT_OW_H_ */
