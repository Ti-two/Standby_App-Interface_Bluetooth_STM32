/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_CC_TX_CAN1_
#define _P_L_CC_TX_CAN1_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define MY_CR_LOCK_Set              ((uint32_t)0x00000080)


/*******************************************************************************
*  Feature Macro Definitions
*******************************************************************************/
#define PL_MASK_ADRESSE             ((U32)0xFFFFFFF0)
#define PL_MASK_SOUS_ADR_GRAPH      ((U32)0x00000001)

//#define PL_CAN_LOT1                 ((U8)0x01)
//#define PL_CAN_LOT2                 ((U8)0x02)
//#define PL_CAN_LOT3                 ((U8)0x03)
//#define PL_CAN_LOT4                 ((U8)0x04)
//#define PL_CAN_LOT5                 ((U8)0x05)
//#define PL_CAN_LOT6                 ((U8)0x06)


#define P_L_CC_ID_DATA_AUTRE         ((U32)0x03)
#define P_L_CC_ID_SEND_DATA_AUTRE         ((U32)0x403)
#define PL_CAN_ID_SEUIL_CFG          ((U8)0x05)
#define PL_CAN_ID_SEUIL_USER         ((U8)0x06)
#define PL_CAN_ID_CONSIGNE_DEMARRAGE ((U8)0x09)
#define PL_CAN_ID_ECLAIRAGE_DEMARRAGE ((U8)0x0A)
#define PL_CAN_ID_LECTURE_DATA_CODE ((U8)0x0B)
#define PL_CAN_ID_AIGUILLAGE_AUDIO  ((U8)0x0D)
    #define APP_MODE_LECTURE     ((U8)0)
    #define APP_MODE_ECRITURE    ((U8)1)

#define P_L_NB_TENTATIVE_CAN        ((U8)0x01)

#define PL_CAN_BIP_OFF              ((U8)0x00)
#define PL_CAN_BIP_ON               ((U8)0x01)

#define PL_CAN_BIP_CONTINUE         ((U8)0x80)
#define PL_CAN_BIP_NON_CONTINUE     ((U8)0x00)

#define PL_CAN_BIP_1                ((U8)2)
#define PL_CAN_BIP_2                ((U8)4)
#define PL_CAN_BIP_3                ((U8)6)
#define PL_CAN_BIP_4                ((U8)8)
#define PL_CAN_BIP_5                ((U8)10)
#define PL_CAN_BIP_6                ((U8)12)
#define PL_CAN_BIP_7                ((U8)14)
#define PL_CAN_BIP_8                ((U8)16)
#define PL_CAN_BIP_9                ((U8)18)
#define PL_CAN_BIP_10               ((U8)20)
#define PL_CAN_BIP_11               ((U8)22)
#define PL_CAN_BIP_12               ((U8)24)
#define PL_CAN_BIP_13               ((U8)26)
#define PL_CAN_BIP_14               ((U8)28)
#define PL_CAN_BIP_15               ((U8)30)
#define PL_CAN_BIP_16               ((U8)32)

#define PL_CAN_ID_INFO_SOFT         ((U8)0x06)
#define PL_CAN_VERSION_SOFT         ((U8)0x00)
#define PL_CAN_CHECKSUM_SOFT        ((U8)0x01)
#define PL_CAN_VERSION_BOOT         ((U8)0x02)

#define PL_CAN_NAME_CFG_1           ((U8)0x03)
#define PL_CAN_NAME_CFG_2           ((U8)0x04)
#define PL_CAN_NAME_CFG_3           ((U8)0x05)


#define PL_CAN_ID_ETAT              ((U8)0x07)

#define PL_CAN_ID_LED1              ((U8)0x08)
#define PL_CAN_ID_LED2              ((U8)0x09)
#define PL_CAN_ID_LED3              ((U8)0x0A)
#define PL_CAN_ID_LED4              ((U8)0x0B)
#define PL_CAN_ID_LED5              ((U8)0x0C)
#define PL_CAN_ID_LED6              ((U8)0x0D)

#define PL_CAN_ID_LED_LOT1          ((U8)0x00)
#define PL_CAN_ID_LED_LOT2          ((U8)0x07)
#define PL_CAN_ID_LED_LOT3          ((U8)0x0E)
#define PL_CAN_ID_LED_LOT4          ((U8)0x15)
#define PL_CAN_ID_LED_LOT5          ((U8)0x1C)
#define PL_CAN_ID_LED_LOT6          ((U8)0x23)

#define PL_CAN_ID_TOR1              ((U8)0x08)
#define PL_CAN_ID_TOR2              ((U8)0x09)
#define PL_CAN_ID_TOR3              ((U8)0x0A)
#define PL_CAN_ID_TOR4              ((U8)0x0B)
#define PL_CAN_ID_TOR5              ((U8)0x0C)
#define PL_CAN_ID_TOR6              ((U8)0x0D)

#define PL_CAN_ID_TOR_LOT1          ((U8)0x00)
#define PL_CAN_ID_TOR_LOT2          ((U8)0x07)
#define PL_CAN_ID_TOR_LOT3          ((U8)0x0E)
#define PL_CAN_ID_TOR_LOT4          ((U8)0x15)
#define PL_CAN_ID_TOR_LOT5          ((U8)0x1C)
#define PL_CAN_ID_TOR_LOT6          ((U8)0x23)

#define PL_CAN_ID_LEDPWM            ((U8)0x0E)

#define PL_CAN_ID_TOR_PWM1          ((U8)0x0E)
#define PL_CAN_ID_TOR_PWM2          ((U8)0x0F)
#define PL_CAN_ID_TOR_PWM3          ((U8)0x10)
#define PL_CAN_ID_TOR_PWM4          ((U8)0x11)
#define PL_CAN_ID_TOR_PWM5          ((U8)0x12)
#define PL_CAN_ID_TOR_PWM6          ((U8)0x13)
#define PL_CAN_ID_TOR_PWM7          ((U8)0x23)
#define PL_CAN_ID_TOR_PWM8          ((U8)0x24)

#define PL_CAN_TOR_PWM_LOT1       ((U8)0x00)
#define PL_CAN_TOR_PWM_LOT2       ((U8)0x03)
#define PL_CAN_TOR_PWM_LOT3       ((U8)0x06)
#define PL_CAN_TOR_PWM_LOT4       ((U8)0x09)
#define PL_CAN_TOR_PWM_LOT5       ((U8)0x0C)



#define PL_CAN_DEFAUT_CC            ((U8)0x06)
#define PL_CAN_DEFAUT_CO1           ((U8)0x04)
#define PL_CAN_DEFAUT_CO2           ((U8)0x02)
#define PL_CAN_POW_ETAT             ((U8)0x01)
#define PL_CAN_MASK_DEFAUT_CO_CC    ((U8)0x06)

#define PL_CAN_CC_VUE               ((U8)0x18)
#define PL_CAN_CO1_VUE              ((U8)0x10)
#define PL_CAN_CO2_VUE              ((U8)0x08)
#define PL_CAN_MASK_CO_CC_VUE       ((U8)0x18)

#define PL_CAN_ID_COUR1             ((U8)0x14)
#define PL_CAN_ID_COUR2             ((U8)0x15)
#define PL_CAN_ID_COUR3             ((U8)0x16)
#define PL_CAN_ID_COUR4             ((U8)0x17)
#define PL_CAN_ID_COUR5             ((U8)0x18)
#define PL_CAN_ID_COUR6             ((U8)0x19)

#define PL_CAN_ID_BIP               ((U8)0x14)
#define PL_CAN_ID_NUM_MESS          ((U8)0x15)


#define PL_CAN_ID_IN1               ((U8)0x1E)

#define PL_CAN_ID_ANA1              ((U8)0x1F)
#define PL_CAN_ID_ANA2              ((U8)0x20)
#define PL_CAN_ID_ANA3              ((U8)0x21)
#define PL_CAN_ID_ANA4              ((U8)0x22)


#define PL_CAN_ID_COUNT1             ((U8)0x28)
#define PL_CAN_ID_COUNT2             ((U8)0x29)
#define PL_CAN_ID_COUNT3             ((U8)0x2A)
#define PL_CAN_ID_COUNT4             ((U8)0x2B)

#define PL_CAN_ID_CMD                ((U8)0x30)

#define PL_ON_OFF_ANALYSE            ((U8)0x01)
#define PL_ON_OFF_SYS                ((U8)0x02)
#define PL_INFO_SYS                  ((U8)0x04)
#define PL_CMD_CHARGE_1              ((U8)0x08)
#define PL_ETAT_CHARGE_1             ((U8)0x10)
#define PL_DIAG_PROD                 ((U8)0x20)
#define PL_CMD_CHARGE_2              ((U8)0x40)
#define PL_ETAT_CHARGE_2             ((U8)0x80)

#define PL_CAN_ID_INFO_COEUR         ((U8)0x31)
#define PL_CAN_INFO_1                ((U8)0x01)
#define PL_CAN_INFO_2                ((U8)0x02)
#define PL_CAN_INFO_3                ((U8)0x03)
#define PL_CAN_INFO_4                ((U8)0x04)

#define PL_CAN_ID_MESS_PMV_1         ((U8)0x40)
#define PL_CAN_ID_MESS_PMV_2         ((U8)0x41)
#define PL_CAN_ID_MESS_PMV_3         ((U8)0x42)
#define PL_CAN_ID_MESS_PMV_4         ((U8)0x43)
#define PL_CAN_ID_MESS_PMV_5         ((U8)0x44)
#define PL_CAN_ID_MESS_PMV_6         ((U8)0x45)
#define PL_CAN_ID_MESS_PMV_7         ((U8)0x46)
#define PL_CAN_ID_MESS_PMV_8         ((U8)0x47)
#define PL_CAN_ID_MESS_PMV_9         ((U8)0x48)

#define PL_CAN_ID_MESS_PMV_10         ((U8)0x49)
#define PL_CAN_ID_MESS_PMV_11         ((U8)0x4A)
#define PL_CAN_ID_MESS_PMV_12         ((U8)0x4B)
#define PL_CAN_ID_MESS_PMV_13         ((U8)0x4C)
#define PL_CAN_ID_MESS_PMV_14         ((U8)0x4D)
#define PL_CAN_ID_MESS_PMV_15         ((U8)0x4E)
#define PL_CAN_ID_MESS_PMV_16         ((U8)0x4F)


#define PL_CAN_ID_ETAT_CARTE_LED_1   ((U8)0x50)
#define PL_CAN_ID_ETAT_CARTE_LED_2   ((U8)0x51)
#define PL_CAN_ID_ETAT_CARTE_LED_3   ((U8)0x52)
#define PL_CAN_ID_ETAT_CARTE_LED_4   ((U8)0x53)
#define PL_CAN_ID_ETAT_CARTE_LED_5   ((U8)0x54)
#define PL_CAN_ID_ETAT_CARTE_LED_6   ((U8)0x55)
#define PL_CAN_ID_ETAT_CARTE_LED_7   ((U8)0x56)
#define PL_CAN_ID_ETAT_CARTE_LED_8   ((U8)0x57)
#define PL_CAN_ID_ETAT_CARTE_LED_9   ((U8)0x58)
#define PL_CAN_ID_ETAT_CARTE_LED_10  ((U8)0x59)
#define PL_CAN_ID_ETAT_CARTE_LED_11  ((U8)0x5A)
#define PL_CAN_ID_ETAT_CARTE_LED_12  ((U8)0x5B)
#define PL_CAN_ID_ETAT_CARTE_LED_13  ((U8)0x5C)
#define PL_CAN_ID_ETAT_CARTE_LED_14  ((U8)0x5D)
#define PL_CAN_ID_ETAT_CARTE_LED_15  ((U8)0x5E)

#define PL_CAN_ID_RELEVAGE           ((U8)0x5F)
#define PL_CAN_MODE_PLUS_PMV          ((U8)0x67)
/*
#define PL_CAN_ID_IN1               ((U8)0x1E)


#define PL_CAN_ID_MESS_PMV_1         ((U8)0x40)
#define PL_CAN_ID_MESS_PMV_2         ((U8)0x41)
#define PL_CAN_ID_MESS_PMV_3         ((U8)0x42)
#define PL_CAN_ID_MESS_PMV_4         ((U8)0x43)
#define PL_CAN_ID_MESS_PMV_5         ((U8)0x44)
#define PL_CAN_ID_MESS_PMV_6         ((U8)0x45)
#define PL_CAN_ID_MESS_PMV_7         ((U8)0x46)
#define PL_CAN_ID_MESS_PMV_8         ((U8)0x47)
#define PL_CAN_ID_MESS_PMV_9         ((U8)0x48)
#define PL_CAN_ID_MESS_PMV_10        ((U8)0x49)
*/
#define P_L_CAN_SW_NAME_VERSION     ((U8)0x00)
#define P_L_CAN_SW_NAME_CHECKSUM    ((U8)0x01)
#define P_L_CAN_INFORMATION         ((U8)0x06)


#define PL_CAN_MESS_PMV_L1           ((U8)0x00)
#define PL_CAN_MESS_PMV_L2           ((U8)0x01)
#define PL_CAN_MESS_PMV_L3           ((U8)0x02)

#define PL_CAN_MESS_PMV_L4           ((U8)0x03)
#define PL_CAN_MESS_PMV_L5           ((U8)0x04)
#define PL_CAN_MESS_PMV_L6           ((U8)0x05)

#define PL_CAN_ID_PMV_STATE          ((U8)0x15)
#define PL_CAN_ID_NUM_MESS_LANGUE    ((U8)0x16)
//#define PL_CAN_ADR_PML_A             ((U16)0xB1)

#define PL_CAN_PUISSANCE_BROADCAST    ((U8)0x70)

#define PL_CAN_BT_INFORMATION    	  ((U8)0x73)

////////////////////////////////////////////////////////
#define APP_REPONSE_FACADE_MESS                 ((U8)0x01)
#define APP_QUESTION_SI_FACADE_MESS             ((U8)0x01)
#define APP_DEMANDE_LECTURE_MESS                ((U8)0x02)
#define APP_REPONSE_DEMANDE_LECTURE_MESS        ((U8)0x02)
#define APP_DEMANDE_EFFACEMENT_MESS             ((U8)0x03)
#define APP_REPONSE_DEMANDE_EFFACEMENT_MESS     ((U8)0x03)
#define APP_NO_OK        ((U8)0x00)
#define APP_OK           ((U8)0x01)
#define APP_RUNNING      ((U8)0x02)

#define APP_ECRITURE_MESS                        ((U8)0x04)
#define APP_REPONSE_ECRITURE_MESS                ((U8)0x04)
#define PL_CAN_ID_SEUIL_CFG                      ((U8)0x05)
#define PL_CAN_ID_SEUIL_USER                     ((U8)0x06)

#define APP_TEMPS_INTER_MESSAGE                  ((U32)3000)
#define APP_TEMPS_TIMEOUT_MESSAGE                ((U32)2000)
#define APP_FILTRAGE_APPUI 					     ((U32)100)

typedef enum P_L_CAN_REQUEST_BUZZER_TYPE_
{
	P_L_CC_BUZZER_TYPE1 = 0x00,
	P_L_CC_BUZZER_TYPE2,
	P_L_CC_BUZZER_TYPE3,
	P_L_CC_BUZZER_TYPE4,
	P_L_CC_BUZZER_TYPE5,
	P_L_CC_BUZZER_TYPE6,
	P_L_CC_BUZZER_TYPE7,
	P_L_CC_BUZZER_TYPE8,
	P_L_CC_BUZZER_TYPE9,
	P_L_CC_BUZZER_TYPE10,
	P_L_CC_BUZZER_TYPE11,
	P_L_CC_BUZZER_TYPE12,
	P_L_CC_BUZZER_TYPE13,
	P_L_CC_BUZZER_TYPE14,
	P_L_CC_BUZZER_TYPE15,
	P_L_CC_BUZZER_TYPE16
} P_L_CAN_REQUEST_BUZZER_TYPE;

typedef enum
{
P_L_MESS_PMV_PART_MODE_RAMPE_PMV,
P_L_MESS_PMV_PART_1_2,
P_L_MESS_PMV_PART_3,
P_L_MESS_PMV_PART_4_5,
P_L_MESS_PMV_PART_6,
P_L_MESS_PMV_PART_7_8,
P_L_MESS_PMV_PART_9,
P_L_MESS_PMV_PART_10_11,
P_L_MESS_PMV_PART_12_13,
P_L_MESS_PMV_PART_14_15,
P_L_MESS_PMV_PART_16,
P_L_FIN_ENVOIE,
P_L_ATTENTE_ENVOIE
}P_L_ETAPE_ENVOIE_TYPE;

/*
typedef enum
{
APP_MESSAGE_PMV_TYPE_NON_UTILISE = 0,
APP_MESSAGE_PMV_TYPE_FIXE = 1,
APP_MESSAGE_PMV_TYPE_CLIGNOTANT = 2,
APP_MESSAGE_PMV_TYPE_CHAINE_2 = 3,
APP_MESSAGE_PMV_TYPE_CHAINE_3 = 4
}APP_PMV_TYPE;
*/

#define APP_TEMPO_SEND_BTN  ((U32)500)

#define P_L_CAN_MAX_LED     HWI_NB_LED
#define P_L_CAN_LED_TOUCH   HWI_NB_LED_BP
#define P_L_CAN_LED_VOYANT  HWI_NB_LED-HWI_NB_LED_BP
#define P_L_CAN_MAX_LED     HWI_NB_LED
#define P_L_CAN_MAX_BIP     16


#define APP_ADR_NOEUD_CANF12T                ((U32)0x50)
#define APP_CANF12T_NB_VOYANT                ((U8)0x18)

#define APP_ADR_NOEUD_CANFGRAPH              ((U32)0x90)


#define APP_CANFGRAPH_1_NB_VOYANT            ((U8)0x1E)
#define APP_CANFGRAPH_2_NB_VOYANT            ((U8)0x1C)

/*******************************************************************************
*  Définition des différentes macros associées à la couche Network Management
*******************************************************************************/

/* Nombre total d'interlocuteurs sur le réseau	*/
#define P_L_CC_N_MAXSTATIONS 24   // 0x10 + 0x8
/*Nombre de stations supervisées: tous moins ce noeud */
#define P_L_CC_N_SUP_STATIONS 23
/* Valeur maximale des compteurs volatiles */
#define P_L_CC_C_MAX 255

/* longeur des trame de données*/
#define P_L_CAN_LENGHT_DATA 8

/* valeur de decrement pour la gestion reseau*/
#define P_L_CAN_DEC_CNT 1

typedef enum{
ONE=0x01,
TWO=0x02,
THREE=0x03
}P_L_CAN_SEND_PARTI_TYPE;


typedef enum{
	SEND_BT_INFO_MODULE,
	SEND_BT_ID,
	SEND_BT_TOR_PWM_1,
	SEND_BT_TOR_PWM_2,
	SEND_BT_TOR,
	SEND_BT_IN,
	SEND_BT_ANA,
	SEND_BT_BTN
}P_L_CAN_ETAPE_TX_BT_TYPE;


typedef enum{
	SEND_BT_INFO_MODULE_PRJ,
	SEND_BT_PRJ_CMD
}P_L_CAN_ETAPE_TX_PROj_BT_TYPE;


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
#ifdef _PROJECTEUR_BT
extern void P_L_Can_Send_PRJ_CMD(U8 TransmitMailbox);
#endif
extern void P_L_Can_Send_Data_Can();
extern void P_L_Can_Send_Data_Can_IT(U8 mail_box);
extern void P_L_Can_Send_Data_Can_Init();
#endif
