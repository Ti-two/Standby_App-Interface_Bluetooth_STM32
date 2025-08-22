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
#include "hwi_watchdog.h"

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
BOOL_TYPE               HWI_reload_watchdog;
IWDG_HandleTypeDef      IwdgHandle;
//static __IO uint32_t uwLsiFreq = 0;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
/* NONE */

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name  : HWI_Run_watchdog
*  Description    :
*	Param		  :
*
*******************************************************************************/
void HWI_Arret_watchdog(void)
{
#ifdef _WATCHDOG
    HWI_reload_watchdog = FALSE;

    IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_4;
	IwdgHandle.Init.Reload    = 1;
	IwdgHandle.Init.Window = 1;

	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
		// Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}
#endif
}

/*******************************************************************************
*
*  Function Name  : HWI_Start_watchdog
*  Description    :
*	Param		  :
*
*******************************************************************************/
void HWI_Start_watchdog(void)
{
#ifdef _WATCHDOG
    IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_256;
	IwdgHandle.Init.Reload    = 0x00A6;
	IwdgHandle.Init.Window = 0x00A6;

	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
		// Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}

    HWI_reload_watchdog = TRUE;
#endif
}

/*******************************************************************************
*
*  Function Name  : HWI_Reload_watchdog
*  Description    :
*	Param		  :
*
*******************************************************************************/
void HWI_Reload_watchdog(void)
{
#ifdef _WATCHDOG
	if (TRUE == HWI_reload_watchdog)
	{
		HAL_IWDG_Refresh(&IwdgHandle);
	}

#endif
}
