#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"

#include "i2c.h"
#include "serial.h"

#include <stdio.h>

#define I2C_SCL_SDA_PORT     (0)
#define I2C_SDA_PIN          (0)
#define I2C_SCL_PIN          (1)
#define I2CDEV               LPC_I2C1

void setupI2C (void)
{
	// Create pin config for our I2C pins
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum     = PINSEL_FUNC_3;
	PinCfg.OpenDrain   = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode     = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum     = I2C_SCL_SDA_PORT;

	// Apply config to SDA pin
	PinCfg.Pinnum      = I2C_SDA_PIN;
	PINSEL_ConfigPin(&PinCfg);

	// Apply config to SCL pin
	PinCfg.Pinnum      = I2C_SCL_PIN;
	PINSEL_ConfigPin(&PinCfg);

	// Configure I2C to run at 100000Hz
	I2C_Init(I2CDEV, 100000);

	// Enable the I2C device
	I2C_Cmd(I2CDEV, ENABLE);
}

Status sendBytes (char address, uint8_t * data, uint8_t data_len)
{
	//I2C transmission config
	I2C_M_SETUP_Type i2c_m_setup;
	i2c_m_setup.sl_addr7bit = address;
	i2c_m_setup.tx_data = data;
	i2c_m_setup.tx_length = data_len;
	i2c_m_setup.rx_data = NULL;
	i2c_m_setup.rx_length = 0;
	i2c_m_setup.retransmissions_max = 0;

	// Transfer data & return status
	return I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
}

Status readBytes (char address, uint8_t * data, uint8_t data_len)
{
	//I2C transmission config
	I2C_M_SETUP_Type i2c_m_setup;
	i2c_m_setup.sl_addr7bit = address;
	i2c_m_setup.tx_data = NULL;
	i2c_m_setup.tx_length = 0;
	i2c_m_setup.rx_data = data;
	i2c_m_setup.rx_length = data_len;
	i2c_m_setup.retransmissions_max = 0;

	// Transfer data & return status
	return I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
}

void nmap (void)
{
	uint8_t data[2] = {0x00};
	char outStr[6];
	int num_devices = 0;
	int i;
	for(i = 0; i < (1 << 7); i++)
	{
		Status i2c_status = sendBytes(i, data, 2);
		if(i2c_status != 0)
		{
			sprintf(outStr, "0x%02X ", i);
			write_usb_serial_blocking(outStr,6);
			num_devices++;
		}
	}
	char outStr2[36];
	sprintf(outStr2, "\n\r%2d devices connected to i2c bus\n\r", num_devices);
	write_usb_serial_blocking(outStr2,36);
}
