#include "soft_i2c_slave.h"
#include "stm32f4xx_hal.h"
#include "string.h"


void Soft_I2C_Callback(Soft_I2C_Slave_Event_t event);

uint8_t TX_Buffer[] = "hello from bit bang i2c slave";

uint8_t RX_Buffer[32] = "";

uint8_t RX_TX_Count;

void Soft_I2C_Callback(Soft_I2C_Slave_Event_t event)
    {
    if (event == Slave_Stop_Detected)
	{
	/* number of bytes received or sent*/
	RX_TX_Count = Soft_I2C_Slave.Buffer_Index;
	}
    }

extern I2C_HandleTypeDef hi2c1;

uint8_t TX_Buffer_M[] = "hello from STM32 I2C master";
uint8_t RX_Buffer_M[32] = "";

void example_main()
    {
    HAL_StatusTypeDef status;
    Soft_I2C_Slave_Init(Soft_I2C_Callback);

    //status = HAL_I2C_Master_Receive(&hi2c1, 0x50<<1, RX_Buffer_M, 0, 1000);

    status = HAL_I2C_Master_Transmit(&hi2c1, 0x50<<1, TX_Buffer_M, sizeof(TX_Buffer_M), 1000);

    HAL_Delay(1000);

    if(strcmp((char*)RX_Buffer, (char*)TX_Buffer_M) == 0)
	{
        // transaction successful
	}

    if(strcmp((char*)RX_Buffer_M, (char*)TX_Buffer) == 0)
	{
        // transaction successful
	}

    while(1)
	{

	}
    }
