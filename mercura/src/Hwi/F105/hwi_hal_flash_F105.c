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
#include "hwi_hal_flash.h"

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
/* NONE*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static U32 HWI_Hal_Flash_Nb_Pages(U32 adresse_debut_zone_flash, U32 adresse_fin_zone_flash);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name   : HWI_Hal_Flash_Erase_Zone
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*******************************************************************************/
U8 HWI_Hal_Flash_Erase_Zone(U32 adresse_debut_zone_flash,U32 adresse_fin_zone_flash)
{
U32 NbrOfPage;
U32 PageError;
FLASH_EraseInitTypeDef EraseInitStruct;
U8  code_rpg_status;

	PageError = 0;
	code_rpg_status = HAL_OK;

	HAL_FLASH_Unlock();

	NbrOfPage = HWI_Hal_Flash_Nb_Pages(adresse_debut_zone_flash,adresse_fin_zone_flash);

	// Fill EraseInit structure
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.PageAddress = adresse_debut_zone_flash;
	EraseInitStruct.NbPages = NbrOfPage;

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

	if (HAL_FLASHEx_Erase(&EraseInitStruct,(uint32_t*) &PageError) != HAL_OK)
	{
		code_rpg_status = HAL_ERROR;
	}

	HAL_FLASH_Lock();
    return code_rpg_status;
}


/*******************************************************************************
* Function Name  : HWI_Hal_Flash_Nb_Pages
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
static U32 HWI_Hal_Flash_Nb_Pages(U32 adresse_debut_zone_flash, U32 adresse_fin_zone_flash)
{
U32 nbpage;
U32 size;

	nbpage = 0x00;
	size = adresse_fin_zone_flash - adresse_debut_zone_flash;

	if((size % HWI_PAGE_SIZE_STM32F105) != 0)
	{
		nbpage = (size / HWI_PAGE_SIZE_STM32F105) + 1;
	}
	else
	{
		nbpage = size / HWI_PAGE_SIZE_STM32F105;
	}
	return nbpage;
}


/*******************************************************************************
* Function Name  : HWI_Hal_Flash_Programs
* Description    : program data in flash memory
*
*******************************************************************************/
U8 HWI_Hal_Flash_Program(U32 adresse_debut_zone_flash, U8* data,U32 size)
{
U32 RamSource;
U32 FlashDestination;
U32 boucle;
U8  code_rpg_status;

	code_rpg_status = HAL_OK;

	RamSource = (U32)data;
	FlashDestination = adresse_debut_zone_flash;

	HAL_FLASH_Unlock();

	for (boucle = 0;boucle < size;boucle += HWI_WRITE_SIZE_STM32F105)
	{
		if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FlashDestination,*(U16*)RamSource))
		{
			HAL_FLASH_Lock();
			code_rpg_status = HAL_ERROR;
			return code_rpg_status;
		}

		// check data write
		if (*(U16*)FlashDestination != *(U16*)RamSource)
		{
			HAL_FLASH_Lock();
			code_rpg_status = HAL_ERROR;
			return code_rpg_status;
		}

		FlashDestination += HWI_WRITE_SIZE_STM32F105;
		RamSource += HWI_WRITE_SIZE_STM32F105;
	}

	HAL_FLASH_Lock();
	return code_rpg_status;
}
