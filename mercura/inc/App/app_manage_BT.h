/*******************************************************************************
 *  Synergie Ing�nierie Industirelle Co.
 *
 *  This document is the property of Synergie Ing�nierie Industrielle
 *  It must not be copied (in whole or in part) or disclosed without
 *  prior written consent of the company. Any copies by any method
 *  must also include a copy of this legend.
 *******************************************************************************/
#ifndef _APP_MANAGE_BT_H_
#define _APP_MANAGE_BT_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
 *  Application Definitions
 *******************************************************************************/
typedef enum
{
	BT_SEND_STATE_ASK_KEY,
	BT_SEND_STATE_WAIT_KEY,
	BT_SEND_STATE_SEND_DATA,
	BT_SEND_STATE_FINISH,
	BT_SEND_STATE_WAIT
} P_L_BT_SEND_STATE;

#define TAILLE_WRD_BT 4
#define WRD_KEY_ASK_BT FCC('k', 'e', 'y', '?')
#define WRD_KEY_BT FCC('k', 'e', 'y', '.')
#define BUFF_KEY_ASK_BT(ptr) FFC_BUFF(ptr, 'k', 'e', 'y', '?')
#define BUFF_KEY_BT(ptr) FFC_BUFF(ptr, 'k', 'e', 'y', '.')
// Commande APP
#define WRD_KEY_CAPP_BT FCC('c', 'a', 'p', 'p')
#define BUFF_CAPP_BT(ptr) FFC_BUFF(ptr, 'c', 'a', 'p', 'p')

#define WRD_DATA_1_BT FCC('d', 'a', '1', '=')
#define BUFF_DATA_1_BT(ptr) FFC_BUFF(ptr, 'd', 'a', '1', '=')
#define BUFF_DATA_1_BT_R(ptr) FFC_BUFF(ptr, 'd', 'a', '1', '.')

#define WRD_BTN_BT FCC('b', 't', 'n', '=')
#define BUFF_BTN_BT(ptr) FFC_BUFF(ptr, 'b', 't', 'n', '=')

#define APP_FILTRAGE_USE_KEY_BT ((U32)3000) //((U32)2000)
#define APP_TIMEOUT_ALL_STEP ((U32)2000)	//((U32)3000)
#define APP_TIMEOUT_1_STEP ((U32)500)		//((U32)3000)

#define APP_TIMEOUT_INTER_SPP_LOOP ((U32)2000) //((U32)2000)

#define HWI_NB_POWER_BT_CMD_RAMPE ((U8)0x04)
#define HWI_NB_POWER_PWM_BT_CMD_RAMPE ((U8)0x04)
// Nb commande button
#define HWI_NB_BUTTON_BT_CMD_APPLI ((U8)0x07)

#define HWI_NB_INPUT_BT_CMD_RAMPE ((U8)0x04)

#define HWI_MES_BAT_SPA_RAMPE ((U8)0x00)
#define HWI_NB_ANA_BT_SPA_RAMPE ((U8)0x03)

#define HWI_NB_INPUT_BT_RAMPE ((U8)0x08)

/****************************************************************/
/*  Valeur de PWM                                               */
/****************************************************************/
#define APP_VALEUR_OFF_PWM ((U8)0x00)
// ben pas de relecture de charge ouverte sur les Hpower en dessous de 0x0B
#define APP_VALEUR_MINI_PWM ((U8)0x01)
#define APP_VALEUR_MINI_PWM_DETECT_PB ((U8)0x0A)
#define APP_VALEUR_MAXI_PWM ((U8)0x32)
#define APP_INSTANTANE ((U8)0x00)
// periode de PWM 100 hz soit 10ms
// rapport cyclique de 20 � 100% par pas de 2% ->200ys pour 1 pas
// on autorise maintenant mais sans detection de charge ouverte ou courcircuit rapport cyclique de 2 � 100% par pas de 2% ->200ys pour 1 pas

typedef struct
{
	U8 cmd;
	U8 consigne_pwm;
	U8 time_pwm;
	BOOL_TYPE etat_pow;
	BOOL_TYPE defaut_cc;
	BOOL_TYPE defaut_co1;
	BOOL_TYPE defaut_co2;
} APP_BT_POWER_TYPE;

// Store status of each bouton
typedef struct
{
	U8 id;
	U8 cmd;
	//U8 trame;
} APP_BT_COMMAND_BOUTON_TYPE;

#define APP_ADR_NOEUD_CAN_BLOCSPA_A ((U32)0x201)

#define APP_TIMEOUT_BT_OUTPUT ((U32)5000)

#define APP_TIMEOUT_BT_RESET ((U32)60000)

#define APP_TIMEOUT_BT_WIFI ((U32)2 * 60000)

/*******************************************************************************
 *  Application Function Prototype
 *******************************************************************************/
extern void APP_Manage_BT_Init(void);
extern void APP_BT_Manage_Send_Data(void);
extern void APP_Gestion_BT_DataRx(U16 data_len, U8 *data);
extern void APP_BT_Can_Data_Power_Bloc(U8 *p_l_can_data_ptr, U8 offset, U8 p_l_can_lenght);
extern void APP_BT_Can_Data_Pwm_Bloc(U8 *p_l_can_data_ptr, U8 offset);
extern void APP_SPA_Can_Data_Power_Bloc(U8 *p_l_can_data_ptr, U8 offset, U8 p_l_can_lenght);
extern void APP_SPA_Can_Data_Analog_Bloc(U8 *p_l_can_data_ptr, U8 offset);
extern void APP_SPA_Can_Data_Input_Bloc(U8 *p_l_can_data_ptr);
extern void APP_Manage_Wifi_BT_Timeout(void);
extern void APP_Manage_Reload_Wifi_BT_Timeout(void);
#endif
