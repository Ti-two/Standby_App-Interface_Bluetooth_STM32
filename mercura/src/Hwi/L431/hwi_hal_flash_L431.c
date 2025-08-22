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
#include "p_l.h"


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
static U32 HWI_Hal_Flash_GetPage(U32 adresse_debut_zone_flash);

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
U32 FirstPage;
U32 NbOfPages;
uint32_t PageError;

FLASH_EraseInitTypeDef EraseInitStruct;
U8  code_rpg_status;

	PageError = 0;
	code_rpg_status = HAL_OK;

	HAL_FLASH_Unlock();

	// Clear OPTVERR bit set on virgin samples
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

	// Get the 1st page to erase
	FirstPage = HWI_Hal_Flash_GetPage(adresse_debut_zone_flash);

	// Get the number of pages to erase from 1st page
	NbOfPages = HWI_Hal_Flash_GetPage(adresse_fin_zone_flash) - FirstPage + 1;

	// Fill EraseInit structure
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks       = FLASH_BANK_1;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		code_rpg_status = HAL_ERROR;
	}

	HAL_FLASH_Lock();
    return code_rpg_status;
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

	for (boucle = 0;boucle < size;boucle += HWI_WRITE_SIZE_STM32L431)
	{
		if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlashDestination,*(U64*)RamSource))
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

		FlashDestination += HWI_WRITE_SIZE_STM32L431;
		RamSource += HWI_WRITE_SIZE_STM32L431;
	}

	HAL_FLASH_Lock();
	return code_rpg_status;
}



/*******************************************************************************
* Function Name  : HWI_Hal_Flash_Programs
* Description    : program data in flash memory
*
*******************************************************************************/
static U32 HWI_Hal_Flash_GetPage(U32 adresse_debut_zone_flash)
{
U32 page;

	page = 0;

	if (adresse_debut_zone_flash < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		//Bank 1
		page = (adresse_debut_zone_flash - FLASH_BASE) / FLASH_PAGE_SIZE;
	}
	else
	{
		//Bank 2
		page = (adresse_debut_zone_flash - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}

