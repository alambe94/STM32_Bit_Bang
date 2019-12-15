#include "stm32f1xx_hal.h"

#ifndef SOFT_I2C_SLAVE_H_
#define SOFT_I2C_SLAVE_H_

#define SOFT_I2C_OK		0
#define SOFT_I2C_ERR		1

typedef enum Soft_I2C_Slave_Event_t
    {
    Slave_Start_Detected,
    Slave_Read_Request,
    Slave_Write_Request,
    Slave_ACK_Received,
    Slave_NACK_Received,
    Slave_Stop_Detected,
    } Soft_I2C_Slave_Event_t;

#endif /* SOFT_I2C_SLAVE_H_ */
