#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"

void SPI_init()
{
	// ---=== Set up required pins ===---
	PINSEL_CFG_Type cfg;
	cfg.OpenDrain = 1;
	cfg.Pinmode = 0;
	cfg.Portnum = 0;
	// Function number depends on chosen SPI channel
	cfg.Funcnum = 3;
	// cfg.Pinnum = 15;
	// PINSEL_ConfigPin(&cfg);
	// cfg.Pinnum = 17;
	// PINSEL_ConfigPin(&cfg);
	cfg.Pinnum = 18;
	PINSEL_ConfigPin(&cfg);

	// ---=== SPI Config ===---
	// Leaving this non-parameterised for now
	SPI_CFG_Type spi_cfg;
	// read on second clock edge
	spi_cfg.CPHA = SPI_CPHA_SECOND;
	// Pull line low to clock
	spi_cfg.CPOL = SPI_CPOL_HI;
	// Clock rate (Hz)
	spi_cfg.ClockRate = 2500000;
	// Bit Order
	spi_cfg.DataOrder = SPI_DATA_MSB_FIRST;
	// Data bit (?)
	spi_cfg.Databit = 16; // ???????
	// SPI Mode
	spi_cfg.Mode = SPI_MASTER_MODE;
	// Initialize spi
	SPI_Init(LPC_SPI, &spi_cfg);
}

void SPI_rw(uint16_t *txbuff, uint16_t *rxbuff, uint32_t buff_len)
{
	SPI_DATA_SETUP_Type xferConfig;
	xferConfig.tx_data = txbuff;
	xferConfig.rx_data = rxbuff;
	xferConfig.length = buff_len;
	SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);
}

void SSP_init()
{
	// ---=== Set up required pins ===---
	PINSEL_CFG_Type cfg;
	cfg.OpenDrain = 0;
	cfg.Pinmode = 0;
	cfg.Portnum = 0;
	// Function number depends on chosen SPI channel
	cfg.Funcnum = 2;
	cfg.Pinnum = 7;
	PINSEL_ConfigPin(&cfg);
	cfg.Pinnum = 8;
	PINSEL_ConfigPin(&cfg);
	cfg.Pinnum = 9;
	PINSEL_ConfigPin(&cfg);

	// ---=== SSP Config ===---
	// Leaving this non-parameterised for now
	SSP_CFG_Type ssp_cfg;
	SSP_ConfigStructInit(&ssp_cfg);
	// // read on second clock edge
	// ssp_cfg.CPHA = SPI_CPHA_SECOND;
	// Pull line low to clock
	// ssp_cfg.CPOL = SPI_CPOL_HI;
	// // Clock rate (Hz)
	// ssp_cfg.ClockRate = 2000000;
	// // Data bit (?)
	ssp_cfg.Databit = SSP_DATABIT_16;
	// // SPI Mode
	// ssp_cfg.Mode = SSP_MASTER_MODE;
	// // Frame Format (~= Data order?)
	//ssp_cfg.FrameFormat = SSP_FRAME_SSP;
	// Initialize ssp
	SSP_Init(LPC_SSP1, &ssp_cfg);

	SSP_Cmd(LPC_SSP1, ENABLE);

}

void SSP_rw(uint8_t *txbuff, uint8_t *rxbuff, uint16_t buff_len)
{
	SSP_DATA_SETUP_Type xferConfig;
	xferConfig.tx_data = txbuff;
	xferConfig.rx_data = rxbuff;
	xferConfig.length = buff_len;
	SSP_ReadWrite(LPC_SSP1, &xferConfig, SSP_TRANSFER_POLLING);
}

void SSP_w(uint8_t datum)
{
	SSP_SendData(LPC_SSP1, datum);
}