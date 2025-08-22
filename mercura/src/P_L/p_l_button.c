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
#include "p_l_button.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_filter.h"
#include "lib_macros.h"
#include "app_manage_mapping.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32                      S_S_1MS_U32_TIME;


/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
P_L_BUTTON_TYPE                 P_L_Button[2*APP_NB_MAX_POSITION_FACADE];
BOOL_TYPE                       P_L_New_button_rx;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name  : P_L_State_Button
*  Description    : on traite les bouttons
*	Param		  :
*
*******************************************************************************/
void P_L_State_Button(U8 position,U8 state)
{
    if (position < (2*APP_NB_MAX_POSITION_FACADE))
    {
        P_L_Button[position].Read_state = state;
        P_L_Button_Manage();
    }
}

/*******************************************************************************
*
*  Function Name  : P_L_Button_Manage
*  Description    : on traite les bouttons
*	Param		  :
*
*******************************************************************************/
void P_L_Button_Manage(void)
{
U8 boucle;
BOOL_TYPE result;

    for (boucle=0;boucle<(2*APP_NB_MAX_POSITION_FACADE);boucle++)
    {
		BT_TIMER_LOCK(P_L_Button[boucle].Timer,S_S_1MS_U32_TIME,APP_FILTRAGE_APPUI,P_L_Button[boucle].Lock,result);

		if (TRUE == result)
		{ // ecoule
			if (P_L_Button[boucle].State != P_L_Button[boucle].Read_state)
			{
				P_L_Button[boucle].State = P_L_Button[boucle].Read_state;
				BT_TIMER_INIT_LOCK(P_L_Button[boucle].Timer,S_S_1MS_U32_TIME,P_L_Button[boucle].Lock);
				P_L_New_button_rx = TRUE;
			}
		}
	}
}

/*******************************************************************************
*
*  Function Name  : P_L_Button_Init
*  Description    : test si l'e2prom est prete
*	Param		  :
*
*******************************************************************************/
void P_L_Button_Init(void)
{
U8 boucle;

    for (boucle=0;boucle<APP_NB_MAX_POSITION_FACADE;boucle++)
    {
        P_L_Button[boucle].State = FALSE;
        P_L_Button[boucle].Read_state = FALSE;
        BT_TIMER_INIT_LOCK(P_L_Button[boucle].Timer,0x00,P_L_Button[boucle].Lock);
    }
    P_L_New_button_rx = FALSE;
}

