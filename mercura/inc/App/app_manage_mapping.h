/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_MAPPING_H_
#define _APP_MANAGE_MAPPING_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define APP_NB_MAX_BLOC             ((U8)0x10)
#define APP_NB_MAX_FACADE           ((U8)0x08)
#define APP_BLOC_VIRTUEL            ((U8)APP_NB_MAX_BLOC-1)



#define APP_NB_MAX_POSITION_BLOC    ((U8)0x20)
#define APP_NB_MAX_POSITION_FACADE  ((U8)0x20)

#define APP_NB_MAX_VOYANT           ((U8)20)
#define APP_NB_MAX_TOUCHE           ((U8)32)
#define APP_TAB_MAX_ETAT_LED        ((U8)APP_NB_MAX_TOUCHE*2)
#define APP_TAB_MAX_ETAT_BIP        ((U8)2)

//////////////////////////////////////////////////////////////////////////////////
// ADRESSE DES MODULES
//////////////////////////////////////////////////////////////////////////////////
#define APP_ADR_NOEUD_CANTPANEL              ((U32)0x190)
#define APP_ADR_NOEUD_CANFGRAPH              ((U32)0x90)

//////////////////////////////////////////////////////////////////////////////////
// CONFIG BLOCS
//////////////////////////////////////////////////////////////////////////////////
#define APP_LISTE_FACADE_CFG_BLOC               ((U32)0x0801A840)
#define APP_LISTE_BLOC_CFG_BLOC					((U32)0x0801A800)
#define APP_GESTION_WIFI_BLT_CFG_BLOC           ((U32)0x0801B8E6)
#define APP_GESTION_WIFI_BT_TAB_CFG_BLOC		((U32)0x0801C82B)

#define APP_TABLEAU_ODOMETRE_CAN_ID             ((U32)0x0801B85C)
#define APP_TABLEAU_ODOMETRE_CAN_BIT_DEPART     ((U32)0x0801B860)
#define APP_TABLEAU_ODOMETRE_CAN_TAILLE_DONNEE  ((U32)0x0801B861)
#define APP_TABLEAU_ODOMETRE_CAN_COEF_A         ((U32)0x0801B864)
#define APP_TABLEAU_ODOMETRE_CAN_COEF_B         ((U32)0x0801B868)
#define APP_TABLEAU_ODOMETRE_CAN_COEF_C         ((U32)0x0801B86C)
#define APP_CAN_ID_BLOC_ODOMETRE                ((U32)0x0801B8B8)

#define APP_TABLEAU_LOGIC_CAN_ID_APC            ((U32)0x0801B870)
#define APP_TABLEAU_LOGIC_CAN_BIT_DEPART_APC    ((U32)0x0801B874)
#define APP_TABLEAU_LOGIC_CAN_TAILLE_DONNEE_APC ((U32)0x0801B875)
#define APP_TABLEAU_LOGIC_CAN_ETAT_ON_APC       ((U32)0x0801B876)
#define APP_TABLEAU_LOGIC_CAN_ETAT_OFF_APC      ((U32)0x0801B877)

#define APP_TABLEAU_LOGIC_CAN_ID_MARRIERE               ((U32)0x0801B878)
#define APP_TABLEAU_LOGIC_CAN_BIT_DEPART_MARRIERE       ((U32)0x0801B87C)
#define APP_TABLEAU_LOGIC_CAN_TAILLE_DONNEE_MARRIERE    ((U32)0x0801B87D)
#define APP_TABLEAU_LOGIC_CAN_ETAT_ON_MARRIERE          ((U32)0x0801B87E)
#define APP_TABLEAU_LOGIC_CAN_ETAT_OFF_MARRIERE         ((U32)0x0801B87F)


//////////////////////////////////////////////////////////////////////////////////
// CONFIG FACADES
//////////////////////////////////////////////////////////////////////////////////
#define APP_TFT_NB_PMV_CARTE_LED              ((U32)0x0801C800)
#define APP_LISTE_FACADE_CFG_FACADE           ((U32)0x0801C804)
#define APP_LISTE_BLOC_CFG_FACADE             ((U32)0x0801CAE4)

#define APP_TFT_TOUCHE                        ((U32)0x0801C824)
#define APP_TFT_VOYANT                        ((U32)0x0801C924)
#define APP_TFT_PICTO_TOUCHE                  ((U32)0x0801C9C4)
#define APP_TFT_TOUCHE_G_CAN                  ((U32)0x0801CACC)
#define APP_TFT_ECRAN_CAN                     ((U32)0x0801CAD4)

#define APP_GESTION_WIFI_BLT_CFG_FACADE       ((U32)0x0801CB6E)
#define APP_GESTION_WIFI_BT_TAB_CFG_FACADE	  ((U32)0x0801CB91)

#define APP_CFG_NAME_FACADE                   ((U32)0x0801CB6F)
#define APP_CFG_NAME_BLOC                     ((U32)0x0801B160)



/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/

#endif
