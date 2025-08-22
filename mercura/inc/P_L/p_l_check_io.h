/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef P_L_CHECK_IO_H
#define P_L_CHECK_IO_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "lib_filter.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
typedef struct
{
BOOL_TYPE	Read_state;
BOOL_TYPE	State;
U8 			Dbnc_cntr; // used for debounce input
M_DEBOUNCE_TYPE Debounce_struct;
}P_L_INPUT_STATE_TYPE;


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Check_Input(void);
extern void P_L_Check_Input_Init(void);
#endif
