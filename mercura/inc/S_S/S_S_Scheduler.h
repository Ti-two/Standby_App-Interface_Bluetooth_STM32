/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _S_S_SCHEDULER_H_
#define _S_S_SCHEDULER_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define S_S_SCHEDULER_PERIOD_CPV 1

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void _Error_Handler(char *file, int line);
extern void Error_Handler(void);
extern void S_S_Scheduler_Callback(void);
extern void S_S_Scheduler_Start(void);
extern void S_S_Scheduler_high_priority(void);
#endif
