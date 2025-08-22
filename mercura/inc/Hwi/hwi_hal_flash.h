/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_FLASH_H
#define __HWI_FLASH_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define HWI_PAGE_SIZE_STM32F105                 0x800
#define HWI_WRITE_SIZE_STM32F105                2

#define HWI_PAGE_SIZE_STM32L431                 0x800
#define HWI_WRITE_SIZE_STM32L431                8


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern U8 HWI_Hal_Flash_Erase_Zone(U32 adresse_debut_zone_flash,U32 adresse_fin_zone_flash);
extern U8 HWI_Hal_Flash_Program(U32 adresse_debut_zone_flash, U8* data,U32 size);
#endif
