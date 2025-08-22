/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
/*******************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "app_manage_wifi.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_macros.h"
#include "p_l_bx310x.h"
#include "p_l_button.h"
#include "app_manage_mapping.h"
#include "hwi_hal_can.h"
#include <string.h>
#include "ff.h"
#ifdef _PROGRAMATION
#include "app_manage_flashage.h"
#include "app_manage_grp_file.h"
#include "p_l_can_flashage_tx.h"
#endif

#include "app_manage_BT.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
//extern U32                                  S_S_1MS_U32_TIME_HIGH_PRIORITY;

extern hw_addr                              P_L_Wifi_Module_Mac;

extern U8 	 		                        P_L_Wifi_Clef_wep[P_L_WIFI_CLEF_WEP_SIZE];

extern U32                                  S_S_1MS_U32_TIME;

extern BOOL_TYPE                            APP_Can_PMV_Non_Vue;
extern U32				                    APP_Timer_filtrage_perte_bloc_pmv;
extern BOOL_TYPE			                APP_Lock_filtrage_perte_bloc_pmv;

//flags indiquant si une config bloc ou facade a ete vue
extern BOOL_TYPE                            APP_Config_bloc_vue;

//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE           APP_Mode_gestion_wifi_blt;


#ifdef _GESTION_ODOMETRE
//interval odometre recue sur le can mercura
extern F64                          P_L_Can_interval_odometre_rx;
//comptage decompatge odometre recue sur le can mercura
extern BOOL_TYPE                    P_L_Can_down_odometre_rx;
//arret temporaire de l odometre recue sur le can mercura
extern BOOL_TYPE                    P_L_Can_stop_odometre_rx;
//arret de la gestion de l odometre recue sur le can mercura
extern BOOL_TYPE                    P_L_Can_on_off_odometre_rx;
//memo de l arret de la gestion de l odometre recue sur le can mercura
extern BOOL_TYPE                    P_L_Can_on_off_odometre_memo;

//valeur odometre envoye sur le can mercura
extern F64                          P_L_Can_valeur_odometre_tx;
//interval odometre envoye sur le can mercura
extern F64                          P_L_Can_interval_odometre_tx;

//flag indiquant que l odometre a change de valeur
extern BOOL_TYPE                    P_L_Can_odometre_chgt;

extern BOOL_TYPE                    P_L_Can_odometre_interval_90;
extern BOOL_TYPE                    P_L_Can_odometre_interval_100;
extern BOOL_TYPE                    P_L_Can_odometre_interval_100_memo;

//valeur etalonnage odometre
extern F64                          P_L_Can_etalonnage_odometre;
#endif

//flag indiquant qu'une carte SD autorisant la reprogramation de module
extern BOOL_TYPE                    P_L_CarteSd_Programation_vue;


#ifdef _PROGRAMATION
//temps de ref�rence au moment du flashage
extern APP_FLASHAGE_TIME_TYPE      APP_Flashage_time;
//step for programming module
extern APP_FLASHAGE_STEP           APP_Flashage_state;
//ask module reprog all
extern BOOL_TYPE                   APP_Flashage_reprog_all_module_ask;
//endpoint ask flashage
extern U8                          APP_Flashage_endpoint;
extern APP_FLASHAGE_OSCFG_TYPE     APP_Flashage_OS_CFG;

//ask send module liste
extern BOOL_TYPE                   APP_Flashage_liste_module_ask;
//liste des blocs vue sur le reseau CAN
extern APP_MODULE_VUE_TYPE         APP_Flashage_module[APP_NB_MAX_FLASHAGE_MODULE];
//memorise les adresse vue entre chaque demande
extern U32                         APP_Flashage_module_memo_adr[APP_NB_MAX_FLASHAGE_MODULE];
//ask module adresse change
extern BOOL_TYPE                   APP_Flashage_chgt_adr_module_ask;
//adresse du module cibl�
extern U32                         APP_Adr_module_chgt_adr;
//adresse du module cibl�
extern U8                          APP_Adr_log_module_chgt_adr;

//ask module historique
extern BOOL_TYPE                   APP_Flashage_historique_module_ask;
//adresse du module cibl�
extern U32                         APP_Adr_module_historique;
//taille de historique a lire
extern BOOL_TYPE                   APP_Full_historique;
//etapes de la lecture d un historique
extern P_L_CAN_HISTORIQUE_TYPE     P_L_Can_step_historique;
//tableau de reception des donnee de l historique
extern U8                          P_L_Can_received_historique[P_L_CAN_TAILLE_BUFFER_HISTORIQUE];
//Nombre historique vue
extern U8                          P_L_Can_Nb_historique;

// etatpes denvoie des donneee historique lue
extern U8                          P_L_Wifi_send_historique;

//flag indiquant que l on est en mode diag wifi
extern BOOL_TYPE                   P_L_Wifi_CAN_diag_on;
//endpoint diagnostique
extern U8                          P_L_Wifi_CAN_diag_endpoint;
//index de tableau ecriture trame CAN
extern U8                          APP_Wifi_count_trame_can;
//tableau de flag inndiquent buffer de reception CAN plein
extern BOOL_TYPE                   APP_Wifi_Send_BuffeSendCan[APP_Wifi_CAN_NB_BUFFER];
//position ou l on doit mettre les trame can recue
extern U8                          APP_Wifi_BuffeSendCan_write_num;
//tableau des donnee can recue
extern U8                          APP_Wifi_BuffeSendCan[APP_Wifi_CAN_NB_BUFFER][APP_Wifi_TAILLE_DONNE_CAN];
//index ecriture des donnee CAN recue
extern U8                          APP_Wifi_BuffeSendCan_index;

//ask mise a jour version file
extern BOOL_TYPE                   APP_Flashage_version_ask;
#endif

#ifdef _RAMPE_SOLAIRE
//demande d'arret d envoie des trame CAN
extern BOOL_TYPE                   APP_Demande_Arret_SendCan;

//demande d arret des blocs
extern BOOL_TYPE			       P_L_Can_Demande_Arret_Bloc;

extern BOOL_TYPE			       APP_Can_stop_request;
#endif

extern U16					    	APP_BT_can_ana[HWI_NB_ANA_BT_SPA_RAMPE];
extern U8					    	APP_BT_can_input;
extern BOOL_TYPE					APP_BT_can_Alarm_Bat_sys;
extern U16					    	APP_Tension_batterie[0x02];

extern BOOL_TYPE					APP_BT_can_Alarm_force_send;

extern BOOL_TYPE					APP_Demande_Arret_SendCan;
extern BOOL_TYPE					APP_Can_stop_request;

extern BOOL_TYPE 					APP_Demande_Arret_Rampe_autonome_wifi;
extern U32                          APP_Timer_Arret_Rampe_autonome_wifi;
extern BOOL_TYPE                    APP_Lock_Arret_Rampe_autonome_wifi;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
U8                                          APP_Wifi_pmv_type;
U8                                          APP_Wifi_pmv_message[APP_NB_MAX_LIGNE_WIFI][APP_TAILLE_MAX_MESS];
U8                                          APP_Num_message_tablette;

APP_WIFI_CLIENTS_TYPE                       APP_Wifi_clients_tab[P_L_WIFI_MAX_CLIENTS];

//table ascii a utiliser
U8                                          APP_Manage_pmv_table_ascii;

//flag indiquant si on doit afficher en mirroir
BOOL_TYPE                                   APP_Manage_mirroir;
BOOL_TYPE                                   APP_Manage_mirroir_2;
BOOL_TYPE                                   APP_Manage_reverse_motor;

//offset sur la consigne de puissance
U8                                          APP_Manage_OffsetPuissance;

// Mode jour force demande par le coeur analyse
BOOL_TYPE                                   APP_Mode_day_forced;
// Mode nuit force demande par le coeur analyse
BOOL_TYPE			                        APP_Mode_nuit_forced;

//conteur de nouvelle demande affichage
U8                                          APP_New_mess_rx;

//offset de modification de vitesse de message
U8                                          APP_offset_Vitesse_message;

//etat des leds des 2 premiers module de la config
U8                                          APP_Tab_Etat_Leds[APP_TAB_MAX_ETAT_LED];

//etat flags indiquant un chagenet etat de led
BOOL_TYPE                                   APP_Tab_Etat_Leds_Change;
BOOL_TYPE                                   APP_Tab_Etat_Leds_Change_vue;
//U8                                          APP_Tab_Etat_Leds_Change_Count;
//U8                                          APP_Tab_Etat_Leds_Change_CountMemo;
U32				                            APP_Timer_filtrage_Leds_Change;
BOOL_TYPE			                        APP_Lock_filtrage_Leds_Change;

//dernier etat e la gestion de l odometre recue sur le can mercura
BOOL_TYPE                                   P_L_Can_on_off_odometre_last;

//gestion semaphore envoie wifi
BOOL_TYPE                                   P_L_Wifi_Semaphore_Brocast;
BOOL_TYPE                                   P_L_Wifi_Semaphore_Brocast_memo;
BOOL_TYPE                                   P_L_Wifi_Semaphore_IT;

//timeout broadcasr
U32				                            P_L_Timer_timeout_Brocast;
BOOL_TYPE			                        P_L_Lock_timeout_Brocast;

//pointeur de clients broadcast
U8                                          P_L_Wifi_client_Sts_ptr;
U8                                          P_L_Wifi_client_Bip_ptr;


//valeur du bip en cours
U8                                          APP_request_buzzer[APP_TAB_MAX_ETAT_BIP];
//valeur du bip en cours envoyer
U8                                          APP_request_buzzer_send[APP_TAB_MAX_ETAT_BIP];
//etat flags indiquant un chagenet etat de bip
BOOL_TYPE                                   APP_request_buzzer_Change;

#ifdef _TEST_ETHERNET
//endpoint pour ethernet diagnostique
U8                                          P_L_Ethernet_diag_endpoint;
//indique que la liste des modules vues a change
BOOL_TYPE                                   P_L_Ethernet_Chgt_Module_vue;
//adresse CAN du module a diagnostique
U32                                         P_L_Ethernet_module_adr_diag;
//flag indiquant qu'il faut diagnostique un module
BOOL_TYPE                                   P_L_Ethernet_Diag_Module;

BOOL_TYPE                                   P_L_Ethernet_Diag_flip_flop;
// etat du module analyse
// mettre le dimension du tableau a 2 pour les module a 2 adresses
U8		                                    P_L_Ethernet_Module_Trame[255][7];

#endif


#ifdef _RAMPE_SOLAIRE
U8                                          P_L_Can_Battery[2];
U32				                            APP_Timer_filtrage_perte_tablette;
BOOL_TYPE			                        APP_Lock_filtrage_perte_tablette;
BOOL_TYPE                                   APP_Stop_Rampe;

BOOL_TYPE                                   APP_Active_State_Analyse[APP_NB_MAX_TOUCHE];
#endif

U32										    S_S_1MS_U32_TIME_HIGH_PRIORITY;

BOOL_TYPE 									APP_Wifi_connecter;


/////////////////////////////////////////////////////////////////////////////////
/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
BOOL_TYPE APP_Manage_Wifi_Find_Client(U8 endpoint,U8 *position_client);
static U8 APP_Manage_Wifi_Record_Client(U8 endpoint);
static BOOL_TYPE APP_Test_Trame_Format(U8* data,U16 data_len,U8 position_client,U16 * data_size_rx);
static BOOL_TYPE APP_Test_Trame_Cheksum(U8* data,U16 data_len,U16 * data_size_rx);
static void APP_Trame_OK(U8 endpoint,U8* Buffer_send);
static void APP_Trame_NOK(U8 endpoint,U8* Buffer_send);
static void APP_Trame_OK_BroadCast(U8 endpoint,U8* Buffer_data);
static void APP_Trame_NOK_BroadCast(U8 endpoint,U8* Buffer_data);
//static void APP_Trame_RUNNING(U8 endpoint,U8* Buffer_send);
static void APP_Trame_ARRET(U8 endpoint,U8* Buffer_send);
static void APP_Trame_ERROR(U8 endpoint,U8* Buffer_send);
#ifdef _GESTION_ODOMETRE
static void APP_Gestion_odometre_send(U8 endpoint);
#endif
#ifdef _PROGRAMATION
static void APP_Trame_Transfert_OK(U8 endpoint,U8* Buffer_send,U8 etapes);
static void APP_Trame_Transfert_NOK(U8 endpoint,U8* Buffer_send,U8 etapes);
static void APP_Trame_Transfert_FIN(U8 endpoint,U8* Buffer_send,U8 etapes);
#endif
/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
void APP_Test_WIFI_Connection()
{
U8 client;
BOOL_TYPE wifi_connection;

	wifi_connection = FALSE;
	for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
	{
		if ((TRUE == APP_Wifi_clients_tab[client].used)&&(FALSE == P_L_Wifi_Connect_Bluetooth(APP_Wifi_clients_tab[client].endpoint)))
		{
			wifi_connection = TRUE;
		}
	}

	APP_Wifi_connecter = wifi_connection;
}
/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_Wifi_Send
 *  Description     : gestion envoie sur evenement
 *
 *
 *******************************************************************************/
void APP_Gestion_Wifi_TestSend()
{
U8 client;
BOOL_TYPE result;

#ifdef _RAMPE_AUTONOME
    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
    {
    	if(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
    	{
    		APP_Manage_Wifi_BT_Timeout();
    	}

    	if (TRUE == APP_Tab_Etat_Leds_Change)
		{
			BT_TIMER_INIT_LOCK(APP_Timer_filtrage_Leds_Change,S_S_1MS_U32_TIME,APP_Lock_filtrage_Leds_Change);
			APP_Tab_Etat_Leds_Change = FALSE;
			APP_Tab_Etat_Leds_Change_vue = TRUE;
		}
		else
		{
			//on attend que tous les etats led soit a jour suite a un changement etat
			BT_TIMER_LOCK(APP_Timer_filtrage_Leds_Change,S_S_1MS_U32_TIME,APP_FILTRAGE_LED_CHANGE,APP_Lock_filtrage_Leds_Change,result);
			if(TRUE == result)
			{
				if (TRUE == APP_Tab_Etat_Leds_Change_vue)
				{
					APP_Tab_Etat_Leds_Change_vue = FALSE;

					for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
					{
						if ((TRUE == APP_Wifi_clients_tab[client].used)&&(FALSE == P_L_Wifi_Connect_Bluetooth(APP_Wifi_clients_tab[client].endpoint)))
						{
							APP_Wifi_clients_tab[client].send_Sts = TRUE;
							//APP_Wifi_clients_tab[client].send_Sts_Count = 3;
						}
					}
				}
			}
		}

    	if (TRUE == APP_request_buzzer_Change)
		{
		   if (0x00 != APP_request_buzzer[0])
		   {
			    APP_request_buzzer_send[0] = APP_request_buzzer[0];

				for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
				{
					if (TRUE == APP_Wifi_clients_tab[client].used)
					{
						APP_Wifi_clients_tab[client].send_Bip = TRUE;
					}
				}
		   }

		   APP_request_buzzer_Change = FALSE;
		}

    	if (TRUE == APP_BT_can_Alarm_force_send)
    	{
    		for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
			{
				if (TRUE == APP_Wifi_clients_tab[client].used)
				{
					APP_Wifi_clients_tab[client].send_Bat = TRUE;
				}
			}
    		APP_BT_can_Alarm_force_send = FALSE;
    	}
    }
    else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    {

    }
#ifdef _PROJECTEUR_BT
	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
	{

	}
#endif
    else
#endif
    {
		if (TRUE == APP_Tab_Etat_Leds_Change)
		{
			BT_TIMER_INIT_LOCK(APP_Timer_filtrage_Leds_Change,S_S_1MS_U32_TIME,APP_Lock_filtrage_Leds_Change);
			APP_Tab_Etat_Leds_Change = FALSE;
			APP_Tab_Etat_Leds_Change_vue = TRUE;
		}
		else
		{
			BT_TIMER_LOCK(APP_Timer_filtrage_Leds_Change,S_S_1MS_U32_TIME,APP_FILTRAGE_LED_CHANGE,APP_Lock_filtrage_Leds_Change,result);
			if(TRUE == result)
			{
				if (TRUE == APP_Tab_Etat_Leds_Change_vue)
				{
					APP_Tab_Etat_Leds_Change_vue = FALSE;

					for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
					{
						if (TRUE == APP_Wifi_clients_tab[client].used)
						{
							APP_Wifi_clients_tab[client].send_Sts = TRUE;
						}
					}
				}
			}
		}

		if (TRUE == APP_request_buzzer_Change)
		{
		   if ((0x00 != APP_request_buzzer[0])||(0x00 != APP_request_buzzer[1]))
		   {
			   if (0x00 != APP_request_buzzer[0])
			   {
				   APP_request_buzzer_send[0] = APP_request_buzzer[0];
			   }

			   if (0x00 != APP_request_buzzer[1])
			   {
				   APP_request_buzzer_send[1] = APP_request_buzzer[1];
			   }

				for (client=0;client<P_L_WIFI_MAX_CLIENTS;client++)
				{
					if (TRUE == APP_Wifi_clients_tab[client].used)
					{
						APP_Wifi_clients_tab[client].send_Bip = TRUE;
					}
				}
		   }

			APP_request_buzzer_Change = FALSE;
		}
    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_Wifi_Send
 *  Description     : gestion envoie sur evenement
 *
 *
 *******************************************************************************/
void APP_Gestion_Wifi_Send()
{
U8 	Buffer_send_Brodcast[P_L_TAILLE_SEND_MAX];
U8 boucle;
BOOL_TYPE result;
U32 TempKey;
U8 position_client;

#ifdef _RAMPE_AUTONOME
    //if (APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)
    //if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
	{
    	/*
		if (FALSE != P_L_Wifi_Semaphore_Brocast)
		{
		   if (FALSE == P_L_Wifi_Semaphore_Brocast_memo)
		   {
			   BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
		   }

			BT_TIMER_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,APP_TIMEOUT_BROADCAST,P_L_Lock_timeout_Brocast,result);
			if(TRUE == result)
			{
				P_L_Wifi_Semaphore_Brocast = FALSE;
			}
		}

		P_L_Wifi_Semaphore_Brocast_memo = P_L_Wifi_Semaphore_Brocast;

		*/

		if ( P_L_Wifi_client_Sts_ptr < P_L_WIFI_MAX_CLIENTS)
		{

			if(TRUE == APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_Key)
			{
				APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_Key = FALSE;

				APP_Manage_Wifi_Find_Client(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,&position_client);

				TempKey = S_S_1MS_U32_TIME_HIGH_PRIORITY;

				APP_Wifi_clients_tab[position_client].wifi_key[0] = (TempKey & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[1] = ((TempKey >> 8 ) & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[2] = ((TempKey >> 16 ) & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[3] = ((TempKey >> 24 ) & 0x000000FF);

				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[0] = APP_Wifi_clients_tab[position_client].wifi_key[0] ^ P_L_Wifi_Module_Mac.addr[0];
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1] = (APP_Wifi_clients_tab[position_client].wifi_key[1] ^ P_L_Wifi_Module_Mac.addr[1]) + 1;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[2]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[0]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[3] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[3]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1]) - 7;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[4] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[4]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[5] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[5]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[3]) - 7;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[6] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[0]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[7] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[1]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2]) - 7;

				APP_Wifi_clients_tab[position_client].wifi_key_ask = TRUE;
				BT_TIMER_INIT_LOCK(APP_Wifi_clients_tab[position_client].timer_key_ask,S_S_1MS_U32_TIME,APP_Wifi_clients_tab[position_client].lock_key_ask);

				BUFF_KEY(Buffer_send_Brodcast);

				Buffer_send_Brodcast[4] = 0x00;
				Buffer_send_Brodcast[5] = (APP_TAILLE_WIFI_KEY+3);
				Buffer_send_Brodcast[6] = 'O';
				Buffer_send_Brodcast[7] = 'K';

				for (boucle=0;boucle<APP_TAILLE_WIFI_KEY;boucle++)
				{
					Buffer_send_Brodcast[8+boucle] = APP_Wifi_clients_tab[position_client].wifi_key[boucle];
				}
				Buffer_send_Brodcast[12] = FIN_CHAINE;
				P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,13,Buffer_send_Brodcast);

			}
			else if(TRUE == APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_Bat)
			{
				APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_Bat = FALSE;
				BUFF_BAT(Buffer_send_Brodcast);

				Buffer_send_Brodcast[4] = 0x00;
				//Buffer_send_Brodcast[5] = 0x07;
				Buffer_send_Brodcast[5] = 0x08;

				if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
				{
					Buffer_send_Brodcast[6] =  APP_Tension_batterie[0];
					Buffer_send_Brodcast[7] =  APP_Tension_batterie[1];
				}
				else
				{
					Buffer_send_Brodcast[6] = (APP_BT_can_ana[0]>> 8) & 0xFF;
					Buffer_send_Brodcast[7] = APP_BT_can_ana[0] & 0xFF;
				}

				Buffer_send_Brodcast[8] = APP_BT_can_input;

				Buffer_send_Brodcast[9] =  APP_Tension_batterie[0];
				Buffer_send_Brodcast[10] =  APP_Tension_batterie[1];

				Buffer_send_Brodcast[11] = APP_BT_can_Alarm_Bat_sys;

				//Buffer_send_Brodcast[12] = FIN_CHAINE;
				//P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,13,Buffer_send_Brodcast);

				Buffer_send_Brodcast[12] = APP_Mode_gestion_wifi_blt;

				Buffer_send_Brodcast[13] = FIN_CHAINE;
				P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,14,Buffer_send_Brodcast);


			}
			else if(TRUE == APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_OK)
			{
				APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_OK = FALSE;

				APP_Trame_OK_BroadCast(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].data_ok_nok);
			}
			else if(TRUE == APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_OK)
			{
				APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].send_NOK = FALSE;
				APP_Trame_NOK_BroadCast(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,APP_Wifi_clients_tab[P_L_Wifi_client_Sts_ptr].data_ok_nok);

			}
			else if (TRUE == APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Sts)
			{
				APP_Tab_Etat_Leds_Change = FALSE;
				//envoie etat led

				BUFF_STS(Buffer_send_Brodcast);

				Buffer_send_Brodcast[4] = 0x00;
				Buffer_send_Brodcast[5] = 65;

				for (boucle=0;boucle<APP_TAB_MAX_ETAT_LED;boucle++)
				{
					Buffer_send_Brodcast[6+boucle] = APP_Tab_Etat_Leds[boucle];
				}

				Buffer_send_Brodcast[70] = FIN_CHAINE;

				P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,71,Buffer_send_Brodcast);


				APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Sts = FALSE;

				BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
			}
			else if (TRUE == APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Bip)
			{
				BUFF_BIP(Buffer_send_Brodcast);
				Buffer_send_Brodcast[4] = 0x00;
				Buffer_send_Brodcast[5] = 0x03;
				Buffer_send_Brodcast[6] = APP_request_buzzer_send[0];
				Buffer_send_Brodcast[7] = APP_request_buzzer_send[1];
				Buffer_send_Brodcast[8] = FIN_CHAINE;
				P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,9,Buffer_send_Brodcast);

				APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Bip= FALSE;

				BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
			}
			else
			{
				 P_L_Wifi_client_Sts_ptr++;
			}
		}
		else
		{
			P_L_Wifi_client_Sts_ptr = 0;
		}
    }

    else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    {

    }
#ifdef _PROJECTEUR_BT
	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
	{

	}
#endif
    else
#endif
    {
		if (FALSE != P_L_Wifi_Semaphore_Brocast)
		{
		   if (FALSE == P_L_Wifi_Semaphore_Brocast_memo)
		   {
			   BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
		   }

			BT_TIMER_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,APP_TIMEOUT_BROADCAST,P_L_Lock_timeout_Brocast,result);
			if(TRUE == result)
			{
				P_L_Wifi_Semaphore_Brocast = FALSE;
			}
		}

		P_L_Wifi_Semaphore_Brocast_memo = P_L_Wifi_Semaphore_Brocast;


		if ( P_L_Wifi_client_Sts_ptr < 2)
		{
			if (TRUE == APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Sts)
			{
				P_L_Wifi_Semaphore_Brocast = TRUE;

				if (FALSE == P_L_Wifi_Semaphore_IT)
				{
					APP_Tab_Etat_Leds_Change = FALSE;
					//envoie etat led

					BUFF_STS(Buffer_send_Brodcast);

					Buffer_send_Brodcast[4] = 0x00;
					Buffer_send_Brodcast[5] = 65;

					for (boucle=0;boucle<APP_TAB_MAX_ETAT_LED;boucle++)
					{
						Buffer_send_Brodcast[6+boucle] = APP_Tab_Etat_Leds[boucle];
					}

					Buffer_send_Brodcast[70] = FIN_CHAINE;

					P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].endpoint,71,Buffer_send_Brodcast);

					APP_Wifi_clients_tab[ P_L_Wifi_client_Sts_ptr].send_Sts = FALSE;
					P_L_Wifi_client_Sts_ptr++;
					P_L_Wifi_Semaphore_Brocast = FALSE;
					BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
				}

			}
			else
			{
				 P_L_Wifi_client_Sts_ptr++;
			}
		}
		else
		{
			if (TRUE == APP_Wifi_clients_tab[ P_L_Wifi_client_Bip_ptr].send_Bip)
			{
				P_L_Wifi_Semaphore_Brocast = TRUE;

				if (FALSE == P_L_Wifi_Semaphore_IT)
				{
					BUFF_BIP(Buffer_send_Brodcast);
					Buffer_send_Brodcast[4] = 0x00;
					Buffer_send_Brodcast[5] = 0x03;
					Buffer_send_Brodcast[6] = APP_request_buzzer_send[0];
					Buffer_send_Brodcast[7] = APP_request_buzzer_send[1];
					Buffer_send_Brodcast[8] = FIN_CHAINE;
					P_L_Wifi_Tx_dataDirect(APP_Wifi_clients_tab[ P_L_Wifi_client_Bip_ptr].endpoint,9,Buffer_send_Brodcast);

					APP_Wifi_clients_tab[ P_L_Wifi_client_Bip_ptr].send_Bip= FALSE;
					P_L_Wifi_client_Bip_ptr++;
					P_L_Wifi_Semaphore_Brocast = FALSE;
					BT_TIMER_INIT_LOCK(P_L_Timer_timeout_Brocast,S_S_1MS_U32_TIME,P_L_Lock_timeout_Brocast);
				}
			}
			else
			{
				 P_L_Wifi_client_Bip_ptr++;
			}

			if ( P_L_Wifi_client_Bip_ptr >= P_L_WIFI_MAX_CLIENTS)
			{
				 P_L_Wifi_client_Sts_ptr = 0;
				 P_L_Wifi_client_Bip_ptr = 0;
			}
		}
    }
}
#ifdef _PROGRAMATION
BOOL_TYPE diag_on = FALSE;
U32 ID_CAN_TX;
U8 DATA_CAN_TX[8];
U8 DLC_CAN_TX;
#endif
/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_Wifi_Data
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
#ifdef _DEBUG_BX310X_COM
U8 	Buffer_send[P_L_TAILLE_SEND_MAX];
#endif

void APP_Gestion_Wifi_Data(U8 endpoint, U16 data_len,U8* data)
{
const U32   *ptr_configU32;
const U8   *ptr_configU8;
U32 SwWord;
#ifndef _DEBUG_BX310X_COM
U8 	Buffer_send[P_L_TAILLE_SEND_MAX];
#endif
U8  FileName[P_L_TAILLE_SEND_MAX];
U32 TempKey;
U8 boucle;
//U8 boucle2;
U8 position_client;
U8 offset;
//U8 offset_vitesse;
//U8 offset_data_size;
BOOL_TYPE client_ok;
BOOL_TYPE new_mess_rx_temp;
U16 data_size_rx;
U32 temp_calcul;

#ifdef _PROGRAMATION
HWI_CAN_OBJECT_TYPE Can_objet;
U8 data_size_read;
#endif


#ifdef _TEST_ETHERNET
U32 Ethernet_CAN_ID;
U8 tempU8;
#endif

    client_ok = APP_Manage_Wifi_Find_Client(endpoint,&position_client);

//    offset_data_size = 2;

	SwWord = LD_DWORD(&data[0]);
/*
#ifdef _PROGRAMATION
    if (SwWord == WRD_DIAG)
    {
        if (TRUE == P_L_CarteSd_Programation_vue)
        {
            if (FALSE == client_ok)
            {
                position_client = APP_Manage_Wifi_Record_Client(endpoint);
            }
            if (('O' == data[6])&&('N' == data[7]))
            {
              P_L_Wifi_CAN_diag_on = TRUE;
              P_L_Wifi_CAN_diag_endpoint = endpoint;
            }
            else if (('O' == data[6])&&('F' == data[7])&&('F' == data[8]))
            {
              P_L_Wifi_CAN_diag_on = FALSE;
            }

        }
    }
    else if (SwWord == WRD_CAN)
    {
        if (TRUE == P_L_CarteSd_Programation_vue)
        {
            if ((TRUE == client_ok)&&(TRUE == P_L_Wifi_CAN_diag_on))
            {
                P_L_Wifi_CAN_diag_endpoint = endpoint;

                temp_calcul = ((U32)(data[6])*256*256*256);
                temp_calcul = temp_calcul + ((U32)(data[7])*256*256);
                temp_calcul = temp_calcul +((U32)(data[8])*256);
                temp_calcul = temp_calcul + (U32)data[9];

                Can_objet.HWI_CAN_MSG_ID = temp_calcul;
                Can_objet.HWI_ID_FORMAT=HWI_STD;
                Can_objet.HWI_CAN_LENGTH = data[10];

                // envoie de la trame can sur CAN1 MAIL BOX 1
                Hwi_Can_Send_Message(CAN1, &Can_objet, &(data[11]),MAIL_BOX1);

            }
        }
    }
#ifdef _TEST_ETHERNET
    else if (SwWord == WRD_CMD_ETHERNET)
    {
        //BUFF_VERSION_FILE_REP(Buffer_send);
        if (TRUE == P_L_CarteSd_Programation_vue)
        {

            if (FALSE == client_ok)
            {
                position_client = APP_Manage_Wifi_Record_Client(endpoint);
            }

            if (TRUE == APP_Test_Trame_Cheksum(data,data_len,&data_size_rx))
            {
                P_L_Ethernet_diag_endpoint = endpoint;

                Ethernet_CAN_ID = ((U32)(data[6])*256*256*256);
                Ethernet_CAN_ID = Ethernet_CAN_ID + ((U32)(data[7])*256*256);
                Ethernet_CAN_ID = Ethernet_CAN_ID +((U32)(data[8])*256);
                Ethernet_CAN_ID = Ethernet_CAN_ID + (U32)data[9];

                //Ethernet_CAN_DLC = data[10];

                if (Ethernet_CAN_ID == PL_CAN_ID_ETHERNET_ASK)
                {
                    BUFF_CMD_ETHERNET_REP(Buffer_send);

                    switch(data[11])
                    {
                        case PL_CAN_ASK_CHGT_MODULE_VUE:
                            P_L_Ethernet_Diag_Module = FALSE;
                            tempU8 = 0;
                            for(boucle=0;boucle<APP_NB_MAX_FLASHAGE_MODULE;boucle++)
                            {
                                if (ADR_NU != APP_Flashage_module[boucle].adresse)
                                {
                                    tempU8++;
                                }

                                if (APP_Flashage_module_memo_adr[boucle] != APP_Flashage_module[boucle].adresse)
                                {
                                    APP_Flashage_module_memo_adr[boucle]  = APP_Flashage_module[boucle].adresse;
                                    P_L_Ethernet_Chgt_Module_vue = TRUE;
                                }
                            }
                            Buffer_send[4] = 0x00;
                            Buffer_send[5] = 14;

                            //ID
                            Ethernet_CAN_ID = (0x400 + PL_CAN_ID_ETHERNET_ASK);
                            Buffer_send[6] = (U8)((Ethernet_CAN_ID >> 24) & 0x000000FF);
                            Buffer_send[7] = (U8)((Ethernet_CAN_ID >> 16) & 0x000000FF);
                            Buffer_send[8] = (U8)((Ethernet_CAN_ID >> 8) & 0x000000FF);
                            Buffer_send[9] = (U8)(Ethernet_CAN_ID & 0x00FF);
                            //DLC
                            Buffer_send[10] =  0x03;
                            //data
                            Buffer_send[11] = PL_CAN_ASK_CHGT_MODULE_VUE;

                            if (TRUE == P_L_Ethernet_Chgt_Module_vue)
                            {
                                Buffer_send[12] = 0x01;
                            }
                            else
                            {
                               Buffer_send[12] = 0x00;
                            }
                            Buffer_send[13] = tempU8;

                            Buffer_send[14] = 0x00;
                            Buffer_send[15] = 0x00;
                            Buffer_send[16] = 0x00;
                            Buffer_send[17] = 0x00;
                            Buffer_send[18] = 0x00;

                            Buffer_send[19] = FIN_CHAINE;
                            P_L_Wifi_Tx_data(endpoint,20,Buffer_send);
                        break;

                        case PL_CAN_ASK_MODULE_VUE:
                            P_L_Ethernet_Chgt_Module_vue = FALSE;
                            P_L_Ethernet_diag_endpoint = endpoint;

                            Buffer_send[4] = 0x00;
                            Buffer_send[5] = 14;

                            //ID
                            Ethernet_CAN_ID = (0x400 + PL_CAN_ID_ETHERNET_ASK);
                            Buffer_send[6] = (U8)((Ethernet_CAN_ID >> 24) & 0x000000FF);
                            Buffer_send[7] = (U8)((Ethernet_CAN_ID >> 16) & 0x000000FF);
                            Buffer_send[8] = (U8)((Ethernet_CAN_ID >> 8) & 0x000000FF);
                            Buffer_send[9] = (U8)(Ethernet_CAN_ID & 0x00FF);
                            //DLC
                            Buffer_send[10] =  0x06;
                            //data
                            Buffer_send[11] = PL_CAN_ASK_MODULE_VUE;

                            Buffer_send[12] = data[12];

                            if (APP_NB_MAX_FLASHAGE_MODULE > data[12])
                            {
                                Buffer_send[13] = (U8)((APP_Flashage_module[data[12]].adresse >> 24) & 0x000000FF);
                                Buffer_send[14] = (U8)((APP_Flashage_module[data[12]].adresse >> 16) & 0x000000FF);
                                Buffer_send[15] = (U8)((APP_Flashage_module[data[12]].adresse >> 8) & 0x000000FF);
                                Buffer_send[16] = (U8)(APP_Flashage_module[data[12]].adresse & 0x00FF);
                            }
                            else
                            {
                                Buffer_send[13] = 0xFF;
                                Buffer_send[14] = 0xFF;
                                Buffer_send[15] = 0xFF;
                                Buffer_send[16] = 0xFF;
                            }

                            Buffer_send[17] = 0x00;
                            Buffer_send[18] = 0x00;

                            Buffer_send[19] = FIN_CHAINE;
                            P_L_Wifi_Tx_data(endpoint,20,Buffer_send);
                        break;

                        default:
                        break;
                    }
                }
                else if (Ethernet_CAN_ID == PL_CAN_ID_ETHERNET_ADR_MODULE)
                {
                    P_L_Ethernet_module_adr_diag = (data[11]*1024) +(data[12]*512) + (data[13]*256)  + data[14];
                    P_L_Ethernet_Diag_Module = TRUE;
                    P_L_Ethernet_diag_endpoint = endpoint;
                }
                else if (Ethernet_CAN_ID == PL_CAN_ID_ETHERNET_READ)
                {
                    BUFF_CMD_ETHERNET_REP(Buffer_send);

                    Buffer_send[4] = 0x00;
                    Buffer_send[5] = 14;

                    //ID
                    Ethernet_CAN_ID = (0x400 + PL_CAN_ID_ETHERNET_READ);
                    Buffer_send[6] = (U8)((Ethernet_CAN_ID >> 24) & 0x000000FF);
                    Buffer_send[7] = (U8)((Ethernet_CAN_ID >> 16) & 0x000000FF);
                    Buffer_send[8] = (U8)((Ethernet_CAN_ID >> 8) & 0x000000FF);
                    Buffer_send[9] = (U8)(Ethernet_CAN_ID & 0x00FF);
                    //DLC
                    Buffer_send[10] =  0x08;
                    //data
                    Buffer_send[11] = data[11];

                    for(boucle=0;boucle<7;boucle++)
                    {
                        Buffer_send[12+boucle] = P_L_Ethernet_Module_Trame[data[11]][boucle];
                    }

                    Buffer_send[19] = FIN_CHAINE;
                    P_L_Wifi_Tx_data(endpoint,20,Buffer_send);

                }
                else if (Ethernet_CAN_ID == PL_CAN_ID_ETHERNET_WRITE)
                {
                    if (data[11] == PL_CAN_ID_CMD)
                    {
                       Can_objet.HWI_CAN_MSG_ID = 0x00;
                    }
                    else
                    {
                        Can_objet.HWI_CAN_MSG_ID = P_L_Ethernet_module_adr_diag;
                    }
                    Can_objet.HWI_ID_FORMAT=HWI_STD;
                    Can_objet.HWI_CAN_LENGTH = data[10];

                    ID_CAN_TX = Can_objet.HWI_CAN_MSG_ID;
                    DLC_CAN_TX = Can_objet.HWI_CAN_LENGTH;
                    for(boucle=0;boucle<8;boucle++)
                    {
                        DATA_CAN_TX[boucle] = data[11+boucle];
                    }

                    if (TRUE == P_L_Ethernet_Diag_flip_flop)
                    {
                      // envoie de la trame can sur CAN1 MAIL BOX 1
                        Hwi_Can_Send_Message(CAN1, &Can_objet, &(data[11]),MAIL_BOX2);
                    }
                    else
                    {
                        // envoie de la trame can sur CAN1 MAIL BOX 1
                        Hwi_Can_Send_Message(CAN1, &Can_objet, &(data[11]),MAIL_BOX3);
                    }
                    P_L_Ethernet_Diag_flip_flop = !P_L_Ethernet_Diag_flip_flop;

                }
                else
                {
                    //do nothing
                }
            }
        }
    }
#endif
    else if (SwWord == WRD_KEY_ASK)
#else
	if (SwWord == WRD_KEY_ASK)
#endif
*/
	if (SwWord == WRD_KEY_ASK)
    {
/////////////////////////////////////////////////////////////////////////////////////
//
//      DEMANDE DE CLEF DE DEVEROUILLAGE
//
////////////////////////////////////////////////////////////////////////////////////
        if (FALSE == client_ok)
        {
            position_client = APP_Manage_Wifi_Record_Client(endpoint);

//            offset_data_size = 2;
        }

        //if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
        if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
        {
        	if (0xFF != position_client)
        	{
        		APP_Wifi_clients_tab[position_client].send_Key = TRUE;
        	}
        }
        else
        {
			if (0xFF != position_client)
			{
				TempKey = S_S_1MS_U32_TIME_HIGH_PRIORITY;

				APP_Wifi_clients_tab[position_client].wifi_key[0] = (TempKey & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[1] = ((TempKey >> 8 ) & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[2] = ((TempKey >> 16 ) & 0x000000FF);
				APP_Wifi_clients_tab[position_client].wifi_key[3] = ((TempKey >> 24 ) & 0x000000FF);

				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[0] = APP_Wifi_clients_tab[position_client].wifi_key[0] ^ P_L_Wifi_Module_Mac.addr[0];
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1] = (APP_Wifi_clients_tab[position_client].wifi_key[1] ^ P_L_Wifi_Module_Mac.addr[1]) + 1;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[2]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[0]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[3] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[3]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1]) - 7;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[4] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[4]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[5] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[5]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[3]) - 7;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[6] = ((APP_Wifi_clients_tab[position_client].wifi_key[2] ^ P_L_Wifi_Module_Mac.addr[0]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[1]) + 78;
				APP_Wifi_clients_tab[position_client].wifi_Unlockkey[7] = ((APP_Wifi_clients_tab[position_client].wifi_key[3] ^ P_L_Wifi_Module_Mac.addr[1]) ^ APP_Wifi_clients_tab[position_client].wifi_Unlockkey[2]) - 7;

				APP_Wifi_clients_tab[position_client].wifi_key_ask = TRUE;
				BT_TIMER_INIT_LOCK(APP_Wifi_clients_tab[position_client].timer_key_ask,S_S_1MS_U32_TIME,APP_Wifi_clients_tab[position_client].lock_key_ask);

				BUFF_KEY(Buffer_send);

				Buffer_send[4] = 0x00;
				Buffer_send[5] = (APP_TAILLE_WIFI_KEY+3);
				Buffer_send[6] = 'O';
				Buffer_send[7] = 'K';

				for (boucle=0;boucle<APP_TAILLE_WIFI_KEY;boucle++)
				{
					Buffer_send[8+boucle] = APP_Wifi_clients_tab[position_client].wifi_key[boucle];
				}
				Buffer_send[12] = FIN_CHAINE;
				P_L_Wifi_Tx_data(endpoint,13,Buffer_send);

			}
			else
			{
				BUFF_KEY(Buffer_send);
				APP_Trame_NOK(endpoint,Buffer_send);
			}
        }
    }
    else if (SwWord == WRD_MESS_ASK)
    {
/////////////////////////////////////////////////////////////////////////////////////
//
//      DEMANDE ETAT DU MESSAGE EN COURS
//
////////////////////////////////////////////////////////////////////////////////////

    	//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
    	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
    	{
			// not used in this case
		}
		else
		{
			BUFF_MESS_REP(Buffer_send);

			Buffer_send[4] = 0x00;
			Buffer_send[5] = 0x09;

			Buffer_send[4+2] = APP_Wifi_pmv_type;

			Buffer_send[5+2] = APP_Manage_pmv_table_ascii;

			Buffer_send[6+2] = APP_Manage_OffsetPuissance;

			if (FALSE == APP_Manage_mirroir)
			{
				Buffer_send[7+2] = 0x00;
			}
			else
			{
				Buffer_send[7+2] = 0x01;
			}

			if (TRUE == APP_Manage_mirroir_2)
			{
			   Buffer_send[7+2] = Buffer_send[7+2] + 0x02;
			}

			if (TRUE == APP_Manage_reverse_motor)
			{
			   Buffer_send[7+2] = Buffer_send[7+2] + 0x04;
			}

			if(TRUE == APP_Mode_day_forced)
			{
				Buffer_send[8+2] = 0x01;
			}
			else if(TRUE == APP_Mode_nuit_forced)
			{
				Buffer_send[8+2] = 0x02;
			}
			else
			{
				Buffer_send[8+2] = 0x00;
			}


			Buffer_send[9+2] = APP_Num_message_tablette;

			if (TRUE == APP_Can_PMV_Non_Vue)
			{
				Buffer_send[10+2] = 0x00;
			}
			else
			{
				Buffer_send[10+2] = 0x00;
			}

			Buffer_send[11+2] = APP_offset_Vitesse_message;



			Buffer_send[12+2] = FIN_CHAINE;

			P_L_Wifi_Tx_data(endpoint,13+2,Buffer_send);
		}
    }
    else if (TRUE == client_ok)
    {
        switch (SwWord) {
            case WRD_MESS_PLUS:
            case WRD_MESS:
/////////////////////////////////////////////////////////////////////////////////////
//
//      ECRITURE DU MESSAGE EN COURS
//
////////////////////////////////////////////////////////////////////////////////////

                if (SwWord == WRD_MESS_PLUS)
                {
                    offset = 48;
                }
                else
                {
                    offset = 0;
                }
/*
                if ((data_len-4) > (69+offset))
                {
                   offset_vitesse = 1;
                }
                else
                {
                   offset_vitesse = 0;
                }
*/


                if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                {

                    new_mess_rx_temp = FALSE;

                    if (APP_Wifi_pmv_type != data[12+2])
                    {
                        APP_Wifi_pmv_type = data[12+2];
                        new_mess_rx_temp = TRUE;
                    }


                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x00][boucle]  != data[13+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x00][boucle] = data[13+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x01][boucle]  != data[29+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x01][boucle] = data[29+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x02][boucle]  != data[45+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x02][boucle] = data[45+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x03][boucle]  != data[61+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x03][boucle] = data[61+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x04][boucle]  != data[77+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x04][boucle] = data[77+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    for (boucle=0;boucle<APP_TAILLE_MAX_MESS;boucle++)
                    {
                        if (APP_Wifi_pmv_message[0x05][boucle]  != data[93+boucle+2])
                        {
                            APP_Wifi_pmv_message[0x05][boucle] = data[93+boucle+2];
                            new_mess_rx_temp = TRUE;
                        }
                    }

                    if (APP_Manage_pmv_table_ascii != data[61+offset+2])
                    {
                        APP_Manage_pmv_table_ascii = data[61+offset+2];
                        new_mess_rx_temp = TRUE;
                    }

                    APP_Manage_OffsetPuissance = data[62+offset+2];

                    if (0x00 == (data[63+offset+2] & 0x01))
                    {
                        APP_Manage_mirroir = FALSE;
                    }
                    else
                    {
                        APP_Manage_mirroir = TRUE;
                    }

                    if (0x01 == ((data[63+offset+2] >> 1 ) & 0x01))
                    {
                        APP_Manage_mirroir_2 = TRUE;
                    }
                    else
                    {
                        APP_Manage_mirroir_2 = FALSE;
                    }

                    if (0x01 == ((data[63+offset+2] >> 2 ) & 0x01))
                    {
                        APP_Manage_reverse_motor = TRUE;
                    }
                    else
                    {
                        APP_Manage_reverse_motor = FALSE;
                    }

                    if (0x01 == data[64+offset+2])
                    {
                        APP_Mode_day_forced = TRUE;
                        APP_Mode_nuit_forced = FALSE;
                    }
                    else if (0x02 == data[64+offset+2])
                    {
                        APP_Mode_day_forced = FALSE;
                        APP_Mode_nuit_forced = TRUE;
                    }
                    else
                    {
                        APP_Mode_day_forced = FALSE;
                        APP_Mode_nuit_forced = FALSE;
                    }

                    APP_Num_message_tablette = data[65+offset+2];

                    if (data_size_rx > (63+offset))
                    {
                        if (APP_offset_Vitesse_message != data[66+offset+2])
                        {
                            APP_offset_Vitesse_message = data[66+offset+2];
                            new_mess_rx_temp = TRUE;
                        }

                    }
                    else
                    {
                       APP_offset_Vitesse_message = (0xFF/2);
                    }

                    if (TRUE == new_mess_rx_temp)
                    {
                        if (10 != APP_New_mess_rx)
                        {
                            APP_New_mess_rx = APP_New_mess_rx+1;
                        }
                    }

                    BUFF_MESS(Buffer_send);
                    APP_Trame_OK(endpoint,Buffer_send);
                }
                else
                {
                    BUFF_MESS(Buffer_send);
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_WEP:
/////////////////////////////////////////////////////////////////////////////////////
//
//      CHAGEMENT DE LA CLEF WEP
//
////////////////////////////////////////////////////////////////////////////////////
                if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                {
                    for (boucle=0;boucle<P_L_WIFI_CLEF_WEP_SIZE;boucle++)
                    {
                        P_L_Wifi_Clef_wep[boucle] = data[12+boucle+2];
                    }

                    //if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                    {
						if (0xFF != position_client)
						{
							BUFF_WEP(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_OK = TRUE;
							P_L_Wifi_Wep_Wr();
						}
					}
					else
					{
						BUFF_WEP(Buffer_send);
						APP_Trame_OK(endpoint,Buffer_send);
						P_L_Wifi_Wep_Wr();
					}
                }
                else
                {
                	//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	{
						if (0xFF != position_client)
						{
							BUFF_WEP(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_NOK = TRUE;
						}
                	}
                	else
                	{
                		BUFF_WEP(Buffer_send);
                		APP_Trame_NOK(endpoint,Buffer_send);
                	}
                }
            break;


            case WRD_CFG:
/////////////////////////////////////////////////////////////////////////////////////
//
//      DEMANDE DE LA CONFIG DES TOUCHES ET VOYANT
//
////////////////////////////////////////////////////////////////////////////////////
                if ((TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx)) && (FALSE == APP_Config_bloc_vue))
                {
                    //si on est en mode message PMV est que l'appli nous demande la config des touches
                    // alors on passe en mode message et BTN
                    if (APP_MODE_MESSAGE_PMV == APP_Mode_gestion_wifi_blt)
                    {
                        APP_Mode_gestion_wifi_blt = APP_MODE_BTN_ET_MESSAGE_PMV;
                        HWI_Can_FilterConfig();
                    }

                    BUFF_CFG(Buffer_send);
                    if (0x00 == data[14])
                    {
                        Buffer_send[4] = 0x00;
                        Buffer_send[5] = 37;

                        Buffer_send[6] = 0x00;

                        ptr_configU32 = (const U32 *)APP_LISTE_FACADE_CFG_FACADE;

                        Buffer_send[7] = (*ptr_configU32 & 0x00FF);
                        Buffer_send[8] = ((*ptr_configU32 >> 8) & 0x00FF);

                        ptr_configU8 = (const U8 *)APP_TFT_ECRAN_CAN;
                        Buffer_send[9] = *ptr_configU8;

                        ptr_configU8 = (const U8 *)APP_TFT_TOUCHE;
                        for(boucle=0;boucle<APP_NB_MAX_TOUCHE;boucle++)
                        {
                            Buffer_send[10+boucle] = *(ptr_configU8+boucle);
                        }

                        Buffer_send[42] = FIN_CHAINE;
                        P_L_Wifi_Tx_data(endpoint,43,Buffer_send);
                    }
                    else
                    {
                        Buffer_send[4] = 0x00;
                        Buffer_send[5] = 88;

                        Buffer_send[6] = 0x01;

                        ptr_configU8 = (const U8 *)APP_TFT_TOUCHE_G_CAN;
                        Buffer_send[7] = *ptr_configU8;

                        ptr_configU8 = (const U8 *)APP_TFT_PICTO_TOUCHE;
                        for(boucle=0;boucle<(2*APP_NB_MAX_TOUCHE);boucle++)
                        {
                            Buffer_send[8+boucle] = *(ptr_configU8+boucle);
                        }

                         ptr_configU8 = (const U8 *)APP_TFT_VOYANT;
                        for(boucle=0;boucle<APP_NB_MAX_VOYANT;boucle++)
                        {
                            Buffer_send[72+boucle] = *(ptr_configU8+boucle);
                        }


                        ptr_configU8 = (const U8 *)APP_TFT_NB_PMV_CARTE_LED;
                        Buffer_send[92] = *ptr_configU8;

                        Buffer_send[93] = FIN_CHAINE;
                        P_L_Wifi_Tx_data(endpoint,94,Buffer_send);
                    }
                }
                else
                {
                    BUFF_CFG(Buffer_send);
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_BTN:
/////////////////////////////////////////////////////////////////////////////////////
//
//     RECEPTION D UN CHANGEMENT ETAT DE TOUCHE
//
////////////////////////////////////////////////////////////////////////////////////
                if ((TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx)) && (FALSE == APP_Config_bloc_vue))
                {
                    P_L_State_Button(data[14], (BOOL_TYPE)data[15]);

                    if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
                    {
                    	if (data[14] == 5)
                    	{
                    		if (data[15] == 1)
                    		{
                    			APP_Demande_Arret_Rampe_autonome_wifi= TRUE;

                    			BT_TIMER_INIT_LOCK(APP_Timer_Arret_Rampe_autonome_wifi,S_S_1MS_U32_TIME,APP_Lock_Arret_Rampe_autonome_wifi);
                    		}
                    	}

                    	if (data[14] != 5)
						{
							if (data[15] == 1)
							{
								APP_Demande_Arret_Rampe_autonome_wifi= FALSE;
							}
						}
                    }

                    //if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                    {
                    	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
						{
							APP_Manage_Reload_Wifi_BT_Timeout();
						}

						if (0xFF != position_client)
						{
							BUFF_BTN(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_OK = TRUE;
						}
					}
					else
					{
						BUFF_BTN(Buffer_send);
						APP_Trame_OK(endpoint,Buffer_send);
					}
#ifdef _RAMPE_SOLAIRE
                    APP_Demande_Arret_SendCan = FALSE;
                    APP_Stop_Rampe  = FALSE;
                    APP_Can_stop_request = FALSE;
#endif

                }
                else
                {
                	//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	{
						if (0xFF != position_client)
						{
							BUFF_BTN(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_NOK = TRUE;
						}
					}
					else
					{
						BUFF_BTN(Buffer_send);
						APP_Trame_NOK(endpoint,Buffer_send);
					}
                }
            break;

            case WRD_STS:
/////////////////////////////////////////////////////////////////////////////////////
//
//     RECEPTION DE LA DEMANDE ETAT DES LEDS
//
////////////////////////////////////////////////////////////////////////////////////
                if ((TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx)) && (FALSE == APP_Config_bloc_vue))
                {
                	//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	{
						if (0xFF != position_client)
						{
							APP_Wifi_clients_tab[position_client].send_Sts = TRUE;
						}
					}
					else
					{
						BUFF_STS(Buffer_send);

						Buffer_send[4] = 0x00;
						Buffer_send[5] = 65;

						for (boucle=0;boucle<APP_TAB_MAX_ETAT_LED;boucle++)
						{
							Buffer_send[6+boucle] = APP_Tab_Etat_Leds[boucle];
						}

						Buffer_send[70] = FIN_CHAINE;
						P_L_Wifi_Tx_data(endpoint,71,Buffer_send);
					}
                }
                else
                {
                	//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
                	{
						if (0xFF != position_client)
						{
							BUFF_STS(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_NOK = TRUE;
						}
					}
					else
					{
						BUFF_STS(Buffer_send);
						APP_Trame_NOK(endpoint,Buffer_send);
					}
                }
            break;


            case WRD_BAT:
/////////////////////////////////////////////////////////////////////////////////////
//
//     RECEPTION DE LA DEMANDE ETAT DE LA BATTERIE
//
////////////////////////////////////////////////////////////////////////////////////
            	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
            	{
					if ((TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx)) && (FALSE == APP_Config_bloc_vue))
					{
						APP_Wifi_clients_tab[position_client].send_Bat = TRUE;
						APP_Manage_Reload_Wifi_BT_Timeout();
					}
					else
					{
						if (0xFF != position_client)
						{
							BUFF_BAT(APP_Wifi_clients_tab[position_client].data_ok_nok);
							APP_Wifi_clients_tab[position_client].send_NOK = TRUE;
						}
					}
            	}
            break;
/*
#ifdef _GESTION_ODOMETRE
/////////////////////////////////////////////////////////////////////////////////////
//
//     RECEPTION DES COMMANDES ODOMETRE
//
////////////////////////////////////////////////////////////////////////////////////
            case WRD_ODO:
                if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                {
                    switch(data[14])
                    {
                        case '0':
                            //Comptage/d�comptage
                            if ('o' == data[15])
                            {
                                P_L_Can_down_odometre_rx = TRUE;
                            }
                            else
                            {
                                P_L_Can_down_odometre_rx = FALSE;
                            }
                        break;

                        case '1':
                            //Stop comptage.
                            if ('o' == data[15])
                            {
                                P_L_Can_stop_odometre_rx = TRUE;
                            }
                            else
                            {
                                P_L_Can_stop_odometre_rx = FALSE;
                            }
                        break;

                         case '2':
                            //Reset comptage.
                            if ('t' == data[15])
                            {
                                P_L_Can_valeur_odometre_tx = 0;
                            }
                            else //if ('i' == data[15])
                            {
                                P_L_Can_interval_odometre_tx = 0;
                                P_L_Can_odometre_interval_100_memo = FALSE;
                            }
                            P_L_Can_odometre_chgt = TRUE;
                        break;

                        case '3':
                            //Set intervalle.
                            temp_calcul = ((U32)(data[15])*256*256);
                            temp_calcul = temp_calcul +((U32)(data[16])*256);
                            temp_calcul = temp_calcul + (U32)data[17];
                            P_L_Can_interval_odometre_rx = (F64)temp_calcul;
                            P_L_Can_interval_odometre_tx = 0;
                            P_L_Can_odometre_chgt = TRUE;
                            P_L_Can_odometre_interval_100_memo = FALSE;
                        break;

                        case '4':
                            //Set Distance totale.
                            temp_calcul = ((U32)(data[15])*256*256);
                            temp_calcul = temp_calcul +((U32)(data[16])*256);
                            temp_calcul = temp_calcul + (U32)data[17];
                            P_L_Can_valeur_odometre_tx = (F64)temp_calcul;
                            P_L_Can_odometre_chgt = TRUE;
                        break;

                        case '5':
                            //On/Off fonction odom�tre
                            //if ('o' == data[15])
                            //{
                            //    P_L_Can_on_off_odometre_rx = TRUE;
                            //}
                            //else
                            //{
                            //    P_L_Can_on_off_odometre_rx = FALSE;
                            //}

                        break;

                        case '6':
                            //Reset Flags.
                            // NU
                        break;

                        case '7':
                            //Valeur Etalonnage.
                            temp_calcul = ((U16)(data[15])*256);
                            temp_calcul = temp_calcul + (U16)data[16];
                            P_L_Can_etalonnage_odometre = (F64)temp_calcul;
                        break;

                        default:
                        break;
                    }
                    BUFF_ODO_REP(Buffer_send);
                    APP_Trame_OK(endpoint,Buffer_send);
                }
                else
                {
                    BUFF_ODO_REP(Buffer_send);
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_ODO_ASK:
                APP_Gestion_odometre_send(endpoint);
            break;
#endif
*/
/*
#ifdef _PROGRAMATION
            case WRD_FILE_OPEN_WR:
                BUFF_FILE_OPEN_WR_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        strncpy((char *)FileName,(const char *)&(data[14]),data_size_rx-APP_TAILLE_WIFI_UNLOCKEDKEY);

                        if (TRUE == APP_Open_WIFI_File(FileName,FA_WRITE))
                        {
                            APP_Trame_OK(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Trame_NOK(endpoint,Buffer_send);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_FILE_TRANSFERT_WR:
                BUFF_FILE_TRANSFERT_WR_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Cheksum(data,data_len,&data_size_rx))
                    {

                        if (TRUE == APP_Write_WIFI_File(&data[7],data_size_rx-2))
                        {
                            APP_Trame_Transfert_OK(endpoint,Buffer_send,data[6]);
                        }
                        else
                        {
                            APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                        }

                    }
                    else
                    {
                        APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                    }
                }
                else
                {
                    APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                }
            break;

            case WRD_FILE_CLOSE_WR:
                BUFF_FILE_CLOSE_WR_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {

                        if (TRUE == APP_Close_WIFI_File())
                        {
                            APP_Trame_OK(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Trame_NOK(endpoint,Buffer_send);
                        }

                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_FILE_OPEN_RD:
                BUFF_FILE_OPEN_RD_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        strncpy((char *)FileName,(const char *)&(data[14]),data_size_rx-APP_TAILLE_WIFI_UNLOCKEDKEY);

                        if (TRUE == APP_Open_WIFI_File(FileName,FA_READ))
                        {
                            APP_Trame_OK(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Trame_NOK(endpoint,Buffer_send);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_FILE_TRANSFERT_RD:
                BUFF_FILE_TRANSFERT_RD_REP(Buffer_send);
                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Cheksum(data,data_len,&data_size_rx))
                    {
                        if (TRUE == APP_Read_WIFI_File(data[6],&Buffer_send[9],&data_size_read))
                        {
                            if (0 < data_size_read)
                            {
                                Buffer_send[4] = 0x00;
                                Buffer_send[5] = (APP_TAILLE_TRANSFERT_STEP+3);
                                Buffer_send[6] = 'O';
                                Buffer_send[7] = 'K';
                                Buffer_send[8] = data[6];
                                Buffer_send[9+data_size_read] = FIN_CHAINE;
                                P_L_Wifi_Tx_data(endpoint,10+data_size_read,Buffer_send);
                            }
                            else
                            {
                               APP_Trame_Transfert_FIN(endpoint,Buffer_send,data[6]);
                            }
                        }
                        else
                        {
                            APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                        }
                    }
                    else
                    {
                        APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                    }
                }
                else
                {
                    APP_Trame_Transfert_NOK(endpoint,Buffer_send,data[6]);
                }
            break;

            case WRD_FILE_CLOSE_RD:
                BUFF_FILE_CLOSE_RD_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {

                        if (TRUE == APP_Close_WIFI_File())
                        {
                            APP_Trame_OK(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Trame_NOK(endpoint,Buffer_send);
                        }

                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_FLASHAGE:
                BUFF_FLASHAGE_REP(Buffer_send);

                if ((TRUE == P_L_CarteSd_Programation_vue)&&((APP_Flashage_state == FLASHAGE_WAIT_EVENT)||(APP_Flashage_state == FLASHAGE_REPROG_ALL_OK)||(APP_Flashage_state == FLASHAGE_REPROG_ALL_ERROR)))
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        APP_Flashage_time.Year = (data[14]*256)  + data[15];
                        APP_Flashage_time.Month = data[16];
                        APP_Flashage_time.Date = data[17];
                        APP_Flashage_time.Hours = data[18];
                        APP_Flashage_time.Minutes = data[19];
                        APP_Flashage_time.Seconds = data[20];

                        APP_Flashage_endpoint = endpoint;

                        APP_Flashage_OS_CFG = data[22];

                        if (0x00 == data[21])
                        {
                           APP_Flashage_reprog_all_module_ask = FALSE;
                           APP_Trame_ARRET(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Flashage_reprog_all_module_ask = TRUE;
                            APP_SendTrame_Flashage_RUNNING(0x00,0x00,0x00,0x00,0x00,0x00,0x00);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_LISTE_MODULE:
                BUFF_LISTE_MODULE_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        APP_Flashage_endpoint = endpoint;

                        if (0x00 == data[14])
                        {
                           APP_Flashage_liste_module_ask = FALSE;
                           APP_Trame_ARRET(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Flashage_liste_module_ask = TRUE;
                            APP_SendTrame_Liste_Module_RUNNING();
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_CHANGE_ADR:
                BUFF_CHANGE_ADR_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {


                        APP_Flashage_endpoint = endpoint;



                        APP_Adr_module_chgt_adr = (data[14]*1024) +(data[15]*512) + (data[16]*256)  + data[17];
                        APP_Adr_log_module_chgt_adr = data[18];

                        if (0x00 == data[19])
                        {
                           APP_Flashage_chgt_adr_module_ask = FALSE;
                           APP_Trame_ARRET(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Flashage_chgt_adr_module_ask = TRUE;
                            APP_SendTrame_Change_adresse_RUNNING(0);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_HISTORIQUE_MODULE:
                BUFF_HISTORIQUE_REP(Buffer_send);

                if ((TRUE == P_L_CarteSd_Programation_vue)&&((APP_Flashage_state == FLASHAGE_WAIT_EVENT)||(APP_Flashage_state == FLASHAGE_HISTORIQUE_OK)||(APP_Flashage_state == FLASHAGE_HISTORIQUE_ERROR)))
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        APP_Flashage_endpoint = endpoint;

                        APP_Adr_module_historique = (data[14]*1024) +(data[15]*512) + (data[16]*256)  + data[17];

                        if (0x00 == data[19])
                        {
                            APP_Full_historique = TRUE;
                        }
                        else
                        {
                           APP_Full_historique = FALSE;
                        }

                        if (0x00 == data[19])
                        {
                           APP_Flashage_historique_module_ask = FALSE;
                           APP_Trame_ARRET(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Flashage_historique_module_ask = TRUE;
                            APP_SendTrame_Historique_RUNNING(0x00);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;

            case WRD_HISTORIQUE_MODULE_ACQ:
                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Cheksum(data,data_len,&data_size_rx))
                    {
                        if (('R' == data[6]) && ('U' == data[7]) && ('N' == data[8]) && ('O' == data[9]) && ('K' == data[10]))
                        {
                            if ( data[11] == P_L_Wifi_send_historique)
                            {
                                if ((P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE * (P_L_Wifi_send_historique+1)) >= (P_L_CAN_TAILLE_1_HISTORIQUE*P_L_Can_Nb_historique) )
                                {
                                    P_L_Can_step_historique = CAN_END_READ_HISTORIQUE;
                                }
                                else
                                {
                                    P_L_Wifi_send_historique++;
                                }
                            }
                        }
                    }
                }
            break;

            case WRD_VERSION_FILE:
                BUFF_VERSION_FILE_REP(Buffer_send);

                if (TRUE == P_L_CarteSd_Programation_vue)
                {
                    if (TRUE == APP_Test_Trame_Format(data,data_len,position_client,&data_size_rx))
                    {
                        APP_Flashage_endpoint = endpoint;

                        if (0x00 == data[14])
                        {
                           APP_Flashage_version_ask = FALSE;
                           APP_Trame_ARRET(endpoint,Buffer_send);
                        }
                        else
                        {
                            APP_Flashage_version_ask = TRUE;
                            APP_SendTrame_Version_File_RUNNING(0,0);
                        }
                    }
                    else
                    {
                        APP_Trame_NOK(endpoint,Buffer_send);
                    }
                }
                else
                {
                    APP_Trame_NOK(endpoint,Buffer_send);
                }
            break;
#endif
*/
            default:
            break;
        }

    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_OK
 *  Description     : envoie trame data ok
 *
 *
 *******************************************************************************/
static void APP_Trame_OK(U8 endpoint,U8* Buffer_send)
{

    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x03;
    Buffer_send[6] = 'O';
    Buffer_send[7] = 'K';
    Buffer_send[8] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,9,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_NOK
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
static void APP_Trame_NOK(U8 endpoint,U8* Buffer_send)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'N';
    Buffer_send[7] = 'O';
    Buffer_send[8] = 'K';
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,10,Buffer_send);
}


/*******************************************************************************
 *
 *  Function Name   : APP_Trame_OK
 *  Description     : envoie trame data ok
 *
 *
 *******************************************************************************/
static void APP_Trame_OK_BroadCast(U8 endpoint,U8* Buffer_data)
{
U8 Buffer_send[9];

	Buffer_send[0] = Buffer_data[0];
	Buffer_send[1] = Buffer_data[1];
	Buffer_send[2] = Buffer_data[2];
	Buffer_send[3] = Buffer_data[3];
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x03;
    Buffer_send[6] = 'O';
    Buffer_send[7] = 'K';
    Buffer_send[8] = FIN_CHAINE;
    P_L_Wifi_Tx_dataDirect(endpoint,9,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_NOK
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
static void APP_Trame_NOK_BroadCast(U8 endpoint,U8* Buffer_data)
{
U8 Buffer_send[10];

	Buffer_send[0] = Buffer_data[0];
	Buffer_send[1] = Buffer_data[1];
	Buffer_send[2] = Buffer_data[2];
	Buffer_send[3] = Buffer_data[3];
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'N';
    Buffer_send[7] = 'O';
    Buffer_send[8] = 'K';
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_dataDirect(endpoint,10,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_RUNNING
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
 /*
static void APP_Trame_RUNNING(U8 endpoint,U8* Buffer_send)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,10,Buffer_send);
}
*/
/*******************************************************************************
 *
 *  Function Name   : APP_Trame_ARRET
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
static void APP_Trame_ARRET(U8 endpoint,U8* Buffer_send)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'S';
    Buffer_send[7] = 'T';
    Buffer_send[8] = 'P';
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,10,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_ARRET
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
static void APP_Trame_ERROR(U8 endpoint,U8* Buffer_send)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'E';
    Buffer_send[7] = 'R';
    Buffer_send[8] = 'O';
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,10,Buffer_send);
}

#ifdef _PROGRAMATION

#ifdef _TEST_ETHERNET
/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_CAN_ethernet
 *  Description     : envoie lzs donnee CAN en ethernet
 *
 *
 *******************************************************************************/
void APP_SendTrame_CAN_ethernet(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght)
{
U8 boucle;

    if ( ((p_l_can_msg_id & 0x400) == 0x400)&&((p_l_can_msg_id & 0x3FF) == P_L_Ethernet_module_adr_diag))
    {
        for (boucle=0;boucle<7;boucle++)
        {
            P_L_Ethernet_Module_Trame[p_l_can_data_ptr[0]][boucle] = p_l_can_data_ptr[boucle+1];
        }
    }
}


#endif


/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_CAN_wifi
 *  Description     : envoie lzs donnee CAN en wifi
 *
 *
 *******************************************************************************/
void APP_SendTrame_CAN_wifi(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght)
{
BOOL_TYPE client_ok;
U8 position_client;
U8 *ptr_Buffer;


    client_ok = APP_Manage_Wifi_Find_Client(P_L_Wifi_CAN_diag_endpoint,&position_client);

    if (TRUE == client_ok)
    {
        if ((p_l_can_msg_id & 0x400) == 0x400)
        {
            APP_Wifi_count_trame_can++;

            if (APP_Wifi_count_trame_can >= APP_Wifi_CAN_TRAME_MAX)
            {
                APP_Wifi_count_trame_can = 0;
                APP_Wifi_Send_BuffeSendCan[APP_Wifi_BuffeSendCan_write_num] = TRUE;
                APP_Wifi_BuffeSendCan_write_num++;
                if (APP_Wifi_BuffeSendCan_write_num >= APP_Wifi_CAN_NB_BUFFER)
                {
                    APP_Wifi_BuffeSendCan_write_num = 0;
                }

                ptr_Buffer = APP_Wifi_BuffeSendCan[APP_Wifi_BuffeSendCan_write_num];
                BUFF_CAN(ptr_Buffer);
                ptr_Buffer[4] = 0x00;
                ptr_Buffer[5] = APP_Wifi_CAN_TRAME_MAX*APP_Wifi_CAN_TAILLE_TRAME;
                APP_Wifi_BuffeSendCan_index = 6;

            }

            ptr_Buffer = APP_Wifi_BuffeSendCan[APP_Wifi_BuffeSendCan_write_num];


            ptr_Buffer[APP_Wifi_BuffeSendCan_index] = (U8)((p_l_can_msg_id >> 24) & 0x000000FF);
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+1] = (U8)((p_l_can_msg_id >> 16) & 0x000000FF);
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+2] = (U8)((p_l_can_msg_id >> 8) & 0x000000FF);
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+3] = (U8)(p_l_can_msg_id & 0x00FF);
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+4] =  p_l_can_lenght;
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+5] = p_l_can_data_ptr[0];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+6] = p_l_can_data_ptr[1];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+7] = p_l_can_data_ptr[2];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+8] = p_l_can_data_ptr[3];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+9] = p_l_can_data_ptr[4];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+10] = p_l_can_data_ptr[5];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+11] = p_l_can_data_ptr[6];
            ptr_Buffer[APP_Wifi_BuffeSendCan_index+12] = p_l_can_data_ptr[7];

            APP_Wifi_BuffeSendCan_index = APP_Wifi_BuffeSendCan_index + APP_Wifi_CAN_TAILLE_TRAME;
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Flashage_Erreur
 *  Description     : envoie trame flashage non ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Change_adresse_Error(void)
{
U8 	Buffer_send[20];

    BUFF_CHANGE_ADR_REP(Buffer_send);
    APP_Trame_ERROR(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Flashage_OK
 *  Description     : envoie trame flashage  ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Change_adresse_OK(void)
{
U8 	Buffer_send[20];

    BUFF_CHANGE_ADR_REP(Buffer_send);
    APP_Trame_OK(APP_Flashage_endpoint,Buffer_send);
}
/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Change_adresse_RUNNING
 *  Description     : envoie trame flashage en cours
 *
 *
 *******************************************************************************/
void APP_SendTrame_Change_adresse_RUNNING(U8 P_L_Can_step_adr_chgt)
{
U8 Buffer_send[15];

    BUFF_CHANGE_ADR_REP(Buffer_send);
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';

    Buffer_send[9] = P_L_Can_step_adr_chgt;

    Buffer_send[10] = FIN_CHAINE;
    P_L_Wifi_Tx_data(APP_Flashage_endpoint,11,Buffer_send);
}
/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Liste_Module_RUNNING
 *  Description     : envoie trame flashage en cours
 *
 *
 *******************************************************************************/
void APP_SendTrame_Liste_Module_RUNNING(void)
{
U8 boucle;
U8 Buffer_send[200];
U8 position;

    BUFF_LISTE_MODULE_REP(Buffer_send);
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';
    position = 9;
    for (boucle = 0; boucle<APP_NB_MAX_FLASHAGE_MODULE;boucle++)
    {
       if (ADR_NU != APP_Flashage_module[boucle].adresse)
       {
            Buffer_send[position] = (U8)((APP_Flashage_module[boucle].adresse >> 24) & 0x000000FF);
            Buffer_send[position+1] = (U8)((APP_Flashage_module[boucle].adresse >> 16) & 0x000000FF);
            Buffer_send[position+2] = (U8)((APP_Flashage_module[boucle].adresse >> 8) & 0x000000FF);
            Buffer_send[position+3] = (U8)(APP_Flashage_module[boucle].adresse & 0x00FF);
            position = position+4;
       }
    }

    Buffer_send[position] = FIN_CHAINE;
    P_L_Wifi_Tx_data(APP_Flashage_endpoint,position+1,Buffer_send);
}



/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Version_File_Error
 *  Description     : envoie trame version file non ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Version_File_Error(void)
{
U8 	Buffer_send[20];

    BUFF_VERSION_FILE_REP(Buffer_send);
    APP_Trame_ERROR(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Version_File_OK
 *  Description     : envoie trame version file  ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Version_File_OK(void)
{
U8 	Buffer_send[20];

    BUFF_VERSION_FILE_REP(Buffer_send);
    APP_Trame_OK(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Version_File_RUNNING
 *  Description     : envoie trame version file en cours
 *
 *
 *******************************************************************************/
void APP_SendTrame_Version_File_RUNNING(U8 file_encours,S8 nb_file)
{
U8 Buffer_send[20];

    BUFF_VERSION_FILE_REP(Buffer_send);

    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x06;
    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';
    Buffer_send[9] = file_encours;
    if (nb_file > 0)
    {
        Buffer_send[10] = nb_file;
    }
    else
    {
        Buffer_send[10] = 0;
    }
    Buffer_send[11] = FIN_CHAINE;

    P_L_Wifi_Tx_data(APP_Flashage_endpoint,12,Buffer_send);

}

/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Historique_Error
 *  Description     : envoie trame historique non ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Historique_Error(void)
{
U8 	Buffer_send[20];

    BUFF_HISTORIQUE_REP(Buffer_send);
    APP_Trame_ERROR(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Historique_OK
 *  Description     : envoie trame historique  ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Historique_OK(void)
{
U8 	Buffer_send[20];

    BUFF_HISTORIQUE_REP(Buffer_send);
    APP_Trame_OK(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Liste_Module_RUNNING
 *  Description     : envoie trame historique en cours
 *
 *
 *******************************************************************************/
void APP_SendTrame_Historique_RUNNING(U8 P_L_Can_step_historique)
{
U8 Buffer_send[P_L_TAILLE_SEND_MAX];
U8 boucle;
U8 index;
U16 offset;
U16 fin_boucle;

    index = P_L_Wifi_send_historique;

    BUFF_HISTORIQUE_REP(Buffer_send);

    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';

    Buffer_send[9] = P_L_Can_step_historique;

    if (CAN_SEND_HISTORIQUE == P_L_Can_step_historique)
    {
        Buffer_send[10] =  index;
        offset = index*P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE;

        if ((offset+P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE) >  P_L_CAN_TAILLE_BUFFER_HISTORIQUE)
        {
           fin_boucle = P_L_CAN_TAILLE_BUFFER_HISTORIQUE - (index*P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE);
        }
        else
        {
            fin_boucle = P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE;
        }

        for (boucle=0;boucle<fin_boucle;boucle++)
        {
            Buffer_send[11+boucle] = P_L_Can_received_historique[boucle+offset];
        }

        Buffer_send[11+fin_boucle] = FIN_CHAINE;
        Buffer_send[4] = 0x00;
        Buffer_send[5] = 0x05+0x01+fin_boucle;
        P_L_Wifi_Tx_data(APP_Flashage_endpoint,12+fin_boucle,Buffer_send);
    }
    else
    {
        P_L_Wifi_send_historique = 0;
        Buffer_send[10] = FIN_CHAINE;
        Buffer_send[4] = 0x00;
        Buffer_send[5] = 0x05;

        P_L_Wifi_Tx_data(APP_Flashage_endpoint,11,Buffer_send);
    }
}
/*******************************************************************************
 *
 *  Function Name   : APP_SendTrame_Flashage_RUNNING
 *  Description     : envoie trame flashage en cours
 *
 *
 *******************************************************************************/
void APP_SendTrame_Flashage_RUNNING(U8 APP_Rpg_all_module_step,U32 APP_Flashage_Adr_module_reprog,U8 APP_Module_reprog,U8 APP_Module_nb_reprog,U8 P_L_Can_step_can_reprog,U32 P_L_Can_donnes_en_cours,U32 P_L_Can_nb_donnee)
{
U8 	Buffer_send[35];

    BUFF_FLASHAGE_REP(Buffer_send);
    Buffer_send[4] = 0x00;
    Buffer_send[5] = 0x04;
    Buffer_send[6] = 'R';
    Buffer_send[7] = 'U';
    Buffer_send[8] = 'N';

    Buffer_send[9] = APP_Rpg_all_module_step;

    Buffer_send[10] = (U8)((APP_Flashage_Adr_module_reprog >> 24) & 0x000000FF);
    Buffer_send[11] = (U8)((APP_Flashage_Adr_module_reprog >> 16) & 0x000000FF);
    Buffer_send[12] = (U8)((APP_Flashage_Adr_module_reprog >> 8) & 0x000000FF);
    Buffer_send[13] = (U8)(APP_Flashage_Adr_module_reprog & 0x00FF);

    Buffer_send[14] = APP_Module_reprog;
    Buffer_send[15] = APP_Module_nb_reprog;
    Buffer_send[16] = P_L_Can_step_can_reprog;

    Buffer_send[17] = (U8)((P_L_Can_donnes_en_cours >> 24) & 0x000000FF);
    Buffer_send[18] = (U8)((P_L_Can_donnes_en_cours >> 16) & 0x000000FF);
    Buffer_send[19] = (U8)((P_L_Can_donnes_en_cours >> 8) & 0x000000FF);
    Buffer_send[20] = (U8)(P_L_Can_donnes_en_cours & 0x00FF);

    Buffer_send[21] = (U8)((P_L_Can_nb_donnee >> 24) & 0x000000FF);
    Buffer_send[22] = (U8)((P_L_Can_nb_donnee >> 16) & 0x000000FF);
    Buffer_send[23] = (U8)((P_L_Can_nb_donnee >> 8) & 0x000000FF);
    Buffer_send[24] = (U8)(P_L_Can_nb_donnee & 0x00FF);

    Buffer_send[25] = FIN_CHAINE;
    P_L_Wifi_Tx_data(APP_Flashage_endpoint,26,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Flashage_Erreur
 *  Description     : envoie trame flashage non ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Flashage_Error(void)
{
U8 	Buffer_send[20];

    BUFF_FLASHAGE_REP(Buffer_send);
    APP_Trame_ERROR(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Flashage_OK
 *  Description     : envoie trame flashage  ok
 *
 *
 *******************************************************************************/
void APP_SendTrame_Flashage_OK(void)
{
U8 	Buffer_send[20];

    BUFF_FLASHAGE_REP(Buffer_send);
    APP_Trame_OK(APP_Flashage_endpoint,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Transfert_OK
 *  Description     : envoie trame data ok
 *
 *
 *******************************************************************************/
static void APP_Trame_Transfert_OK(U8 endpoint,U8* Buffer_send,U8 etapes)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = (APP_TAILLE_TRANSFERT_STEP+3);
    Buffer_send[6] = 'O';
    Buffer_send[7] = 'K';
    Buffer_send[8] = etapes;
    Buffer_send[9] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,10,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Transfert_FIN
 *  Description     : envoie trame fin data
 *
 *
 *******************************************************************************/
static void APP_Trame_Transfert_FIN(U8 endpoint,U8* Buffer_send,U8 etapes)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = (APP_TAILLE_TRANSFERT_STEP+3);
    Buffer_send[6] = 'F';
    Buffer_send[7] = 'I';
    Buffer_send[8] = 'N';
    Buffer_send[9] = etapes;
    Buffer_send[10] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,11,Buffer_send);
}

/*******************************************************************************
 *
 *  Function Name   : APP_Trame_Transfert_NOK
 *  Description     : envoie trame data non ok
 *
 *
 *******************************************************************************/
static void APP_Trame_Transfert_NOK(U8 endpoint,U8* Buffer_send,U8 etapes)
{
    Buffer_send[4] = 0x00;
    Buffer_send[5] = (APP_TAILLE_TRANSFERT_STEP+3);
    Buffer_send[6] = 'N';
    Buffer_send[7] = 'O';
    Buffer_send[9] = 'K';
    Buffer_send[9] = etapes;
    Buffer_send[10] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,11,Buffer_send);
}
#endif

/*******************************************************************************
 *
 *  Function Name   : APP_Test_Trame_Format
 *  Description     : test si format de la trame et securite ok
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_Test_Trame_Format(U8* data,U16 data_len,U8 position_client,U16 * data_size_rx)
{
BOOL_TYPE result = FALSE;
BOOL_TYPE flag_key_ok = TRUE;
U16 data_size;
U8 boucle;

U8 cheksum_1;
U8 cheksum_2;

    data_size = (data[4] *256) + data[5];

    *data_size_rx = data_size;

    if (data_size + 4 + 2 + 2 == data_len) //data_size(data+unlockey) + keyword + taille + cheksum
    {
        if (TRUE == APP_Wifi_clients_tab[position_client].wifi_key_ask)
        {
            APP_Wifi_clients_tab[position_client].wifi_key_ask = FALSE;

            for (boucle=0;boucle<APP_TAILLE_WIFI_UNLOCKEDKEY;boucle++)
            {
                if (data[4+boucle+2] != APP_Wifi_clients_tab[position_client].wifi_Unlockkey[boucle])
                {
                   flag_key_ok = FALSE;
                    boucle = APP_TAILLE_WIFI_UNLOCKEDKEY;
                }
            }

            if (TRUE == flag_key_ok)
            {
                cheksum_1 = 0;
                cheksum_2 = 0;

                for (boucle=0;boucle<(data_size+2+4);boucle++) //data_size(data+unlockey) + taille + keyword
                {
                    cheksum_1 = cheksum_1 + data[boucle];
                    cheksum_2 = cheksum_2 + (data[boucle]*(boucle+1));
                }

                if ((cheksum_1 == data[data_size+4+2])&&(cheksum_2 == data[data_size+5+2]))
                {
                    result = TRUE;
                    APP_Wifi_clients_tab[position_client].gestion_com_ok = TRUE;
                }
            }
        }
    }
//ben ajout le 02/09/2022
#ifdef _RAMPE_SOLAIRE
    return TRUE;
#else
    return result;
#endif
}

/*******************************************************************************
 *
 *  Function Name   : APP_Test_Trame_Cheksum
 *  Description     : test si format de la trame et securite ok
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_Test_Trame_Cheksum(U8* data,U16 data_len,U16 * data_size_rx)
{
BOOL_TYPE result = FALSE;
U16 data_size;
U8 boucle;

U8 cheksum_1;
U8 cheksum_2;

    data_size = (data[4] *256) + data[5];

    *data_size_rx = data_size;

    if (data_size + 4 + 2 + 2 == data_len)
    {
        cheksum_1 = 0;
        cheksum_2 = 0;

        for (boucle=0;boucle<(data_size+2+4);boucle++)
        {
            cheksum_1 = cheksum_1 + data[boucle];
            cheksum_2 = cheksum_2 + (data[boucle]*(boucle+1));
        }

        if ((cheksum_1 == data[data_size+4+2])&&(cheksum_2 == data[data_size+5+2]))
        {
            result = TRUE;
        }

    }
    return result;
}
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Find_Client
 *  Description     : initialisation du wifi
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Manage_Wifi_Find_Client(U8 endpoint,U8 *return_position_client)
{
U8 position_client;
BOOL_TYPE client_found;

    client_found = FALSE;

	for (position_client=0;position_client<P_L_WIFI_MAX_CLIENTS;position_client++)
	{
		if(TRUE == APP_Wifi_clients_tab[position_client].used)
		{
			if (endpoint == APP_Wifi_clients_tab[position_client].endpoint)
			{
                *return_position_client = position_client;
				position_client = P_L_WIFI_MAX_CLIENTS;
				client_found = TRUE;
			}
		}
	}
	return client_found;
}


/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Find_Client
 *  Description     : initialisation du wifi
 *
 *
 *******************************************************************************/
static U8 APP_Manage_Wifi_Record_Client(U8 endpoint)
{
U8 position_client;
U8 return_position_client;

    return_position_client = 0xFF;

	for (position_client=0;position_client<P_L_WIFI_MAX_CLIENTS;position_client++)
	{
		if(FALSE == APP_Wifi_clients_tab[position_client].used)
		{
            APP_Wifi_clients_tab[position_client].used = TRUE;
			APP_Wifi_clients_tab[position_client].endpoint = endpoint;
			APP_Wifi_clients_tab[position_client].gestion_com_ok = FALSE;

#ifdef _RAMPE_AUTONOME
			if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
			{
				APP_Wifi_clients_tab[position_client].send_Sts = TRUE;
			}
			else
			{
				APP_Wifi_clients_tab[position_client].send_Sts = FALSE;
			}
#endif

			return_position_client = position_client;
            position_client = P_L_WIFI_MAX_CLIENTS;
		}
	}

	return return_position_client;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Find_Client_Leave
 *  Description     : initialisation du wifi
 *
 *
 *******************************************************************************/
void  APP_Manage_Wifi_Find_Client_Leave(U8 endpoint)
{
U8 position_client;

    if (TRUE == APP_Manage_Wifi_Find_Client(endpoint,&position_client))
    {
        APP_Wifi_clients_tab[position_client].used = FALSE;
        APP_Wifi_clients_tab[position_client].wifi_key_ask = FALSE;
        APP_Wifi_clients_tab[position_client].gestion_com_ok = FALSE;
    }
}


/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Client
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_Wifi_Client(void)
{
U8 position_client;
BOOL_TYPE result;

	for (position_client=0;position_client<P_L_WIFI_MAX_CLIENTS;position_client++)
	{
	    if (TRUE == APP_Wifi_clients_tab[position_client].wifi_key_ask)
        {
            BT_TIMER_LOCK(APP_Wifi_clients_tab[position_client].timer_key_ask,S_S_1MS_U32_TIME,APP_FILTRAGE_USE_KEY,APP_Wifi_clients_tab[position_client].lock_key_ask,result);
            if(TRUE == result)
            {
                APP_Wifi_clients_tab[position_client].wifi_key_ask = FALSE;
            }
        }

	}

    BT_TIMER_LOCK(APP_Timer_filtrage_perte_bloc_pmv,S_S_1MS_U32_TIME,APP_FILTRAGE_PERTE_BLOC_PMV_CAN,APP_Lock_filtrage_perte_bloc_pmv,result);
    if(TRUE == result)
    {
        APP_Can_PMV_Non_Vue = TRUE;
    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_Wifi_Data_Send
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
/*
void APP_Gestion_Wifi_Data_Send(void)
{
#ifdef _GESTION_ODOMETRE
U8 position_client;
    if (APP_MODE_ODOMETRE == APP_Mode_gestion_wifi_blt)
    {
        if ( (TRUE == P_L_Can_on_off_odometre_rx) || (P_L_Can_on_off_odometre_last != P_L_Can_on_off_odometre_rx) )
        {
            for (position_client=0;position_client<P_L_WIFI_MAX_CLIENTS;position_client++)
            {
                if ((TRUE == APP_Wifi_clients_tab[position_client].used) &&
                    (TRUE == APP_Wifi_clients_tab[position_client].gestion_com_ok))
                {
                    P_L_Can_on_off_odometre_last = P_L_Can_on_off_odometre_rx;
                    APP_Gestion_odometre_send(APP_Wifi_clients_tab[position_client].endpoint);
                }
            }
        }
}
#endif
}
*/

/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_odometre_send
 *  Description     :
 *
 *
 *******************************************************************************/
#ifdef _GESTION_ODOMETRE
static void APP_Gestion_odometre_send(U8 endpoint)
{
U64 odo_metre_tx;
U8 	Buffer_send[25];


    BUFF_ODO(Buffer_send);

    Buffer_send[4] = 0x00;
    Buffer_send[5] = 44;

    // valeur odometre
    odo_metre_tx = (U64)P_L_Can_valeur_odometre_tx;
    Buffer_send[8] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
    Buffer_send[7] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);
    Buffer_send[6] = (U8)((odo_metre_tx >> 16) &(U32) 0x000000FF);

    // interval odometre
    odo_metre_tx = (U64)P_L_Can_interval_odometre_tx;
    Buffer_send[11] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
    Buffer_send[10] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);
    Buffer_send[9] = (U8)((odo_metre_tx >> 16) &(U32) 0x000000FF);

    // valeur de  l'interval
    odo_metre_tx = (U64)P_L_Can_interval_odometre_rx;
    Buffer_send[14] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
    Buffer_send[13] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);
    Buffer_send[12] = (U8)((odo_metre_tx >> 16) &(U32) 0x000000FF);

    // Etalonage odometre
    odo_metre_tx = (U64)P_L_Can_etalonnage_odometre;
    Buffer_send[16] = (U8)(odo_metre_tx         &(U32) 0x000000FF);
    Buffer_send[15] = (U8)((odo_metre_tx >> 8)  &(U32) 0x000000FF);

    //flags
    if (TRUE == P_L_Can_odometre_interval_90)
    {
        Buffer_send[17]= 'o';
    }
    else
    {
        Buffer_send[17]= 'n';
    }

    if (TRUE == P_L_Can_odometre_interval_100)
    {
        Buffer_send[18]= 'o';
        P_L_Can_odometre_interval_100 = FALSE;
    }
    else
    {
        Buffer_send[18]= 'n';
    }

    if (TRUE == P_L_Can_on_off_odometre_rx)
    {
        Buffer_send[19]= 'o';
    }
    else
    {
        Buffer_send[19]= 'n';
    }

    if (TRUE == P_L_Can_down_odometre_rx)
    {
        Buffer_send[20]= 'o';
    }
    else
    {
        Buffer_send[20]= 'n';
    }

    if (TRUE == P_L_Can_stop_odometre_rx)
    {
        Buffer_send[21]= 'o';
    }
    else
    {
        Buffer_send[21]= 'n';
    }

    Buffer_send[22] = FIN_CHAINE;
    P_L_Wifi_Tx_data(endpoint,23,Buffer_send/*,TRUE*/);

}
#endif

#ifdef _RAMPE_SOLAIRE
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Perte_Tablette
 *  Description     : gestion de la perte de la tablette
 *
 *
 *******************************************************************************/
void APP_Manage_Perte_Tablette(void)
{
BOOL_TYPE result;
U8 boucle;
BOOL_TYPE lock_timeout;

    lock_timeout = FALSE;
    for (boucle=0;boucle<APP_NB_MAX_TOUCHE;boucle++)
	{
        if (TRUE == APP_Active_State_Analyse[boucle])
        {
            if ( 0x00 != (APP_Tab_Etat_Leds[boucle] & 0x7F))
            {
                lock_timeout = TRUE;
                boucle = APP_NB_MAX_TOUCHE;
            }
        }
	}

    if(FALSE == lock_timeout)
    {
        BT_TIMER_LOCK(APP_Timer_filtrage_perte_tablette,S_S_1MS_U32_TIME,APP_FILTRAGE_PERTE_TABLETTE,APP_Lock_filtrage_perte_tablette,result);
        if(TRUE == result)
        {
            APP_Stop_Rampe = TRUE;
            if (FALSE == APP_Demande_Arret_SendCan)
            {
                APP_Demande_Arret_SendCan = TRUE;
                P_L_Can_Demande_Arret_Bloc = TRUE;

                HWI_Led_1_Verte_Cmd(FALSE);
                HWI_Led_2_Verte_Cmd(FALSE);
                HWI_Led_1_Rouge_Cmd(FALSE);
                HWI_Led_2_Rouge_Cmd(FALSE);
            }
        }
    }
    else
    {
        BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_tablette,S_S_1MS_U32_TIME,APP_Lock_filtrage_perte_tablette);
    }
}
#endif

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_Wifi_Init(void)
{
U8 position_client;
U8 boucle;
const U8  *ptr_configU8;

	for (position_client=0;position_client<P_L_WIFI_MAX_CLIENTS;position_client++)
	{
	    APP_Wifi_clients_tab[position_client].used = FALSE;
	    APP_Wifi_clients_tab[position_client].wifi_key_ask = FALSE;
        APP_Wifi_clients_tab[position_client].gestion_com_ok = FALSE;
        APP_Wifi_clients_tab[position_client].send_Sts = FALSE;
        //APP_Wifi_clients_tab[position_client].send_Sts_Count = 0;

        APP_Wifi_clients_tab[position_client].send_Bip = FALSE;
        APP_Wifi_clients_tab[position_client].send_Key = FALSE;
        APP_Wifi_clients_tab[position_client].send_Bat = FALSE;
        APP_Wifi_clients_tab[position_client].send_OK = FALSE;
		APP_Wifi_clients_tab[position_client].send_NOK = FALSE;
	}

    for (boucle=0;boucle<APP_TAB_MAX_ETAT_LED;boucle++)
	{
        APP_Tab_Etat_Leds[boucle] = 0x00;
	}

    APP_Tab_Etat_Leds_Change = FALSE;
//    APP_Tab_Etat_Leds_Change_Count = 0;
//    APP_Tab_Etat_Leds_Change_CountMemo = 0;
    APP_Tab_Etat_Leds_Change_vue = FALSE;

	APP_Wifi_pmv_type = APP_MESSAGE_PMV_TYPE_NON_UTILISE;

    APP_Manage_OffsetPuissance = 0;
    APP_Manage_mirroir = FALSE;
    APP_Manage_mirroir_2 = FALSE;
    APP_Manage_reverse_motor = FALSE;
    APP_New_mess_rx = 0x00;
    APP_offset_Vitesse_message = (0xFF/2);
    APP_Num_message_tablette = 0x00;
    P_L_Can_on_off_odometre_last = FALSE;

    //gestion semaphore envoie wifi
    P_L_Wifi_Semaphore_Brocast = FALSE;
    P_L_Wifi_Semaphore_Brocast_memo = FALSE;
    P_L_Wifi_Semaphore_IT = FALSE;
    //valeur du bip en cours
    APP_request_buzzer[0x00] = 0x00;
    APP_request_buzzer[0x01] = 0x00;
    APP_request_buzzer_send[0x00] = 0x00;
    APP_request_buzzer_send[0x01] = 0x00;

    P_L_Wifi_client_Sts_ptr = 0;
    P_L_Wifi_client_Bip_ptr = 0;

    //etat flags indiquant un chagenet etat de bip
    APP_request_buzzer_Change = FALSE;

#ifdef _PROGRAMATION
    P_L_Wifi_send_historique = 0x00;
    P_L_Wifi_CAN_diag_on = FALSE;
#ifdef _TEST_ETHERNET
    P_L_Ethernet_Chgt_Module_vue = FALSE;
    P_L_Ethernet_Diag_Module = FALSE;
#endif
#endif

#ifdef _RAMPE_SOLAIRE
    BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_tablette,0x00,APP_Lock_filtrage_perte_tablette);
    APP_Stop_Rampe = FALSE;

    ptr_configU8 = (const U8 *)APP_TFT_TOUCHE;
    //recherche du bouton ON/OFF
    for (boucle=0;boucle<APP_NB_MAX_TOUCHE;boucle++)
	{
        if ((*ptr_configU8 == PICTO_TOUCHE_ON_OFF)||(*ptr_configU8 == PICTO_PAS_DE_TOUCHE)||(*ptr_configU8 == PICTO_TOUCHE_NU))
        {
            APP_Active_State_Analyse[boucle] = FALSE;

        }
        else
        {
            APP_Active_State_Analyse[boucle] = TRUE;
        }
	}

#endif

    S_S_1MS_U32_TIME_HIGH_PRIORITY = 0;
    APP_Wifi_connecter = FALSE;
    APP_Demande_Arret_Rampe_autonome_wifi = FALSE;
}

