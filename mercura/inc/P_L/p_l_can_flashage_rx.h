/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_RX_CAN_FLASHAGE_H_
#define _P_L_RX_CAN_FLASHAGE_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define PL_CAN_ID_INFO_COEUR         ((U8)0x31)


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
void P_L_Can_flashage_rx_frame(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght);
#endif
