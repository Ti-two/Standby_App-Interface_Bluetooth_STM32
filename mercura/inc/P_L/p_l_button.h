/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_BUTTON_H_
#define _P_L_BUTTON_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define APP_FILTRAGE_APPUI 					((U32)100)

typedef struct
{
	BOOL_TYPE   Read_state;
	BOOL_TYPE   State;
	U32         Timer;
	BOOL_TYPE   Lock;
}P_L_BUTTON_TYPE;


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Button_Init(void);
extern void P_L_Button_Manage(void);
extern void P_L_State_Button(U8 position,U8 state);
#endif
