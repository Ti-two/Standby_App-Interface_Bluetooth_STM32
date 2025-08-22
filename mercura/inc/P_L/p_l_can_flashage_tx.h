/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_TX_CAN_FLASHAGE_H_
#define _P_L_TX_CAN_FLASHAGE_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "app_manage_flashage.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define APP_PAS_DE_MODULE                  (U32) 0xFFFFFFFF

#define HWI_ADR_FAV_GRAPH_DISPLAY_BASE_WR		(U16)0x90
#define HWI_ADR_FAV_TPANEL_DISPLAY_BASE_WR		(U16)0x190

#define P_L_ADRESSE_START_GCM      (U32) 0x08002000
#define P_L_ADRESSE_END_GCM        (U32) 0x0801A7FC


#define P_L_ADRESSE_START_GCM_OLD      (U32) 0x08002000
#define P_L_ADRESSE_END_GCM_OLD        (U32) 0x0801A7FC

#define P_L_ADRESSE_START_GCM_NEW      (U32) 0x08020000
#define P_L_ADRESSE_END_GCM_NEW        (U32) 0x081FFFFF

#define P_L_OFFSET_PROGRAMATION_GCM     (U32)(P_L_ADRESSE_START_GCM_NEW-P_L_ADRESSE_START_GCM_OLD)

#define P_L_ADRESSE_START_GCM_T_PANEL  (U32) 0x00003000
#define P_L_ADRESSE_END_GCM_T_PANEL    (U32) 0x00080000
#define P_L_TAILLE_ZONE_GCM_T_PANEL    (U32)(P_L_ADRESSE_END_GCM_T_PANEL-P_L_ADRESSE_START_GCM_T_PANEL)

#define P_L_ADRESSE_START_CCM_OLD      (U32) 0x0801A800
#define P_L_ADRESSE_END_CCM_OLD        (U32) 0x0801F7FE


#define P_L_ADRESSE_START_CCM_NEW      (U32) 0x0800C000
#define P_L_ADRESSE_END_CCM_NEW        (U32) 0x0801FFFF

#define P_L_ADRESSE_START_CCM_ZONE_1_OLD  (U32) 0x0801A800
#define P_L_ADRESSE_START_CCM_ZONE_2_OLD  (U32) 0x0801C800

#define P_L_ADRESSE_START_CCM_ZONE_NEW_1  (U32) 0x0800C000
#define P_L_ADRESSE_START_CCM_ZONE_NEW_2  (U32) 0x08010000

#define P_L_ADRESSE_START_CCM_LOGO       (U32) 0x08020000
#define P_L_ADRESSE_END_CCM_LOGO         (U32) 0x0802FFFF

#define P_L_TAILLE_ZONE_CCM              (U32)(P_L_ADRESSE_END_CCM_LOGO-P_L_ADRESSE_START_CCM_NEW)

#define P_L_TAILLE_ZONE_GCM_OLD          (U32)(P_L_ADRESSE_END_GCM_OLD-P_L_ADRESSE_START_GCM_OLD)
#define P_L_TAILLE_ZONE_GCM_NEW          (U32)(P_L_ADRESSE_END_GCM_NEW-P_L_ADRESSE_START_GCM_NEW)

#define P_L_OFFSET_PROGRAMATION_ZONE_1   (U32)(P_L_ADRESSE_START_CCM_ZONE_1_OLD-P_L_ADRESSE_START_CCM_ZONE_NEW_1)
#define P_L_OFFSET_PROGRAMATION_ZONE_2   (U32)(P_L_ADRESSE_START_CCM_ZONE_2_OLD-P_L_ADRESSE_START_CCM_ZONE_NEW_2)


#define P_L_TAILLE_SEGMENT_T_PANEL       512

#define P_L_TAILLE_BUFFER_SEGMENT       (40*P_L_TAILLE_SEGMENT_T_PANEL)

#define P_L_ADRESSE_RAM           (U32) 0x20000000
#define P_L_ADRESSE_RAM_T_PANEL   (U32) 0x10000000

#define P_L_CAN_NB_MAX_HISTORIQUE         ((U16)32)
#define P_L_CAN_TAILLE_1_HISTORIQUE       ((U16)64)
#define P_L_CAN_TAILLE_BUFFER_HISTORIQUE  P_L_CAN_NB_MAX_HISTORIQUE*P_L_CAN_TAILLE_1_HISTORIQUE
#define P_L_CAN_NB_LOT_CAN_HISTORIQUE     ((U8)13)


#define P_L_CAN_NB_ESSAI_MAX            ((U8)0x03)
#define P_L_CAN_TIMEOUT_SEND            ((U32)2000)
#define P_L_CAN_TIMEOUT_NEXT            ((U32)2)
#define P_L_TIMEOUT_SEND_ERASE_XXL      ((U32)20000)


typedef enum
{
    CAN_START_CHGT_ADRESSE = 0,
    CAN_RETOUR_EN_BOOT_TX = 1,
    CAN_RETOUR_EN_BOOT_RX = 2,
    CAN_CHGT_LOG_ADR_TX = 3,
    CAN_CHGT_LOG_ADR_RX = 4,
    CAN_QUITER_BOOT_TX = 5,
    CAN_QUITER_BOOT_RX = 6,
    CAN_FIN_CHGT_ADR = 7,
    CAN_ERROR_CHGT_ADR = 8
}P_L_CAN_CHGT_ADR_STEP;

typedef enum
{
    CAN_START_REPROG = 0,
    CAN_RETOUR_EN_BOOT_REPROG_TX = 1,
    CAN_RETOUR_EN_BOOT_REPROG_RX = 2,
    CAN_ANALYSE_MODULE_REPROG = 3,
    CAN_SEGEMENT_REPROG = 4,
    CAN_START_TELECHARGEMENT_TX = 5,
    CAN_START_TELECHARGEMENT_RX = 6,
    CAN_TRANSFERT_FIRST_DATA_TX = 7,
    CAN_TRANSFERT_FIRST_DATA_RX = 8,
    CAN_TRANSFERT_NEXT_DATA = 9,
    CAN_TRANSFERT_END_TX = 10,
    CAN_TRANSFERT_END_RX = 11,
    CAN_CALCUL_CHEKSUM_TX = 12,
    CAN_CALCUL_CHEKSUM_RX = 13,
    CAN_WRITE_CHEKSUM_TX = 14,
    CAN_WRITE_CHEKSUM_RX = 15,
    CAN_READ_HISTORIQUE_TX = 16,
    CAN_READ_HISTORIQUE_RX = 17,
    CAN_ERASE_HISTORIQUE_TX = 18,
    CAN_ERASE_HISTORIQUE_RX = 19,
    CAN_WRITE_HISTORIQUE_1 = 20,
    CAN_WRITE_HISTORIQUE = 21,
    CAN_SORTIE_BOOT_TX = 22,
    CAN_SORTIE_BOOT_RX = 23,
    CAN_END_REPROG = 24,
    CAN_ERROR_REPROG = 25
}P_L_CAN_REPROG_MOD_TYPE;


typedef enum
{
    CAN_START_READ_HISTORIQUE = 0,
    CAN_RETOUR_EN_BOOT_HISTORIQUE_TX = 1,
    CAN_RETOUR_EN_BOOT_HISTORIQUE_RX = 2,
    CAN_READ_READ_HISTORIQUE_TX = 3,
    CAN_READ_READ_HISTORIQUE_RX = 4,
    CAN_SORTIE_BOOT_HISTORIQUE_TX = 5,
    CAN_SORTIE_BOOT_HISTORIQUE_RX = 6,
    CAN_SEND_HISTORIQUE = 7,
    CAN_END_READ_HISTORIQUE = 8,
    CAN_ERROR_READ_HISTORIQUE = 9
}P_L_CAN_HISTORIQUE_TYPE;


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Can_flashage_Tx_IT(void);
extern void P_L_Can_Send_Wake_UP(void);

extern APP_FLASHAGE_STATE P_L_Can_Manage_Change_Adr(void);
extern void P_L_Can_Manage_Change_Adr_Init(U32 adresse,U8 log_adresse);
extern APP_FLASHAGE_STATE P_L_Can_Manage_Read_Historique(void);
extern APP_FLASHAGE_STATE P_L_Can_Manage_Reprog_One(void);
extern void P_L_Can_Manage_Reprog_Init(U32 adresse,U8  *file_reprog);
extern void P_L_Can_Manage_Historique_Init(U32 adresse,BOOL_TYPE full_historique);
#endif
