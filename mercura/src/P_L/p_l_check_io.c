/*******************************************************************************
*                    Synergie Ing�nierie Industrielle
*
*                    This document is the property of
*                    Synergie Ing�nierie Industrielle
*                    It must not be copied (in whole or in part)
*                    or disclosed without prior written consent
*                    of the company. Any copies by any method
*                    must also include a copy of this legend.

*******************************************************************************/

/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "p_l_check_io.h"

/********************************************************************/
/*  Application Level Include Files                                 */
/********************************************************************/
#include "lib_filter.h"
#include "hwi_hal_gpio.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE       P_L_CarteSd_Atester_vue;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
P_L_INPUT_STATE_TYPE   P_L_Bp1;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name  : P_L_Check_Input
*  Description    : on debouce les entrees
*	Param		  :
*
*******************************************************************************/
void P_L_Check_Input(void)
{
	P_L_Bp1.Read_state = HWI_Button_read();
	P_L_Bp1.State = M_DEBOUNCE(P_L_Bp1.Read_state,8,8,&(P_L_Bp1.Debounce_struct));

	if(TRUE == P_L_CarteSd_Atester_vue)
	{
		HWI_Led_1_Verte_Cmd(P_L_Bp1.Read_state);
	}
}

/*******************************************************************************
*
*  Function Name  : P_L_Check_Input_Init
*  Description    : test si l'e2prom est prete
*	Param				:
*
*******************************************************************************/
void P_L_Check_Input_Init(void)
{
	M_DEBOUNCE_INIT(FALSE,&(P_L_Bp1.Dbnc_cntr),&(P_L_Bp1.Debounce_struct));
}

