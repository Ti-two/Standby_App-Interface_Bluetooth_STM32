/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "p_l_rx_can1.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_hal_can.h"
#include "p_l.h"
#include "lib_macros.h"
#include "p_l_tx_can1.h"
#include "hwi_hal_gpio.h"
#include "app_manage_mapping.h"
#include "app_manage_wifi.h"
#include "p_l_bx310x.h"
#include "lib_filter.h"
#include "p_l_rpg.h"
#include "app_manage_BT.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
// adresse globale de communication pour la reception des données sur le reseaux CAN
extern U32                                  APP_Adresse_module;

//validité de l'adresse globale
extern BOOL_TYPE                            APP_Adresse_can_valid;

//info boot
extern U8                                   P_L_Rpg_boot_demande[P_L_MAX_DATA_RAM_COM];
extern U8                                   P_L_Ram_bloc_adr_low;
extern U8                                   P_L_Ram_bloc_adr_high;
extern U8                                   P_L_Ram_bloc_adr_low2;
extern U8                                   P_L_Ram_bloc_adr_high2;
extern BOOL_TYPE                            P_L_Rpg_retour_boot;

extern BOOL_TYPE                           APP_Demande_Arret_SendCan;

//definit si la position du module dans la configuration
extern U8                                   APP_Position_module;

extern U32                                  S_S_1MS_U32_TIME;

extern BOOL_TYPE                            APP_Can_stop_perte_reseau;

//etat des leds des 2 premiers module de la config
extern U8                                   APP_Tab_Etat_Leds[APP_TAB_MAX_ETAT_LED];

//etat flags indiquant un chagenet etat de led
extern BOOL_TYPE                            APP_Tab_Etat_Leds_Change;
//extern U8                                   APP_Tab_Etat_Leds_Change_Count;

//valeur du bip en cours
extern U8                                   APP_request_buzzer[APP_TAB_MAX_ETAT_BIP];
//etat flags indiquant un chagenet etat de bip
extern BOOL_TYPE                            APP_request_buzzer_Change;

//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE           APP_Mode_gestion_wifi_blt;

//flag indiquant qu'une carte SD autorisant le flashage a ete vue
extern BOOL_TYPE                            P_L_CarteSd_Flashage_vue;

//flag indiquant qu'une carte SD autorisant le mode testeur
extern BOOL_TYPE                            P_L_CarteSd_Testeur_vue;

//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE                            P_L_CarteSd_Atester_vue;

extern hw_addr                              P_L_Wifi_Module_Mac_a_tester;
extern BOOL_TYPE                            P_L_Wifi_ssid_mac_a_tester_recue;
//flag indiquant qu'une carte SD autorisant la reprogramation de module
extern BOOL_TYPE                            P_L_CarteSd_Programation_vue;

#ifdef _RAMPE_SOLAIRE
extern U8                                   P_L_Can_Battery[2];
extern BOOL_TYPE                            APP_Stop_Rampe;
extern BOOL_TYPE                            P_L_Can_Demande_Arret_Bloc;
#endif

#ifdef _RAMPE_AUTONOME
extern U8      			   					P_L_BT_spp_mac_pair_rampe[P_L_BT_MAC_SIZE];
extern U8									APP_BT_can_input;
#endif

extern BOOL_TYPE							APP_Arret_Rampe_autonome_wifi;
extern BOOL_TYPE 							APP_Demande_Arret_Rampe_autonome_wifi;
/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
//demande envoie info soft
BOOL_TYPE                   P_L_Can_request_send_info;
//memorisation demande envoie info soft
BOOL_TYPE                   P_L_Can_request_send_info_last_request;
//demande d'arret du coeur d'analyse
BOOL_TYPE                   APP_Arret_analyse;
//demande d arret
BOOL_TYPE                   APP_Can_stop_request;

U32				            APP_Timer_filtrage_perte_reseau_can_stop;
BOOL_TYPE					APP_Lock_filtrage_perte_reseau_can_stop;

BOOL_TYPE                   HWI_Cmd_Led_2_verte;
BOOL_TYPE                   HWI_Cmd_Led_2_rouge;
BOOL_TYPE                   HWI_Cmd_Led_Flip;
U8                          HWI_Cmd_Led_Count;

BOOL_TYPE                   APP_Can_PMV_Non_Vue;
U32				            APP_Timer_filtrage_perte_bloc_pmv;
BOOL_TYPE					APP_Lock_filtrage_perte_bloc_pmv;

#ifdef _GESTION_ODOMETRE
//valeur odometre recue sur le can ext
F64                         P_L_Can_valeur_odometre_memo;
//valeur apc recue sur le can ext
BOOL_TYPE                   P_L_Can_valeur_apc_rx;
//memo valeur apc recue sur le can ext
BOOL_TYPE                   P_L_Can_valeur_apc_memo;
//filtrage changement apc
U32			                P_L_Can_Timer_filtrage_apc;
BOOL_TYPE	                P_L_Can_Lock_filtrage_apc;

//valeur marche arriere recue sur le can ext
BOOL_TYPE                   P_L_Can_valeur_marche_arriere_rx;

//demande initialisation de odometre
BOOL_TYPE                   P_L_Can_init_odometre;
//valeur odometre envoye sur le can mercura
F64                         P_L_Can_valeur_odometre_tx;
//interval odometre envoye sur le can mercura
F64                         P_L_Can_interval_odometre_tx;
//interval odometre recue sur le can mercura
F64                         P_L_Can_interval_odometre_rx;

//comptage decompatge odometre recue sur le can mercura
BOOL_TYPE                   P_L_Can_down_odometre_rx;
//arret temporaire de l odometre recue sur le can mercura
BOOL_TYPE                   P_L_Can_stop_odometre_rx;
//arret de la gestion de l odometre recue sur le can mercura
BOOL_TYPE                   P_L_Can_on_off_odometre_rx;

//memo de l arret de la gestion de l odometre recue sur le can mercura
BOOL_TYPE                   P_L_Can_on_off_odometre_memo;

//flag indiquant que l odometre a change de valeur
BOOL_TYPE                   P_L_Can_odometre_chgt;

//flag indiquant que l odometre 90% de interval atteind
BOOL_TYPE                   P_L_Can_odometre_interval_90;
//BOOL_TYPE   P_L_Can_odometre_interval_90_memo;

//flag indiquant que l odometre 100% de interval atteind
BOOL_TYPE                   P_L_Can_odometre_interval_100;
BOOL_TYPE                   P_L_Can_odometre_interval_100_memo;

F64                         P_L_Can_valeur_odometre_rx;

//etat du frein a main recue sur le can est en gestion antivol
BOOL_TYPE                   P_L_Can_Antivol_frein_a_main;
//etat du moteur recue sur le can est en gestion antivol
BOOL_TYPE                   P_L_Can_Antivol_moteur_on;

//valeur etalonnage odometre
F64                         P_L_Can_etalonnage_odometre;
#endif

U32				            APP_Timer_filtrage_perte_reseau_can_led;
BOOL_TYPE					APP_Lock_filtrage_perte_reseau_can_led;



//Numero du module qui active la fonction PA (0xFF = pas actif)
U8                           APP_Can_module_PA;
//Numero du module qui emets le message (0xFF = pas de message)
U8                           APP_Can_module_emission;
//Mode du message (0x00 = lecture, 0x01 = ecriture)
U8                           APP_Can_emission_mode;
//Mumero du message a emettre (0x00 = pas de message)
U8                           APP_Can_emission_message;
//Numero du module qui diffuse le son (0xFF = pas de diffusion)
U8                           APP_Can_module_diffusion;

U16					    	 APP_Tension_batterie[0x02];

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE */

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
 /*******************************************************************************
*
*  Function Name   :  P_L_Can_Msg
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : gestion des messages can hors ccp
*******************************************************************************/
U32 P_L_Can_rx_count = 0;

void P_L_Can_1_rx_frame(U32 p_l_can_msg_id,U8* p_l_can_data_ptr,  U8 p_l_can_lenght)
{
U16 p_l_bloc_adresse;
const U32   *ptr_configU32;
const U32   *ptr_configU32Bis;
const U32   *ptr_configU32Ter;
const U8    *ptr_configU8;
const U8    *ptr_configU8Bis;
U8  		offset;
U8 			boucle;
U8          resultU8;
BOOL_TYPE   led_change = FALSE;
U32 		type_module;
U8 			nb_voyant;
U8			position;

#ifdef _GESTION_ODOMETRE
BOOL_TYPE        up_down;
BOOL_TYPE        res;
F64              ecart_odometre_rx;
F64              calcul_odometre;
F64              calcul_interval;
F64              valeur_max_odometre_rx;
F64              temp_90;
#endif

	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
	{


		if ((APP_Can_stop_request == TRUE)||(APP_Demande_Arret_SendCan == TRUE))
		{

			P_L_Can_rx_count++;

			if (P_L_Can_rx_count > 200)
			{
				APP_Demande_Arret_SendCan = FALSE;
				APP_Can_stop_request = FALSE;
			}
		}

		if ((APP_Can_stop_request == FALSE)&&(APP_Demande_Arret_SendCan == FALSE))
		{
			P_L_Can_rx_count = 0;
		}

	}
	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
	{

		if ((APP_Arret_Rampe_autonome_wifi == FALSE)&&(APP_Demande_Arret_Rampe_autonome_wifi == FALSE))
		{
			if ((APP_Can_stop_request == TRUE)||(APP_Demande_Arret_SendCan == TRUE))
			{

				P_L_Can_rx_count++;

				if (P_L_Can_rx_count > 200)
				{
					APP_Demande_Arret_SendCan = FALSE;
					APP_Can_stop_request = FALSE;
				}
			}

			if ((APP_Can_stop_request == FALSE)&&(APP_Demande_Arret_SendCan == FALSE))
			{
				P_L_Can_rx_count = 0;
			}
		}

	}



#ifdef _RAMPE_SOLAIRE
	if (TRUE == APP_Stop_Rampe)
	{
		P_L_Can_Demande_Arret_Bloc = TRUE;
	}
#endif

	if (PL_CAN_ID_CMD == p_l_can_data_ptr[0])
	{
		P_L_CarteSd_Testeur_vue = FALSE;
	}


	if(TRUE == P_L_CarteSd_Testeur_vue)
	{
		if(0x06 == (U16)p_l_can_msg_id)
		{
			P_L_Wifi_Module_Mac_a_tester.addr[0] = p_l_can_data_ptr[0];
			P_L_Wifi_Module_Mac_a_tester.addr[1] = p_l_can_data_ptr[1];
			P_L_Wifi_Module_Mac_a_tester.addr[2] = p_l_can_data_ptr[2];
			P_L_Wifi_Module_Mac_a_tester.addr[3] = p_l_can_data_ptr[3];
			P_L_Wifi_Module_Mac_a_tester.addr[4] = p_l_can_data_ptr[4];
			P_L_Wifi_Module_Mac_a_tester.addr[5] = p_l_can_data_ptr[5];

			P_L_Wifi_ssid_mac_a_tester_recue = TRUE;
		}
	}
	else if(TRUE == P_L_CarteSd_Atester_vue)
	{
		if (FALSE == HWI_Cmd_Led_Flip)
		{

			HWI_Cmd_Led_Count++;
			if (HWI_Cmd_Led_Count >= 10)
			{
				HWI_Cmd_Led_Flip = TRUE;
				HWI_Cmd_Led_Count = 0x00;
				HWI_Cmd_Led_2_verte = FALSE;
				HWI_Led_2_Verte_Cmd(HWI_Cmd_Led_2_verte);
			}
			else
			{
				HWI_Led_2_Verte_Cmd(HWI_Cmd_Led_2_verte);
				HWI_Cmd_Led_2_verte = !HWI_Cmd_Led_2_verte;
			}
		}
		else
		{

			HWI_Cmd_Led_Count++;
			if (HWI_Cmd_Led_Count >= 10)
			{
				HWI_Cmd_Led_Flip = FALSE;
				HWI_Cmd_Led_Count = 0x00;
				HWI_Cmd_Led_2_rouge = FALSE;
				HWI_Led_2_Rouge_Cmd(HWI_Cmd_Led_2_rouge);
			}
			else
			{
				HWI_Led_2_Rouge_Cmd(HWI_Cmd_Led_2_rouge);
				HWI_Cmd_Led_2_rouge = !HWI_Cmd_Led_2_rouge;
			}
		}

#ifdef _VERIN_CAN
		if (0x7FF < p_l_can_msg_id)
		{

		}
		else
#endif
		if(P_L_CC_ID_REPROG == (U16)p_l_can_msg_id)
		{
			// Analyse de l'adresse du bloc à flahser
			if((0x00 == p_l_can_data_ptr[0]) && (P_L_CC_RPG_BOOT_JUMP_RQ == p_l_can_data_ptr[1]))
			{
				p_l_bloc_adresse = ((U16)(p_l_can_data_ptr[2])*256);
				p_l_bloc_adresse =  p_l_bloc_adresse + (U16)p_l_can_data_ptr[3];


				if(p_l_bloc_adresse == APP_Adresse_module)
				{
					P_L_Rpg_boot_demande[0] = P_L_SERVICE_RETOUR_APL_BOOT;
					P_L_Ram_bloc_adr_low = (APP_Adresse_module & 0x000000FF);
					P_L_Ram_bloc_adr_high = ((APP_Adresse_module >> 8) & 0x000000FF);
					P_L_Ram_bloc_adr_low2 = ((APP_Adresse_module >> 16) & 0x000000FF);
					P_L_Ram_bloc_adr_high2 = ((APP_Adresse_module >> 24) & 0x000000FF);
					P_L_Rpg_retour_boot = TRUE;

				}
			}
		}
		else
		{
			if (PL_CAN_ID_CMD == p_l_can_data_ptr[0])
			{
				// demande gestion des dfonnées du noeud
				if (P_L_Can_request_send_info_last_request != (BOOL_TYPE)(p_l_can_data_ptr[1]&PL_INFO_SYS))
				{
					P_L_Can_request_send_info = TRUE;
				}
				else
				{
					// do nothing
				}

				if (PL_ON_OFF_SYS == (p_l_can_data_ptr[1] & PL_ON_OFF_SYS))
				{ //demande d'arret du systeme
					APP_Can_stop_request = TRUE;
				}
				else
				{//demande d arret de l'arret du systeme
					APP_Can_stop_request = FALSE;
				}
			}
		}
	}
	else
	{

		BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_reseau_can_stop,S_S_1MS_U32_TIME,APP_Lock_filtrage_perte_reseau_can_stop);
		BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_reseau_can_led,S_S_1MS_U32_TIME,APP_Lock_filtrage_perte_reseau_can_led);

		HWI_Led_2_Verte_Cmd(HWI_Cmd_Led_2_verte);
		HWI_Cmd_Led_2_verte = !HWI_Cmd_Led_2_verte;

		ptr_configU32 = (const U32 *)APP_LISTE_FACADE_CFG_FACADE;

		if ((APP_ADR_NOEUD_CAN_BLOC_PMV+1) == (p_l_can_msg_id&P_L_CC_MASK_ID))
		{
			BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_bloc_pmv,S_S_1MS_U32_TIME,APP_Lock_filtrage_perte_bloc_pmv);
			APP_Can_PMV_Non_Vue = FALSE;
		}

		if(P_L_CC_ID_REPROG == (U16)p_l_can_msg_id)
		{
			// Analyse de l'adresse du bloc à flahser
			if((0x00 == p_l_can_data_ptr[0]) && (P_L_CC_RPG_BOOT_JUMP_RQ == p_l_can_data_ptr[1]))
			{
				p_l_bloc_adresse = ((U16)(p_l_can_data_ptr[2])*256);
				p_l_bloc_adresse =  p_l_bloc_adresse + (U16)p_l_can_data_ptr[3];


				if(p_l_bloc_adresse == APP_Adresse_module)
				{
						P_L_Rpg_boot_demande[0] = P_L_SERVICE_RETOUR_APL_BOOT;
						P_L_Ram_bloc_adr_low = (APP_Adresse_module & 0x000000FF);
						P_L_Ram_bloc_adr_high = ((APP_Adresse_module >> 8) & 0x000000FF);
						P_L_Ram_bloc_adr_low2 = ((APP_Adresse_module >> 16) & 0x000000FF);
						P_L_Ram_bloc_adr_high2 = ((APP_Adresse_module >> 24) & 0x000000FF);
						P_L_Rpg_retour_boot = TRUE;

				}
			}
		}
		else if ((APP_MODE_BTN_ET_MESSAGE_PMV == APP_Mode_gestion_wifi_blt)||(APP_MODE_MESSAGE_PMV == APP_Mode_gestion_wifi_blt)||(APP_MODE_AUDIO == APP_Mode_gestion_wifi_blt))
		{

			if(P_L_CC_ID_DATA_AUTRE == (U16)p_l_can_msg_id)
			{
				switch(p_l_can_data_ptr[0])
				{
					case PL_CAN_ID_AIGUILLAGE_AUDIO:
						//octet 2 = Numero du module qui active la fonction PA
						APP_Can_module_PA = p_l_can_data_ptr[1];
						//octet 3 = Numero du module qui diffuse le son
						APP_Can_module_diffusion = p_l_can_data_ptr[2];
						//octet 4 = Numero du module qui emets le message
						APP_Can_module_emission = p_l_can_data_ptr[3];
						//octet 5 = Mode du message
						APP_Can_emission_mode = p_l_can_data_ptr[4];
						//octet 6 = Numero du message a emettre
						APP_Can_emission_message = p_l_can_data_ptr[5];
					  break;

					  default:
					  break;
				}

			}
			else if ( (*ptr_configU32 == p_l_can_msg_id) || (*(ptr_configU32+1) == p_l_can_msg_id) )
			{


				if (PL_CAN_ID_BIP == p_l_can_data_ptr[0])
				{// gestion du buzzer

					if (*ptr_configU32 == p_l_can_msg_id)
					{
						offset = 0;
					}
					else
					{
						offset = 1;
					}

					if (APP_request_buzzer[offset] != p_l_can_data_ptr[1])
					{
						APP_request_buzzer[offset] = p_l_can_data_ptr[1];
						APP_request_buzzer_Change = TRUE;
					}

				}
				else if ((PL_CAN_ID_LED1 <= p_l_can_data_ptr[0])&&(PL_CAN_ID_LED6 >= p_l_can_data_ptr[0]))
				{// gestion des leds

					if (*ptr_configU32 == p_l_can_msg_id)
					{
						offset = 0;
					}
					else
					{
					   offset = APP_NB_MAX_TOUCHE;
					}

					offset = offset  + (7 * (p_l_can_data_ptr[0] - PL_CAN_ID_LED1));

					for (boucle = 0;boucle<7;boucle++)
					{
						if (APP_Tab_Etat_Leds[offset+boucle] != p_l_can_data_ptr[boucle+1])
						{
							APP_Tab_Etat_Leds[offset+boucle] = p_l_can_data_ptr[boucle+1];
							led_change = TRUE;
							//APP_Tab_Etat_Leds_Change_Count++;
						}
					}

					if (TRUE == led_change)
					{
						APP_Tab_Etat_Leds_Change = TRUE;
					}
				}
				else if (PL_CAN_ID_LEDPWM == p_l_can_data_ptr[0])
				{// gestion rapport PWM pour les leds

					//P_L_Can_Consigne_Led_pwm = p_l_can_data_ptr[1];
				}
			}
			else
			{
				if (PL_CAN_ID_CMD == p_l_can_data_ptr[0])
				{
					// demande gestion des dfonnées du noeud
					if (P_L_Can_request_send_info_last_request != (BOOL_TYPE)(p_l_can_data_ptr[1]&PL_INFO_SYS))
					{
						P_L_Can_request_send_info = TRUE;
					}
					else
					{
						// do nothing
					}

					 //marche/arret coeur analyse
					if (PL_ON_OFF_ANALYSE == (p_l_can_data_ptr[1] & PL_ON_OFF_ANALYSE))
					{ //demande d'arret du coeur d'analyse
						APP_Arret_analyse = TRUE;
					}
					else
					{//demande de mise en route du coeur d'analyse
						APP_Arret_analyse = FALSE;
					}


					if (PL_ON_OFF_SYS == (p_l_can_data_ptr[1] & PL_ON_OFF_SYS))
					{ //demande d'arret du systeme
						APP_Can_stop_request = TRUE;
					}
					else
					{//demande d arret de l'arret du systeme
						APP_Can_stop_request = FALSE;
					}
				}
#ifdef _RAMPE_SOLAIRE
				else if (PL_CAN_ID_INFO_COEUR == p_l_can_data_ptr[0])
				{
					if (PL_CAN_INFO_1 == p_l_can_data_ptr[1])
					{
						P_L_Can_Battery[0] = p_l_can_data_ptr[3];
						P_L_Can_Battery[1] = p_l_can_data_ptr[4];
					}
				}
#endif
			}
		}
#ifdef _GESTION_ODOMETRE
		else if (APP_MODE_ODOMETRE == APP_Mode_gestion_wifi_blt)
		{
		///////////////////////
		//gestion odometre
		//////////////////////
			ptr_configU32 = (const U32 *)APP_TABLEAU_ODOMETRE_CAN_ID;
			ptr_configU32Bis = (const U32 *)APP_CAN_ID_BLOC_ODOMETRE;
			if ((*ptr_configU32 != (U32)0xFFFFFFFF)&&(*ptr_configU32Bis == APP_Adresse_module))
			{
				//test si la fonction odometre est active
				if (TRUE == P_L_Can_on_off_odometre_rx)
				{
					if (p_l_can_msg_id == *ptr_configU32)
					{
						ptr_configU8 = (const U8 *)APP_TABLEAU_ODOMETRE_CAN_BIT_DEPART;
						ptr_configU8Bis = (const U8 *)APP_TABLEAU_ODOMETRE_CAN_TAILLE_DONNEE;

						P_L_Can_valeur_odometre_rx = (F64)ExtractData(p_l_can_data_ptr,*ptr_configU8,*ptr_configU8Bis);
					}

					ptr_configU32 =(const U32 *)APP_TABLEAU_LOGIC_CAN_ID_APC;
					if (p_l_can_msg_id == *ptr_configU32)
					{
						ptr_configU8 = (const U8 *)APP_TABLEAU_LOGIC_CAN_BIT_DEPART_APC;
						ptr_configU8Bis = (const U8 *)APP_TABLEAU_LOGIC_CAN_TAILLE_DONNEE_APC;

						resultU8 = (U8)ExtractData(p_l_can_data_ptr,*ptr_configU8,*ptr_configU8Bis);

						ptr_configU8 = (const U8 *)APP_TABLEAU_LOGIC_CAN_ETAT_ON_APC;
						ptr_configU8Bis = (const U8 *)APP_TABLEAU_LOGIC_CAN_ETAT_OFF_APC;
						if (*ptr_configU8 == resultU8)
						{
							P_L_Can_valeur_apc_rx = TRUE;
						}
						else if (*ptr_configU8Bis == resultU8)
						{
							P_L_Can_valeur_apc_rx = FALSE;
						}
					}

					ptr_configU32 = (const U32 *)APP_TABLEAU_LOGIC_CAN_ID_MARRIERE;
					if (p_l_can_msg_id == *ptr_configU32)
					{
						ptr_configU8 = (const U8 *)APP_TABLEAU_LOGIC_CAN_BIT_DEPART_MARRIERE;
						ptr_configU8Bis = (const U8 *)APP_TABLEAU_LOGIC_CAN_TAILLE_DONNEE_MARRIERE;

						resultU8 = (U8)ExtractData(p_l_can_data_ptr,*ptr_configU8,*ptr_configU8Bis);

						ptr_configU8 = (const U8 *)APP_TABLEAU_LOGIC_CAN_ETAT_ON_MARRIERE;
						ptr_configU8Bis = (const U8 *)APP_TABLEAU_LOGIC_CAN_ETAT_OFF_MARRIERE;
						if (*ptr_configU8 == resultU8)
						{
							P_L_Can_valeur_marche_arriere_rx = TRUE;
						}
						else if (*ptr_configU8Bis == resultU8)
						{
							P_L_Can_valeur_marche_arriere_rx = FALSE;
						}
					}

					//test si on vient de mettre en route la fonction odometre
					if (FALSE == P_L_Can_on_off_odometre_memo)
					{//on vient de mettre en route odometre
						P_L_Can_on_off_odometre_memo = P_L_Can_on_off_odometre_rx;

						//valeur odometre envoye sur le can mercura
						P_L_Can_valeur_odometre_tx = 0;
						//interval odometre envoye sur le can mercura
						P_L_Can_interval_odometre_tx = 0;
						//demande initialisation de la valeur recue quand le +apc sera actif
						P_L_Can_init_odometre = TRUE;
						//mise a jour des valeurs est demande envoie
						P_L_Can_odometre_chgt = TRUE;
					}

					//test si le + apc est actif
					if (TRUE == P_L_Can_valeur_apc_rx)
					{//+apc actif

						//test retournement de compteur xxxx -> FFFF puis 0000 au démarrage moteur
						if (FALSE == P_L_Can_valeur_apc_memo)
						{//le +apc n etait pas actif avant
							P_L_Can_valeur_apc_memo = P_L_Can_valeur_apc_rx;
							//filtrage du retournement de compteur sur +apc
							BT_TIMER_INIT_LOCK(P_L_Can_Timer_filtrage_apc,S_S_1MS_U32_TIME,P_L_Can_Lock_filtrage_apc);
							//a l apparition du +apc la valeur de odometre recue retombe a zero
							//arret de la demande initialisation de la valeur recue quand le +apc sera actif
							P_L_Can_init_odometre = TRUE;
							//remise a zero du compteur odometrique
							P_L_Can_valeur_odometre_memo = 0;
						}

						BT_TIMER_LOCK(P_L_Can_Timer_filtrage_apc,S_S_1MS_U32_TIME,APP_FILTRAGE_APC_ODOMETRE,P_L_Can_Lock_filtrage_apc,res);
						if(TRUE == res)
						{// filtrage +apc ecoule
							//test si demande init car mise en route de la fonction

							if (TRUE == P_L_Can_init_odometre)
							{
								P_L_Can_init_odometre = FALSE;
								P_L_Can_valeur_odometre_memo = P_L_Can_valeur_odometre_rx;
							}


							//calcul de ecart
							if (P_L_Can_valeur_odometre_memo <= P_L_Can_valeur_odometre_rx)
							{
								ecart_odometre_rx = P_L_Can_valeur_odometre_rx - P_L_Can_valeur_odometre_memo;
							}
							else
							{
								//exemple taille 1 octets -> APP_TABLEAU_ODOMETRE_CAN_TAILLE_DONNEE = 8
								//on decale 1 de 8 sur la gauche 1 << 8 = 0x100; on soustrait 1 -> 0xFF;
								ptr_configU8 = (const U8 *)APP_TABLEAU_ODOMETRE_CAN_TAILLE_DONNEE;
								valeur_max_odometre_rx = (1 << (U8)*ptr_configU8)-1;
								ecart_odometre_rx = (valeur_max_odometre_rx - P_L_Can_valeur_odometre_memo) + P_L_Can_valeur_odometre_rx;
							}
							//ben le 19/05/2017
							ptr_configU32 = (const U32 *)APP_TABLEAU_ODOMETRE_CAN_COEF_A;
							ptr_configU32Bis = (const U32 *)APP_TABLEAU_ODOMETRE_CAN_COEF_B;
							ptr_configU32Ter = (const U32 *)APP_TABLEAU_ODOMETRE_CAN_COEF_C;
							ecart_odometre_rx = (F64)((((F64)*ptr_configU32 * ecart_odometre_rx) + (F64)*ptr_configU32Bis)/(F64)*ptr_configU32Ter);
							//ben le 03/10/2017
							ecart_odometre_rx = ecart_odometre_rx * (F64)1000;
							ecart_odometre_rx = ecart_odometre_rx / P_L_Can_etalonnage_odometre;



							//memorisation de la valeur recue
							P_L_Can_valeur_odometre_memo = P_L_Can_valeur_odometre_rx;

							//test si l on doit metre a jour la valeur
							if (FALSE == P_L_Can_stop_odometre_rx)
							{   //test du sens comptage / decomptage
								if (FALSE == P_L_Can_down_odometre_rx)
								{//comptage
									//test de la marche arriere
									if (FALSE == P_L_Can_valeur_marche_arriere_rx)
									{//comptage
										up_down = TRUE;
									}
									else
									{//decomptage
										up_down = FALSE;
									}
								}
								else
								{//decomptage
									//test de la marche arriere
									if (FALSE == P_L_Can_valeur_marche_arriere_rx)
									{//decomptage
										up_down = FALSE;
									}
									else
									{//comptage
										up_down = TRUE;
									}
								}
								if (TRUE == up_down)
								{//comptage
									//compteur odometre
									calcul_odometre = P_L_Can_valeur_odometre_tx+ecart_odometre_rx;
									//test du retournement du compteur limite a APP_VALEUR_MAX_ODOMETRE
									if (calcul_odometre > APP_VALEUR_MAX_ODOMETRE)
									{
									   calcul_odometre = calcul_odometre - (APP_VALEUR_MAX_ODOMETRE+1);
									}

									//interval odometre
									//test si fonction interval est active
									if (0 != P_L_Can_interval_odometre_rx)
									{
										calcul_interval = P_L_Can_interval_odometre_tx+ecart_odometre_rx;

										//test du retournement du compteur limite a P_L_Can_interval_odometre_rx
										//interval odometre depassé
										if (calcul_interval > P_L_Can_interval_odometre_rx)
										{   //on sosutrait la valeur atteinte a la valeur courante
											calcul_interval = calcul_interval - (P_L_Can_interval_odometre_rx+1);
											//on test si on a deja positione le flag 100% atteind
											if (FALSE == P_L_Can_odometre_interval_100_memo)
											{//non
												//on set le falg 100% atteind
												P_L_Can_odometre_interval_100 = TRUE;
											}
											//on reset les flags de memorisation
											P_L_Can_odometre_interval_100_memo = FALSE;
										}
										//interval odometre atteind
										else if (calcul_interval == P_L_Can_interval_odometre_rx)
										{
											//on set le falg 100% atteind
											P_L_Can_odometre_interval_100 = TRUE;
											//on set le flag de memorisation 100%
											P_L_Can_odometre_interval_100_memo = TRUE;
										}
									}

								}
								else
								{//decomptage
									//compteur odometre
									calcul_odometre = P_L_Can_valeur_odometre_tx-ecart_odometre_rx;

									//test du retournement du compteur limite a APP_VALEUR_MAX_ODOMETRE
									if (calcul_odometre < 0)
									{
										calcul_odometre = APP_VALEUR_MAX_ODOMETRE + calcul_odometre;
									}

									//interval odometre
									//test si fonction interval est active
									if (0 != P_L_Can_interval_odometre_rx)
									{
										calcul_interval = P_L_Can_interval_odometre_tx-ecart_odometre_rx;
										//test du retournement du compteur limite a P_L_Can_interval_odometre_rx
										//interval odometre depassé

										if (calcul_interval < 0)
										{
											calcul_interval = P_L_Can_interval_odometre_rx+calcul_interval;

											//on test si on a deja positione le flag 100% atteind
											if (FALSE == P_L_Can_odometre_interval_100_memo)
											{//non
												//on set le falg 100% atteind
												P_L_Can_odometre_interval_100 = TRUE;
											}
											//on reset les flags de memorisation
											P_L_Can_odometre_interval_100_memo = FALSE;
										}
										//interval odometre atteind
										else if (calcul_interval == P_L_Can_interval_odometre_rx)
										{
											//on set le falg 100% atteind
											P_L_Can_odometre_interval_100 = TRUE;
											//on set le flag de memorisation 100%
											P_L_Can_odometre_interval_100_memo = TRUE;
										}
									}
								}

								//mise a jour des valeurs est demande envoie
								//mise a jour de la valeur odometrique
								P_L_Can_valeur_odometre_tx = calcul_odometre;
								//test si fonction interval est active
								if (0 != P_L_Can_interval_odometre_rx)
								{//oui
									//mise a jour de la valeur interval
									P_L_Can_interval_odometre_tx = calcul_interval;
								}
								else
								{//non
									//reset des flags alarme est des valeurs
									P_L_Can_interval_odometre_tx = 0;
									P_L_Can_odometre_interval_90 = FALSE;
									P_L_Can_odometre_interval_100 = FALSE;
									P_L_Can_odometre_interval_100_memo = FALSE;
								}


								//test si fonction interval est active est valeur a 90% pas deja atteinte
								if (0 !=P_L_Can_interval_odometre_rx)
								{
									//calcul des 90%
									temp_90 = P_L_Can_interval_odometre_tx*100;
									temp_90 = temp_90/P_L_Can_interval_odometre_rx;

									//test si on est supperieur ou egale a 90% est en comptage
									if ((temp_90 >= 90)&&(TRUE == up_down))
									{//oui
										//on set le flag de memorisation 90%
										P_L_Can_odometre_interval_90 = TRUE;
									}
									//test si on est inferrieur ou egale a 10% est en decomptage
									else if ((temp_90 <= 10)&&(FALSE == up_down))
									{//oui
										//on set le flag de memorisation 90%
										P_L_Can_odometre_interval_90 = TRUE;
									}
									//sinon reset du flag
									else
									{
										P_L_Can_odometre_interval_90 = FALSE;
									}

								}
								//demande envoie des valeur odometrique
								P_L_Can_odometre_chgt = TRUE;
							}

						}
					}
					else
					{
						P_L_Can_on_off_odometre_memo = P_L_Can_on_off_odometre_rx;
						P_L_Can_valeur_apc_memo = P_L_Can_valeur_apc_rx;
					}
				}
				else
				{
					if (TRUE == P_L_Can_on_off_odometre_memo)
					{
						P_L_Can_on_off_odometre_memo = P_L_Can_on_off_odometre_rx;
					}
					P_L_Can_valeur_apc_memo = P_L_Can_valeur_apc_rx;
				}
			}
		}
#endif
#ifdef _RAMPE_AUTONOME
		else if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)
		||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt))
		{
			//ben le 12/02/2023
			if (PL_CAN_ID_CMD == p_l_can_data_ptr[0])
			{
				// demande gestion des dfonnées du noeud
				if (P_L_Can_request_send_info_last_request != (BOOL_TYPE)(p_l_can_data_ptr[1]&PL_INFO_SYS))
				{
					P_L_Can_request_send_info = TRUE;
				}
				else
				{
					// do nothing
				}

				 //marche/arret coeur analyse
				if (PL_ON_OFF_ANALYSE == (p_l_can_data_ptr[1] & PL_ON_OFF_ANALYSE))
				{ //demande d'arret du coeur d'analyse
					APP_Arret_analyse = TRUE;
				}
				else
				{//demande de mise en route du coeur d'analyse
					APP_Arret_analyse = FALSE;
				}


				if (PL_ON_OFF_SYS == (p_l_can_data_ptr[1] & PL_ON_OFF_SYS))
				{ //demande d'arret du systeme
					APP_Can_stop_request = TRUE;

					if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
					{

						APP_Demande_Arret_SendCan = TRUE;

					}

				}
				else
				{//demande d arret de l'arret du systeme
					APP_Can_stop_request = FALSE;
				}
			}
			///////

			ptr_configU32 = (const U32 *)APP_LISTE_FACADE_CFG_FACADE;


			if ( (*ptr_configU32 == p_l_can_msg_id) || (*(ptr_configU32+1) == p_l_can_msg_id) )
			{
				if (PL_CAN_ID_BIP == p_l_can_data_ptr[0])
				{// gestion du buzzer

					if (*ptr_configU32 == p_l_can_msg_id)
					{
						offset = 0;
					}
					else
					{
						offset = 1;
					}

					if (APP_request_buzzer[offset] != p_l_can_data_ptr[1])
					{
						APP_request_buzzer[offset] = p_l_can_data_ptr[1];
						APP_request_buzzer_Change = TRUE;
					}

				}
				else if ((PL_CAN_ID_LED1 <= p_l_can_data_ptr[0])&&(PL_CAN_ID_LED6 >= p_l_can_data_ptr[0]))
				{// gestion des leds

					 type_module = (p_l_can_msg_id & PL_MASK_ADRESSE);

					if (APP_ADR_NOEUD_CANF12T == type_module)
					{
						//nb_trame_voyant = APP_CANF12T_NB_T_C_VOYANT;
						nb_voyant = APP_CANF12T_NB_VOYANT;

					}
					else if (APP_ADR_NOEUD_CANFGRAPH == type_module)
					{
						switch(p_l_can_msg_id & PL_MASK_SOUS_ADR_GRAPH)
						{
							case 0:
								nb_voyant = APP_CANFGRAPH_2_NB_VOYANT;
							break;

							case 1:
								nb_voyant = APP_CANFGRAPH_1_NB_VOYANT;
							break;

							default:
								nb_voyant = 0;
							break;
						}
					}

					if (*ptr_configU32 == p_l_can_msg_id)
					{
						offset = 0;
					}
					else
					{
					   offset = APP_NB_MAX_TOUCHE;
					}

					offset = offset  + (7 * (p_l_can_data_ptr[0] - PL_CAN_ID_LED1));



					for (boucle = 0;boucle<7;boucle++)
					{
						position = offset+boucle;
						if(position < nb_voyant)
						{
							if (APP_Tab_Etat_Leds[offset+boucle] != p_l_can_data_ptr[boucle+1])
							{
								APP_Tab_Etat_Leds[offset+boucle] = p_l_can_data_ptr[boucle+1];
								led_change = TRUE;
								//APP_Tab_Etat_Leds_Change_Count++;
							}
						}
					}

					if (TRUE == led_change)
					{
						APP_Tab_Etat_Leds_Change = TRUE;
					}
				}
				else if (PL_CAN_ID_LEDPWM == p_l_can_data_ptr[0])
				{// gestion rapport PWM pour les leds

					//P_L_Can_Consigne_Led_pwm = p_l_can_data_ptr[1];
				}
			}
			else
			{
				if((APP_ADRESSE_BASE_WIFI_BT+P_L_CC_MASK_WR) == (p_l_can_msg_id&P_L_CC_MASK_ADR_LOG))
				{
					if (p_l_can_data_ptr[0] == PL_CAN_BT_INFORMATION)
					{
						if (p_l_can_data_ptr[1] == MODE_RAMPE_AUTONOME)
						{
							P_L_Bx310x_Test_Pair_RX(&p_l_can_data_ptr[2]);
						}
					}
				}

				if(APP_Adresse_module ==  (p_l_can_msg_id & P_L_CC_MASK_ID))
				{
					if (PL_CAN_ID_TOR1 == p_l_can_data_ptr[0])
					{
						APP_BT_Can_Data_Power_Bloc(p_l_can_data_ptr,(p_l_can_data_ptr[0] - (U8)PL_CAN_ID_TOR1),p_l_can_lenght);
					}
					else if ((PL_CAN_ID_TOR_PWM1 == p_l_can_data_ptr[0])||(PL_CAN_ID_TOR_PWM2 == p_l_can_data_ptr[0]))
					{
						APP_BT_Can_Data_Pwm_Bloc(p_l_can_data_ptr,(p_l_can_data_ptr[0] - (U8)PL_CAN_ID_TOR_PWM1));
					}
					else
					{
						// do nothing
					}
				}
				else
				{
					// do nothing
				}

				if (PL_CAN_ID_INFO_COEUR == p_l_can_data_ptr[0])
				{
					if (PL_CAN_INFO_1 == p_l_can_data_ptr[1])
					{
						 //Tension batterie systeme
						APP_Tension_batterie[0] = p_l_can_data_ptr[3];
						APP_Tension_batterie[1] = p_l_can_data_ptr[4];
					}
				}
			}

			if (MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt)
			{
				if(APP_ADR_NOEUD_CAN_BLOCSPA_A ==  (p_l_can_msg_id & P_L_CC_MASK_ID))
				{
					if (PL_CAN_ID_IN1 == p_l_can_data_ptr[0])
					{
						APP_BT_can_input = p_l_can_data_ptr[1];
					}
					else
					{
						// do nothing
					}
				}
				else
				{
					// do nothing
				}
			}
		}
		else if (MODE_RAMPE_AUTONOME == APP_Mode_gestion_wifi_blt)
		{
			//ben le 1//02/2023
			if (PL_CAN_ID_CMD == p_l_can_data_ptr[0])
			{
				// demande gestion des dfonnées du noeud
				if (P_L_Can_request_send_info_last_request != (BOOL_TYPE)(p_l_can_data_ptr[1]&PL_INFO_SYS))
				{
					P_L_Can_request_send_info = TRUE;
				}
				else
				{
					// do nothing
				}

				 //marche/arret coeur analyse
				if (PL_ON_OFF_ANALYSE == (p_l_can_data_ptr[1] & PL_ON_OFF_ANALYSE))
				{ //demande d'arret du coeur d'analyse
					APP_Arret_analyse = TRUE;
				}
				else
				{//demande de mise en route du coeur d'analyse
					APP_Arret_analyse = FALSE;
				}


				if (PL_ON_OFF_SYS == (p_l_can_data_ptr[1] & PL_ON_OFF_SYS))
				{ //demande d'arret du systeme
					APP_Can_stop_request = TRUE;
					APP_Demande_Arret_SendCan = TRUE;
				}
				else
				{//demande d arret de l'arret du systeme
					APP_Can_stop_request = FALSE;
				}
			}
			///////

			if((APP_ADRESSE_BASE_WIFI_BT+P_L_CC_MASK_WR) == (p_l_can_msg_id&P_L_CC_MASK_ADR_LOG))
			{
				if (p_l_can_data_ptr[0] == PL_CAN_BT_INFORMATION)
				{
					if ((p_l_can_data_ptr[1] == MODE_CMD_RAMPE_AUTONOME_BLEU)||(p_l_can_data_ptr[1] == MODE_CMD_RAMPE_AUTONOME_ORANGE))
					{
						P_L_Bx310x_Test_Pair_RX(&p_l_can_data_ptr[2]);
					}
				}
			}

			if(APP_ADR_NOEUD_CAN_BLOCSPA_A ==  (p_l_can_msg_id & P_L_CC_MASK_ID))
			{
				if ((PL_CAN_ID_TOR1 == p_l_can_data_ptr[0])||(PL_CAN_ID_TOR2 == p_l_can_data_ptr[0]))
				{
					APP_SPA_Can_Data_Power_Bloc(p_l_can_data_ptr,(p_l_can_data_ptr[0] - (U8)PL_CAN_ID_TOR1),p_l_can_lenght);
				}
				else if (PL_CAN_ID_ANA1 == p_l_can_data_ptr[0])
				{
					APP_SPA_Can_Data_Analog_Bloc(p_l_can_data_ptr,(p_l_can_data_ptr[0] - (U8)PL_CAN_ID_ANA1));
				}
				else if (PL_CAN_ID_IN1 == p_l_can_data_ptr[0])
				{
					APP_SPA_Can_Data_Input_Bloc(p_l_can_data_ptr);
				}
				else
				{
					// do nothing
				}
			}
			else
			{
				// do nothing
			}
		}
#endif
#ifdef _PROJECTEUR_BT
		else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
		{

		}
#endif
		else
		{
			// do nothing
		}
	}
}



/*******************************************************************************
*
*  Function Name   : P_L_Manage_Perte_Reseau
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : init function
*******************************************************************************/
void P_L_Manage_Perte_Reseau(void)
{
BOOL_TYPE result;

    if (FALSE == P_L_CarteSd_Programation_vue)
    {
        if ((TRUE == P_L_CarteSd_Testeur_vue)||(TRUE == P_L_CarteSd_Atester_vue))
        {
            APP_Can_stop_perte_reseau = FALSE;
        }
        else
        {
            BT_TIMER_LOCK(APP_Timer_filtrage_perte_reseau_can_led,S_S_1MS_U32_TIME,APP_TEMPO_AV_FIN_LED,APP_Lock_filtrage_perte_reseau_can_led,result);
            if (TRUE == result)
            {
                HWI_Cmd_Led_2_verte = FALSE;
                HWI_Led_2_Verte_Cmd(HWI_Cmd_Led_2_verte);
            }

            BT_TIMER_LOCK(APP_Timer_filtrage_perte_reseau_can_stop,S_S_1MS_U32_TIME,APP_FILTRAGE_PERTE_RESEAU_CAN,APP_Lock_filtrage_perte_reseau_can_stop,result);
            if(TRUE == result)
            {
                APP_Can_stop_perte_reseau = TRUE;
            }
            else
            {
               APP_Can_stop_perte_reseau = FALSE;
            }
        }
    }
}

/*******************************************************************************
*
*  Function Name   : P_L_Can_Rx_Data_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : init function
*******************************************************************************/
void P_L_Can_Rx_Data_Init(void)
{
    P_L_Can_request_send_info = FALSE;
    P_L_Can_request_send_info_last_request = FALSE;
    APP_Arret_analyse = FALSE;

    APP_Can_stop_request = FALSE;

    BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_reseau_can_stop,S_S_1MS_U32_TIME,APP_Lock_filtrage_perte_reseau_can_stop);

    HWI_Cmd_Led_2_verte = FALSE;
    HWI_Cmd_Led_2_rouge = FALSE;
    HWI_Cmd_Led_Flip = FALSE;
    HWI_Cmd_Led_Count = 0x00;
    APP_Can_PMV_Non_Vue = FALSE;

#ifdef _GESTION_ODOMETRE
    //valeur odometre recue sur le can ext
    P_L_Can_valeur_odometre_memo = 0;
    //valeur apc recue sur le can ext
    P_L_Can_valeur_apc_rx = FALSE;
    //memo valeur apc recue sur le can ext
    P_L_Can_valeur_apc_memo = FALSE;
    //filtrage changement apc
    BT_TIMER_INIT_LOCK(P_L_Can_Timer_filtrage_apc,0x00,P_L_Can_Lock_filtrage_apc);

    //demande init de la valeur odometre
    P_L_Can_init_odometre = FALSE;
    //valeur marche arriere recue sur le can ext
    P_L_Can_valeur_marche_arriere_rx = FALSE;

    //valeur odometre envoye sur le can mercura
    P_L_Can_valeur_odometre_tx = 0;
    //interval odometre envoye sur le can mercura
    P_L_Can_interval_odometre_tx = 0;
    //interval odometre recue sur le can mercura
    P_L_Can_interval_odometre_rx = 0;
    //comptage decompatge odometre recue sur le can mercura
    P_L_Can_down_odometre_rx = FALSE;
    //arret temporaire de l odometre recue sur le can mercura
    P_L_Can_stop_odometre_rx = FALSE;
    //arret de la gestion de l odometre recue sur le can mercura
    //P_L_Can_on_off_odometre_rx = FALSE;
    //mise en route de la gestion de l odometre recue sur le can mercura
    P_L_Can_on_off_odometre_rx = TRUE;
    //memo de l arret de la gestion de l odometre recue sur le can mercura
    P_L_Can_on_off_odometre_memo = FALSE;
    //flag indiquant que l odometre a change de valeur
    P_L_Can_odometre_chgt = FALSE;
    //flag indiquant que l odometre 90% de interval atteind
    P_L_Can_odometre_interval_90 = FALSE;
//    P_L_Can_odometre_interval_90_memo = FALSE;
    //flag indiquant que l odometre 100% de interval atteind
    P_L_Can_odometre_interval_100 = FALSE;
    P_L_Can_odometre_interval_100_memo = FALSE;

    //etat du frein a main recue sur le can est en gestion antivol
    P_L_Can_Antivol_frein_a_main = FALSE;
    //etat du moteur recue sur le can est en gestion antivol
    P_L_Can_Antivol_moteur_on = FALSE;

    //valeur usine de etalonage odometre
    P_L_Can_etalonnage_odometre = (F64)1000;
#endif
    BT_TIMER_INIT_LOCK(APP_Timer_filtrage_perte_reseau_can_led,0x00,APP_Lock_filtrage_perte_reseau_can_led);

    APP_Can_module_PA = 0xFF;
    APP_Can_module_emission = 0xFF;
    APP_Can_emission_mode = APP_MODE_LECTURE;
    APP_Can_emission_message = 0x00;
    APP_Can_module_diffusion = 0xFF;
    APP_Tension_batterie[0x00] = 0x00;
    APP_Tension_batterie[0x01] = 0x00;
}


