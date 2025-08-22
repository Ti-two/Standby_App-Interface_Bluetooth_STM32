/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_I2S_H
#define __HWI_I2S_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"


/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define HWI_SAI_DATA_SIZE			((U8)2)
#define HWI_SAI_IN_NB_CHANNELS		((U8)1)
#define DECALAGE_AUDIO              ((U8)8)

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_I2S_Init(void);
extern void HWI_I2S_Start(void);
extern void HWI_I2S_Stop(void);
#endif
