/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_LINK_H_
#define _APP_MANAGE_LINK_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
typedef struct
{
U8 octet_pos;
U8 bit_pos;
}P_L_LED_TYPE;

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void App_Manage_Link_Init(void);
extern void App_Manage_Command(void);
extern void App_Manage_Touche_touch_screen(void);
extern void App_Manage_Touche_Handy(void);
extern void App_Manage_Voyant_Handy(void);


#endif
