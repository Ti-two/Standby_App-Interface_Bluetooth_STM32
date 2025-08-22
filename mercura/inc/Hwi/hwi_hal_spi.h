/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_SPI_H
#define __HWI_SPI_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void Hwi_Hal_Spi_Init(void);
extern void Hwi_Spi_Slow_Init(void);
extern void Hwi_Spi_Fast_Init(void);
extern U8 Hwi_Spi_ReadByte(void);
extern U8 Hwi_Spi_ReadBytes(U8 *data, U16 numBytes);
extern U8 Hwi_Spi_SendByte(U8 data);
extern U8 Hwi_Spi_SendBytes(U8 *data, U16 numBytes);
extern void Hwi_Spi_SDCard_Cs_Init(void);
extern void Hwi_Spi_SDCard_ChipSelect(void);
extern void Hwi_Spi_SDCard_ChipDeselect(void);
#endif
