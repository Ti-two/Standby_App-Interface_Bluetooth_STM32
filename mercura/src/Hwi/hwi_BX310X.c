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
#include "hwi_bx310x.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_hal_uart.h"
#include "lib_macros.h"
#include "string.h"
#include "stdio.h"
#include "p_l_bx310x.h"
#include "app_manage_wifi.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/


/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32					            S_S_1MS_U32_TIME;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : HWI_Bx310x_active_RF_Cmd
 *  Description     : commande le  wifi
 *
 *******************************************************************************/
void HWI_Bx310x_active_RF_Cmd(BOOL_TYPE cmd)
{
    HWI_EN_Rs232_Cmd(cmd);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Bx310x_Configuration_RF_High_Prio
 *  Description     : passe UART en high prio
 *
 *******************************************************************************/
void HWI_Bx310x_Configuration_RF_High_Prio()
{
    HWI_Configuration_Rs232_High_Prio();
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Bx310x_Configuration_RF_Normal_Prio
 *  Description     : passe UART en normal prio
 *
 *******************************************************************************/
void HWI_Bx310x_Configuration_RF_Normal_Prio()
{
    HWI_Configuration_Rs232_Normal_Prio();
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Bx310x_tx
 *  Description     :
 * 						Envoie de donn�e sur UART
 *******************************************************************************/
U8 data_tx[255];

U16 HWI_Bx310x_tx(U16 data_length, U8* data)
{


    strncpy((char *)data_tx,(char *)data,data_length);
    data_tx[data_length] = '\r';
    data_length++;
    HWI_Uart_tx(data_length,data_tx,HWI_TIMEMOUT_TX_RS232);
    return(data_length);
}
