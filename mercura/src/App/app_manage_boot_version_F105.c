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
#include "app_manage_boot_version.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
U32			APP_Boot_flash_adr;
U32			APP_Boot_sw_name_flash_adr;
U32			APP_Boot_hw_name_flash_adr;
U32			APP_Boot_sw_version_flash_adr;
U8			APP_Boot_version;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name   : APP_Test_Version_Boot
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : test boot version a set address
*******************************************************************************/
void APP_Test_Version_Boot(void)
{
U8_CAL* ptr_adr;

	//1 search for boot version adresse
	ptr_adr = (U8_CAL *)P_L_BOOT_VERSION_TEXT_FLASH_NEW_ADR;
	APP_Boot_version = (U8) *ptr_adr;

	//test if valid data
	if (0xFF == APP_Boot_version)
	{
		ptr_adr = (U8_CAL *)P_L_BOOT_VERSION_TEXT_FLASH_OLD_ADR;
		APP_Boot_version = (U8) *ptr_adr;

		APP_Boot_sw_version_flash_adr = P_L_BOOT_SW_VERSION_FLASH_OLD_ADR;
		APP_Boot_hw_name_flash_adr = P_L_BOOT_HW_NAME_FLASH_OLD_ADR;
		APP_Boot_sw_name_flash_adr = P_L_BOOT_SW_NAME_FLASH_OLD_ADR;
		APP_Boot_flash_adr = P_L_BOOT_FLASH_OLD_ADR;
	}
	else
	{
		APP_Boot_sw_version_flash_adr = P_L_BOOT_SW_VERSION_FLASH_NEW_ADR;
		APP_Boot_hw_name_flash_adr = P_L_BOOT_HW_NAME_FLASH_NEW_ADR;
		APP_Boot_sw_name_flash_adr = P_L_BOOT_SW_NAME_FLASH_NEW_ADR;
		APP_Boot_flash_adr = P_L_BOOT_FLASH_NEW_ADR;
	}
}
