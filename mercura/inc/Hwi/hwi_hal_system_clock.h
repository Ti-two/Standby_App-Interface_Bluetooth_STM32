/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_SYSTEM_CLOCK_H
#define __HWI_SYSTEM_CLOCK_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define HWI_QUARTZ_8_MHz   8000000U
#define HWI_QUARTZ_20_MHz 20000000U

#define HSEStartUp_TimeOut   ((uint16_t)0x0500)

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_SystemClock_Config(void);

#endif
