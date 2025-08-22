/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _HWI_BX310X_H_
#define _HWI_BX310X_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
// TIMEOUT MODIFIÉ POUR BLUETOOTH SPP - Augmenté de 1s à 5s pour éviter déconnexions
#define HWI_TIMEMOUT_TX_RS232       ((U32)5000) // 5000ms (était 1000ms)

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_Bx310x_active_RF_Cmd(BOOL_TYPE cmd);
extern void HWI_Bx310x_Configuration_RF_High_Prio();
extern void HWI_Bx310x_Configuration_RF_Normal_Prio();
extern U16 HWI_Bx310x_tx(U16 data_length, U8* data);
#endif
