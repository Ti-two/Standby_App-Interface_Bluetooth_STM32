
/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/


/*******************************************************************************
*  Library Include Files
*******************************************************************************/
#include <p_l_rx_can1.h>
#include <p_l_tx_can1.h>
#include "hwi.h"
#include "hwi_hal_can.h"
#include "app.h"
#include "p_l_check_io.h"
#include "app_manage_mapping.h"
#include "lib_macros.h"
#include "app_manage_wifi.h"
#include "p_l_button.h"
#include "p_l_bx310x.h"
#include "hwi_hal_i2s.h"
#include "app_manage_BT.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
// APP_SW_NAME ne doit pas dépasser 4 caractères HEXA
extern const U8 	APP_SW_NAME[APP_SW_NAME_SIZE];
// APP_HW_VERSION TEXT doit absolument être formaté en 8 caractères
extern const U8 	APP_HW_NAME[APP_HW_NAME_SIZE];
// APP_SW_VERSION TEXT doit absolument être formaté en xxyy
extern const U8 	APP_SW_VERSION_CAN_TEXT[APP_SW_VERSION_TEXT_CAN_SIZE];

extern const U8     APP_SW_VERSION_TEXT[APP_SW_VERSION_TEXT_SIZE];


/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32			APP_Boot_sw_version_flash_adr;
extern U32			APP_Boot_hw_name_flash_adr;

extern U32          S_S_1MS_U32_TIME;

extern U32 			APP_Adresse_module;

//validité de l'adresse globale
extern BOOL_TYPE 	APP_Adresse_global_valid;

// valeur du checksum applicatif
extern U16 			P_L_Rpg_checksum_applicatif;
// valeur du checksum config
extern U16 			P_L_Rpg_checksum_config;
// information validité checksum
extern BOOL_TYPE 	P_L_Rpg_calc_checksum_ready;
// request d'envoi de data
extern BOOL_TYPE 	P_L_Can_request_send_info;

//demande d'arret du coeur d'analyse
extern BOOL_TYPE                            APP_Arret_analyse;

extern BOOL_TYPE                            APP_Demande_Arret_SendCan;


extern U8                                   APP_Wifi_pmv_message[APP_NB_MAX_LIGNE_WIFI][APP_TAILLE_MAX_MESS];
extern U8                                   APP_Wifi_pmv_type;
extern U8                                   APP_Manage_pmv_table_ascii;


extern BOOL_TYPE                            APP_Manage_mirroir;
extern BOOL_TYPE                            APP_Manage_mirroir_2;
extern BOOL_TYPE                            APP_Manage_reverse_motor;
extern BOOL_TYPE                            APP_Mode_day_forced;
extern BOOL_TYPE                            APP_Mode_nuit_forced;

extern BOOL_TYPE                            APP_Can_stop_request;

//offset sur la consigne de puissance
extern U8                                   APP_Manage_OffsetPuissance;

extern U8                                   APP_New_mess_rx;
extern BOOL_TYPE                            P_L_New_button_rx;
extern P_L_BUTTON_TYPE                      P_L_Button[2*APP_NB_MAX_POSITION_FACADE];

//flags indiquant si une config bloc ou facade a ete vue
extern BOOL_TYPE                            APP_Config_bloc_vue;

//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE           APP_Mode_gestion_wifi_blt;

//offset de modification de vitesse de message
extern U8                                   APP_offset_Vitesse_message;

extern BOOL_TYPE                            P_L_Rpg_applicatif_mode;

//arret de la gestion de l odometre recue sur le can mercura
extern BOOL_TYPE                            P_L_Can_on_off_odometre_rx;

//valeur odometre envoye sur le can mercura
extern F64                                  P_L_Can_valeur_odometre_tx;
extern F64                                  P_L_Can_interval_odometre_tx;

extern hw_addr                              P_L_Wifi_Module_Mac;
extern BOOL_TYPE                            P_L_Wifi_ssid_mac_lue;

//flag indiquant qu'une carte SD autorisant le flashage a ete vue
extern BOOL_TYPE                            P_L_CarteSd_Flashage_vue;
//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE                            P_L_CarteSd_Atester_vue;
//flag indiquant qu'une carte SD autorisant le mode testeur
extern BOOL_TYPE                            P_L_CarteSd_Testeur_vue;
//flag indiquant qu'une carte SD autorisant la reprogramation de module
extern BOOL_TYPE                            P_L_CarteSd_Programation_vue;

//etat de l'entree boutton
extern P_L_INPUT_STATE_TYPE                 P_L_Bp1;

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
//etat pedale micro PA
extern P_L_BT_AUDIO_STATE_TYPE 				P_L_BT_audio_state;
extern U16 									HWI_InternalBufferAudio[HWI_SAI_IN_NB_CHANNELS];
#endif
#endif

#ifdef _RAMPE_AUTONOME
extern hw_addr                 				P_L_BT_Module_Mac;
extern BOOL_TYPE							P_L_BT_ssid_mac_lue;
extern P_L_BT_STATE_TYPE      				P_L_BT_spp_rampe_state;
extern APP_BT_POWER_TYPE					APP_BT_can_output[HWI_NB_POWER_BT_CMD_RAMPE];

extern U16					   	 			APP_BT_can_ana[HWI_NB_ANA_BT_SPA_RAMPE];
extern U8					    			APP_BT_can_input;

extern BOOL_TYPE							APP_BT_can_Alarm_Bat_sys;
#endif

extern BOOL_TYPE							APP_Wifi_connecter;

extern BOOL_TYPE							APP_BT_WifiBT_input_not_veille;

extern const U8 P_L_MASK_READ[8]/* = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}*/;
extern const U8 P_L_MASK_SET[8]/* = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}*/;
extern const U8 P_L_MASK_RESET[8]/* = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F}*/;

#ifdef _PROJECTEUR_BT
extern U8 APP_BT_projecteur_cmd;
#endif

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
U8                                          P_L_Etape_send_info;

//status envoie de la trame button 1
BOOL_TYPE                                   P_L_Can_tx_button_1;
//status envoie de la trame button 2
BOOL_TYPE                                   P_L_Can_tx_button_2;

//etape d'envoie du message pmv sur le reseau can
P_L_ETAPE_ENVOIE_TYPE                       P_L_Etape_envoie_Mess;

//numero denvoie du message PMV
U8                                          P_L_Numero_envoie;

//timer envoie message PMV
U32                                         P_L_Mess_timer;
BOOL_TYPE                                   P_L_Mess_lock;

//timeout envoie message PMV
U32                                         P_L_MessSend_timer;
BOOL_TYPE                                   P_L_MessSend_lock;

//flag indiquant un nouveau message a envoyer
BOOL_TYPE                                   P_L_Can_new_mess_rx_Tx;

//flag indiquant que la mail box 1 est vide
BOOL_TYPE                                   P_L_Can_mail_box1_vue;
//flag indiquant que la mail box 2 est vide
BOOL_TYPE                                   P_L_Can_mail_box2_vue;

//timeout broadcasr
U32				                            P_L_Timer_send_btn;
BOOL_TYPE			                        P_L_Lock_send_btn;

#ifdef _RAMPE_SOLAIRE
//demande d arret des blocs
BOOL_TYPE			                        P_L_Can_Demande_Arret_Bloc;
#endif


U8											P_L_Etape_send_data_can;

U16											P_L_Puissance_audio_PA;

#ifdef _RAMPE_AUTONOME
//etape d'envoi BT sur le reseau can
P_L_CAN_ETAPE_TX_BT_TYPE                     P_L_Can_etape_BT;
#endif

#ifdef _PROJECTEUR_BT
P_L_CAN_ETAPE_TX_PROj_BT_TYPE			     P_L_Can_etape_PROJ_BT;
#endif

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static void P_L_Can_Send_INFO_SOFT_VERSION(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_INFO_SOFT_CHEKSUM(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_INFO_BOOT(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_INFO_CFG(U32 adresse_module,U8 TransmitMailbox,P_L_CAN_SEND_PARTI_TYPE partie);
static BOOL_TYPE P_L_Can_Send_Data_Can_State(void);

static void P_L_Can_Send_Status(void);
static void P_L_Can_Send_Status_Testeur(void);
static void P_L_Can_Send_Mac(void);
static void P_L_Can_Tx_ModePmv_Rampe(void);
static void P_L_Can_Tx_Msg_Pml1_2(void);
static void P_L_Can_Tx_Msg_Pml3(void);
static void P_L_Can_Tx_Msg_Pml4_5(void);
static void P_L_Can_Tx_Msg_Pml6(void);
static void P_L_Can_Tx_Msg_Pml7_8(void);
static void P_L_Can_Tx_Msg_Pml9(void);
static void P_L_Can_Tx_Msg_Pml10_11(void);
static void P_L_Can_Tx_Msg_Pml12_13(void);
static void P_L_Can_Tx_Msg_Pml14_15(void);
static void P_L_Can_Tx_Msg_Pml16(void);
static void P_L_CAN_Tx_touche(U8 lot);

static BOOL_TYPE P_L_Can_Send_Data_Can_State(void);
static void P_L_Can_Send_Data_Can_Mess(void);
static void P_L_Can_Send_Data_Can_Mess_Do(void);
static void P_L_Can_Send_Data_Can_Mess_IT(void);
static BOOL_TYPE P_L_Can_Send_Data_Can_Btn(void);

#ifdef _GESTION_ODOMETRE
static void P_L_Can_Send_Data_Can_Odometre(void);
#endif

#ifdef _RAMPE_SOLAIRE
static void P_L_Can_Send_Data_Can_Arret(void);
#endif

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
static void P_L_Can_Send_IN(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_ANA(U32 adresse_module,U8 TransmitMailbox);
#endif
#endif

#ifdef _RAMPE_AUTONOME
static void P_L_Can_Send_BT_ID(void);
static void P_L_Can_Send_TOR_PWM(U32 adresse_module,U8 id_trame,U8 nbdata,U8 TransmitMailbox);
static void P_L_Can_Send_TOR_SPECIAL(U32 adresse_module,U8 id_trame,U8 TransmitMailbox);
static void P_L_Can_Send_IN_CMD_OUTPUT(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_ANA_CMD(U32 adresse_module,U8 TransmitMailbox);
static void P_L_Can_Send_IN_CMD(U32 adresse_module,U8 TransmitMailbox);
#endif

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Debug
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
/*
#include "p_l.h"

extern U8			P_L_BT_spp_mac_pair_rampe[P_L_SSP_MAC_PAIR_FLASH_SIZE];
extern U8			APP_Position_module;

extern U16		    APP_BT_can_ana[HWI_NB_ANA_BT_SPA_RAMPE];
extern hw_addr                P_L_BT_Spp_Mac;
U8 CountDebuger = 0x00;
U8 CountDebugerok= 0x00;
U8 StepDebugerok= 0x00;
BOOL_TYPE filpflopdebug = FALSE;

void P_L_Can_Send_Debug(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	if (FALSE ==filpflopdebug)
	{
		can_objet.HWI_CAN_MSG_ID = 0x111;

		//Taille de la trame
		can_objet.HWI_CAN_LENGTH = 0x08;

		data_can_Tx[0] = APP_Config_bloc_vue;

		data_can_Tx[1] = APP_Position_module;
		data_can_Tx[2] = APP_Mode_gestion_wifi_blt;
		data_can_Tx[3] = P_L_BT_spp_rampe_state;
		data_can_Tx[4] = 0x00;
		data_can_Tx[5] = 0x00;
		data_can_Tx[6] = 0x00;
		data_can_Tx[7] = 0x00;


		// envoie de la trame can sur CAN1 MAIL BOX
		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

		can_objet.HWI_CAN_MSG_ID = 0x112;

			data_can_Tx[0] = P_L_BT_spp_mac_pair_rampe[0];
			data_can_Tx[1] = P_L_BT_spp_mac_pair_rampe[1];
			data_can_Tx[2] = P_L_BT_spp_mac_pair_rampe[2];
			data_can_Tx[3] = P_L_BT_spp_mac_pair_rampe[3];
			data_can_Tx[4] = P_L_BT_spp_mac_pair_rampe[4];
			data_can_Tx[5] = P_L_BT_spp_mac_pair_rampe[5];
			data_can_Tx[6] = 0x00;
			data_can_Tx[7] = 0x00;

			// envoie de la trame can sur CAN1 MAIL BOX
			Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);


			can_objet.HWI_CAN_MSG_ID = 0x113;

			data_can_Tx[0] = (U8)((APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE]  >> 0x08) &(U16) 0x00FF);
			data_can_Tx[1] = (U8)(APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE]  &(U16) 0x00FF);
			data_can_Tx[2] = 0x00;
			data_can_Tx[3] = 0x00;
			data_can_Tx[4] = 0x00;
			data_can_Tx[5] = StepDebugerok;
			data_can_Tx[6] = CountDebugerok;
			data_can_Tx[7] = CountDebuger;

			// envoie de la trame can sur CAN1 MAIL BOX
			Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);
	}
	else
	{
		can_objet.HWI_CAN_LENGTH = 0x08;
		can_objet.HWI_CAN_MSG_ID = 0x114;

		data_can_Tx[0] = P_L_BT_Spp_Mac.addr[0];
		data_can_Tx[1] = P_L_BT_Spp_Mac.addr[1];
		data_can_Tx[2] = P_L_BT_Spp_Mac.addr[2];
		data_can_Tx[3] = P_L_BT_Spp_Mac.addr[3];
		data_can_Tx[4] = P_L_BT_Spp_Mac.addr[4];
		data_can_Tx[5] = P_L_BT_Spp_Mac.addr[5];
		data_can_Tx[6] = 0x00;
		data_can_Tx[7] = 0x00;

		// envoie de la trame can sur CAN1 MAIL BOX
		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

		can_objet.HWI_CAN_MSG_ID = 0x115;

		data_can_Tx[0] = P_L_BT_Module_Mac.addr[0];
		data_can_Tx[1] = P_L_BT_Module_Mac.addr[1];
		data_can_Tx[2] = P_L_BT_Module_Mac.addr[2];
		data_can_Tx[3] = P_L_BT_Module_Mac.addr[3];
		data_can_Tx[4] = P_L_BT_Module_Mac.addr[4];
		data_can_Tx[5] = P_L_BT_Module_Mac.addr[5];
		data_can_Tx[6] = 0x00;
		data_can_Tx[7] = 0x00;

		// envoie de la trame can sur CAN1 MAIL BOX
		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);

	}
	filpflopdebug = !filpflopdebug;
}

*/

#ifdef _DEBUG_WIFI_BT
void P_L_Can_Send_Data_Can_Debug(U8 *data_debug,U32 TransmitMailbox )
{

U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	if (FALSE == APP_Demande_Arret_SendCan)
	{
		can_objet.HWI_CAN_LENGTH = 0x08;
		can_objet.HWI_CAN_MSG_ID = 0xFF;

		data_can_Tx[0] = 0xF0;
		data_can_Tx[1] = data_debug[0];
		data_can_Tx[2] = data_debug[1];
		data_can_Tx[3] = data_debug[2];
		data_can_Tx[4] = data_debug[3];
		data_can_Tx[5] = data_debug[4];
		data_can_Tx[6] = data_debug[5];
		data_can_Tx[7] = data_debug[6];

		// envoie de la trame can sur CAN1 MAIL BOX
		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
	}

}
#endif

 /*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : envoie des requette sur le can
*******************************************************************************/
void P_L_Can_Send_Data_Can(void)
{
BOOL_TYPE dataSend = FALSE;
	if(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
	{
		if (P_L_BT_spp_rampe_state == BT_connected)
		{

			APP_Demande_Arret_SendCan = FALSE;
			APP_Can_stop_request = FALSE;
		}
	}

#ifdef _PROGRAMATION
	if (FALSE == P_L_CarteSd_Programation_vue)
#endif
	{
		if (TRUE == P_L_CarteSd_Atester_vue)
		{
			if (TRUE == P_L_Wifi_ssid_mac_lue)
			{
			  P_L_Can_Send_Mac();
			}

			//P_L_Can_Send_Status();
			//mail BOX 3
			dataSend = P_L_Can_Send_Data_Can_State();

			if (FALSE == dataSend)
			{
				P_L_Can_Send_Status();
			}
		}
		else if (TRUE == P_L_CarteSd_Testeur_vue)
		{
			P_L_Can_Send_Status_Testeur();
		}
		else if (((FALSE == APP_Demande_Arret_SendCan) &&  (APP_MODE_ODOMETRE != APP_Mode_gestion_wifi_blt)) ||  (TRUE == P_L_CarteSd_Flashage_vue)|| (FALSE == P_L_Rpg_applicatif_mode))
		{


#ifdef _GESTION_ODOMETRE
			if (APP_MODE_ODOMETRE == APP_Mode_gestion_wifi_blt)
			{
				P_L_Can_Send_Data_Can_Odometre();
				P_L_Can_Send_Status();
			}
			else if ((APP_MODE_BTN_ET_MESSAGE_PMV == APP_Mode_gestion_wifi_blt) || (APP_MODE_MESSAGE_PMV == APP_Mode_gestion_wifi_blt))
#else
			if ((APP_MODE_BTN_ET_MESSAGE_PMV == APP_Mode_gestion_wifi_blt) || (APP_MODE_MESSAGE_PMV == APP_Mode_gestion_wifi_blt))
#endif
			{
				if (FALSE == APP_Arret_analyse)
				{
					//mail BOX 1 et 2
					P_L_Can_Send_Data_Can_Mess();

					if (APP_MODE_BTN_ET_MESSAGE_PMV == APP_Mode_gestion_wifi_blt)
					{
						//mail BOX 3
						dataSend = P_L_Can_Send_Data_Can_Btn();
					}
					else
					{
						//do nothing
					}


				}

				if (FALSE == dataSend)
				{
					//mail BOX 3
					dataSend = P_L_Can_Send_Data_Can_State();
				}

				if (FALSE == dataSend)
				{
					P_L_Can_Send_Status();
				}
			}
#ifdef _RAMPE_AUTONOME
			else if ((MODE_CMD_RAMPE_AUTONOME_BLEU == APP_Mode_gestion_wifi_blt)||(MODE_CMD_RAMPE_AUTONOME_ORANGE == APP_Mode_gestion_wifi_blt)||(MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt))
			{
				dataSend = P_L_Can_Send_Data_Can_Btn();

				if (FALSE == dataSend)
				{
					switch(P_L_Can_etape_BT)
					{
						case SEND_BT_ID:
							P_L_Can_Send_BT_ID();
							P_L_Can_etape_BT = SEND_BT_INFO_MODULE;
						break;


						case SEND_BT_INFO_MODULE:
							dataSend = P_L_Can_Send_Data_Can_State();
							if (FALSE == dataSend)
							{
								P_L_Can_Send_Status();
							}

							if (MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt)
							{
								P_L_Can_etape_BT = SEND_BT_BTN;
							}
							else
							{
								if (P_L_BT_spp_rampe_state == BT_connected)
								{
									P_L_Can_etape_BT = SEND_BT_TOR_PWM_1;
								}
								else
								{
									P_L_Can_etape_BT = SEND_BT_ID;
								}
							}
						break;


						case SEND_BT_TOR_PWM_1:
							P_L_Can_Send_TOR_PWM(APP_Adresse_module,PL_CAN_ID_TOR_PWM1,3,MAIL_BOX3);
							P_L_Can_etape_BT = SEND_BT_TOR_PWM_2;
						break;

						case SEND_BT_TOR_PWM_2:
							P_L_Can_Send_TOR_PWM(APP_Adresse_module,PL_CAN_ID_TOR_PWM2,1,MAIL_BOX3);
							P_L_Can_etape_BT = SEND_BT_TOR;
						break;

						case SEND_BT_TOR:
							P_L_Can_Send_TOR_SPECIAL(APP_Adresse_module,PL_CAN_ID_TOR1,MAIL_BOX3);
							P_L_Can_etape_BT = SEND_BT_IN;
						break;

						case  SEND_BT_IN:
							P_L_Can_Send_IN_CMD(APP_Adresse_module,MAIL_BOX3);
							P_L_Can_etape_BT = SEND_BT_ANA;
						break;

						case  SEND_BT_ANA:
							P_L_Can_Send_ANA_CMD(APP_Adresse_module,MAIL_BOX3);
							P_L_Can_etape_BT = SEND_BT_BTN;
						break;

						case SEND_BT_BTN:
							P_L_New_button_rx = TRUE;
							dataSend = P_L_Can_Send_Data_Can_Btn();
							P_L_Can_etape_BT = SEND_BT_ID;
						break;

						default:
							P_L_Can_etape_BT = SEND_BT_ID;
						break;
					}
				}
			}
			else if (MODE_RAMPE_AUTONOME == APP_Mode_gestion_wifi_blt)
			{
				switch(P_L_Can_etape_BT)
				{
					case SEND_BT_ID:
						P_L_Can_Send_BT_ID();
						P_L_Can_etape_BT = SEND_BT_INFO_MODULE;
					break;

					case SEND_BT_INFO_MODULE:
						dataSend = P_L_Can_Send_Data_Can_State();
						if (FALSE == dataSend)
						{
							P_L_Can_Send_Status();
						}

						//ben le 29/01/2025 pb no send APP_BT_WifiBT_input_not_veille

						/*
						if (P_L_BT_spp_rampe_state == BT_connected)
						{
							P_L_Can_etape_BT = SEND_BT_IN;
						}
						else
						{
							P_L_Can_etape_BT = SEND_BT_ID;
						}
						*/
						P_L_Can_etape_BT = SEND_BT_IN;
					break;

					case SEND_BT_IN:
						P_L_Can_Send_IN_CMD_OUTPUT(APP_Adresse_module,MAIL_BOX3);
						P_L_Can_etape_BT = SEND_BT_ID;
					break;

					default:
						P_L_Can_etape_BT = SEND_BT_ID;
					break;
				}
			}
#ifdef _PROJECTEUR_BT
		else if (MODE_PROJECTEUR_BT == APP_Mode_gestion_wifi_blt)
		{
			switch(P_L_Can_etape_PROJ_BT)
			{
				case SEND_BT_INFO_MODULE_PRJ:
					dataSend = P_L_Can_Send_Data_Can_State();
					if (FALSE == dataSend)
					{
						P_L_Can_Send_Status();
					}


					P_L_Can_etape_PROJ_BT = SEND_BT_PRJ_CMD;
				break;

				case SEND_BT_PRJ_CMD:
					P_L_Can_Send_PRJ_CMD(MAIL_BOX3);
					P_L_Can_etape_PROJ_BT = SEND_BT_INFO_MODULE_PRJ;
				break;

				default:
					P_L_Can_etape_PROJ_BT = SEND_BT_INFO_MODULE_PRJ;
				break;
			}
		}
#endif
		else
		{
			P_L_Can_Send_Status();
		}
#endif
		}
#ifdef _RAMPE_SOLAIRE
		else if (TRUE == P_L_Can_Demande_Arret_Bloc)
		{
			P_L_Can_Demande_Arret_Bloc  = FALSE;
			P_L_Can_Send_Data_Can_Arret();
		}
#endif
		else if (FALSE == APP_Demande_Arret_SendCan)
		{
			P_L_Can_Send_Status();
		}
	}
#ifdef _PROGRAMATION
	else
	{

	}
#endif

}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Arret
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX1
*
*******************************************************************************/
#ifdef _RAMPE_SOLAIRE
static void P_L_Can_Send_Data_Can_Arret(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
	can_objet.HWI_CAN_LENGTH = 0x08;
	can_objet.HWI_ID_FORMAT=HWI_STD;

    //octet 0 = identifiant de la trame
    data_can_Tx[0] = PL_CAN_ID_CMD;
    //octet 2 cmd
    data_can_Tx[1] = PL_ON_OFF_SYS;

    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
}
#endif

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Odometre
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX1
*
*******************************************************************************/
#ifdef _GESTION_ODOMETRE
U64 odo_metre_tx;
static void P_L_Can_Send_Data_Can_Odometre(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    if (TRUE == P_L_Can_on_off_odometre_rx)
    {
    	can_objet.HWI_CAN_MSG_ID=P_L_CC_ID_DATA_AUTRE;
    	can_objet.HWI_CAN_LENGTH = 0x08;
    	can_objet.HWI_ID_FORMAT=HWI_STD;
        // gestion des noeud du reseau can


        //octet 0 = identifiant de la trame
        data_can_Tx[0] = 0x00;
        data_can_Tx[1] = 0x00;

        // valeur odometre
        odo_metre_tx = (U64)P_L_Can_valeur_odometre_tx;
        data_can_Tx[4] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
        data_can_Tx[3] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);
        data_can_Tx[2] = (U8)((odo_metre_tx >> 16) &(U32) 0x000000FF);

        // interval odometre
        odo_metre_tx = (U64)P_L_Can_interval_odometre_tx;
        data_can_Tx[7] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
        data_can_Tx[6] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);
        data_can_Tx[5] = (U8)((odo_metre_tx >> 16) &(U32) 0x000000FF);

        // envoie de la trame can sur CAN1 MAIL BOX 1
        Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
    }
}
#endif


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Btn
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX3
*
*******************************************************************************/
static BOOL_TYPE P_L_Can_Send_Data_Can_Btn(void)
{
const U32   *ptr_app_liste_facade;
BOOL_TYPE dataSend;
BOOL_TYPE result;

    dataSend = FALSE;

    if (TRUE == P_L_New_button_rx)
    {
        P_L_New_button_rx = FALSE;
        P_L_Can_tx_button_1 = TRUE;

        ptr_app_liste_facade = (const U32 *) APP_LISTE_FACADE_CFG_FACADE;

        if ((*ptr_app_liste_facade == (const U32 )APP_ADR_NOEUD_CANTPANEL)||(*ptr_app_liste_facade == (const U32 )APP_ADR_NOEUD_CANFGRAPH))
        {
            P_L_Can_tx_button_2 = TRUE;
        }
    }
    else
    {
        BT_TIMER_LOCK(P_L_Timer_send_btn,S_S_1MS_U32_TIME,APP_TEMPO_SEND_BTN,P_L_Lock_send_btn,result);
        if(TRUE == result)
        {
            P_L_Can_tx_button_1 = TRUE;

            ptr_app_liste_facade = (const U32 *) APP_LISTE_FACADE_CFG_FACADE;

            if ((*ptr_app_liste_facade == (const U32 )APP_ADR_NOEUD_CANTPANEL)||(*ptr_app_liste_facade == (const U32 )APP_ADR_NOEUD_CANFGRAPH))
            {
                P_L_Can_tx_button_2 = TRUE;
            }

            BT_TIMER_INIT_LOCK(P_L_Timer_send_btn,S_S_1MS_U32_TIME,P_L_Lock_send_btn);
        }
    }

    if (TRUE == P_L_Can_tx_button_1)
    {
        P_L_Can_tx_button_1 = FALSE;
        P_L_CAN_Tx_touche(0x00);
        dataSend = TRUE;
    }
    else if (TRUE == P_L_Can_tx_button_2)
    {
        P_L_Can_tx_button_2 = FALSE;
        P_L_CAN_Tx_touche(0x01);
        dataSend = TRUE;
    }
    return dataSend;
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Mess
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*******************************************************************************/
static void P_L_Can_Send_Data_Can_Mess(void)
{
BOOL_TYPE result;

    if (FALSE == P_L_Can_new_mess_rx_Tx)
    {
        if (0x00 != APP_New_mess_rx)
        {
            APP_New_mess_rx--;
            P_L_Numero_envoie = P_L_Numero_envoie + 1;
            P_L_Can_Send_Data_Can_Mess_Do();
        }
        else
        {

            BT_TIMER_LOCK(P_L_Mess_timer,S_S_1MS_U32_TIME,APP_TEMPS_INTER_MESSAGE,P_L_Mess_lock,result);
            if (TRUE == result)
            {//oui
                P_L_Can_Send_Data_Can_Mess_Do();
            }

        }
    }
    else
    {
        BT_TIMER_LOCK(P_L_MessSend_timer,S_S_1MS_U32_TIME,APP_TEMPS_TIMEOUT_MESSAGE,P_L_MessSend_lock,result);
        if (TRUE == result)
        {//oui
           P_L_Can_new_mess_rx_Tx = FALSE;
           P_L_Etape_envoie_Mess = P_L_ATTENTE_ENVOIE;
        }
    }
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Mess
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*******************************************************************************/
static void P_L_Can_Send_Data_Can_Mess_Do(void)
{
    P_L_Can_mail_box1_vue = FALSE;
    P_L_Can_mail_box2_vue = FALSE;
    P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_MODE_RAMPE_PMV;
    P_L_Can_new_mess_rx_Tx = TRUE;
    BT_TIMER_INIT_LOCK(P_L_MessSend_timer,S_S_1MS_U32_TIME,P_L_MessSend_lock);
    if (APP_Demande_Arret_SendCan == FALSE)
    {
    	P_L_Can_Send_Data_Can_Mess_IT();
    }
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_IT
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX1 et MAIL_BOX2 et MAIL_BOX2
*
*******************************************************************************/
void P_L_Can_Send_Data_Can_IT(U8 mail_box)
{
    if (0x00 == mail_box)
    {
        P_L_Can_mail_box1_vue = TRUE;
    }
    else if (0x01 == mail_box)
    {
        P_L_Can_mail_box2_vue = TRUE;
    }
    else
    {
        //do nothing
    }

    if ((TRUE == P_L_Can_mail_box1_vue)&&(TRUE == P_L_Can_mail_box2_vue)&&(TRUE == P_L_Can_new_mess_rx_Tx))
    {
        P_L_Can_mail_box1_vue = FALSE;
        P_L_Can_mail_box2_vue = FALSE;
        if (APP_Demande_Arret_SendCan == FALSE)
        {
        	P_L_Can_Send_Data_Can_Mess_IT();
        }
    }
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Mess_IT
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX1 et MAIL_BOX2
*
*******************************************************************************/
static void P_L_Can_Send_Data_Can_Mess_IT(void)
{

    switch(P_L_Etape_envoie_Mess)
    {
        case P_L_ATTENTE_ENVOIE:
            //do nothing
        break;

        case P_L_MESS_PMV_PART_MODE_RAMPE_PMV:
            P_L_Can_Tx_ModePmv_Rampe();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_1_2;
        break;

        case P_L_MESS_PMV_PART_1_2:
            P_L_Can_Tx_Msg_Pml1_2();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_3;
        break;


        case P_L_MESS_PMV_PART_3:
            P_L_Can_Tx_Msg_Pml3();
            if ((APP_MESSAGE_PMV_TYPE_CHAINE_2 == APP_Wifi_pmv_type) || (APP_MESSAGE_PMV_TYPE_CHAINE_3 == APP_Wifi_pmv_type) || (APP_MESSAGE_PMV_TYPE_DEFILEMENT == APP_Wifi_pmv_type) || (APP_MESSAGE_PMV_TYPE_DEFILEMENT_MAX == APP_Wifi_pmv_type)|| (APP_MESSAGE_PMV_TYPE_6 == APP_Wifi_pmv_type))
            {
                P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_4_5;
            }
            else
            {
                P_L_Etape_envoie_Mess = P_L_FIN_ENVOIE;
            }
        break;

        case P_L_MESS_PMV_PART_4_5:
            P_L_Can_Tx_Msg_Pml4_5();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_6;
        break;

        case P_L_MESS_PMV_PART_6:
            P_L_Can_Tx_Msg_Pml6();

            if ((APP_MESSAGE_PMV_TYPE_CHAINE_3 == APP_Wifi_pmv_type)|| (APP_MESSAGE_PMV_TYPE_DEFILEMENT == APP_Wifi_pmv_type)|| (APP_MESSAGE_PMV_TYPE_DEFILEMENT_MAX == APP_Wifi_pmv_type)|| (APP_MESSAGE_PMV_TYPE_6 == APP_Wifi_pmv_type))
            {
                P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_7_8;
            }
            else
            {
                P_L_Etape_envoie_Mess = P_L_FIN_ENVOIE;
            }
        break;

        case P_L_MESS_PMV_PART_7_8:
            P_L_Can_Tx_Msg_Pml7_8();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_9;
        break;

        case P_L_MESS_PMV_PART_9:
            P_L_Can_Tx_Msg_Pml9();
            if ((APP_MESSAGE_PMV_TYPE_DEFILEMENT_MAX == APP_Wifi_pmv_type) || (APP_MESSAGE_PMV_TYPE_6 == APP_Wifi_pmv_type))
            {
                P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_10_11;
            }
            else
            {
                P_L_Etape_envoie_Mess = P_L_FIN_ENVOIE;
            }
        break;

        case P_L_MESS_PMV_PART_10_11:
            P_L_Can_Tx_Msg_Pml10_11();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_12_13;
        break;

        case P_L_MESS_PMV_PART_12_13:
            P_L_Can_Tx_Msg_Pml12_13();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_14_15;
        break;

        case P_L_MESS_PMV_PART_14_15:
            P_L_Can_Tx_Msg_Pml14_15();
            P_L_Etape_envoie_Mess = P_L_MESS_PMV_PART_16;
        break;

        case P_L_MESS_PMV_PART_16:
            P_L_Can_Tx_Msg_Pml16();
            P_L_Etape_envoie_Mess = P_L_FIN_ENVOIE;
        break;

        case P_L_FIN_ENVOIE:
            BT_TIMER_INIT_LOCK(P_L_Mess_timer,S_S_1MS_U32_TIME,P_L_Mess_lock);
            P_L_Can_new_mess_rx_Tx = FALSE;
            P_L_Etape_envoie_Mess = P_L_ATTENTE_ENVOIE;
        break;

        default:
            BT_TIMER_INIT_LOCK(P_L_Mess_timer,S_S_1MS_U32_TIME,P_L_Mess_lock);
            P_L_Etape_envoie_Mess = P_L_ATTENTE_ENVOIE;
        break;
    }
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Status
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : envoie du status de la carte sur le reseau can
*******************************************************************************/
static void P_L_Can_Send_Status(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	can_objet.HWI_CAN_MSG_ID=(U16)(APP_Adresse_module + P_L_CC_MASK_WR);
	can_objet.HWI_CAN_LENGTH = 0x04;
	can_objet.HWI_ID_FORMAT=HWI_STD;
    // gestion des noeud du reseau can

    data_can_Tx[0] = 0x07;
    data_can_Tx[1] = 0x00;//HWI_Info_reveil;
    data_can_Tx[2] = 0x00;
    data_can_Tx[3] = 0xFF;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);

}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_BT_ID
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
#ifdef _RAMPE_AUTONOME
static void P_L_Can_Send_BT_ID(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	if (P_L_BT_ssid_mac_lue == TRUE)
	{
		can_objet.HWI_CAN_MSG_ID=(U16)(APP_Adresse_module + P_L_CC_MASK_WR);
		can_objet.HWI_CAN_LENGTH = 0x08;
		can_objet.HWI_ID_FORMAT=HWI_STD;
		// gestion des noeud du reseau can

		data_can_Tx[0] = PL_CAN_BT_INFORMATION;
		data_can_Tx[1] = APP_Mode_gestion_wifi_blt;
		data_can_Tx[2] = P_L_BT_Module_Mac.addr[0];
		data_can_Tx[3] = P_L_BT_Module_Mac.addr[1];
		data_can_Tx[4] = P_L_BT_Module_Mac.addr[2];
		data_can_Tx[5] = P_L_BT_Module_Mac.addr[3];
		data_can_Tx[6] = P_L_BT_Module_Mac.addr[4];
		data_can_Tx[7] = P_L_BT_Module_Mac.addr[5];

		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);
	}

}



/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_TOR_PWM
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_TOR_PWM(U32 adresse_module,U8 id_trame,U8 nbdata,U8 TransmitMailbox)
{
U8 boucle;
U8 position;
U8 offset;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;


    if (id_trame > PL_CAN_ID_TOR_PWM6)
    {
        offset = ((PL_CAN_ID_TOR_PWM6 - PL_CAN_ID_TOR_PWM1)*3) + (((id_trame - PL_CAN_ID_TOR_PWM7)+1)*3);
    }
    else
    {
        offset = ((id_trame - PL_CAN_ID_TOR_PWM1)*3);
    }

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_CC_SLAVE);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x01 + (2*nbdata);

    //Envoie des commandes TOR PWM
    //octet 0 = identifiant de la trame
    data_can_Tx[0] = id_trame;

    // on passe en revue les 3 premiers powers
    for(boucle=(U8)0x00;boucle<nbdata;boucle++)
    {   // position de la sortie
        position = boucle+offset;

        // on recopie la congine Pwm dans la trame can
        data_can_Tx[(2*boucle)+1] = APP_BT_can_output[position].consigne_pwm ;
        // on recopie la vitesse de transition dans la trame can
        data_can_Tx[(2*boucle)+2] = APP_BT_can_output[position].time_pwm;
    }
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_TOR
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_TOR_SPECIAL(U32 adresse_module,U8 id_trame,U8 TransmitMailbox)
{
U8 boucle;
U8 position;
U8 data_power;
U8 offset;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    offset = ((id_trame - PL_CAN_ID_TOR1)*7);

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_CC_SLAVE);


    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x01 + HWI_NB_POWER_BT_CMD_RAMPE + 1;

    //Envoie des commandes TOR
    //octet 0 = identifiant de la trame
    data_can_Tx[0] = id_trame;

    // on passe en revue les 7 premiers powers
    for(boucle=(U8)0x00;boucle<HWI_NB_POWER_BT_CMD_RAMPE;boucle++)
    {   // position de la sortie
        position = boucle+offset;

        // test l etat de la commande
        // on ne mets pas APP_BT_can_output[output].etat_pow; car surchagerai reseau can si module wifi pas vu
        // on mets a la place la commande que l on a recu APP_BT_can_output[position].cmd
        data_power =  APP_BT_can_output[position].cmd;

        if (TRUE == APP_BT_can_output[position].defaut_cc)
        {
            data_power = data_power | PL_CAN_DEFAUT_CC;
        }
        else if (TRUE == APP_BT_can_output[position].defaut_co1)
        {
            data_power = data_power | PL_CAN_DEFAUT_CO1;
        }
        else if (TRUE == APP_BT_can_output[position].defaut_co2)
        {
            data_power = data_power | PL_CAN_DEFAUT_CO2;
        }
        else
        {
            // do nothing
        }

        /* on ne gere pas cela en version com wifi
        if ((U8)NBPBCC != APP_Power[position].Nb_short_circuit_pow)
        {
            data_power = data_power | PL_CAN_CC_VUE;
        }
        else if (((U16)32*(U16)NBPBCHOUV1) != APP_Power[position].Nb_open_load_1_pow)
        {
            data_power = data_power | PL_CAN_CO1_VUE;
        }
        else if (((U16)32*(U16)NBPBCHOUV2) != APP_Power[position].Nb_open_load_2_pow)
        {
            data_power = data_power | PL_CAN_CO2_VUE;
        }
        else
        {
            // do nothing
        }
*/
        data_can_Tx[boucle+1] = data_power;
    }

    data_can_Tx[HWI_NB_POWER_BT_CMD_RAMPE+1] = APP_BT_can_Alarm_Bat_sys;

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
}


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_TOR
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/

static void P_L_Can_Send_IN_CMD_OUTPUT(U32 adresse_module,U8 TransmitMailbox)
{
U8 boucle;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;
U8 ptr_data;

	can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_CC_SLAVE);

	//Taille de la trame
	can_objet.HWI_CAN_LENGTH = 0x02;

	//Envoie des commandes TOR
	//octet 0 = identifiant de la trame
	data_can_Tx[0] = PL_CAN_ID_IN1;


	data_can_Tx[1] = 0;
	//bit 0  pédale micro cas module BT audio
	//bit 1 entrée 1+
	//bit 2 entrée 1-
	//bit 3 entrée 2+
	//bit 4 entrée 2-
	//bit 5 entrée 3+
	//bit 6 entrée 3-
	//bit 7 entrée 4+ keepOn


	if (P_L_BT_spp_rampe_state == BT_connected)
	{

		for(boucle=0x00;boucle<(HWI_NB_POWER_BT_CMD_RAMPE-1);boucle++)
		{

			if (0x01 == APP_BT_can_output[boucle].cmd)
			{
				data_can_Tx[1] = data_can_Tx[1] | P_L_MASK_SET[(2*boucle)+1];
			}
		}

#ifdef _WIFI_BT_ARRET_SPA
		if (0x01 == APP_BT_WifiBT_input_not_veille)
		{
			data_can_Tx[1] = data_can_Tx[1] | P_L_MASK_SET[7];
		}
#endif

	}
	else
	{

	}




    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_TOR
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_IN_CMD(U32 adresse_module,U8 TransmitMailbox)
{
U8 boucle;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;
U8 ptr_data;

	can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_CC_SLAVE);

	//Taille de la trame
	//can_objet.HWI_CAN_LENGTH = 0x02;
	can_objet.HWI_CAN_LENGTH = 0x03;

	//Envoie des commandes TOR
	//octet 0 = identifiant de la trame
	data_can_Tx[0] = PL_CAN_ID_IN1;


	data_can_Tx[1] = ((APP_BT_can_input << 1) & 0xFE);

	data_can_Tx[2] = APP_Wifi_connecter;

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
}
/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_ANA_CMD
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_ANA_CMD(U32 adresse_module,U8 TransmitMailbox)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;


		can_objet.HWI_CAN_MSG_ID= (adresse_module | P_L_ID_SLAVE_TO_MASTER);
		can_objet.HWI_CAN_LENGTH = 0x07;
		can_objet.HWI_ID_FORMAT=HWI_STD;
		// gestion des noeud du reseau can

		data_can_Tx[0] = PL_CAN_ID_ANA1;
		// I2S
		//data_can_Tx[1] = (U8)((HWI_InternalBufferAudio[0] >> 0x08) &(U16) 0x00FF);
		//data_can_Tx[2] = (U8)(HWI_InternalBufferAudio[0] &(U16) 0x00FF);
		data_can_Tx[2] = 0x00;
	    data_can_Tx[1] = 0x00;
		//puissance PA
		data_can_Tx[3] = (U8)((P_L_Puissance_audio_PA >> 0x08) &(U16) 0x00FF);
		data_can_Tx[4] = (U8)(P_L_Puissance_audio_PA &(U16) 0x00FF);
		//puissance PA
		data_can_Tx[5] = (U8)((APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE]  >> 0x08) &(U16) 0x00FF);
		data_can_Tx[6] = (U8)(APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE]  &(U16) 0x00FF);



		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);

}
#endif

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_PRJ_CMD
*
*******************************************************************************/
#ifdef _PROJECTEUR_BT
void P_L_Can_Send_PRJ_CMD(U8 TransmitMailbox)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	can_objet.HWI_CAN_MSG_ID=0x00;
	can_objet.HWI_CAN_LENGTH = 0x01;
	can_objet.HWI_ID_FORMAT=HWI_STD;
	// gestion des noeud du reseau can

	data_can_Tx[0] = APP_BT_projecteur_cmd;

	Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,TransmitMailbox);
}
#endif


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Status_Testeur
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : envoie du status de la carte sur le reseau can
*******************************************************************************/
static void P_L_Can_Send_Status_Testeur(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	can_objet.HWI_CAN_MSG_ID=0x07;
	can_objet.HWI_CAN_LENGTH = 0x04;
	can_objet.HWI_ID_FORMAT=HWI_STD;
    // gestion des noeud du reseau can

    data_can_Tx[0] = 0x07;
    data_can_Tx[1] = 0x00;//HWI_Info_reveil;
    data_can_Tx[2] = 0x00;
    data_can_Tx[3] = 0xFF;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);

}


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Mac
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : envoie du status de la carte sur le reseau can
*******************************************************************************/
static void P_L_Can_Send_Mac(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	can_objet.HWI_CAN_MSG_ID=(U16)0x06;
	can_objet.HWI_CAN_LENGTH = 0x07;
	can_objet.HWI_ID_FORMAT=HWI_STD;

    data_can_Tx[0] = P_L_Wifi_Module_Mac.addr[0];
    data_can_Tx[1] = P_L_Wifi_Module_Mac.addr[1];
    data_can_Tx[2] = P_L_Wifi_Module_Mac.addr[2];
    data_can_Tx[3] = P_L_Wifi_Module_Mac.addr[3];
    data_can_Tx[4] = P_L_Wifi_Module_Mac.addr[4];
    data_can_Tx[5] = P_L_Wifi_Module_Mac.addr[5];
    data_can_Tx[6] = P_L_Bp1.State;
    data_can_Tx[7] = 0x00;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

}

/*******************************************************************************
*
*  Function Name   : APP_Can_Data_ModePmv
*  Description     :
*
*
*******************************************************************************/
static void P_L_Can_Tx_ModePmv_Rampe(void)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
U8 p_l_can_touch_buffer;
HWI_CAN_OBJECT_TYPE  can_objet;


    data_can_Tx[0] = PL_CAN_MODE_PLUS_PMV;
    data_can_Tx[1] = APP_Manage_OffsetPuissance;

    p_l_can_touch_buffer = 0;

    if (TRUE == APP_Manage_mirroir)
    {
        p_l_can_touch_buffer = 1;
    }

    if (TRUE == APP_Manage_mirroir_2)
    {
        p_l_can_touch_buffer = (p_l_can_touch_buffer + (0x01 << 1));
    }

    if (TRUE == APP_Manage_reverse_motor)
    {
         p_l_can_touch_buffer = (p_l_can_touch_buffer + (0x01 << 2));
    }

    data_can_Tx[2] = p_l_can_touch_buffer;


    p_l_can_touch_buffer = 0;

    if (TRUE == APP_Mode_day_forced)
    {
        p_l_can_touch_buffer = 1;
    }


    if (TRUE == APP_Mode_nuit_forced)
    {
        p_l_can_touch_buffer = (p_l_can_touch_buffer + (0x01 << 1));
    }

    data_can_Tx[3] = p_l_can_touch_buffer;

    data_can_Tx[4] = APP_Manage_pmv_table_ascii;

    data_can_Tx[5] = APP_Wifi_pmv_type;
    data_can_Tx[6] =  (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L6][15];

    data_can_Tx[7] = APP_offset_Vitesse_message;

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    P_L_Can_mail_box2_vue = TRUE;
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml1_2                     MAIL_BOX1   MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml1_2(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_1;
    data_can_Tx[1] = P_L_Numero_envoie;

    data_can_Tx[2] = (APP_Wifi_pmv_type & 0x0F) + ((APP_Manage_pmv_table_ascii<<4)&0xF0);

    for(octet=0;octet<5;octet++)
    {
        data_can_Tx[3+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L1][octet];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);


    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_2;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L1][octet+5];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml3                   MAIL_BOX1
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml3(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_3;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        if ( 5 > octet)
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L1][octet+5+6];
        }
        else
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L2][octet-5];
        }
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    P_L_Can_mail_box2_vue = TRUE;
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

}
/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml4_5             MAIL_BOX1       MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml4_5(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_4;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L2][octet+1];
    }


    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_5;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L2][octet+1+6];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml6                   MAIL_BOX1
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml6(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_6;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        if ( octet < 3)
        {
             data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L2][octet+1+6+6];
        }
        else
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L3][octet-3];
        }
    }


    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    P_L_Can_mail_box2_vue = TRUE;
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml7_8                 MAIL_BOX1   MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml7_8(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_7;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L3][octet+3];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_8;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L3][octet+3+6];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}
/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml9                       MAIL_BOX1
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml9(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

        data_can_Tx[0] = PL_CAN_ID_MESS_PMV_9;
        data_can_Tx[1] = P_L_Numero_envoie;

        data_can_Tx[2] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L3][3+6+6];

        for(octet=1;octet<6;octet++)
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L4][octet-1];
        }

        can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
        can_objet.HWI_CAN_LENGTH = 0x08;
        can_objet.HWI_ID_FORMAT=HWI_STD;

        P_L_Can_mail_box2_vue = TRUE;
        Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml10_11                 MAIL_BOX1   MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml10_11(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_10;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L4][octet+5];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_11;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<5;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L4][octet+5+6];
    }

    data_can_Tx[2+5] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L5][0x00];

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml12_13                 MAIL_BOX1   MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml12_13(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_12;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L5][octet+1];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_13;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L5][octet+1+6];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml14_15                 MAIL_BOX1   MAIL_BOX2
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml14_15(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_14;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        if (octet < 3)
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L5][octet+1+6+6];
        }
        else
        {
            data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L6][octet-3];
        }
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_15;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L6][octet+3];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX2);
}


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Tx_Msg_Pml16              MAIL_BOX1
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Tx_Msg_Pml16(void)
{
U8 octet;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    data_can_Tx[0] = PL_CAN_ID_MESS_PMV_16;
    data_can_Tx[1] = P_L_Numero_envoie;

    for(octet=0;octet<6;octet++)
    {
        data_can_Tx[2+octet] = (U8)APP_Wifi_pmv_message[PL_CAN_MESS_PMV_L6][octet+3+6];
    }

    can_objet.HWI_CAN_MSG_ID=APP_Adresse_module;
    can_objet.HWI_CAN_LENGTH = 0x08;
    can_objet.HWI_ID_FORMAT=HWI_STD;

    P_L_Can_mail_box2_vue = TRUE;
    Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX1);
}

/*******************************************************************************
 *
 *  Function Name   :  P_L_CAN_Tx_touche
 *  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
 *  Description     : Envoie de l etat des touches
 *******************************************************************************/
static void P_L_CAN_Tx_touche(U8 lot)
{
const U32   *ptr_app_liste_facade;
HWI_CAN_OBJECT_TYPE  can_objet;
S8 cmp;
U8 p_l_can_touch_buffer;
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
U8 offset;

    ptr_app_liste_facade = (const U32 *)APP_LISTE_FACADE_CFG_FACADE;

    if (lot == 0x00)
    {
        offset = 0;
    }
    else
    {
        offset = APP_NB_MAX_POSITION_FACADE;
        ptr_app_liste_facade++;
    }

    if (*ptr_app_liste_facade < 0x3FF)
    {
        data_can_Tx[0] = PL_CAN_ID_IN1;
        p_l_can_touch_buffer = 0;

        for(cmp=7;cmp>=0;cmp--)
        {
            if (FALSE == P_L_Button[offset+cmp].State)
            {
                p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
            }
            else
            {
                p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
            }
        }
        data_can_Tx[1] = p_l_can_touch_buffer;

        p_l_can_touch_buffer = 0;
        for(cmp=15;cmp>7;cmp--)
        {
#ifdef _WIFI_BT_ARRET_SPA

			if (cmp == 11)
			{
				if (FALSE == APP_BT_WifiBT_input_not_veille)
				{
					p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
				}
				else
				{
					p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
				}
			}
			else
			{
				if (FALSE == P_L_Button[offset+cmp].State)
				{
					p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
				}
				else
				{
					p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
				}
			}


#else
            if (FALSE == P_L_Button[offset+cmp].State)
            {
                p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
            }
            else
            {
                p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
            }
#endif
        }
        data_can_Tx[2] = p_l_can_touch_buffer;


        p_l_can_touch_buffer = 0;
        for(cmp=23;cmp>15;cmp--)
        {
            if (FALSE == P_L_Button[offset+cmp].State)
            {
                p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
            }
            else
            {
                p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
            }
        }
        data_can_Tx[3] = p_l_can_touch_buffer;

        p_l_can_touch_buffer = 0;
        for(cmp=31;cmp>23;cmp--)
        {
            if (FALSE == P_L_Button[offset+cmp].State)
            {
                p_l_can_touch_buffer = (p_l_can_touch_buffer << 1);
            }
            else
            {
                p_l_can_touch_buffer = ((p_l_can_touch_buffer << 1) + 1);
            }
        }

        data_can_Tx[4] = p_l_can_touch_buffer;

        data_can_Tx[5] = 0;
        data_can_Tx[6] = 0;
        data_can_Tx[7] = 0;

        can_objet.HWI_CAN_MSG_ID=(U16)(*ptr_app_liste_facade + P_L_CC_MASK_WR);
        can_objet.HWI_CAN_LENGTH = 0x05;
        can_objet.HWI_ID_FORMAT=HWI_STD;

        Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);
    }
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_State
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : MAIL_BOX3
*
*******************************************************************************/
static BOOL_TYPE P_L_Can_Send_Data_Can_State(void)
{
BOOL_TYPE data_send;

    data_send = FALSE;

    if ((TRUE == P_L_Can_request_send_info)&&(TRUE == P_L_Rpg_calc_checksum_ready))
    {
        switch(P_L_Etape_send_info)
        {
             case 0:
            	P_L_Can_Send_INFO_SOFT_VERSION(APP_Adresse_module,MAIL_BOX3);
                data_send = TRUE;
                P_L_Etape_send_info++;
             break;

             case 1:
            	 P_L_Can_Send_INFO_SOFT_CHEKSUM(APP_Adresse_module,MAIL_BOX3);
                data_send = TRUE;
                P_L_Etape_send_info++;
             break;

            case 2:
            	P_L_Can_Send_INFO_BOOT(APP_Adresse_module,MAIL_BOX3);
            	data_send = TRUE;
                P_L_Etape_send_info++;
             break;

            case 3:
                P_L_Can_Send_INFO_CFG(APP_Adresse_module,MAIL_BOX3,ONE);
                data_send = TRUE;
                P_L_Etape_send_info++;
            break;

            case 4:
                P_L_Can_Send_INFO_CFG(APP_Adresse_module,MAIL_BOX3,TWO);
                data_send = TRUE;
                P_L_Etape_send_info++;
            break;

            case 5:
                P_L_Can_Send_INFO_CFG(APP_Adresse_module,MAIL_BOX3,THREE);
                data_send = TRUE;
                P_L_Etape_send_info++;
            break;


            case 6:
                P_L_Can_request_send_info = FALSE;
                P_L_Etape_send_info = 0;
             break;

             default:
                P_L_Can_request_send_info = FALSE;
                P_L_Etape_send_info = 0;
             break;
        }
    }
    else
    {
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
		switch(P_L_Etape_send_data_can)
		{
			case 0:
				P_L_Can_Send_IN(APP_Adresse_module,MAIL_BOX3);
				data_send = TRUE;
				P_L_Etape_send_data_can++;
			break;

			case 1:
				P_L_Can_Send_ANA(APP_Adresse_module,MAIL_BOX3);
				data_send = TRUE;
			    P_L_Etape_send_data_can++;
			break;

			case 2:
				P_L_Etape_send_data_can = 0;
			break;

			default:
				P_L_Etape_send_data_can = 0;
			break;
		}
#endif
#endif
    }
    return data_send;
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_IN
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
static void P_L_Can_Send_IN(U32 adresse_module,U8 TransmitMailbox)
{
U8 boucle;
U8 can_buffer_send[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_ID_SLAVE_TO_MASTER);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x02;

    can_objet.HWI_ID_FORMAT = HWI_STD;

    // id envoie info software
    can_buffer_send[0] = PL_CAN_ID_IN1;

    if (audio_stream_started == P_L_BT_audio_state)
    {
    	can_buffer_send[1] = 0x01;
    }
    else
    {
    	can_buffer_send[1] = 0x00;
    }

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, can_buffer_send,TransmitMailbox);
}
#endif
#endif

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Status
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : envoie du status de la carte sur le reseau can
*******************************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
static void P_L_Can_Send_ANA(U32 adresse_module,U8 TransmitMailbox)
{
U8 data_can_Tx[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

	if (audio_stream_started == P_L_BT_audio_state)
	{
		can_objet.HWI_CAN_MSG_ID= (adresse_module | P_L_ID_SLAVE_TO_MASTER);
		can_objet.HWI_CAN_LENGTH = 0x05;
		can_objet.HWI_ID_FORMAT=HWI_STD;
		// gestion des noeud du reseau can

		data_can_Tx[0] = PL_CAN_ID_ANA1;
		// I2S
		data_can_Tx[2] = (U8)(HWI_InternalBufferAudio[0] &(U16) 0x00FF);
		data_can_Tx[1] = (U8)((HWI_InternalBufferAudio[0] >> 0x08) &(U16) 0x00FF);
		//puissance PA
		data_can_Tx[4] = (U8)(P_L_Puissance_audio_PA &(U16) 0x00FF);
		data_can_Tx[3] = (U8)((P_L_Puissance_audio_PA >> 0x08) &(U16) 0x00FF);



		Hwi_Can_Send_Message(CAN1, &can_objet, data_can_Tx,MAIL_BOX3);
	}
}
#endif
#endif

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_INFO_SOFT_VERSION
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_INFO_SOFT_VERSION(U32 adresse_module,U8 TransmitMailbox)
{
U8 boucle;
U8 can_buffer_send[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_ID_SLAVE_TO_MASTER);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x08;

    can_objet.HWI_ID_FORMAT = HWI_STD;

    // id envoie info software
    can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

    // envoie version de soft
    can_buffer_send[1] = PL_CAN_VERSION_SOFT;

    //nom du software
    for(boucle=(U8)0x00;boucle<0x04;boucle++)
    {
    	can_buffer_send[2+boucle] = APP_SW_NAME[boucle];
    }

    can_buffer_send[6] = ((APP_SW_VERSION_TEXT[0]-0x30)*16) +  (APP_SW_VERSION_TEXT[1]-0x30);

    can_buffer_send[7] = ((APP_SW_VERSION_TEXT[3]-0x30)*16) +  (APP_SW_VERSION_TEXT[4]-0x30);

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, can_buffer_send,TransmitMailbox);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_INFO_SOFT_CHEKSUM
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_INFO_SOFT_CHEKSUM(U32 adresse_module,U8 TransmitMailbox)
{
U32 temp_value;
U8 can_buffer_send[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_ID_SLAVE_TO_MASTER);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x08;

    can_objet.HWI_ID_FORMAT = HWI_STD;

    // id envoie info software
    can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

    // envoie checksum de soft
    can_buffer_send[1] = PL_CAN_CHECKSUM_SOFT;

    // checksum applicatif partie 1
    can_buffer_send[2] = (U8)((P_L_Rpg_checksum_applicatif/256)& 0x00FF);

    // checksum applicatif partie 2
    can_buffer_send[3] = (U8)(P_L_Rpg_checksum_applicatif & 0x00FF);

    // checksum config partie 1
    can_buffer_send[4] = (U8)((P_L_Rpg_checksum_config/256)& 0x00FF);

    // checksum config partie 2
    can_buffer_send[5] = (U8)(P_L_Rpg_checksum_config & 0x00FF);

    can_buffer_send[6] = 0x00;

    temp_value = FLASH->CR | MY_CR_LOCK_Set;
    if ((U32)0x00 == temp_value)
    {
    	can_buffer_send[7] = 0x00;
    }
    else
    {
    	can_buffer_send[7] = 0x01;
    }

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, can_buffer_send,TransmitMailbox);

}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_INFO_BOOT
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_INFO_BOOT(U32 adresse_module,U8 TransmitMailbox)
{
U8_CAL* ptr_version_boot;
U8 boucle;
U8 can_buffer_send[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_ID_SLAVE_TO_MASTER);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x08;

    can_objet.HWI_ID_FORMAT = HWI_STD;

    // id envoie info software
    can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

    // envoie checksum de soft
    can_buffer_send[1] = PL_CAN_VERSION_BOOT;

    ptr_version_boot=(U8_CAL*)APP_Boot_hw_name_flash_adr;

  //nom du software
    for(boucle=(U8)0x00;boucle<0x04;boucle++)
    {
    	can_buffer_send[2+boucle] = ptr_version_boot[boucle];
    }

    ptr_version_boot=(U8_CAL*)APP_Boot_sw_version_flash_adr;

    can_buffer_send[6] = ((ptr_version_boot[0]-0x30)*16) +  (ptr_version_boot[1]-0x30);

    can_buffer_send[7] = ((ptr_version_boot[3]-0x30)*16) +  (ptr_version_boot[4]-0x30);

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, can_buffer_send,TransmitMailbox);
}


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_INFO_CFG
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
static void P_L_Can_Send_INFO_CFG(U32 adresse_module,U8 TransmitMailbox,P_L_CAN_SEND_PARTI_TYPE partie)
{
U8 boucle;
U8 can_buffer_send[P_L_CAN_LENGHT_DATA];
HWI_CAN_OBJECT_TYPE  can_objet;
U8_CAL* ptr_Cfg_name;

    can_objet.HWI_CAN_MSG_ID = (adresse_module | P_L_ID_SLAVE_TO_MASTER);

    //Taille de la trame
    can_objet.HWI_CAN_LENGTH = 0x07;
    can_objet.HWI_ID_FORMAT = HWI_STD;

    if (TRUE == APP_Config_bloc_vue)
	{
		ptr_Cfg_name =(U8_CAL*)APP_CFG_NAME_BLOC;
	}
	else
	{
	   ptr_Cfg_name =(U8_CAL*)APP_CFG_NAME_FACADE;
	}

    if (ONE == partie)
	{
		// id envoie info software
    	can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

		// envoie name cfg 1
    	can_buffer_send[1] = PL_CAN_NAME_CFG_1;

		// on initialise la trame can
		for(boucle=(U8)0x00;boucle<0x05;boucle++)
		{
			//Nom du fichier de configurartion
			can_buffer_send[boucle+2] = (U8)ptr_Cfg_name[boucle];
		}
	}
	else if (TWO == partie)
	{
		// id envoie info software
		can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

		// envoie name cfg 2
		can_buffer_send[1] = PL_CAN_NAME_CFG_2;

		// on initialise la trame can
		for(boucle=(U8)0x00;boucle<0x05;boucle++)
		{
			//Nom du fichier de configurartion
			can_buffer_send[boucle+2] = (U8)ptr_Cfg_name[boucle+5];
		}

	}
	else if (THREE == partie)
	{
		// id envoie info software
		can_buffer_send[0] = PL_CAN_ID_INFO_SOFT;

		// envoie name cfg 3
		can_buffer_send[1] = PL_CAN_NAME_CFG_3;

		// on initialise la trame can
		for(boucle=(U8)0x00;boucle<0x05;boucle++)
		{
			//Nom du fichier de configurartion
			can_buffer_send[boucle+2] = (U8)ptr_Cfg_name[boucle+10];
		}
	}

    // envoie de la trame can sur CAN1 MAIL BOX
    Hwi_Can_Send_Message(CAN1, &can_objet, can_buffer_send,TransmitMailbox);
}


/*******************************************************************************
*
*  Function Name   :  P_L_Can_Send_Data_Can_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
void P_L_Can_Send_Data_Can_Init(void)
{
	P_L_Etape_send_info = 0x00;
	P_L_Etape_send_data_can = 0x00;
	P_L_Puissance_audio_PA = 4095;//2577;
//#define APP_VAL_MAX_E2POT_PUISSANCE             ((U16)4095)
#ifdef _RAMPE_AUTONOME
	P_L_Can_etape_BT = SEND_BT_ID;
#endif

}
