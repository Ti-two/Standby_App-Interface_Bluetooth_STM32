/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef P_L_CC_RX_CAN1_H
#define P_L_CC_RX_CAN1_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define P_L_CC_ID_REPROG      			((U32)0x001)

#define P_L_CC_MASK_ID           		((U32)0x03FF)
#define P_L_CC_MASK_ADR_LOG           	((U32)0x0FF0)
#define P_L_CC_MASK_WR           		((U32)0x0400)
#define P_L_CC_MASTER            		((U32)0x0000)
#define P_L_CC_SLAVE             		((U32)0x0400)

#define P_L_CC_MASK_TYPE            	((U32)0x0030)
#define P_L_CC_MASK_PHYSIQUE_ADR    	((U32)0x03C0)
#define MODULE_CONTROLPANEL         	(U8)0x01
#define PHY_FACADE_GRAPH_DISPLAY    	(U8)0x02
#define PHY_FACADE_TPANEL_DISPLAY   	(U8)0x06


#define P_L_ID_SLAVE_TO_MASTER 			((U32)0x0400)


#define APP_FILTRAGE_PERTE_RESEAU_CAN   ((U32)10000)

#define APP_ADR_NOEUD_CAN_BLOC_PMV      ((U32)0x180)

#define APP_TEMPO_AV_FIN_LED            ((U32)1000)

#define CARRIAGE_RETURN          		((U8)0x0D)

#define P_L_CC_RPG_BOOT_JUMP_RQ          0x33    // Requête entrée/sortie mode boot

#define APP_ADRESSE_BASE_WIFI_BT		(U32)0x00E0

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Can_1_rx_frame(U32 p_l_can_msg_id,U8* p_l_can_data_ptr,  U8 p_l_can_lenght);
extern void P_L_Manage_Perte_Reseau(void);
extern void P_L_Can_Rx_Data_Init(void);
#endif
