/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _HWI_WATCHDOG_H_
#define _HWI_WATCHDOG_H_
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
extern void HWI_Arret_watchdog(void);
extern void HWI_Start_watchdog(void);
extern void HWI_Reload_watchdog(void);
#endif

