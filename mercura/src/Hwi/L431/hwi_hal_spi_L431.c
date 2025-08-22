/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "hwi_hal_spi.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
/* NONE*/


/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
SPI_HandleTypeDef Spi1Handle;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/


/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_Init
 *  Description     : initialisation de SPI
 *
 *
 *******************************************************************************/
void Hwi_Hal_Spi_Init(void)
{
	Spi1Handle.Instance = SPI1;
	Spi1Handle.Init.Mode = SPI_MODE_MASTER;
	Spi1Handle.Init.Direction = SPI_DIRECTION_2LINES;
	Spi1Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	Spi1Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	Spi1Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	Spi1Handle.Init.NSS = SPI_NSS_SOFT;
	Spi1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	Spi1Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	Spi1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	Spi1Handle.Init.CRCPolynomial = 7;
	Spi1Handle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	Spi1Handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
}


/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_Slow_Init
 *  Description     : initialisation de SPI en Slow
 *
 *
 *******************************************************************************/
void Hwi_Spi_Slow_Init(void)
{
	Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	HAL_SPI_Init(&Spi1Handle);
}

/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_Fast_Init
 *  Description     : initialisation de SPI en Fast
 *
 *
 *******************************************************************************/
void Hwi_Spi_Fast_Init(void)
{
	Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;//SPI_BAUDRATEPRESCALER_2;
	HAL_SPI_Init(&Spi1Handle);
}



/*******************************************************************************
 *
 *  Function Name   : HAL_SPI_MspInit
 *  Description     : This function configures the hardware resources used in this example
 *
 *
 *******************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(hspi->Instance==SPI1)
	{
		// SPI1 clock enable
		__HAL_RCC_SPI1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}


/*******************************************************************************
 *
 *  Function Name   : HAL_SPI_MspDeInit
 *  Description     : This function freeze the hardware resources used in this example
 *
 *
 *******************************************************************************/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    // Peripheral clock disable
    __HAL_RCC_SPI1_CLK_DISABLE();

    //SPI1 GPIO Configuration
    //PA5     ------> SPI1_SCK
    //PA6     ------> SPI1_MISO
    //PA7     ------> SPI1_MOSI

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8);

  }
}



/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_ReadByte
 *  Description     : SPI read one byte
 *
 *
 *******************************************************************************/
U8 Hwi_Spi_ReadByte(void)
{
U8 ReceivedByte = 0, Dummy = 0xFF;

	while ((HAL_SPI_GetState(&Spi1Handle) != HAL_SPI_STATE_READY))
	{
	};

	HAL_SPI_TransmitReceive(&Spi1Handle, &Dummy, &ReceivedByte, 1, 5000);
	return ReceivedByte;
}

/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_ReadBytes
 *  Description     : SPI read x bytes
 *
 *
 *******************************************************************************/
U8 Hwi_Spi_ReadBytes(U8 *data, U16 numBytes)
{
U16 i = 0;

	for(i=0;i<numBytes;i++)
	{
		*(data+i) = Hwi_Spi_ReadByte();
	}
	return 0;
}
/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_SendByte
 *  Description     : SPI send one byte
 *
 *
 *******************************************************************************/
U8 Hwi_Spi_SendByte(U8 data)
{
U8 ReceivedByte = 0;

	while (HAL_SPI_GetState(&Spi1Handle) != HAL_SPI_STATE_READY)
	{
	};

	HAL_SPI_TransmitReceive(&Spi1Handle, &data, &ReceivedByte, 1, 5000);
	return ReceivedByte;
}
/*******************************************************************************
 *
 *  Function Name   : Hwi_Spi_SendByte
 *  Description     : SPI send x bytes
 *
 *
 *******************************************************************************/
U8 Hwi_Spi_SendBytes(U8 *data, U16 numBytes)
{
	HAL_SPI_Transmit(&Spi1Handle, data, numBytes, HAL_MAX_DELAY);
	return 0;
}

/*******************************************************************************
*
*  Function Name   : Hwi_Spi_Cs_SDCard_Init
*  Description     :
*
*
*******************************************************************************/
void Hwi_Spi_SDCard_Cs_Init(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	Hwi_Spi_Slow_Init();
}

/*******************************************************************************
*
*  Function Name   : Hwi_Spi_Cs_SDCard_Init
*  Description     : Selects the SD Card (Chip Select Pin)
*
*
*******************************************************************************/
void Hwi_Spi_SDCard_ChipSelect(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);	//Chip Select (Low Active)
}

/*******************************************************************************
*
*  Function Name   : Hwi_Spi_Cs_SDCard_Init
*  Description     : Deselects the SD Card (Chip Select Pin)
*
*
*******************************************************************************/
void Hwi_Spi_SDCard_ChipDeselect(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);	//Chip Select (Low Active)
}


