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
#include "p_l_bx310x.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_hal_uart.h"
#include "hwi_bx310x.h"
#include "lib_macros.h"
#include "string.h"
#include "stdio.h"
#include "p_l_rpg.h"
#include "p_l.h"
#include "p_l_check_io.h"
#include "hwi_hal_gpio.h"
#include "app_manage_wifi.h"
#include "hwi_hal_flash.h"
#include "app_manage_BT.h"
//ADD
#define _RAMPE_AUTONOME
#define _PROJECTEUR_BT

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
#define _BT_CLASSIQUE
#else
#ifdef _RAMPE_AUTONOME
#define _DATA_BT_CALSSIQUE_SPP
#define _BT_CLASSIQUE
#else
#ifdef _TELECOMMANDE_BT
#define _BT_CLASSIQUE
#define _BT_BLE
#else
#ifdef _BLUETOOTH_REPROG
#define _DATA_BT_CALSSIQUE_SPP
#define _BT_CLASSIQUE
#else
 erreur choisir audio ou DATA
#endif
#endif
#endif
#endif

#ifndef _BT_CLASSIQUE
#ifndef _BT_BLE
 erreur choisir _BT_CLASSIQUE ou _BT_BLE ou les deux
#endif
#endif

#endif


/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/



/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U16 		                            HWI_Uart_wifi_ptr_received_string;
extern U32 		                            S_S_1MS_U32_TIME;

//entree logique du bloc
extern P_L_INPUT_STATE_TYPE   				P_L_Bp1;

//flag indiquant qu'une carte SD autorise aucun mode particulier
extern BOOL_TYPE                            P_L_CarteSd_No_Mode_vue;
//flag indiquant qu'une carte SD autorisant le mode testeur
extern BOOL_TYPE                            P_L_CarteSd_Testeur_vue;
//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE                            P_L_CarteSd_Atester_vue;
//flag indiquant qu'une carte SD autorisant la mise a jour du module wifi
extern BOOL_TYPE                            P_L_CarteSd_MiseAJour_vue;
extern U8                                   P_L_CarteSd_Clef_wep_MAJ[P_L_WIFI_CLEF_WEP_SIZE+1];
extern U8                                   P_L_CarteSd_ssid_mac_MAJ[P_L_Wifi_SSID_MAX];

//gestion semaphore envoie wifi
extern BOOL_TYPE                            P_L_Wifi_Semaphore_Brocast;
extern BOOL_TYPE                            P_L_Wifi_Semaphore_IT;

extern APP_WIFI_CLIENTS_TYPE                APP_Wifi_clients_tab[P_L_WIFI_MAX_CLIENTS];

//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE           APP_Mode_gestion_wifi_blt;

#ifdef _RAMPE_AUTONOME
extern BOOL_TYPE	P_L_Flash_Mac_spp_Wr;
extern U8			P_L_Flash_Mac_spp_rx[P_L_SSP_MAC_PAIR_FLASH_SIZE];
#endif

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/

/////////////////////////////////////////////////////////////////////////////
//IP client

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Information a configurer par le clients
//
/////////////////////////////////////////////////////////////////////////////
//uint8_t* default_ssid = "Mercura_XXL";
///** The default module IP address (192.168.1.254) */
//uint32_t default_adresse = 0xFE01A8C0;
///** The default module net mask (255.255.0.0) */
//uint32_t default_netmask = 0x0000FFFF;
///** The default remote port TCP. */
//uint16_t default_remote_port = 80;

//BOOL_TYPE 	            P_L_Wifi_on;

U8 	 		            P_L_Wifi_Clef_wep[P_L_WIFI_CLEF_WEP_SIZE+2]; //+2 pour etre un nombre multiple de 8 pour la sauvegarde en flash
U8			            P_L_Wifi_ssid_mac[P_L_Wifi_SSID_MAX];
hw_addr                 P_L_Wifi_Module_Mac;
BOOL_TYPE               P_L_Wifi_ssid_mac_lue;

U8			            P_L_BT_name_mac[P_L_Wifi_SSID_MAX];
hw_addr                 P_L_BT_Module_Mac;
BOOL_TYPE               P_L_BT_ssid_mac_lue;


U8 	 		            P_L_Wifi_Clef_wep_a_tester[P_L_WIFI_CLEF_WEP_SIZE+2]; //+2 pour etre un nombre multiple de 8 pour la sauvegarde en flash
U8			            P_L_Wifi_ssid_mac_a_tester[P_L_Wifi_SSID_MAX];
hw_addr                 P_L_Wifi_Module_Mac_a_tester;
BOOL_TYPE               P_L_Wifi_ssid_mac_a_tester_recue;
BOOL_TYPE               P_L_Wifi_ATester_vue;
U8                      P_L_Wifi_endpointTest;
BOOL_TYPE               P_L_Wifi_MiseAJour_vue;


U32                     P_L_Wifi_wep_timer;
BOOL_TYPE               P_L_Wifi_wep_lock;
BOOL_TYPE               P_L_Wifi_wep_flag;

BOOL_TYPE               P_L_Bx310x_timeout_configure_lock;
U32                     P_L_Bx310x_timeout_configure;

BOOL_TYPE               P_L_Bx310x_Flag_module_init;

P_L_Wifi_AP_TYPE        P_L_Wifi_AP[P_L_WIFI_MAX_CLIENTS];
U32                     P_L_Bx310x_cmd_unknow;
U32                     P_L_Bx310x_cmd_error;
U32                     P_L_Bx310x_cmd_ready;


//#ifdef _BLUETOOTH
P_L_BT_STATE_TYPE       P_L_BT_state;

U8          P_L_BT_mac_pair_ask[P_L_BT_MAC_SIZE];
U8          P_L_BT_pin_pair_ask[P_L_BT_PIN_CODE_SIZE];
BOOL_TYPE   P_L_BT_pair_ask;

// #ifdef _GESTION_AUDIO_BT
// P_L_BT_AUDIO_STATE_TYPE P_L_BT_audio_state;
// #ifdef _PAIR_TO_DEVICE
// U8      P_L_BT_mac_pair_Audio_ask[P_L_BT_MAC_SIZE];
// #endif
// #endif



//#endif

BOOL_TYPE               HWI_Cmd_Led_1_verte;

// Current state of the system.
// Current state of the system.
P_L_BX310X_STATE        P_L_Bx310x_internal_state;
P_L_BX310X_STATE        P_L_Bx310x_futur_state;


const U8 digit[16] ={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static const U8 APP_HEX[] = "0123456789abcdef";

#ifdef _DEBUG_BX310X_COM
U8 data_send[242];
U8 P_L_Bx310x_READY = 0;
U8 P_L_Bx310x_TCP_SRVREQ = 0;
U8 P_L_Bx310x_TCP_NOTIF = 0;
U8 P_L_Bx310x_TCP_DATA = 0;
U8 P_L_Bx310x_ERROR_CME = 0;
U8 P_L_Bx310x_CME_ERROR_TAB[300];
U8 P_L_Bx310x_ERROR = 0;
U8 P_L_Bx310x_ERROR_TAB[300];
U8 P_L_Bx310x_CMD_UNKNOW = 0;
#endif
U32 P_L_Bx310x_error_code_cme;

const U8 temp_clef_wep[P_L_WIFI_CLEF_WEP_SIZE] = "123456789012345678901234567890";
const U8 Bx310x_state_START = Bx310x_state_RESET;

U8 P_L_Bx310x_version[10];


// #ifdef _RAMPE_AUTONOME
BOOL_TYPE              P_L_BT_spp_pair_rampe;

BOOL_TYPE              P_L_BT_spp_connect_rampe;
BOOL_TYPE              P_L_BT_spp_start_rampe;
U8   		   		   P_L_BT_spp_rampe_id;
P_L_BT_STATE_TYPE      P_L_BT_spp_rampe_state;
P_L_BT_STATE_TYPE      P_L_BT_spp_rampe_state_memo;
U8      			   P_L_BT_spp_mac_pair_rampe[P_L_BT_MAC_SIZE];
hw_addr                P_L_BT_Spp_Mac;
U32					   P_L_BT_spp_Connect_timer;
BOOL_TYPE              P_L_BT_spp_Connect_lock;
// #endif

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static U8 P_L_Wifi_Found_AP_IP(IP_TYPE client_ip);
static U8 P_L_Wifi_Found_AP_ID(U32 client_ip);
static U32 P_L_Wifi_Found_AP_CLIENT_ID(U8 endpoint);
static void P_L_Wifi_Record_AP(IP_TYPE client_ip,U32 client_id,BOOL_TYPE bluetooth);
static void P_L_Wifi_Remove_AP(U32 client_id);
static void P_L_Wifi_MiseAJour_AP(U8 endpoint,U32 client_id);

static U32 P_L_Bx310x_Read_All_number(U16 *ptr_received_string,U8 * received_string,char delimiteur);
static U32 P_L_Bx310x_Read_All_Hexa(U16 *ptr_received_string,U8 * received_string,char delimiteur);
static U8 P_L_Bx310x_Read_Hexa(U16 *ptr_received_string,U8 * received_string);
static void P_L_Bx310x_Write_Hexa(U16 *ptr_received_string,U8 * received_string,U8 number);
static void P_L_Bx310x_Write_number(U16 *ptr_received_string,U8 * received_string,U32 number);
static void P_L_Bx310x_goto_state(P_L_BX310X_STATE state);



static void P_L_Bx310x_cmd_system_ati();
static void P_L_Bx310x_cmd_system_RWCFG();
static void P_L_Bx310x_cmd_system_Ask_RWCFG();
static void P_L_Bx310x_cmd_system_RWAPCFG();
static void P_L_Bx310x_cmd_system_RWAPCFGNU();

static void P_L_Bx310x_cmd_system_RWAPNETCFG();
static void P_L_Bx310x_cmd_system_KTCPCFG();
static void P_L_Bx310x_cmd_system_KTCPCNX();

static void P_L_Bx310x_cmd_system_SRBTSYSTEM_0();

// #ifdef _BLUETOOTH
static void P_L_Bx310x_cmd_system_SRBTADDR();
static void P_L_Bx310x_cmd_system_SRBTNAME();
static void P_L_Bx310x_cmd_system_SRBTCSECPARAMS();
static void P_L_Bx310x_cmd_system_SRBTSYSTEM_CFG();
// #ifdef _GESTION_AUDIO_BT
// static void P_L_Bx310x_cmd_system_KI2SCFG();
// #endif
#ifdef _TELECOMMANDE_BT
static void P_L_Bx310x_cmd_system_SRBTINQ();
static void P_L_Bx310x_cmd_system_SRBLESCAN();
static void P_L_Bx310x_cmd_system_SRBTPAIR();
#endif


// #ifdef _PAIR_TO_DEVICE
// static void P_L_Bx310x_cmd_system_SRBTINQ();
// static void P_L_Bx310x_cmd_system_SRBTPAIR(U8* id);
// #endif

// #ifdef _RAMPE_AUTONOME
static void Bx310x_state_BT_SRSPPCFG(U8 *mac);
static void Bx310x_state_BT_SRSPPCNX(U8 id);
static void P_L_Bx310x_cmd_system_SRBTUNPAIR();
//static void P_L_Bx310x_cmd_system_SRBTCPINCFG();
//static void Bx310x_state_BT_SRBTPAIR(U8 *mac);
//static void P_L_Bx310x_cmd_system_SRBTINQ();
//static void P_L_Bx310x_cmd_system_SRBTPWR();

// #endif

static void P_L_Bx310x_cmd_system_SRBTCPROFILES();
static void P_L_Bx310x_cmd_system_SRBTSTATE();
// #endif

static void P_L_Bx310x_cmd_module_and_cfg_reset();
static void P_L_Bx310x_cmd_module_reset();
static void P_L_Bx310x_cmd_system_ctrl_flux();

static void P_L_Bx310x_cmd_system_SRWSTACFG();

static void P_L_Bx310x_cmd_system_WDSI();
static void P_L_Bx310x_cmd_system_WDSS();

//static void wifi_cmd_software_version();
//static void P_L_Bx310x_Module_Init_Ask(void);
static void P_L_Bx310x_Module_Init(void);

// #ifdef _BLUETOOTH
static void P_L_Bx310x_cmd_system_SRBTCPIN(U8* mac,BOOL_TYPE accept,U8 *pincode);
static void P_L_Bx310x_cmd_system_SRBTCSECNUMCMP(U8* mac,BOOL_TYPE accept);
static void P_L_Bx310x_WIFI_RX_Analyse(U8 start_data,U8* received_string);
// #endif

static void P_L_Bx310x_BT_RX_Analyse(U8 start_data,U8* received_string);


//ben test define a enlever
//#define _TEST_WIFI
/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Test_Timeout_Cmd
 *  Description     : gestion timeout initialisation du module bx310x
 *
 *
 *******************************************************************************/
void P_L_Bx310x_Test_Timeout_Cmd(void)
{           
BOOL_TYPE result;
U8 	Buffer_sendTest[20];
U16 ptr_data_send;

    if(TRUE == P_L_CarteSd_Testeur_vue)
    {
         result = ((P_L_Bx310x_futur_state != Bx310x_state_FINIT)&&(P_L_Bx310x_futur_state !=  Bx310x_state_SRWSTACFG)&&(P_L_Bx310x_futur_state !=  Bx310x_state_WAIT_WIFI_TEST));
    }
    else if (TRUE == P_L_Wifi_MiseAJour_vue)
    {
         result = ((P_L_Bx310x_futur_state != Bx310x_state_FINIT)&&(P_L_Bx310x_futur_state !=  Bx310x_state_WDSS));
    }
    else
    {
       result =  (P_L_Bx310x_futur_state != Bx310x_state_FINIT);
    }

    if (TRUE == result)
    {
        BT_TIMER_LOCK(P_L_Bx310x_timeout_configure,S_S_1MS_U32_TIME,APP_TIMEOUT_RESET_CMD,P_L_Bx310x_timeout_configure_lock,result);
        if (TRUE == result)
        {// ecoule
            P_L_Bx310x_Module_Init();
        }
    }

    if (TRUE == P_L_Bx310x_Flag_module_init)
    {
        P_L_Bx310x_Flag_module_init = FALSE;
        P_L_Bx310x_Module_Init();
    }

    if ((P_L_Bx310x_internal_state == Bx310x_state_FINIT)&&(TRUE == P_L_CarteSd_Testeur_vue))
    {
        strncpy((char*)Buffer_sendTest,(char*)"AT+KTCPSND=",11);

        ptr_data_send = 11;

        P_L_Bx310x_Write_number(&ptr_data_send,Buffer_sendTest,P_L_Wifi_endpointTest);
        Buffer_sendTest[ptr_data_send] = ',';
        ptr_data_send++;
        strncpy((char*)&Buffer_sendTest[ptr_data_send],(char*)"HELLO",5);
        ptr_data_send = ptr_data_send+5;
        HWI_Bx310x_tx(ptr_data_send,Buffer_sendTest);
    }

    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
    {
    	if (P_L_BT_spp_rampe_state_memo != P_L_BT_spp_rampe_state)
    	{
    		P_L_BT_spp_rampe_state_memo = P_L_BT_spp_rampe_state;

			if (P_L_BT_spp_rampe_state == BT_connected)
			{
				HWI_Led_1_Verte_Cmd(TRUE);
			}
			else
			{
				HWI_Led_1_Verte_Cmd(FALSE);
			}
    	}

    	BT_TIMER_LOCK(P_L_BT_spp_Connect_timer,S_S_1MS_U32_TIME,APP_TIME_INTER_SPP_CONNECT,P_L_BT_spp_Connect_lock,result);
    	if(TRUE == result)
    	{
			if ((P_L_BT_spp_rampe_state == BT_disconnected) && (P_L_Bx310x_internal_state == Bx310x_state_FINIT))
			{
				P_L_BT_spp_connect_rampe = TRUE;
				BT_TIMER_INIT_LOCK(P_L_BT_spp_Connect_timer,S_S_1MS_U32_TIME,P_L_BT_spp_Connect_lock);
			}
    	}

		if ((TRUE == P_L_BT_spp_pair_rampe) && (P_L_Bx310x_internal_state == Bx310x_state_FINIT))
		{
			P_L_BT_spp_pair_rampe = FALSE;
			P_L_Bx310x_goto_state(Bx310x_state_SRBTUNPAIR);
		}


    }
    else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    {
    	if (P_L_BT_spp_rampe_state== BT_connected)
		{
			HWI_Led_1_Verte_Cmd(TRUE);
		}
		else
		{
			HWI_Led_1_Verte_Cmd(FALSE);
		}

    	if ((TRUE == P_L_BT_spp_pair_rampe) && (P_L_Bx310x_internal_state == Bx310x_state_FINIT))
		{
			P_L_BT_spp_pair_rampe = FALSE;
			P_L_Bx310x_goto_state(Bx310x_state_SRBTUNPAIR);
		}
    	else
    	{
    		//do nothing
    	}
    }

}

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Module_Init_Ask
 *  Description     :
 *
 *
 *******************************************************************************/
//static void P_L_Bx310x_Module_Init_Ask(void)
void P_L_Bx310x_Module_Init_Ask(void)
{
    P_L_Bx310x_Flag_module_init = TRUE;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Module_Init
 *  Description     :
 *
 *
 *******************************************************************************/
static void P_L_Bx310x_Module_Init(void)
{
    HWI_Bx310x_active_RF_Cmd(FALSE);
    HAL_Delay(200);
    HWI_Bx310x_active_RF_Cmd(TRUE);
    HAL_Delay(500);
	P_L_Bx310x_goto_state(Bx310x_state_START);
}

/*******************************************************************************
 *
 *  Function Name   : goto_state
 *  Description     : demande etapes futur module
 *
 *
 *
 *******************************************************************************/
static void P_L_Bx310x_goto_state(P_L_BX310X_STATE state)
{
    P_L_Bx310x_futur_state = state;
    BT_TIMER_INIT_LOCK(P_L_Bx310x_timeout_configure,S_S_1MS_U32_TIME,P_L_Bx310x_timeout_configure_lock);
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Manage_State
 *  Description     : traitement des etapes du module
 *
 *
 *
 *******************************************************************************/
void P_L_Bx310x_Manage_State(void)
{
BOOL_TYPE result;

    if(TRUE == P_L_CarteSd_Testeur_vue)
    {
         result = ((P_L_Bx310x_internal_state != P_L_Bx310x_futur_state)||(P_L_Bx310x_internal_state == Bx310x_state_RESET)||(P_L_Bx310x_internal_state == Bx310x_state_WAIT_WIFI_TEST));
    }
    else
    {
       result =  ((P_L_Bx310x_internal_state != P_L_Bx310x_futur_state)||(P_L_Bx310x_internal_state == Bx310x_state_RESET));
    }

    if (TRUE == result)
    {
        switch (P_L_Bx310x_futur_state)
        {

            case Bx310x_state_RESET:
                HWI_Bx310x_Configuration_RF_High_Prio();
                HWI_Uart_Init(115200,FALSE);

                HAL_Delay(100);

                P_L_Bx310x_cmd_module_and_cfg_reset();
                //wifi_cmd_software_version();
                P_L_Bx310x_goto_state(Bx310x_state_ATTENTE);

                if(TRUE == P_L_CarteSd_No_Mode_vue)
                {
                    HWI_Led_1_Rouge_Cmd(TRUE);
                }
           break;

            case Bx310x_state_ATTENTE:

            break;
            //Break of bluetooth
            case Bx310x_state_SRBTSYSTEM_0:
                P_L_Bx310x_cmd_system_SRBTSYSTEM_0();
            break;


            case Bx310x_state_RESET_module:
                P_L_Bx310x_cmd_module_reset();
                P_L_Bx310x_goto_state(Bx310x_state_ATTENTE_RESET_module);
            break;

            case Bx310x_state_ATTENTE_RESET_module:

            break;

            case Bx310x_state_CONTROL_FLUX:
               P_L_Bx310x_cmd_system_ctrl_flux();
            break;

            case Bx310x_state_ATI:
                HWI_Bx310x_Configuration_RF_Normal_Prio();
                HWI_Uart_Init(115200,TRUE);
                P_L_Bx310x_cmd_system_ati();
            break;

            case Bx310x_state_RWCFG:
                P_L_Bx310x_cmd_system_RWCFG();
            break;

            case Bx310x_state_RWAPNETCFG:
                P_L_Bx310x_cmd_system_RWAPNETCFG();
            break;
                // Lecture de la configuration WiFi courante
            case Bx310x_state_ASK_RWCFG:
                P_L_Bx310x_cmd_system_Ask_RWCFG();
            break;
                // Configuration du Point d'Accès WiFi
            case Bx310x_state_RWAPCFG:
                P_L_Bx310x_cmd_system_RWAPCFG();
            break;

            case Bx310x_state_RWAPCFGNU:
            	P_L_Bx310x_cmd_system_RWAPCFGNU();
            break;

            case Bx310x_state_KTCPCFG:
                P_L_Bx310x_cmd_system_KTCPCFG();
            break;

            case Bx310x_state_KTCPCNX:
                P_L_Bx310x_cmd_system_KTCPCNX();
            break;

// #ifdef _BLUETOOTH
                // Configuration du sous-système Bluetooth
            case Bx310x_state_SRBTSYSTEM_CFG:
                P_L_Bx310x_cmd_system_SRBTSYSTEM_CFG();
            break;
                //Interroge l'adresse MAC Bluetooth du module
            case Bx310x_state_SRBTADDR:
                P_L_Bx310x_cmd_system_SRBTADDR();
            break;
                //Configuration du nom Bluetooth visible
            case Bx310x_state_SRBTNAME:
                P_L_Bx310x_cmd_system_SRBTNAME();
            break;
                //Configuration sécurité Bluetooth
            case Bx310x_state_SRBTCSECPARAMS:
                P_L_Bx310x_cmd_system_SRBTCSECPARAMS();
            break;

// #ifdef _GESTION_AUDIO_BT
//             case Bx310x_state_KI2SCFG:
//             	P_L_Bx310x_cmd_system_KI2SCFG();
//             break;
// #endif

            case Bx310x_state_SRBTCPROFILES:
                P_L_Bx310x_cmd_system_SRBTCPROFILES();
            break;

// #ifdef _TELECOMMANDE_BT
//             case Bx310x_state_start_general_inquiry:
//                 P_L_Bx310x_cmd_system_SRBLESCAN();
//             break;

//             case Bx310x_state_start_pair_request:
//                 P_L_Bx310x_cmd_system_SRBTPAIR();
//             break;

// #endif
            case Bx310x_state_SRBTSTATE:
                P_L_Bx310x_cmd_system_SRBTSTATE();
            break;
// #endif

            case Bx310x_state_SRWSTACFG:
                if ((TRUE == P_L_Wifi_ssid_mac_a_tester_recue) || (TRUE == P_L_CarteSd_MiseAJour_vue))
                {
                    P_L_Bx310x_cmd_system_SRWSTACFG();
                }
            break;

            case Bx310x_state_WAIT_WIFI_TEST:
            break;

            case Bx310x_state_WAIT_WIFI_AirVantage:
            break;

            case Bx310x_state_WDSI:
                P_L_Bx310x_cmd_system_WDSI();
            break;

            case Bx310x_state_WDSS:
                P_L_Bx310x_cmd_system_WDSS();
            break;

// #ifdef _PAIR_TO_DEVICE
//             case Bx310x_state_LIST_BT_DEVICE:
//                  P_L_Bx310x_cmd_system_SRBTINQ();
//             break;

//             case Bx310x_state_PAIR_AUDIO:
//             	P_L_Bx310x_cmd_system_SRBTPAIR(P_L_BT_mac_pair_Audio_ask);
//             break;
// #endif

// #ifdef _RAMPE_AUTONOME
//            case Bx310x_state_SRBTUNPAIR:
//         	   P_L_Bx310x_cmd_system_SRBTUNPAIR();
//            break;

//            case Bx310x_state_BT_SSP_CONNECT:
//         	   Bx310x_state_BT_SRSPPCFG(P_L_BT_spp_mac_pair_rampe);
//            break;

//            case Bx310x_state_BT_SSP_START:
//         	   Bx310x_state_BT_SRSPPCNX(P_L_BT_spp_rampe_id);
//            break;

// #endif
            case Bx310x_state_FINIT:
                break;

            default:
                break;

        }
        P_L_Bx310x_internal_state = P_L_Bx310x_futur_state;
    }

}

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Rx
 *  Description     : Reception de donnee RS
 *
 *******************************************************************************/
 #ifdef _TELECOMMANDE_BT
U8 reponse = 0;
U8 data_inq[20][200]; 
U8 data_ble[20][200];
BOOL_TYPE found = FALSE;
#endif

void P_L_Bx310x_Rx(U16 ptr_received_string,U8 * received_string)
{
IP_TYPE	client_ip;
U8 taille;
U16 ptr_data;
U32	client_id;
U8 endpoint;
U16 boucle;
U16 data_len;
BOOL_TYPE spp_pair_rampe_OK;

#ifndef _BLUETOOTH
U8 data_received[TAILLE_BUFFER_RX_RS];
U16 ptr_data_received;
#endif


    if (0 == strncmp((char*)"BX310x",(char*)received_string,6))
    {
    	 if (ptr_received_string > 6)
    	 {
    		  strncpy((char*)P_L_Bx310x_version,(char*)&received_string[7],10);
    	 }
    	 else
    	 {
    		 //do nothing
    	 }
    }
    else if (0 == strncmp((char*)"READY",(char*)received_string,5))
    {
// #ifdef _DEBUG_BX310X_COM
//         P_L_Bx310x_READY++;
// #endif

//        P_L_Bx310x_goto_state(Bx310x_state_CONTROL_FLUX);

        if (P_L_Bx310x_internal_state == Bx310x_state_ATTENTE_RESET_module)
        {
           P_L_Bx310x_goto_state(Bx310x_state_CONTROL_FLUX);
        }
        else
        {
        	P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_0);
        }

       // P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_0);
    }
    else if (0 == strncmp((char*)"OK",(char*)received_string,2))
    {

        	switch (P_L_Bx310x_internal_state)
        	{

        	    case Bx310x_state_SRBTSYSTEM_0:
                    P_L_Bx310x_goto_state(Bx310x_state_RESET_module);
                    //P_L_Bx310x_goto_state(Bx310x_state_CONTROL_FLUX);
                break;

                case Bx310x_state_RESET:
                // pas pret attendre reception ready
                break;

                case Bx310x_state_ATTENTE:
                    // pas pret attendre reception ready
                break;

                case Bx310x_state_ATTENTE_RESET_module:

                break;

                case Bx310x_state_CONTROL_FLUX:
                    P_L_Bx310x_goto_state(Bx310x_state_ATI);
                break;

                case Bx310x_state_ATI:
                    P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_CFG);
                break;

                case Bx310x_state_RWCFG:
                    P_L_Bx310x_goto_state(Bx310x_state_RWAPNETCFG);
                break;

                case Bx310x_state_RWAPNETCFG:
                    P_L_Bx310x_goto_state(Bx310x_state_ASK_RWCFG);
                break;

                case Bx310x_state_RWAPCFG:
                    P_L_Bx310x_goto_state(Bx310x_state_KTCPCFG);
                break;

                case Bx310x_state_RWAPCFGNU:
                	P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_CFG);
                break;

                case Bx310x_state_KTCPCFG:
                    P_L_Bx310x_goto_state(Bx310x_state_KTCPCNX);
                break;

                case Bx310x_state_KTCPCNX:
// #ifdef _BLUETOOTH
                	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
                	{
                		P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                	}
                	else
                	{
                		P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_CFG);
                	}
                break;

#ifdef _BLUETOOTH
                case Bx310x_state_SRBTNAME:
/*
#ifdef _GESTION_AUDIO_BT
                    P_L_Bx310x_goto_state(Bx310x_state_KI2SCFG);
#else
                    P_L_Bx310x_goto_state(Bx310x_state_SRBTCPROFILES);
#endif
*/
                     P_L_Bx310x_goto_state(Bx310x_state_SRBTCSECPARAMS);
//                   P_L_Bx310x_goto_state(Bx310x_state_SRBTUNPAIR);
                break;

                case Bx310x_state_SRBTCSECPARAMS:
// #ifdef _GESTION_AUDIO_BT
//                     P_L_Bx310x_goto_state(Bx310x_state_KI2SCFG);
// #else
                    P_L_Bx310x_goto_state(Bx310x_state_SRBTCPROFILES);
// #endif
                break;

                case Bx310x_state_SRBTSYSTEM_CFG:
                    //P_L_Bx310x_goto_state(Bx310x_state_SRBTNAME);
                    P_L_Bx310x_goto_state(Bx310x_state_SRBTADDR);
                break;

// #ifdef _GESTION_AUDIO_BT
//                 case Bx310x_state_KI2SCFG:
//                     P_L_Bx310x_goto_state(Bx310x_state_SRBTCPROFILES);
//                 break;
// #endif

                case Bx310x_state_SRBTCPROFILES:
                    P_L_Bx310x_goto_state(Bx310x_state_SRBTSTATE);
                break;

                case Bx310x_state_SRBTSTATE:


                    P_L_BT_spp_connect_rampe = TRUE;
                    P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                break;
// #ifdef _TELECOMMANDE_BT
//                     P_L_Bx310x_goto_state(Bx310x_state_start_general_inquiry);
// #else
// #ifdef _PAIR_TO_DEVICE

//                     P_L_Bx310x_goto_state(Bx310x_state_LIST_BT_DEVICE);
// #else
//                     if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
// 					{
// 						P_L_BT_spp_connect_rampe = TRUE;
// 					// }
//                     P_L_Bx310x_goto_state(Bx310x_state_FINIT);
// // #endif
// // #endif
//                 break;

#ifdef _TELECOMMANDE_BT
                case Bx310x_state_start_pair_request:
                    P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                break;
#endif

#endif
                case Bx310x_state_SRWSTACFG:
                    if (TRUE == P_L_CarteSd_MiseAJour_vue)
                    {
                        P_L_Bx310x_goto_state(Bx310x_state_WAIT_WIFI_AirVantage);
                    }
                    else
                    {
                        P_L_Bx310x_goto_state(Bx310x_state_WAIT_WIFI_TEST);
                    }
                break;


                case Bx310x_state_WAIT_WIFI_TEST:
                break;

                case Bx310x_state_WAIT_WIFI_AirVantage:
                break;

                case Bx310x_state_WDSI:
                    P_L_Bx310x_goto_state(Bx310x_state_WDSS);
                break;

                case Bx310x_state_WDSS:
                break;

// #ifdef _PAIR_TO_DEVICE
//                 case Bx310x_state_LIST_BT_DEVICE:

//                 break;

//                 case Bx310x_state_PAIR_AUDIO:

//                 break;
// #endif

#ifdef _RAMPE_AUTONOME
                case Bx310x_state_SRBTUNPAIR:
                	P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
                	{
                		P_L_BT_spp_connect_rampe = TRUE;
                	}
                break;

                case Bx310x_state_BT_SSP_CONNECT:
                	P_L_BT_spp_start_rampe = TRUE;
                	P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                break;

                case Bx310x_state_BT_SSP_START:
                	P_L_Bx310x_goto_state(Bx310x_state_FINIT);
                break;
#endif

                case Bx310x_state_FINIT:
                break;

                default:
                break;
        	}

    }
    else if (0 == strncmp((char*)"+SRWCFG:",(char*)received_string,8))
    {

		if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME))
		{
			P_L_Wifi_ssid_mac[0] = 'S';
			P_L_Wifi_ssid_mac[1] = 't';
			P_L_Wifi_ssid_mac[2] = 'b';
			P_L_Wifi_ssid_mac[3] = 'y';
			P_L_Wifi_ssid_mac[4] = '_';
		}
		else
		{
			P_L_Wifi_ssid_mac[0] = 'M';
			P_L_Wifi_ssid_mac[1] = 'e';
			P_L_Wifi_ssid_mac[2] = 'r';
			P_L_Wifi_ssid_mac[3] = 'c';
			P_L_Wifi_ssid_mac[4] = '_';
		}

		P_L_Wifi_ssid_mac[5] = received_string[34];
		P_L_Wifi_ssid_mac[6] = received_string[35];

		P_L_Wifi_ssid_mac[7] = received_string[37];
		P_L_Wifi_ssid_mac[8] = received_string[38];

		P_L_Wifi_ssid_mac[9] = received_string[40];
		P_L_Wifi_ssid_mac[10] = received_string[41];

		P_L_Wifi_ssid_mac[11] = received_string[43];
		P_L_Wifi_ssid_mac[12] = received_string[44];

		P_L_Wifi_ssid_mac[13] = received_string[46];
		P_L_Wifi_ssid_mac[14] = received_string[47];

		P_L_Wifi_ssid_mac[15] = received_string[49];
		P_L_Wifi_ssid_mac[16] = received_string[50];

        ptr_data = 34;
        P_L_Wifi_Module_Mac.addr[0] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_Wifi_Module_Mac.addr[1] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_Wifi_Module_Mac.addr[2] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_Wifi_Module_Mac.addr[3] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_Wifi_Module_Mac.addr[4] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_Wifi_Module_Mac.addr[5] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,'"');

        P_L_Wifi_ssid_mac_lue = TRUE;

        P_L_Bx310x_goto_state(Bx310x_state_RWAPCFG);
    }
    else if (0 == strncmp((char*)"+KTCP_SRVREQ:",(char*)received_string,13))
    {   //ouverture socket

        //+KTCP_SRVREQ: 1,2,"192.168.1.1",39203
        //tcp session id ,tcp_client id, adresse ip, port
        ptr_data = 14;

#ifdef _DEBUG_BX310X_COM
        P_L_Bx310x_TCP_SRVREQ++;
#endif

        if ('1' == received_string[ptr_data])
        {
            ptr_data = 16;
            client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
            ptr_data++;
            client_ip.ip[0] = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'.');
            client_ip.ip[1] = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'.');
            client_ip.ip[2] = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'.');
            client_ip.ip[3] = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'"');

            endpoint = P_L_Wifi_Found_AP_IP(client_ip);

            if (0xFF == endpoint)
            {
                P_L_Wifi_Record_AP(client_ip,client_id,FALSE);
            }
            else
            {
                P_L_Wifi_MiseAJour_AP(endpoint,client_id);
            }
        }
    }
    else if (0 == strncmp((char*)"+KTCP_NOTIF:",(char*)received_string,12))
    {   //fermeture socket

        //+KTCP_NOTIF: 3,4
        //tcp_client id ->4 TCP disconnection by the server or remote client

#ifdef _DEBUG_BX310X_COM
        P_L_Bx310x_TCP_NOTIF++;
#endif

        ptr_data = 13;
        client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

        if('4' == received_string[ptr_data])
        {
            endpoint = P_L_Wifi_Found_AP_ID(client_id);
            P_L_Wifi_Remove_AP(client_id);

            if (0xFF != endpoint)
            {
                 APP_Manage_Wifi_Find_Client_Leave(endpoint);
            }
        }

    }
    else if (0 == strncmp((char*)"+SRWAPSTA:",(char*)received_string,10))
    {
        //+SRWAPSTA: 1,"10:8e:e0:b4:e9:fc"
    }
    else if (0 == strncmp((char*)"+KTCPCFG:",(char*)received_string,9))
    {
        if(TRUE == P_L_CarteSd_Testeur_vue)
        {
            ptr_data = 9;
            P_L_Wifi_endpointTest = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'\r');

            P_L_Bx310x_goto_state(Bx310x_state_KTCPCNX);
        }
    }
    else if (0 == strncmp((char*)"+KTCP_DATA:",(char*)received_string,11))
    {
        
        P_L_Bx310x_WIFI_RX_Analyse(12,received_string);}

    else if (0 == strncmp((char*)"+CME ERROR:",(char*)received_string,11))
    {
#ifdef _RAMPE_AUTONOME
    	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
    	{
			ptr_data = 12;
			P_L_Bx310x_error_code_cme = P_L_Bx310x_Read_All_number(&ptr_data,received_string,'\r');
			if (933 == P_L_Bx310x_error_code_cme)
			{
				P_L_BT_spp_start_rampe = TRUE;
			}
			else
			{
				if (P_L_Bx310x_internal_state == Bx310x_state_BT_SSP_CONNECT)
				{
					P_L_Bx310x_goto_state(Bx310x_state_FINIT);
				}
				else if (P_L_Bx310x_internal_state == Bx310x_state_BT_SSP_START)
				{
					P_L_Bx310x_goto_state(Bx310x_state_FINIT);
				}
    		}
    	}
    	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    	{
    		// do nothing
    	}
#ifdef _PROJECTEUR_BT
		else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
		{
			//do nothing
		}
#endif

#endif


#ifdef _DEBUG_BX310X_COM
        P_L_Bx310x_ERROR_CME++;

        for (boucle=0;boucle<ptr_received_string;boucle++)
        {
            if (boucle < 0xFF)
            {
                P_L_Bx310x_CME_ERROR_TAB[boucle] = received_string[boucle];
            }
        }
#endif
    }
    else if (0 == strncmp((char*)"ERROR",(char*)received_string,5))
    {

#ifdef _DEBUG_BX310X_COM
        P_L_Bx310x_ERROR++;

        for (boucle=0;boucle<ptr_received_string;boucle++)
        {
            if (boucle < 0xFF)
            {
                P_L_Bx310x_ERROR_TAB[boucle] = received_string[boucle];
            }
        }
#else
        P_L_Bx310x_Module_Init_Ask();
#endif
    }
#ifdef _BLUETOOTH
    else if (0 == strncmp((char*)"+SRBTCSECNUMCMP:",(char*)received_string,16))
    {
    	spp_pair_rampe_OK = TRUE;

        for (boucle=0;boucle<P_L_BT_MAC_SIZE;boucle++)
        {
            P_L_BT_mac_pair_ask[boucle] = received_string[boucle+18];
            if (P_L_BT_spp_mac_pair_rampe[boucle] != P_L_BT_mac_pair_ask[boucle])
            {
            	spp_pair_rampe_OK = FALSE;
            }
        }

        for (boucle=0;boucle<P_L_BT_PIN_CODE_SIZE;boucle++)
        {
            P_L_BT_pin_pair_ask[boucle] = received_string[boucle+37];
        }
        P_L_BT_pair_ask = TRUE;

        if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
		{
        	P_L_Bx310x_cmd_system_SRBTCSECNUMCMP(P_L_BT_mac_pair_ask,spp_pair_rampe_OK);
		}
		else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
		{
			P_L_Bx310x_cmd_system_SRBTCSECNUMCMP(P_L_BT_mac_pair_ask,spp_pair_rampe_OK);
		}
		else
		{
			P_L_Bx310x_cmd_system_SRBTCSECNUMCMP(P_L_BT_mac_pair_ask,P_L_Bp1.State);
		}

    }
    else  if (0 == strncmp((char*)"+SRBTPAIR:",(char*)received_string,10))
    {
      P_L_BT_pair_ask = FALSE;
    }
    else if (0 == strncmp((char*)"+SRBTADDR:",(char*)received_string,10))
    {

        P_L_BT_name_mac[0] = 'M';
        P_L_BT_name_mac[1] = 'e';
        P_L_BT_name_mac[2] = 'r';
        P_L_BT_name_mac[3] = 'c';
        P_L_BT_name_mac[4] = '_';

        if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
       	{
        	P_L_BT_name_mac[5] = 'C';
			P_L_BT_name_mac[6] = 'M';
			P_L_BT_name_mac[7] = 'D';
			P_L_BT_name_mac[8] = '_';
			P_L_BT_name_mac[9] = 'R';
			P_L_BT_name_mac[10] = 'A';
			P_L_BT_name_mac[11] = 'M';
			P_L_BT_name_mac[12] = 'P';
			P_L_BT_name_mac[13] = 'E';
			P_L_BT_name_mac[14] = ' ';
			P_L_BT_name_mac[15] = ' ';
			P_L_BT_name_mac[16] = ' ';
       	}
        else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
        {
        	P_L_BT_name_mac[5] = 'R';
			P_L_BT_name_mac[6] = 'A';
			P_L_BT_name_mac[7] = 'M';
			P_L_BT_name_mac[8] = 'P';
			P_L_BT_name_mac[9] = 'E';
			P_L_BT_name_mac[10] = ' ';
			P_L_BT_name_mac[11] = ' ';
			P_L_BT_name_mac[12] = ' ';
			P_L_BT_name_mac[13] = ' ';
			P_L_BT_name_mac[14] = ' ';
			P_L_BT_name_mac[15] = ' ';
			P_L_BT_name_mac[16] = ' ';
        }
        else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
		{
        	P_L_BT_name_mac[5] = 'W';
			P_L_BT_name_mac[6] = 'I';
			P_L_BT_name_mac[7] = 'F';
			P_L_BT_name_mac[8] = 'I';
			P_L_BT_name_mac[9] = '_';
			P_L_BT_name_mac[10] = 'R';
			P_L_BT_name_mac[11] = 'A';
			P_L_BT_name_mac[12] = 'M';
			P_L_BT_name_mac[13] = 'P';
			P_L_BT_name_mac[14] = 'E';
			P_L_BT_name_mac[15] = ' ';
			P_L_BT_name_mac[16] = ' ';
		}
        else
        {
        	P_L_BT_name_mac[5] = received_string[12];
        	P_L_BT_name_mac[6] = received_string[13];

			P_L_BT_name_mac[7] = received_string[15];
			P_L_BT_name_mac[8] = received_string[16];

			P_L_BT_name_mac[9] = received_string[18];
			P_L_BT_name_mac[10] = received_string[19];

			P_L_BT_name_mac[11] = received_string[21];
			P_L_BT_name_mac[12] = received_string[22];

			P_L_BT_name_mac[13] = received_string[24];
			P_L_BT_name_mac[14] = received_string[25];

			P_L_BT_name_mac[15] = received_string[27];
			P_L_BT_name_mac[16] = received_string[28];
        }

        ptr_data = 12;
        P_L_BT_Module_Mac.addr[0] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_BT_Module_Mac.addr[1] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_BT_Module_Mac.addr[2] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_BT_Module_Mac.addr[3] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_BT_Module_Mac.addr[4] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,':');
        P_L_BT_Module_Mac.addr[5] =  P_L_Bx310x_Read_All_Hexa(&ptr_data,received_string,'"');

        P_L_BT_ssid_mac_lue = TRUE;

        P_L_Bx310x_goto_state(Bx310x_state_SRBTNAME);
    }
    else if (0 == strncmp((char*)"+SRSPPCNX:",(char*)received_string,10))
    {
#ifdef _RAMPE_AUTONOME
    	//if (APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME)
    	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI) )
    	{
			ptr_data = 11;
			client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
			if (client_id == P_L_BT_spp_rampe_id)
			{
				//ben modif a valider est simplidfier par une fonction le 10/01/2023
				if(received_string[ptr_data] == '1')
				{
					P_L_BT_spp_rampe_state = BT_connected;

					client_ip.ip[0] = client_id;
					client_ip.ip[1] = 0;
					client_ip.ip[2] = 0;
					client_ip.ip[3] = 0;

					endpoint = P_L_Wifi_Found_AP_IP(client_ip);
					if (0xFF == endpoint)
					{
						P_L_Wifi_Record_AP(client_ip,client_id,TRUE);
					}
				}
				else if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
				{
					P_L_BT_spp_rampe_state = BT_disconnected;

					endpoint = P_L_Wifi_Found_AP_ID(client_id);
					P_L_Wifi_Remove_AP(client_id);

					if (0xFF != endpoint)
					{
						 APP_Manage_Wifi_Find_Client_Leave(endpoint);
					}
				}

				 P_L_Bx310x_goto_state(Bx310x_state_FINIT);
			}
    	}
#ifdef _PROJECTEUR_BT
		else if ((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
#else
    	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
#endif
    	{
    		ptr_data = 11;
			client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
			if (client_id == P_L_BT_spp_rampe_id)
			{
				//ben modif a valider est simplidfier par une fonction le 10/01/2023
				if(received_string[ptr_data] == '1')
				{
					P_L_BT_spp_rampe_state = BT_connected;

					client_ip.ip[0] = client_id;
					client_ip.ip[1] = 0;
					client_ip.ip[2] = 0;
					client_ip.ip[3] = 0;

					endpoint = P_L_Wifi_Found_AP_IP(client_ip);
					if (0xFF == endpoint)
					{
						P_L_Wifi_Record_AP(client_ip,client_id,TRUE);
					}
				}
				else
				{
					P_L_BT_spp_rampe_state = BT_disconnected;

					endpoint = P_L_Wifi_Found_AP_ID(client_id);
					P_L_Wifi_Remove_AP(client_id);

					if (0xFF != endpoint)
					{
						 APP_Manage_Wifi_Find_Client_Leave(endpoint);
					}
				}
			}
    	}
    	else
    	{
    		ptr_data = 11;
			client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

			if(received_string[ptr_data] == '1')
			{
				client_ip.ip[0] = client_id;
				client_ip.ip[1] = 0;
				client_ip.ip[2] = 0;
				client_ip.ip[3] = 0;

				endpoint = P_L_Wifi_Found_AP_IP(client_ip);
				if (0xFF == endpoint)
				{
					P_L_Wifi_Record_AP(client_ip,client_id,TRUE);
				}
			}
			else
			{
				endpoint = P_L_Wifi_Found_AP_ID(client_id);
				P_L_Wifi_Remove_AP(client_id);

				if (0xFF != endpoint)
				{
					 APP_Manage_Wifi_Find_Client_Leave(endpoint);
				}
			}
    	}
#else
        ptr_data = 11;
        client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

        if(received_string[ptr_data] == '1')
        {
            client_ip.ip[0] = client_id;
            client_ip.ip[1] = 0;
            client_ip.ip[2] = 0;
            client_ip.ip[3] = 0;

            endpoint = P_L_Wifi_Found_AP_IP(client_ip);
            if (0xFF == endpoint)
            {
                P_L_Wifi_Record_AP(client_ip,client_id,TRUE);
            }
        }
        else
        {
            endpoint = P_L_Wifi_Found_AP_ID(client_id);
            P_L_Wifi_Remove_AP(client_id);

            if (0xFF != endpoint)
            {
                 APP_Manage_Wifi_Find_Client_Leave(endpoint);
            }
        }
#endif
    }
    else if (0 == strncmp((char*)"+SRSPPCLOSE:",(char*)received_string,12))
    {
#ifdef _RAMPE_AUTONOME
    	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
    	{
			ptr_data = 13;
			client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
			if (client_id == P_L_BT_spp_rampe_id)
			{
				if(received_string[ptr_data] == '1')
				{
					P_L_BT_spp_rampe_state = BT_connected;
				}
				else
				{
					P_L_BT_spp_rampe_state = BT_disconnected;
				}
			}
    	}
#ifdef _PROJECTEUR_BT
		else if ((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
#else
    	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
#endif
    	{
    		ptr_data = 13;
			client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
			if (client_id == P_L_BT_spp_rampe_id)
			{
				if(received_string[ptr_data] == '1')
				{
					P_L_BT_spp_rampe_state = BT_connected;
				}
				else
				{
					P_L_BT_spp_rampe_state = BT_disconnected;
				}
			}
    	}
    	else
    	{
    		ptr_data = 13;
    		client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
			endpoint = P_L_Wifi_Found_AP_ID(client_id);
			P_L_Wifi_Remove_AP(client_id);

			if (0xFF != endpoint)
			{
				 APP_Manage_Wifi_Find_Client_Leave(endpoint);
			}
    	}

#else
        ptr_data = 13;
        client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
        endpoint = P_L_Wifi_Found_AP_ID(client_id);
        P_L_Wifi_Remove_AP(client_id);

        if (0xFF != endpoint)
        {
             APP_Manage_Wifi_Find_Client_Leave(endpoint);
        }
#endif
    }

    else if (0 == strncmp((char*)"+SRBTCFG:",(char*)received_string,9))
    {
#ifdef _RAMPE_AUTONOME
        if (TRUE)//((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
        {
        	if (0 == strncmp((char*)"SPP",(char*)&received_string[34],3))
        	{
                //if (0 == strncmp((char*)&P_L_BT_spp_mac_pair_rampe,(char*)&received_string[15],P_L_BT_MAC_SIZE))
        		// if (0 == strncmp((char*)&P_L_BT_spp_mac_pair_rampe,(char*)&received_string[15],P_L_BT_MAC_SIZE))
        		// {

                // //Copie nouvelle addresse mac:
                    strncpy((char*)P_L_BT_spp_mac_pair_rampe, (char*)&received_string[15], 17);
                    //depassement j'imagine
                    //P_L_BT_spp_mac_pair_rampe[17] = '\0';

        			ptr_data= 10;
                    //id
        			P_L_BT_spp_rampe_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
                    //Cette ligne récupère le statut de connexion SPP
        			// P_L_BT_spp_rampe_state =  received_string[ptr_data];
                    P_L_BT_spp_start_rampe = TRUE;
                    P_L_BT_spp_rampe_state =  BT_connected;

        			// if (P_L_BT_spp_rampe_state == BT_connected)
        			// {
        			// 	P_L_BT_spp_start_rampe = TRUE;
        			// }
        			// else
        			// {
        			// 	P_L_BT_spp_start_rampe = FALSE;
        			// }
        		//}
        	}
        }
#ifdef _PROJECTEUR_BT
		else if ((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
#else
        else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
#endif
        {
        	if (0 == strncmp((char*)"SPP",(char*)&received_string[34],3))
			{
				ptr_data= 10;
				P_L_BT_spp_rampe_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');
				P_L_BT_spp_rampe_state =  received_string[ptr_data];
			}
        }
        else
        {
            P_L_BT_state =  received_string[12];
#ifdef _GESTION_AUDIO_BT
            if (BT_disconnected ==  P_L_BT_state)
            {
                P_L_BT_audio_state = audio_stream_stopped;
                HWI_I2S_Stop();
            }
#endif
        }
#else
        P_L_BT_state =  received_string[12];
#ifdef _GESTION_AUDIO_BT
        if (BT_disconnected ==  P_L_BT_state)
        {
            P_L_BT_audio_state = audio_stream_stopped;
            HWI_I2S_Stop();
        }
#endif
#endif
    }
#endif

#ifdef _TELECOMMANDE_BT
    else if (0 == strncmp((char*)"+SRBTINQ:",(char*)received_string,9))
    {
        for (boucle=0;boucle<ptr_received_string;boucle++)
        {
            data_inq[reponse][boucle] = received_string[boucle];
        }

        reponse++;
    }
    else if (0 == strncmp((char*)"+SRBLESCAN:",(char*)received_string,11))
    {
        for (boucle=0;boucle<ptr_received_string;boucle++)
        {
            data_ble[reponse][boucle] = received_string[boucle];
        }

        if (0 == strncmp((char*)"+SRBLESCAN: \"ff:ff:99:00:59:9e\"",(char*)received_string,31))
        {
           found = TRUE;
           if (P_L_Bx310x_futur_state == Bx310x_state_start_general_inquiry)
           {
                P_L_Bx310x_goto_state(Bx310x_state_start_pair_request);
           }
        }
        reponse++;
    }
#endif
#ifdef _BLUETOOTH_REPROG
    else if (0 == strncmp((char*)"+SRBTCPIN:",(char*)received_string,10))
    {
        P_L_Bx310x_cmd_system_SRBTCPIN(&received_string[11],TRUE,(U8*)"0000");
    }
#endif
    else if (0 == strncmp((char*)"+SRSPP_ERROR:",(char*)received_string,13))
    {
    	P_L_Bx310x_goto_state(Bx310x_state_FINIT);
    }
    else if (0 == strncmp((char*)"+SRSPP_DATA:",(char*)received_string,12))
    {
#ifdef _RAMPE_AUTONOME
#ifdef _PROJECTEUR_BT
		//if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)||(APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
        
        //ADD
        if(TRUE)
        #else
    	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
#endif
    	{
    		P_L_Bx310x_BT_RX_Analyse(12,received_string);

    	}
    	else
    	{
    		P_L_Bx310x_WIFI_RX_Analyse(12,received_string);
    	}
#else
#ifdef _BLUETOOTH
    	P_L_Bx310x_WIFI_RX_Analyse(12,received_string);
#endif
#endif
    }
    else if (0 == strncmp((char*)"+SRWSTASTATUS:",(char*)received_string,14))
    {

        if (0 == strncmp((char*)"1",(char*)(&received_string[15]),1))
        {
            if (0 == strncmp((const char *)P_L_Wifi_ssid_mac_a_tester,(char*)(&received_string[17]),17))
            {
               P_L_Wifi_ATester_vue = TRUE;
            }
            else if (0 == strncmp((const char *)P_L_CarteSd_ssid_mac_MAJ,(char*)(&received_string[17]),strlen((const char *)P_L_CarteSd_ssid_mac_MAJ)-1))
            {
               P_L_Wifi_MiseAJour_vue = TRUE;
            }
        }
        else
        {
            if (0 == strncmp((const char *)P_L_Wifi_ssid_mac_a_tester,(char*)(&received_string[17]),17))
            {
               P_L_Wifi_ATester_vue = FALSE;
            }
            else if (0 == strncmp((const char *)P_L_CarteSd_ssid_mac_MAJ,(char*)(&received_string[17]),strlen((const char *)P_L_CarteSd_ssid_mac_MAJ)-1))
            {
               P_L_Wifi_MiseAJour_vue = FALSE;
            }
        }

        //"+SRWSTASTATUS: 1,\"Merc_cc934a00778d\",\"cc:93:4a:00:77:8d\",11,3
    }
    else if (0 == strncmp((char*)"+SRWSTAIP:",(char*)received_string,10))
    {
        //"+SRWSTAIP: \"192.168.1.1\",\"255.255.255.0\",\"192.168.1.254\"
        if (TRUE == P_L_CarteSd_MiseAJour_vue)
        {
            if (TRUE == P_L_Wifi_MiseAJour_vue)
            {
               P_L_Bx310x_goto_state(Bx310x_state_WDSI);
            }
        }
        else if (TRUE == P_L_CarteSd_Testeur_vue)
        {
            P_L_Bx310x_goto_state(Bx310x_state_KTCPCFG);
        }
        else
        {

        }
    }
    else
    {
        if (0 == strncmp((char*)"ets",(char*)received_string,3))
        {

        }
        else if (0 == strncmp((char*)"rst",(char*)received_string,3))
        {

        }
        else if (0 == strncmp((char*)"configsip:",(char*)received_string,10))
        {

        }
        else if (0 == strncmp((char*)"clk_drv:",(char*)received_string,8))
        {

        }
        else if (0 == strncmp((char*)"mode:",(char*)received_string,5))
        {

        }
        else if (0 == strncmp((char*)"load:",(char*)received_string,5))
        {

        }
        else if (0 == strncmp((char*)"Sierra",(char*)received_string,6))
        {

        }
        else if (0 == strncmp((char*)"secure",(char*)received_string,6))
        {

        }
        else if (0 == strncmp((char*)"Sets",(char*)received_string,4))
        {

        }
        else
        {
            // do nothing
#ifdef _DEBUG_BX310X_COM
            P_L_Bx310x_CMD_UNKNOW++;
#endif
        }

    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_send_data_wifi
 *  Description     : envoie de donne en wifi attention MAX 242 octets
 *
 *
 *******************************************************************************/
void P_L_BT_Tx_data(U32 client_id,U8 data_len,U8* data)
{
	P_L_BT_Tx_dataDirect(client_id,data_len,data);
}
/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_send_data_wifi
 *  Description     : envoie de donne en wifi attention MAX 242 octets
 *
 *
 *******************************************************************************/

 //ENVOIE BLUETOOTH
// ATTENTION MAX 242 octets
void P_L_BT_Tx_dataDirect(U32 client_id,U8 data_len,U8* data)
{
#ifndef _DEBUG_BX310X_COM
U8 data_send[242];
#endif
U16 ptr_data_send;
U16 boucle;
U8 cheksum_1;
U8 cheksum_2;

    strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);//commande bluetooth
    ptr_data_send = 12;

    P_L_Bx310x_Write_number(&ptr_data_send,data_send,client_id);
    data_send[ptr_data_send] = ',';
    ptr_data_send++;

    ///////////////////////////////////////////////
    //copie du mot code
    for(boucle=0;boucle<TAILLE_WRD_BT;boucle++)
	{
    	data_send[ptr_data_send] = data[boucle];
    	ptr_data_send++;
	}

    cheksum_1 = 0;
    cheksum_2 = 0;

    // for (boucle=0;boucle<data_len;boucle++)
	// {
	// 	cheksum_1 = cheksum_1 + data[boucle];
	// 	cheksum_2 = cheksum_2 + (data[boucle]*(boucle+1));
	// }

    for(boucle=4;boucle<data_len;boucle++)
    {
    	P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,data[boucle]);
    }

    P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,cheksum_1);
    P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,cheksum_2);

    //strncpy((char*)&(data_send[ptr_data_send]),(char*)data,data_len);
    //ptr_data_send = ptr_data_send+data_len;
    HWI_Bx310x_tx(ptr_data_send,data_send);

}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_software_version
*  Description     : Commande AT+CGMR - Lecture version firmware du module
*                    Retourne la version du firmware du module BX310 (fonction désactivée)
*
*******************************************************************************/
/*
static void P_L_Bx310x_cmd_software_version()
{
    // AT+CGMR : Demande la version du firmware du module
    // Retourne des informations sur la version logicielle installée
    HWI_Bx310x_tx(7,(U8*)"AT+CGMR");
}
*/

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_ati
*  Description     : Commande ATI - Demande l'identification du module
*                    Retourne les informations du module (modèle, version, etc.)
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_ati()
{
    // ATI : Commande AT standard d'identification du module
    // Retourne des informations comme le nom du module, version firmware, etc.
    HWI_Bx310x_tx(3,(U8*)"ATI");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_module_and_cfg_reset
*  Description     : Commande AT&F - Reset complet du module et restauration 
*                    des paramètres d'usine par défaut
*
*******************************************************************************/
static void P_L_Bx310x_cmd_module_and_cfg_reset()
{
    // AT&F : Reset du module et restauration des paramètres d'usine
    // Efface toute la configuration utilisateur et remet les valeurs par défaut
    HWI_Bx310x_tx(4,(U8*)"AT&F");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_module_reset
*  Description     : Commande AT+RST - Redémarrage logiciel du module
*                    Le module redémarre et recharge sa configuration
*
*******************************************************************************/
static void P_L_Bx310x_cmd_module_reset()
{
    // AT+RST : Redémarrage logiciel du module BX310
    // Équivalent à un reset sans perdre la configuration
    HWI_Bx310x_tx(6,(U8*)"AT+RST");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_ctrl_flux
*  Description     : Commande AT&K3 - Configuration du contrôle de flux RTS/CTS
*                    Active le contrôle de flux matériel sur l'interface UART
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_ctrl_flux()
{
    // AT&K3 : Active le contrôle de flux RTS/CTS 
    // Permet de gérer le flux de données entre le MCU et le module BX310
    HWI_Bx310x_tx(5,(U8*)"AT&K3");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_RWCFG
*  Description     : Commande AT+SRWCFG - Configuration du mode WiFi
*                    Mode 1 : WiFi Station, Mode 2 : WiFi Point d'Accès
*                    Paramètre 3 : Mode mixte Station + Point d'Accès
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_RWCFG()
{
    if(TRUE == P_L_CarteSd_Testeur_vue)
    {
        // AT+SRWCFG=1,3 : Mode Station WiFi + mode mixte
        // Utilisé en mode testeur pour se connecter à un réseau existant
        HWI_Bx310x_tx(13,(U8*)"AT+SRWCFG=1,3");
    }
    else
    {
        // AT+SRWCFG=2,3 : Mode Point d'Accès WiFi + mode mixte  
        // Crée un point d'accès WiFi pour que les clients se connectent
        HWI_Bx310x_tx(13,(U8*)"AT+SRWCFG=2,3");
    }
}


/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_Ask_RWCFG
*  Description     : Commande AT+SRWCFG? - Lecture de la configuration WiFi courante
*                    Retourne le mode WiFi actuellement configuré
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_Ask_RWCFG()
{
    // AT+SRWCFG? : Interroge la configuration WiFi actuelle
    // Retourne les paramètres de mode WiFi configurés
    HWI_Bx310x_tx(10,(U8*)"AT+SRWCFG?");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_RWAPCFG
*  Description     : Commande AT+SRWAPCFG - Configuration du Point d'Accès WiFi
*                    Configure le SSID, mot de passe, sécurité WPA2, canal 11, beacon 100ms
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_RWAPCFG()
{

U8 data_send[300];
U16 ptr_data_send;

    ptr_data_send = 0;
    strncpy((char*)data_send,(char*)"AT+SRWAPCFG=\"",13);
    ptr_data_send = ptr_data_send + 13;
    // Utilise le SSID configuré (basé sur l'adresse MAC)
    strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_ssid_mac,17);
    ptr_data_send = ptr_data_send + 17;
    strncpy((char*)&data_send[ptr_data_send],(char*)"\",\"",3);
    ptr_data_send = ptr_data_send + 3;

    // Ajoute le mot de passe WPA2
    strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_Clef_wep,P_L_WIFI_CLEF_WEP_SIZE);
    ptr_data_send = ptr_data_send + P_L_WIFI_CLEF_WEP_SIZE;

    //wifi chanel auto
/*
    strncpy(&data_send[ptr_data_send],"\",3,0,0,100",11);
    ptr_data_send = ptr_data_send + 11;
*/

    //wifi chanel 11 beacon interval 100
    //strncpy((char*)&data_send[ptr_data_send],(char*)"\",3,11,0,100",12);
    //ptr_data_send = ptr_data_send + 12;

     //wifi chanel 11 beacon interval 100

    // AT+SRWAPCFG="SSID","password",3,11,0,100
    // 3=WPA2, 11=canal WiFi, 0=pas de canal caché, 100=intervalle beacon
    strncpy((char*)&data_send[ptr_data_send],(char*)"\",3,11,0,100",12);
    ptr_data_send = ptr_data_send + 12;

    HWI_Bx310x_tx(ptr_data_send,data_send);
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_RWAPCFGNU
*  Description     : Commande AT+SRWAPCFG - Configuration Point d'Accès "Non Utilisé"
*                    Configure un AP avec SSID "Do not use000000" pour désactivation
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_RWAPCFGNU()
{
U8 data_send[300];
U16 ptr_data_send;

	// Force le SSID à "Do not use000000" pour indiquer un AP non utilisé
	P_L_Wifi_ssid_mac[0] = 'D';
	P_L_Wifi_ssid_mac[1] = 'o';
	P_L_Wifi_ssid_mac[2] = ' ';
	P_L_Wifi_ssid_mac[3] = 'n';
	P_L_Wifi_ssid_mac[4] = 'o';
	P_L_Wifi_ssid_mac[5] = 't';
	P_L_Wifi_ssid_mac[6] = ' ';

	P_L_Wifi_ssid_mac[7] = 'u';
	P_L_Wifi_ssid_mac[8] = 's';

	P_L_Wifi_ssid_mac[9] = 'e';
	P_L_Wifi_ssid_mac[10] = '0';

	P_L_Wifi_ssid_mac[11] = '0';
	P_L_Wifi_ssid_mac[12] = '0';

	P_L_Wifi_ssid_mac[13] = '0';
	P_L_Wifi_ssid_mac[14] = '0';

	P_L_Wifi_ssid_mac[15] = '0';
	P_L_Wifi_ssid_mac[16] = '0';

	ptr_data_send = 0;
	strncpy((char*)data_send,(char*)"AT+SRWAPCFG=\"",13);
	ptr_data_send = ptr_data_send + 13;
	strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_ssid_mac,17);
	ptr_data_send = ptr_data_send + 17;
	strncpy((char*)&data_send[ptr_data_send],(char*)"\",\"",3);
	ptr_data_send = ptr_data_send + 3;

	strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_Clef_wep,P_L_WIFI_CLEF_WEP_SIZE);
	ptr_data_send = ptr_data_send + P_L_WIFI_CLEF_WEP_SIZE;

	// AT+SRWAPCFG="Do not use000000","password",3,11,0,100
	// Configure un AP avec un nom indiquant qu'il ne doit pas être utilisé
	strncpy((char*)&data_send[ptr_data_send],(char*)"\",3,11,0,100",12);
	ptr_data_send = ptr_data_send + 12;

	HWI_Bx310x_tx(ptr_data_send,data_send);
	P_L_Bx310x_goto_state(Bx310x_state_SRBTSYSTEM_CFG);
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_RWAPNETCFG
*  Description     : Commande AT+SRWAPNETCFG - Configuration réseau du Point d'Accès
*                    Configure IP, passerelle, plage DHCP et durée de bail
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_RWAPNETCFG()
{
    // AT+SRWAPNETCFG=1,"192.168.1.254","192.168.1.1","192.168.1.100",120
    // 1=DHCP activé, IP du module, passerelle, IP de début DHCP, durée bail (120s)
    HWI_Bx310x_tx(66,(U8*)"AT+SRWAPNETCFG=1,\"192.168.1.254\",\"192.168.1.1\",\"192.168.1.100\",120");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_KTCPCFG
*  Description     : Commande AT+KTCPCFG - Configuration du serveur TCP
*                    Configure l'écoute TCP sur le port 80 (HTTP)
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_KTCPCFG()
{
    if(TRUE == P_L_CarteSd_Testeur_vue)
    {
        // AT+KTCPCFG=,0,192.168.1.254,80 : Mode client TCP vers IP fixe
        // Se connecte à un serveur TCP externe sur le port 80
        HWI_Bx310x_tx(30,(U8*)"AT+KTCPCFG=,0,192.168.1.254,80");
    }
    else
    {
        // AT+KTCPCFG=,1,,80 : Mode serveur TCP sur port 80
        // Ouvre un serveur TCP qui écoute les connexions entrantes
        HWI_Bx310x_tx(17,(U8*)"AT+KTCPCFG=,1,,80");
    }
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_KTCPCNX
*  Description     : Commande AT+KTCPCNX - Activation de la connexion TCP
*                    Démarre la connexion TCP configurée précédemment
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_KTCPCNX()
{
U8 	Buffer_sendTest[20];
U16 ptr_data_send;

    if(TRUE == P_L_CarteSd_Testeur_vue)
    {
        // AT+KTCPCNX={endpoint} : Active la connexion TCP client vers endpoint spécifique
        strncpy((char*)Buffer_sendTest,(char*)"AT+KTCPCNX=",11);

        ptr_data_send = 11;

        P_L_Bx310x_Write_number(&ptr_data_send,Buffer_sendTest,(U32)P_L_Wifi_endpointTest);

        HWI_Bx310x_tx(ptr_data_send,Buffer_sendTest);
    }
    else
    {
        // AT+KTCPCNX=1 : Active le serveur TCP sur le socket 1
        // Le serveur commence à écouter les connexions entrantes
        HWI_Bx310x_tx(12,(U8*)"AT+KTCPCNX=1");
    }
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTSYSTEM_0
*  Description     : Commande AT+SRBTSYSTEM=0 - Désactivation complète du Bluetooth
*                    Éteint le sous-système Bluetooth du module BX310
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTSYSTEM_0()
{
  // AT+SRBTSYSTEM=0 : Désactive complètement le Bluetooth
  // Économise l'énergie si seul le WiFi est utilisé
  HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=0");
}

#ifdef _BLUETOOTH
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTADDR
*  Description     : Commande AT+SRBTADDR? - Lecture de l'adresse MAC Bluetooth
*                    Retourne l'adresse MAC unique du module Bluetooth
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTADDR()
{
    // AT+SRBTADDR? : Interroge l'adresse MAC Bluetooth du module
    // Retourne une adresse unique de type "XX:XX:XX:XX:XX:XX"
    HWI_Bx310x_tx(12,(U8*)"AT+SRBTADDR?");
}
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTNAME
*  Description     : Commande AT+SRBTNAME - Configuration du nom Bluetooth visible
*                    Définit le nom que les autres appareils verront lors du scan
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTNAME()
{
// U8 data_send[300];
// U16 ptr_data_send;

//     ptr_data_send = 0;
//     // AT+SRBTNAME={nom} : Configure le nom Bluetooth visible
//     strncpy((char*)data_send,(char*)"AT+SRBTNAME=",12);
//     ptr_data_send = ptr_data_send + 12;
// #ifdef _RAMPE_AUTONOME
// #ifdef _PROJECTEUR_BT
// 	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)||(APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
// #else
//     if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
// #endif
//     {
//     	// Utilise le nom BT spécifique pour les rampes autonomes
//     	strncpy((char*)&data_send[ptr_data_send],(char*)P_L_BT_name_mac,17);
//     }
//     else
//     {
//     	// Utilise le SSID WiFi comme nom Bluetooth par défaut
//     	strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_ssid_mac,17);
//     }
// #else
//     strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_ssid_mac,17);
// #endif
    // ptr_data_send = ptr_data_send + 17;
    // HWI_Bx310x_tx(ptr_data_send,data_send);

    HWI_Bx310x_tx(22,(U8*)"AT+SRBTNAME=BT_MERCURA");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTCSECPARAMS
*  Description     : Commande AT+SRBTCSECPARAMS - Configuration sécurité Bluetooth
*                    Paramètre 1: Pairing autorisé, 3: Pairing + authentification renforcée
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTCSECPARAMS()
{
#ifdef _RAMPE_AUTONOME
#ifdef _PROJECTEUR_BT
	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)||(APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT))
#else
	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE)||(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
#endif
	{
		//HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=3");
		//HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=1");  // Ancienne config avec PIN
		// AT+SRBTCSECPARAMS=0 : Désactive l'authentification (pas de PIN requis)
		HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=0");
	}
	else
	{
		//HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=1");  // Ancienne config avec PIN
		// AT+SRBTCSECPARAMS=0 : Désactive l'authentification (pas de PIN requis)
		HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=0");
	}
#else
    //HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=1");  // Ancienne config avec PIN
    // AT+SRBTCSECPARAMS=0 : Désactive l'authentification (pas de PIN requis)
    // 0=Pas d'authentification, 1=Pairing autorisé, 3=Sécurité renforcée
    HWI_Bx310x_tx(19,(U8*)"AT+SRBTCSECPARAMS=0");
#endif

}


/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTSYSTEM_CFG
*  Description     : Commande AT+SRBTSYSTEM - Configuration du sous-système Bluetooth
*                    0=Désactivé, 1=BLE seul, 2=Classic seul, 3=BLE+Classic
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTSYSTEM_CFG()
{
// 	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
// 	{
// 		// AT+SRBTSYSTEM=2 : Active uniquement Bluetooth Classic pour rampes
// 		//enable bluetooth classique
// 		HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=2");
// 	}
// 	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
// 	{
// 		// AT+SRBTSYSTEM=2 : Active uniquement Bluetooth Classic  
// 		//enable bluetooth classique
// 		HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=2");
// 	}
// 	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
// 	{
// 		// AT+SRBTSYSTEM=0 : Désactive Bluetooth pour mode WiFi seul
// 		HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=0");
// 	}
// #ifdef _PROJECTEUR_BT
// 	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT )
// 	{
// 		// AT+SRBTSYSTEM=2 : Active Bluetooth Classic pour projecteur
// 		//enable bluetooth classique
// 		HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=2");
// 	}
// #endif
// 	else
// 	{
// #ifdef _BT_CLASSIQUE
// #ifdef _BT_BLE
// 		// AT+SRBTSYSTEM=3 : Active Bluetooth Classic ET BLE
// 		//enable bluetooth classique et BLE
// 		HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=3");
// #else
    	// AT+SRBTSYSTEM=2 : Active uniquement Bluetooth Classic
    	//enable bluetooth classique
    	HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=2");
// #endif
// #else
//     	// AT+SRBTSYSTEM=1 : Active uniquement Bluetooth Low Energy (BLE)
//     	//enable bluetooth BLE
//     	HWI_Bx310x_tx(15,(U8*)"AT+SRBTSYSTEM=1");
// #endif
// 	}
}


#ifdef _GESTION_AUDIO_BT
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_KI2SCFG
*  Description     : Commande AT+KI2SCFG - Configuration interface I2S pour audio Bluetooth
*                    Configure les GPIO et format audio pour A2DP/streaming audio
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_KI2SCFG()
{
/*
	I2S port number 			0
		enable					0 : non ; 1 : oui
		communication format	0 : I2S MSB format ; 1 : I2S LSB format ; 2 : PCM Short format ; 3 PCM Long format
		channel format			0 : Right and Left ; 1 : All Right ; 2 : All Left ; 3 : Only Right ; 4 : Only Left
		mclk enable				0 : non ; 1 : oui
		bck						26 Gpio number
		ws						18 Gpio number
		data out				32 Gpio number
		data in					33 Gpio number
*/


#ifdef _PCM
	// AT+KI2SCFG=0,1,3,2,0,26,18,32,33 : Configuration I2S pour format PCM
	// Port 0, activé, PCM Long format, stéréo, pas de MCLK, GPIO 26/18/32/33
	HWI_Bx310x_tx(32,(U8*)"AT+KI2SCFG=0,1,3,2,0,26,18,32,33");
#else
	// AT+KI2SCFG=0,1,0,2,0,26,18,32,33 : Configuration I2S pour format I2S standard
	// Port 0, activé, I2S MSB format, stéréo, pas de MCLK, GPIO 26/18/32/33
	HWI_Bx310x_tx(32,(U8*)"AT+KI2SCFG=0,1,0,2,0,26,18,32,33");
#endif

/*
#ifdef _PCM
    HWI_Bx310x_tx(34,(U8*)"AT+KI2SCFG=0,1,0,0,3,2,26,18,32,33");
#else
   HWI_Bx310x_tx(34,(U8*)"AT+KI2SCFG=0,1,0,0,0,2,26,18,32,33");
#endif
*/
}
#endif


#ifdef _TELECOMMANDE_BT
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTINQ
*  Description     : Commande AT+SRBTINQ - Lancement scan des périphériques Bluetooth Classic
*                    Recherche les appareils Bluetooth Classic dans la zone
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTINQ()
{
    // AT+SRBTINQ=2,1,0 : Scan Bluetooth Classic pendant 2*1.28s, 1 réponse max, pas de limitation de classe
    // Découvre les périphériques Bluetooth Classic appairables à proximité
    HWI_Bx310x_tx(17,(U8*)"AT+SRBTINQ=2,1,0");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBLESCAN
*  Description     : Commande AT+SRBLESCAN - Lancement scan des périphériques BLE
*                    Recherche les appareils Bluetooth Low Energy dans la zone
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBLESCAN()
{
   // AT+SRBLESCAN=10,0 : Scan BLE pendant 10 secondes, tous types d'appareils
   // Découvre les périphériques BLE (Bluetooth Low Energy) à proximité
   HWI_Bx310x_tx(17,(U8*)"AT+SRBLESCAN=10,0");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTPAIR
*  Description     : Commande AT+SRBTPAIR - Appairage avec un périphérique Bluetooth
*                    Lance l'appairage avec une adresse MAC Bluetooth spécifique
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTPAIR()
{
    // AT+SRBTPAIR=ff:ff:99:00:59:9e : Lance l'appairage avec l'adresse MAC spécifiée
    // Initie la procédure d'appairage Bluetooth avec l'appareil cible
    HWI_Bx310x_tx(29,(U8*)"AT+SRBTPAIR=ff:ff:99:00:59:9e");
}
#endif


#ifdef _RAMPE_AUTONOME
/*******************************************************************************
*
*  Function Name   : Bx310x_state_BT_SRSPPCFG
*  Description     : Commande AT+SRSPPCFG - Configuration Serial Port Profile Bluetooth
*                    Configure une connexion série Bluetooth avec une adresse MAC
*
*******************************************************************************/
static void Bx310x_state_BT_SRSPPCFG(U8 *mac)
{
U8 data_send[300];
U16 ptr_data_send;

	ptr_data_send = 0;
	// AT+SRSPPCFG={mac_address} : Configure SPP pour l'adresse MAC spécifiée
	strncpy((char*)data_send,(char*)"AT+SRSPPCFG=",12);
	ptr_data_send = ptr_data_send + 12;
	// Ajoute l'adresse MAC du périphérique cible pour la connexion SPP
	for (U8 boucle=0;boucle<P_L_BT_MAC_SIZE;boucle++)
	{
		data_send[ptr_data_send] = mac[boucle];
		ptr_data_send++;
	}
	HWI_Bx310x_tx(ptr_data_send,data_send);
}
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTINQ
*  Description     :
*
*******************************************************************************/
/*
static void P_L_Bx310x_cmd_system_SRBTINQ()
{
    HWI_Bx310x_tx(17,(U8*)"AT+SRBTINQ=10,1,0");
}
*/
/*******************************************************************************
*
*  Function Name   : Bx310x_state_BT_SRSPPCNX
*  Description     : Commande AT+SRSPPCNX - Activation connexion Serial Port Profile
*                    Démarre la connexion SPP avec l'ID de session configuré
*
*******************************************************************************/
static void Bx310x_state_BT_SRSPPCNX(U8 id)
{
U8 data_send[300];
U16 ptr_data_send;

	ptr_data_send = 0;
	// AT+SRSPPCNX={id} : Active la connexion SPP avec l'ID de session spécifié
	strncpy((char*)data_send,(char*)"AT+SRSPPCNX=",12);
	ptr_data_send = ptr_data_send + 12;
	// Ajoute l'ID de session SPP pour démarrer la connexion
	P_L_Bx310x_Write_number(&ptr_data_send,data_send,id);
	HWI_Bx310x_tx(ptr_data_send,data_send);

}
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTUNPAIR
*  Description     : Commande AT+SRBTUNPAIR - Suppression de tous les appairages Bluetooth
*                    Efface la liste des périphériques appairés stockés dans le module
*
*******************************************************************************/

static void P_L_Bx310x_cmd_system_SRBTUNPAIR()
{
    // AT+SRBTUNPAIR : Supprime tous les appairages Bluetooth stockés
    // Efface la liste complète des périphériques appairés du module
    HWI_Bx310x_tx(13,(U8*)"AT+SRBTUNPAIR");
}
/*******************************************************************************
*
*  Function Name   : Bx310x_state_BT_SRBTPAIR
*  Description     :
*
*******************************************************************************/
/*
static void Bx310x_state_BT_SRBTPAIR(U8 *mac)
{
U8 data_send[300];
U16 ptr_data_send;

	ptr_data_send = 0;
	strncpy((char*)data_send,(char*)"AT+SRBTPAIR=",12);
	ptr_data_send = ptr_data_send + 12;
	for (U8 boucle=0;boucle<P_L_BT_MAC_SIZE;boucle++)
	{
		data_send[ptr_data_send] = mac[boucle];
		ptr_data_send++;
	}
	HWI_Bx310x_tx(ptr_data_send,data_send);
}
*/
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTCPINCFG
*  Description     :
*
*******************************************************************************/
/*
static void P_L_Bx310x_cmd_system_SRBTCPINCFG()
{
	HWI_Bx310x_tx(21,(U8*)"AT+SRBTCPINCFG=1,1234");
}
*/
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTPWR
*  Description     :
*
*******************************************************************************/
/*
static void P_L_Bx310x_cmd_system_SRBTPWR()
{
	HWI_Bx310x_tx(17,(U8*)"AT+SRBTCTXPWR=4,7");
}
*/

#endif


#ifdef _BLUETOOTH
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTCSECNUMCMP
*  Description     : Commande AT+SRBTCSECNUMCMP - Confirmation numérique Bluetooth
*                    Confirme ou refuse la comparaison numérique lors de l'appairage SSP
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTCSECNUMCMP(U8* mac,BOOL_TYPE accept)
{
#ifndef _DEBUG_BX310X_COM
U8 data_send[300];
#endif
U16 ptr_data_send;

    ptr_data_send = 0;
    // AT+SRBTCSECNUMCMP="{mac}",{0/1} : Répond à une demande de confirmation numérique
    strncpy((char*)data_send,(char*)"AT+SRBTCSECNUMCMP=\"",19);
    ptr_data_send = ptr_data_send + 19;

    // Ajoute l'adresse MAC du périphérique demandant confirmation
    strncpy((char*)&data_send[ptr_data_send],(const char*)mac,17);

    ptr_data_send = ptr_data_send + 17;
    if (TRUE == accept)
    {
        // 1 = Accepte la comparaison numérique (appairage réussi)
        strncpy((char*)&data_send[ptr_data_send],"\",1",3);
    }
    else
    {
        // 0 = Refuse la comparaison numérique (appairage échoué)
        strncpy((char*)&data_send[ptr_data_send],"\",0",3);
    }
    ptr_data_send = ptr_data_send + 3;

    HWI_Bx310x_tx(ptr_data_send,data_send);
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTCPIN
*  Description     : Commande AT+SRBTCPIN - Réponse au code PIN Bluetooth
*                    Envoie le code PIN lors de l'appairage Bluetooth Classic
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTCPIN(U8* mac,BOOL_TYPE accept,U8 *pincode)
{
#ifndef _DEBUG_BX310X_COM
U8 data_send[300];
#endif
U16 ptr_data_send;

    ptr_data_send = 0;
    // AT+SRBTCPIN={mac},{accept},{pincode} : Réponse à une demande de code PIN
    strncpy((char*)data_send,(char*)"AT+SRBTCPIN=",12);
    ptr_data_send = ptr_data_send + 12;

    // Ajoute l'adresse MAC du périphérique demandant l'appairage
    strncpy((char*)&data_send[ptr_data_send],(const char*)mac,17);

    ptr_data_send = ptr_data_send + 17;
    if (TRUE == accept)
    {
        // Accepte l'appairage et fournit le code PIN
        strncpy((char*)&data_send[ptr_data_send],",1,",3);
    }
    else
    {
        // Refuse l'appairage
        strncpy((char*)&data_send[ptr_data_send],",0,",3);
    }
    ptr_data_send = ptr_data_send + 3;

        // Ajoute le code PIN à 4 chiffres
    strncpy((char*)&data_send[ptr_data_send],(const char*)pincode,4);
    ptr_data_send = ptr_data_send + 4;

    HWI_Bx310x_tx(ptr_data_send,data_send);

}

#endif

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTCPROFILES
*  Description     : Commande AT+SRBTCPROFILES - Configuration des profils Bluetooth
*                    Active les profils: 1=SPP, 2=A2DP, 4=AVRCP, 7=Tous
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTCPROFILES()
{
// 	if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
// 	{
// 		// AT+SRBTCPROFILES=1 : Active uniquement Serial Port Profile pour rampes
// 		//enable profils SSP
// 		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=1");
// 	}
// 	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
// 	{
// 		// AT+SRBTCPROFILES=1 : Active Serial Port Profile pour communication série
// 		//enable profils SSP
// 		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=1");
// 	}
// #ifdef _PROJECTEUR_BT
// 	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT )
// 	{
// 		// AT+SRBTCPROFILES=1 : Active SPP pour projecteur
// 		//enable profils SSP
// 		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=1");
// 	}
// #endif
// 	else
// 	{
// #ifdef _TELECOMMANDE_BT
// 		// AT+SRBTCPROFILES=7 : Active tous les profils (SPP + A2DP + AVRCP)
// 		//enable tous les profils SSP A2DP AVRCP
// 		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=7");
// #endif
// #ifdef _GESTION_AUDIO_BT
// 		// AT+SRBTCPROFILES=2 : Active uniquement A2DP pour streaming audio
// 		//enable tous les profils A2DP
// 		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=2");
// #endif
// #ifdef _DATA_BT_CALSSIQUE_SPP
// 		// AT+SRBTCPROFILES=1 : Active SPP pour transfert de données série
// 		//enable tous les profils SSP
		HWI_Bx310x_tx(18,(U8*)"AT+SRBTCPROFILES=1");
// #endif
// 	}
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTSTATE
*  Description     : Commande AT+SRBTSTATE - Configuration des notifications d'état Bluetooth
*                    Active les notifications d'événements Bluetooth
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTSTATE()
{
    // AT+SRBTSTATE=1,2 : Active les notifications d'état Bluetooth
    // 1=Active les notifications, 2=Format des notifications
    HWI_Bx310x_tx(16,(U8*)"AT+SRBTSTATE=1,2");
}

#endif
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_WDSI
*  Description     : Commande AT+WDSI - Configuration Watchdog Software Interface
*                    Configure le watchdog logiciel (actuellement désactivé)
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_WDSI()
{
    // AT+WDSI=8191 : Configuration watchdog software (commenté - non utilisé)
    // Permettrait de surveiller les timeouts et redémarrages automatiques
    //HWI_Bx310x_tx(12,(U8*)"AT+WDSI=8191");
}

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_WDSS
*  Description     : Commande AT+WDSS - Démarrage Watchdog Software Service
*                    Lance le service de watchdog logiciel (actuellement désactivé)
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_WDSS()
{
    // AT+WDSS=1,1 : Démarrage service watchdog (commenté - non utilisé)
    // Activerait la surveillance automatique des services système
    //HWI_Bx310x_tx(11,(U8*)"AT+WDSS=1,1");
}

#ifdef _PAIR_TO_DEVICE
/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_WDSS
*  Description     :
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTINQ()
{
    //HWI_Bx310x_tx(16,(U8*)"AT+SRBTINQ=1,1,0");
	HWI_Bx310x_tx(13,(U8*)"AT+SRBTUNPAIR");
}


/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRBTPAIR
*  Description     :
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRBTPAIR(U8 *id)
{
	U8 data_send[300];
	U16 ptr_data_send;
	/*
	        ptr_data_send = 0;
	        strncpy((char*)data_send,(char*)"AT+SRBTPAIR=",12);
	        ptr_data_send = ptr_data_send + 12;
	        strncpy((char*)&data_send[ptr_data_send],(const char*)id,17);
	        ptr_data_send = ptr_data_send + 17;
	        HWI_Bx310x_tx(ptr_data_send,data_send);
	*/
	        HWI_Bx310x_tx(29,(U8*)"AT+SRBTPAIR=e0:9d:fa:80:c4:7c");
			// HWI_Bx310x_tx(29,(U8*)"AT+SRBTPAIR=4c:dd:31:27:cf:a2");
	        //HWI_Bx310x_tx(12,(U8*)"AT+SRBTPAIR?");
}

#endif

/*******************************************************************************
*
*  Function Name   : P_L_Bx310x_cmd_system_SRWSTACFG
*  Description     : Commande AT+SRWSTACFG - Configuration WiFi Station (client)
*                    Configure le module pour se connecter à un réseau WiFi existant
*
*******************************************************************************/
static void P_L_Bx310x_cmd_system_SRWSTACFG()
{
#ifndef _DEBUG_BX310X_COM
U8 data_send[300];
#endif
U16 ptr_data_send;
U8 boucle;
U8 taille;

    if (TRUE == P_L_Wifi_ssid_mac_a_tester_recue)
    {
        // Construction du SSID pour mode test : "Merc_" + adresse MAC
        P_L_Wifi_ssid_mac_a_tester[0] = 'M';
        P_L_Wifi_ssid_mac_a_tester[1] = 'e';
        P_L_Wifi_ssid_mac_a_tester[2] = 'r';
        P_L_Wifi_ssid_mac_a_tester[3] = 'c';
        P_L_Wifi_ssid_mac_a_tester[4] = '_';
        ptr_data_send = 5;

        for(boucle=0;boucle<6;boucle++)
        {
            P_L_Bx310x_Write_Hexa(&ptr_data_send,P_L_Wifi_ssid_mac_a_tester,P_L_Wifi_Module_Mac_a_tester.addr[boucle]);
        }

        ptr_data_send = 0;
        // AT+SRWSTACFG="SSID","password",1 : Configuration station WiFi avec auto-connect
        strncpy((char*)data_send,(char*)"AT+SRWSTACFG=\"",14);
        ptr_data_send = ptr_data_send + 14;
        strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_ssid_mac_a_tester,17);
        ptr_data_send = ptr_data_send + 17;
        strncpy((char*)&data_send[ptr_data_send],(char*)"\",\"",3);
        ptr_data_send = ptr_data_send + 3;

        // Ajoute le mot de passe du réseau WiFi cible
        strncpy((char*)&data_send[ptr_data_send],(char*)P_L_Wifi_Clef_wep_a_tester,P_L_WIFI_CLEF_WEP_SIZE);
        ptr_data_send = ptr_data_send + P_L_WIFI_CLEF_WEP_SIZE;

        // 1 = Active la connexion automatique au réseau configuré
        strncpy((char*)&data_send[ptr_data_send],(char*)"\",1",3);
        ptr_data_send = ptr_data_send + 3;

        HWI_Bx310x_tx(ptr_data_send,data_send);
    }
    else
    {
        // Utilise les paramètres de la carte SD pour la mise à jour
        ptr_data_send = 0;
        strncpy((char*)data_send,(char*)"AT+SRWSTACFG=\"",14);
        ptr_data_send = ptr_data_send + 14;

        taille = (U8) strlen((const char *)P_L_CarteSd_ssid_mac_MAJ)-1;
        strncpy((char*)&data_send[ptr_data_send],(char*)P_L_CarteSd_ssid_mac_MAJ,taille);
        ptr_data_send = ptr_data_send + taille;
        strncpy((char*)&data_send[ptr_data_send],(char*)"\",\"",3);
        ptr_data_send = ptr_data_send + 3;

        taille = (U8)strlen((const char *)P_L_CarteSd_Clef_wep_MAJ)-1;
        strncpy((char*)&data_send[ptr_data_send],(char*)P_L_CarteSd_Clef_wep_MAJ,taille);
        ptr_data_send = ptr_data_send + taille;

        // 1 = Active la connexion automatique
        strncpy((char*)&data_send[ptr_data_send],(char*)"\",1",3);
        ptr_data_send = ptr_data_send + 3;

        HWI_Bx310x_tx(ptr_data_send,data_send);

    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_send_data_wifi
 *  Description     : envoie de donne en wifi attention MAX 242 octets
 *
 *
 *******************************************************************************/
void P_L_Wifi_Tx_data(U8 endpoint,U8 data_len,U8* data)
{
BOOL_TYPE result;

    if (FALSE == P_L_Wifi_Semaphore_Brocast)
    {
        P_L_Wifi_Semaphore_IT = TRUE;

        P_L_Wifi_Tx_dataDirect(endpoint,data_len,data),

        P_L_Wifi_Semaphore_IT = FALSE;
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_send_data_wifi
 *  Description     : envoie de donne en wifi attention MAX 242 octets
 *
 *
 *******************************************************************************/
void P_L_Wifi_Tx_dataDirect(U8 endpoint,U8 data_len,U8* data)
{
#ifndef _DEBUG_BX310X_COM
//taille max que peut envoyer le BX310
U8 data_send[P_L_TAILLE_SEND_BX310];
#endif
U32 client_id;
U16 ptr_data_send;
U16 boucle;
U8 cheksum_1;
U8 cheksum_2;

    client_id = P_L_Wifi_Found_AP_CLIENT_ID(endpoint);

#ifdef _RAMPE_AUTONOME
    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
    {
    	//ben modif a valider le 10/01/2023
	    if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
	    {
		   strncpy((char*)data_send,(char*)"AT+KTCPSND=",11);
		   ptr_data_send = 11;
	    }
	    else
	    {
		   strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);
		   ptr_data_send = 12;
	    }
    }
    else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    {
    	 strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);
    	 ptr_data_send = 12;
    }
    else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
    {
    	strncpy((char*)data_send,(char*)"AT+KTCPSND=",11);
    	ptr_data_send = 11;
    }
#ifdef _PROJECTEUR_BT
	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT )
	{
		strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);
		ptr_data_send = 12;
	}
#endif
    else if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
    {
        strncpy((char*)data_send,(char*)"AT+KTCPSND=",11);
        ptr_data_send = 11;
    }
    else
    {
        strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);
        ptr_data_send = 12;
    }
#else
#ifdef _BLUETOOTH
    if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
    {
        strncpy((char*)data_send,(char*)"AT+KTCPSND=",11);
        ptr_data_send = 11;
    }
    else
    {
        strncpy((char*)data_send,(char*)"AT+SRSPPSND=",12);
        ptr_data_send = 12;
    }
#else
    strncpy((char*)data_send,(char*)"AT+KTCPSND=",11);
    ptr_data_send = 11;
#endif
#endif

    P_L_Bx310x_Write_number(&ptr_data_send,data_send,client_id);
    data_send[ptr_data_send] = ',';
    ptr_data_send++;

///////////////////////////////////////////////
//copie du mot code
    strncpy((char*)&data_send[ptr_data_send],(char*)data,4);

    data_send[ptr_data_send] = data_send[ptr_data_send] - 0x20; // passage 1er lettre de miniscule � majuscule
    data_send[ptr_data_send+1] = data_send[ptr_data_send+1] - 0x20; // passage 2eme lettre de miniscule � majuscule si nouvelle gestion

    if (data_send[ptr_data_send+3] == ' ')
    {
       data_send[ptr_data_send+3] = '.';
    }

    ptr_data_send = ptr_data_send + 4;


    cheksum_1 = 0;
    cheksum_2 = 0;

    for (boucle=0;boucle<data_len;boucle++)
    {
        cheksum_1 = cheksum_1 + data[boucle];
        cheksum_2 = cheksum_2 + (data[boucle]*(boucle+1));
    }

    for(boucle=4;boucle<data_len;boucle++)
    {
        P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,data[boucle]);
    }

// #ifdef _PROGRAMATION ben supression le 11/012023 voir si les appli avant fonctionne toujours
    P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,cheksum_1);
    P_L_Bx310x_Write_Hexa(&ptr_data_send,data_send,cheksum_2);
//#endif

#ifdef _RAMPE_AUTONOME
    if ((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
    {
	    if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
	    {
	    	data_send[ptr_data_send] = '\n';
	    	ptr_data_send++;
	    }
    }
    else  if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
    {
	    data_send[ptr_data_send] = '\n';
	    ptr_data_send++;
    }
#ifdef _PROJECTEUR_BT
	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT )
	{
		if (FALSE == P_L_Wifi_Connect_Bluetooth(endpoint))
		{
			data_send[ptr_data_send] = '\n';
			ptr_data_send++;
		}
	}
#endif
#endif
    HWI_Bx310x_tx(ptr_data_send,data_send);

}

/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Erase_Flash_Wifi_Config
*  Description     :
*
*******************************************************************************/
static U8 P_L_Rpg_Erase_Flash_Wifi_Config(void)
{
/*
FLASH_EraseInitTypeDef EraseInitStruct;
U32 NbrOfPage;
U32 PageError;
U8  p_l_code_rpg_status = P_L_RPG_PASS;

    HAL_FLASH_Unlock();
    // Erase the user Flash area
    //(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR)

    // P_L_FIRST_CONF_WIFI_ADR en zone tonalite SPA et message PMV facades revue avec le decalage de zone du de flashe de 2k aulieu 1k

    NbrOfPage = FLASH_PagesMask((U32)P_L_FIRST_CONF_WIFI_ADR,((U32)P_L_END_CONF_WIFI_ADR));

    // Fill EraseInit structure
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = P_L_FIRST_CONF_WIFI_ADR;
    EraseInitStruct.NbPages = NbrOfPage;

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    __HAL_FLASH_CLEAR_FLAG(FLASH_SR_WRPRTERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_SR_PGERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_SR_BSY);

    if (HAL_FLASHEx_Erase(&EraseInitStruct,(uint32_t *) &PageError) != HAL_OK)
    {
        p_l_code_rpg_status = P_L_RPG_FAIL;
    }

    HAL_FLASH_Lock();

    return p_l_code_rpg_status;
   */

U8 status;

	status = P_L_RPG_TEST_PASS;

	// P_L_FIRST_CONF_WIFI_ADR en zone tonalite SPA et message PMV facades revue avec le decalage de zone du de flashe de 2k aulieu 1k
	if (HAL_OK != HWI_Hal_Flash_Erase_Zone((U32)P_L_FIRST_CONF_WIFI_ADR,(U32)P_L_END_CONF_WIFI_ADR))
	{
		status = P_L_RPG_TEST_FAIL;
	}
	return status;
}


/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Write_Flash_Wifi_Config
*  Description     :
*
*******************************************************************************/
static U8 P_L_Rpg_Write_Flash_Wifi_Config(void)
{
/*
U16 DataWr;
U32 FlashAdress;
U8  p_l_code_rpg_status;
U8 boucle;
HAL_StatusTypeDef status;

    p_l_code_rpg_status =  P_L_Rpg_Erase_Flash_Wifi_Config();

    if(P_L_RPG_PASS == p_l_code_rpg_status)
    {
        HAL_FLASH_Unlock();

        FlashAdress = P_L_FIRST_CONF_WIFI_ADR+8;

        for (boucle=0;boucle<(P_L_WIFI_CLEF_WEP_SIZE+1)/2;boucle++)
        {
            DataWr = (P_L_Wifi_Clef_wep[(2*boucle)+1]*256) + P_L_Wifi_Clef_wep[2*boucle];
            // Program the data received into STM32F10x Flash
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAdress,DataWr);
            // v�rifie que le code en MEMOIRE est bien celui du buffer.
            if ((*(U16*)FlashAdress != DataWr)||(status != HAL_OK))
            {
                p_l_code_rpg_status = P_L_RPG_FAIL;
            }

            FlashAdress += 2;
        }

        if (p_l_code_rpg_status == P_L_RPG_PASS)
        {
            FlashAdress = P_L_FIRST_CONF_WIFI_ADR;

            //1er data a �crite clef qui dit que la flash a ete initialise
            DataWr = 0xAA55;
            // Program the data received into STM32F10x Flash

            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAdress,DataWr);
            // v�rifie que le code en MEMOIRE est bien celui du buffer.
            if ((*(U16*)FlashAdress != DataWr)||(status != HAL_OK))
            {
                p_l_code_rpg_status = P_L_RPG_FAIL;
            }

            FlashAdress += 2;

            DataWr = 0x55AA;
            // Program the data received into STM32F10x Flash
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAdress,DataWr);
            // v�rifie que le code en MEMOIRE est bien celui du buffer.
            if ((*(U16*)FlashAdress != DataWr)||(status != HAL_OK))
            {
                p_l_code_rpg_status = P_L_RPG_FAIL;
            }

            FlashAdress += 2;

            //1er data a �crite clef qui dit que la flash a ete initialise
            DataWr = 0xAA55;
            // Program the data received into STM32F10x Flash

            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAdress,DataWr);
            // v�rifie que le code en MEMOIRE est bien celui du buffer.
            if ((*(U16*)FlashAdress != DataWr)||(status != HAL_OK))
            {
                p_l_code_rpg_status = P_L_RPG_FAIL;
            }

            FlashAdress += 2;

            DataWr = 0x55AA;
            // Program the data received into STM32F10x Flash
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAdress,DataWr);
            // v�rifie que le code en MEMOIRE est bien celui du buffer.
            if ((*(U16*)FlashAdress != DataWr)||(status != HAL_OK))
            {
                p_l_code_rpg_status = P_L_RPG_FAIL;
            }
        }

        HAL_FLASH_Lock();
    }
    return p_l_code_rpg_status;
*/

U8 DataWr[HWI_WRITE_SIZE_STM32L431];
U8 boucle;
U8  p_l_code_rpg_status;
U8 Wifi_Clef_wep[HWI_WRITE_SIZE_STM32L431*4];


	p_l_code_rpg_status =  P_L_Rpg_Erase_Flash_Wifi_Config();

	if(P_L_RPG_PASS == p_l_code_rpg_status)
	{

		for (boucle=0;boucle<P_L_WIFI_CLEF_WEP_SIZE;boucle++)
		{
			Wifi_Clef_wep[boucle] = P_L_Wifi_Clef_wep[boucle];
		}

		for (boucle=0;boucle<5;boucle++)
		{
			if (HAL_OK != HWI_Hal_Flash_Program(P_L_FIRST_CONF_WIFI_ADR+(8*(boucle+1)), &Wifi_Clef_wep[8*boucle],HWI_WRITE_SIZE_STM32L431))
			{
				p_l_code_rpg_status = P_L_RPG_FAIL;
				boucle = 5;

			}

		}

		if (p_l_code_rpg_status == P_L_RPG_PASS)
		{
			//1er data a �crite clef qui dit que la flash a ete initialise
			DataWr[0] = 0x55;
			DataWr[1] = 0xAA;

			DataWr[2] = 0xAA;
			DataWr[3] = 0x55;

			DataWr[4] = 0x55;
			DataWr[6] = 0xAA;

			DataWr[6] = 0xAA;
			DataWr[7] = 0x55;

			if (HAL_OK != HWI_Hal_Flash_Program(P_L_FIRST_CONF_WIFI_ADR, &DataWr[0],HWI_WRITE_SIZE_STM32L431))
			{
				p_l_code_rpg_status = P_L_RPG_FAIL;
			}
		}

	}

	return p_l_code_rpg_status;
}


/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Config_Init
 *  Description     : initialisation du wifi
 *
 *
 *******************************************************************************/
static void P_L_Wifi_Config_Init(BOOL_TYPE force_init)
{

U8 boucle;
U32 FlashAdress;
U16 data;

    FlashAdress = P_L_FIRST_CONF_WIFI_ADR;

    if ((*(U16*)FlashAdress != 0xAA55)||(*(U16*)(FlashAdress+2) != 0x55AA)||(*(U16*)(FlashAdress+4) != 0xAA55)||(*(U16*)(FlashAdress+6) != 0x55AA)||(TRUE == force_init))
    {
        for (boucle=0;boucle<P_L_WIFI_CLEF_WEP_SIZE;boucle++)
        {
            P_L_Wifi_Clef_wep[boucle] = temp_clef_wep[boucle];
        }

        P_L_Rpg_Write_Flash_Wifi_Config();
    }
    else
    {
        FlashAdress = P_L_FIRST_CONF_WIFI_ADR+8;

        for (boucle=0;boucle<(P_L_WIFI_CLEF_WEP_SIZE+1)/2;boucle++)
        {
            data = *(U16*)FlashAdress;

            P_L_Wifi_Clef_wep[2*boucle] = (data & 0xFF);
            P_L_Wifi_Clef_wep[(2*boucle)+1] = ((data>>8) & 0xFF);

            FlashAdress += 2;
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Wep_Wr
 *  Description     :
 *
 *
 *******************************************************************************/
void P_L_Wifi_Wep_Wr(void)
{
    P_L_Rpg_Write_Flash_Wifi_Config();

    P_L_Bx310x_Module_Init_Ask();
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Parametre_Wifi_Init
 *  Description     : initialisation du wifi
 *
 *
 *******************************************************************************/
void P_L_Wifi_Test_Init_Wep(void)
{
BOOL_TYPE result;

    if (TRUE == P_L_Bp1.State)
    {
        BT_TIMER_LOCK(P_L_Wifi_wep_timer,S_S_1MS_U32_TIME,P_L_FILTRAGE_WEP_ERASE,P_L_Wifi_wep_lock,result);
        if(TRUE == result)
        {
            if (TRUE == P_L_Wifi_wep_flag)
            {
                P_L_Wifi_Config_Init(TRUE);
                P_L_Wifi_wep_flag = FALSE;
            }
        }
    }
    else
    {
        BT_TIMER_INIT_LOCK(P_L_Wifi_wep_timer,S_S_1MS_U32_TIME,P_L_Wifi_wep_lock);
        P_L_Wifi_wep_flag = TRUE;
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Record_AP
 *  Description     : i
 *
 *
 *******************************************************************************/
static void P_L_Wifi_Record_AP(IP_TYPE client_ip,U32 client_id,BOOL_TYPE bluetooth)
{
U8 boucle;

    for (boucle=0;boucle<P_L_WIFI_MAX_CLIENTS;boucle++)
    {
        if (FALSE == P_L_Wifi_AP[boucle].used)
        {
            P_L_Wifi_AP[boucle].used= TRUE;
            P_L_Wifi_AP[boucle].client_id = client_id;
            P_L_Wifi_AP[boucle].client_ip.ip[0] = client_ip.ip[0];
            P_L_Wifi_AP[boucle].client_ip.ip[1] = client_ip.ip[1];
            P_L_Wifi_AP[boucle].client_ip.ip[2] = client_ip.ip[2];
            P_L_Wifi_AP[boucle].client_ip.ip[3] = client_ip.ip[3];
            P_L_Wifi_AP[boucle].bluetooth = bluetooth;

            boucle = P_L_WIFI_MAX_CLIENTS;
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Found_AP_IP
 *  Description     : i
 *
 *
 *******************************************************************************/
static U8 P_L_Wifi_Found_AP_IP(IP_TYPE client_ip)
{
U8 boucle;
U8 result = 0xFF;

    for (boucle=0;boucle<P_L_WIFI_MAX_CLIENTS;boucle++)
    {
        if ((TRUE == P_L_Wifi_AP[boucle].used) &&
            (P_L_Wifi_AP[boucle].client_ip.ip[0] == client_ip.ip[0]) &&
            (P_L_Wifi_AP[boucle].client_ip.ip[1] == client_ip.ip[1]) &&
            (P_L_Wifi_AP[boucle].client_ip.ip[2] == client_ip.ip[2]) &&
            (P_L_Wifi_AP[boucle].client_ip.ip[3] == client_ip.ip[3]) )
        {
            result = boucle;
            boucle = P_L_WIFI_MAX_CLIENTS;
        }
    }
    return result;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Found_AP_ID
 *  Description     : i
 *
 *
 *******************************************************************************/
static U8 P_L_Wifi_Found_AP_ID(U32 client_id)
{
U8 boucle;
U8 result = 0xFF;

    for (boucle=0;boucle<P_L_WIFI_MAX_CLIENTS;boucle++)
    {
        if ( (TRUE == P_L_Wifi_AP[boucle].used) && (P_L_Wifi_AP[boucle].client_id == client_id) )
        {
            result = boucle;
            boucle = P_L_WIFI_MAX_CLIENTS;
        }
    }
    return result;
}


/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Find_record_end_point
 *  Description     : i
 *
 *
 *******************************************************************************/
static void P_L_Wifi_Remove_AP(U32 client_id)
{
U8 boucle;

    for (boucle=0;boucle<P_L_WIFI_MAX_CLIENTS;boucle++)
    {
         if ((TRUE == P_L_Wifi_AP[boucle].used)&&(client_id == P_L_Wifi_AP[boucle].client_id))
        {
            P_L_Wifi_AP[boucle].used = FALSE;
            P_L_Wifi_AP[boucle].client_id = 0x00;
            P_L_Wifi_AP[boucle].bluetooth = FALSE;
            boucle = P_L_WIFI_MAX_CLIENTS;
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Find_record_end_point
 *  Description     : i
 *
 *
 *******************************************************************************/
static void P_L_Wifi_MiseAJour_AP(U8 endpoint,U32 client_id)
{
    P_L_Wifi_AP[endpoint].client_id = client_id;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Find_record_end_point
 *  Description     : i
 *
 *
 *******************************************************************************/
static U32 P_L_Wifi_Found_AP_CLIENT_ID(U8 endpoint)
{
    return P_L_Wifi_AP[endpoint].client_id;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Connect_Bluetooth
 *  Description     : i
 *
 *
 *******************************************************************************/
#ifdef _BLUETOOTH
BOOL_TYPE P_L_Wifi_Connect_Bluetooth(U8 endpoint)
{
    return P_L_Wifi_AP[endpoint].bluetooth;
}
#endif

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Write_number
 *
 *
 *******************************************************************************/
static void P_L_Bx310x_Write_Hexa(U16 *ptr_received_string,U8 * received_string,U8 number)
{
    received_string[*ptr_received_string] = digit[((number>>4)&0x0F)];
    *ptr_received_string = *ptr_received_string + 1;
    received_string[*ptr_received_string] = digit[(number&0x0F)];
    *ptr_received_string = *ptr_received_string + 1;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Find_nb_data
 *  Description     :
 *
 *
 *******************************************************************************/
static U32 P_L_Bx310x_Read_All_Hexa(U16 *ptr_received_string,U8 * received_string,char delimiteur)
{
S8 boucle;
U32 result;
U8 data[10];
U8 nb_data;
U32 multiplieur;
U8 data_temp;

    nb_data = 0;

    for (boucle=0;boucle<10;boucle++)
    {
        data_temp = received_string[*ptr_received_string];

        if (delimiteur != data_temp)
        {
            if ((data_temp >= '0') && (data_temp <= '9'))
            {
                data[nb_data] = data_temp - '0';
            }
            else
            {
               data[nb_data] = 10 + (data_temp - 'a');
            }

            (*ptr_received_string)++;
            nb_data++;
        }
        else
        {
            (*ptr_received_string)++;
            boucle = 10;
        }
    }

    result = 0;
    multiplieur = 16;

    if (nb_data>0)
    {
       nb_data--;
    }

    for (boucle=nb_data;boucle>=0;boucle--)
    {
        if (boucle == nb_data)
        {
            result = data[boucle];
        }
        else
        {
            result =  result + (data[boucle]*multiplieur);
            multiplieur = 16*multiplieur;
        }
    }
    return result;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Write_number
 *
 *
 *******************************************************************************/
static U8 P_L_Bx310x_Read_Hexa(U16 *ptr_received_string,U8 * received_string)
{
U8 boucle;
U8 data_temp;
U8 data[2];
U8 result;

    result = 0x00;

    for (boucle=0;boucle<2;boucle++)
    {
        data_temp = received_string[*ptr_received_string];

        if ((data_temp >= '0') && (data_temp <= '9'))
        {
            data[boucle] = data_temp - '0';
        }
        else
        {
           data[boucle] = 10 + (data_temp - 'a');
        }

        (*ptr_received_string)++;
    }
    result = data[1] + 16*data[0];
    return result;
}



/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Find_nb_data
 *  Description     :
 *
 *
 *******************************************************************************/
static U32 P_L_Bx310x_Read_All_number(U16 *ptr_received_string,U8 * received_string,char delimiteur)
{
S8 boucle;
U32 result;
U8 data[10];
U8 nb_data;
U32 multiplieur;

    nb_data = 0;

    for (boucle=0;boucle<10;boucle++)
    {
        if (' ' == received_string[*ptr_received_string])
        {
           (*ptr_received_string)++;
        }
        else if (delimiteur != received_string[*ptr_received_string])
        {
            data[nb_data] = received_string[*ptr_received_string] - 0x30;
            (*ptr_received_string)++;
            nb_data++;
        }
        else
        {
            (*ptr_received_string)++;
            boucle = 10;
        }
    }

    result = 0;
    multiplieur = 10;
    if (nb_data>0)
    {
       nb_data--;
    }

    for (boucle=nb_data;boucle>=0;boucle--)
    {
        if (boucle == nb_data)
        {
            result = data[boucle];
        }
        else
        {
            result =  result + (data[boucle]*multiplieur);
            multiplieur = 10*multiplieur;
        }
    }

    return result;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Wifi_Write_number
 *
 *
 *******************************************************************************/
static void P_L_Bx310x_Write_number(U16 *ptr_received_string,U8 * received_string,U32 number)
{
S8 boucle;
U8 nb_data;
U8 data[10];
U64 value_memo;
U64 value_source;
U32 temp;
U64 diviseur;

    nb_data = 0;
    value_memo = number;
    value_source = number;
    diviseur = 1000000000;

    for (boucle=0;boucle<10;boucle++)
    {
        if (value_source >= diviseur)
        {
            temp = value_memo / diviseur;
            data[nb_data] = 0x30 + temp;
            value_memo = value_memo - (diviseur*temp);
            nb_data++;
        }
        diviseur = diviseur / 10;
    }

    for (boucle=0;boucle<nb_data;boucle++)
    {
        received_string[*ptr_received_string] = data[boucle];
        (*ptr_received_string)++;
    }
}


/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_WIFI_RX_Analyse
 *  Description     : Analyse et mise en forme des donn� recue
 *
 *
 *******************************************************************************/
BOOL_TYPE break_to_debug = FALSE;

U8 data_received_Wifi[TAILLE_BUFFER_RX_RS];

#ifdef _BLUETOOTH
static void P_L_Bx310x_WIFI_RX_Analyse(U8 start_data,U8* received_string)
{
U16 ptr_data;
U32	client_id;
U8 endpoint;
U16 data_len;
//U8 data_received_Wifi[TAILLE_BUFFER_RX_RS];
U16 ptr_data_received;
U8 boucle;

    ptr_data = start_data;
    client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

    endpoint = P_L_Wifi_Found_AP_ID(client_id);

    if (0xFF != endpoint)
    {

        data_len = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

        if (data_len > 50)
        {
        	break_to_debug = TRUE;
        }

        if (data_len > (TAILLE_BUFFER_RX_RS-1))
        {
        	data_len  =  (TAILLE_BUFFER_RX_RS-1);
        }

        strncpy((char*)data_received_Wifi,(char*)&received_string[ptr_data],4);

        if (data_received_Wifi[0x00] < 0x61)
        {
        	data_received_Wifi[0x00]  = data_received_Wifi[0x00]  + 0x20;
        }

        if (data_received_Wifi[0x01] < 0x61)
        {
        	data_received_Wifi[0x01]  = data_received_Wifi[0x01]  + 0x20;
        }

        if (data_received_Wifi[0x03] == '.')
        {
        	data_received_Wifi[0x03] = ' ';
        }


        ptr_data = ptr_data + 4;
        ptr_data_received = 4;

        data_len = ((data_len - 4)/2) + 4;

        for(boucle=0;boucle<(data_len-4);boucle++)
        {
        	data_received_Wifi[ptr_data_received] = P_L_Bx310x_Read_Hexa(&ptr_data,received_string);
            ptr_data_received++;
        }
        data_received_Wifi[ptr_data_received] = '\r';



        APP_Gestion_Wifi_Data(endpoint,data_len,data_received_Wifi);

        if(FALSE == P_L_CarteSd_Testeur_vue)
        {
            HWI_Led_1_Rouge_Cmd(HWI_Cmd_Led_1_verte);
            HWI_Cmd_Led_1_verte = !HWI_Cmd_Led_1_verte;
        }
    }
}
#endif

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_BT_RX_Analyse
 *  Description     : Analyse et mise en forme des donn� recue
 *
 *
 *******************************************************************************/
static void P_L_Bx310x_BT_RX_Analyse(U8 start_data,U8* received_string)
{
#ifdef _RAMPE_AUTONOME
U16 ptr_data;
U32	client_id;
U16 data_len;
U8 data_received_BT[TAILLE_BUFFER_RX_RS];
U16 ptr_data_received;
U8 boucle;

    ptr_data = start_data;//=12
    client_id = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

    if (TRUE)//if (client_id == P_L_BT_spp_rampe_id)
    {
        data_len = P_L_Bx310x_Read_All_number(&ptr_data,received_string,',');

        if (data_len > 70)
		{
			break_to_debug = TRUE;
		}

        if (data_len > (TAILLE_BUFFER_RX_RS-1))
        {
        	data_len  =  (TAILLE_BUFFER_RX_RS-1);
        }

         strncpy((char*)data_received_BT,(char*)&received_string[ptr_data],4);
        // strncpy((char*)data_received_BT,(char*)&received_string[ptr_data],24);

        ptr_data = ptr_data + 4;
        ptr_data_received = 4;

        data_len = ((data_len - 4)/2) + 4;
        // data_len = 23;

        //strncpy((char*)data_received_BT,(char*)&received_string[ptr_data],data_len - 4);
        //Là c'est problématique
        for(boucle=0;boucle<(data_len-4);boucle++)
        {
        	data_received_BT[ptr_data_received] = P_L_Bx310x_Read_Hexa(&ptr_data,received_string);
            ptr_data_received++;
        }
        data_received_BT[ptr_data_received] = '\r';



        APP_Gestion_BT_DataRx(data_len,data_received_BT);
    }
#endif
}

/*******************************************************************************
*
*  Function Name   : P_L_Manage_Perte_Reseau
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : init function
*******************************************************************************/
#ifdef _RAMPE_AUTONOME
void P_L_Bx310x_Test_Pair_RX(U8* rxPair)
{
U8 boucle;
BOOL_TYPE result;
const U8 *ptr;

	if (FALSE == P_L_Flash_Mac_spp_Wr)
	{
		result = TRUE;
/*
#ifdef _SANSBOOT
	ptr = (const U8 *) (((P_L_SSP_MAX_PAIR_RX_OK_FLASH_ADR-P_L_FIRST_PARAM_FLASH_ADR)+P_L_FIRST_CONF_FLASH_ADR)-P_L_PARAM_FLASH_SIZE);
#else
	ptr = (const U8 *) P_L_SSP_MAX_PAIR_RX_OK_FLASH_ADR;
#endif
		if (0x01 != *ptr)
		{
			result = FALSE;
		}
		else
		{
		*/
			if (P_L_Flash_Mac_spp_Wr == FALSE)
			{


#ifdef _SANSBOOT
				ptr = (const U8 *) (((P_L_SSP_MAC_PAIR_FLASH_ADR-P_L_FIRST_PARAM_FLASH_ADR)+P_L_FIRST_CONF_FLASH_ADR)-P_L_PARAM_FLASH_SIZE);
#else
				ptr = (const U8 *) P_L_SSP_MAC_PAIR_FLASH_ADR;
#endif
				for(boucle=0;boucle<P_L_SSP_MAC_PAIR_FLASH_SIZE;boucle++)
				{
					if (rxPair[boucle] != *(ptr+boucle))
					{
						result = FALSE;
					}
				}
			}

			if (result == FALSE)
			{

				for(boucle=0;boucle<P_L_SSP_MAC_PAIR_FLASH_SIZE;boucle++)
				{
					P_L_Flash_Mac_spp_rx[boucle] = rxPair[boucle];
				}
				P_L_Flash_Mac_spp_Wr = TRUE;

				P_L_BT_Spp_Mac.addr[0] = rxPair[0];
				P_L_BT_Spp_Mac.addr[1] = rxPair[1];
				P_L_BT_Spp_Mac.addr[2] = rxPair[2];
				P_L_BT_Spp_Mac.addr[3] = rxPair[3];
				P_L_BT_Spp_Mac.addr[4] = rxPair[4];
				P_L_BT_Spp_Mac.addr[5] = rxPair[5];

				APP_2binHexa(P_L_BT_Spp_Mac.addr[0],&P_L_BT_spp_mac_pair_rampe[0]);
				P_L_BT_spp_mac_pair_rampe[2] =  ':';
				APP_2binHexa(P_L_BT_Spp_Mac.addr[1],&P_L_BT_spp_mac_pair_rampe[3]);
				P_L_BT_spp_mac_pair_rampe[5] =  ':';
				APP_2binHexa(P_L_BT_Spp_Mac.addr[2],&P_L_BT_spp_mac_pair_rampe[6]);
				P_L_BT_spp_mac_pair_rampe[8] =  ':';
				APP_2binHexa(P_L_BT_Spp_Mac.addr[3],&P_L_BT_spp_mac_pair_rampe[9]);
				P_L_BT_spp_mac_pair_rampe[11] = ':';
				APP_2binHexa(P_L_BT_Spp_Mac.addr[4],&P_L_BT_spp_mac_pair_rampe[12]);
				P_L_BT_spp_mac_pair_rampe[14] = ':';
				APP_2binHexa(P_L_BT_Spp_Mac.addr[5],&P_L_BT_spp_mac_pair_rampe[15]);

				P_L_BT_spp_pair_rampe = TRUE;

			}
		}
	//}
}
#endif
/*******************************************************************************
*
*  Function Name  : APP_Hex2bin
*  Description    :
*  Param		  :
*
*******************************************************************************/
U8 APP_Hex2bin(const U8 *Hexa)
{
    return (U8)APP_HexbinX(Hexa,2);
}

/*******************************************************************************
*
*  Function Name  : APP_HexbinX
*  Description    :
*  Param		  :
*
*******************************************************************************/
U32 APP_HexbinX(const U8 *Hexa,U8 size)
{
U32 Result;
U8 boucle;

    Result = 0;
	for(boucle=0;boucle<size;boucle++)
    {
        if (( Hexa[boucle] >= '0' && Hexa[boucle] <= '9') || ( Hexa[boucle] >= 'A' && Hexa[boucle] <= 'F' ))
        {
            // Si le premier caract�re est valide
            // Conversion des poids forts
            if(( Hexa[boucle] - '0' ) <= 9 )
            {
                Result = Result*16 + (( Hexa[boucle] - '0' ) & 0x0F);
            }
            else
            {
                Result = Result*16 + ( ( Hexa[boucle] - 'A' + 10 )& 0x0F);
            }
        }
        else
        {
        }
	}

	// Retour du r�sultat
	return (Result);
}

/*******************************************************************************
*
*  Function Name  : HWI_2binHexa
*  Description    :
*  Param		  :
*
*******************************************************************************/
void  APP_2binHexa(U8 Value,U8 *Hexa)
{
	Hexa[1] = APP_HEX[Value & 0x0F];
    Hexa[0] = APP_HEX[(Value >> 4) & 0x0F];
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Bx310x_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void P_L_Bx310x_Init(BOOL_TYPE init_clef_wep) {
U8 boucle;

#ifdef _DEBUG_BX310X_COM
P_L_Bx310x_READY = 0;
P_L_Bx310x_TCP_SRVREQ = 0;
P_L_Bx310x_TCP_NOTIF = 0;
P_L_Bx310x_TCP_DATA = 0;
P_L_Bx310x_ERROR_CME = 0;
P_L_Bx310x_ERROR = 0;
P_L_Bx310x_CMD_UNKNOW = 0;
#endif

    for (boucle=0;boucle<P_L_WIFI_MAX_CLIENTS;boucle++)
    {
        P_L_Wifi_AP[boucle].used = FALSE;
        P_L_Wifi_AP[boucle].client_id = 0x00;
        P_L_Wifi_AP[boucle].bluetooth = FALSE;
    }

    HWI_Uart_wifi_ptr_received_string = 0;

    P_L_Bx310x_goto_state(Bx310x_state_ATTENTE);

    if (TRUE == init_clef_wep)
    {
         P_L_Wifi_Config_Init(FALSE);
    }

   HWI_Uart_Init(115200,FALSE);

    BT_TIMER_INIT_LOCK(P_L_Wifi_wep_timer,0x00,P_L_Wifi_wep_lock);
    BT_TIMER_INIT_LOCK(P_L_Bx310x_timeout_configure,0x00,P_L_Bx310x_timeout_configure_lock);


    P_L_Bx310x_Flag_module_init = FALSE;
    P_L_Bx310x_Module_Init();

    HWI_Cmd_Led_1_verte = FALSE;

#ifdef _BLUETOOTH
    P_L_BT_state = BT_disconnected;
#ifdef _GESTION_AUDIO_BT
    P_L_BT_audio_state = audio_stream_stopped;
#endif
#endif


    for (boucle=0;boucle<P_L_WIFI_CLEF_WEP_SIZE;boucle++)
    {
        P_L_Wifi_Clef_wep_a_tester[boucle] = temp_clef_wep[boucle];
    }

    P_L_Wifi_ssid_mac_a_tester_recue = FALSE;
    P_L_Wifi_ATester_vue = FALSE;
    P_L_Wifi_MiseAJour_vue = FALSE;
    P_L_Wifi_ssid_mac_lue = FALSE;
    P_L_BT_ssid_mac_lue = FALSE;
#ifdef _BLUETOOTH
    P_L_BT_pair_ask = FALSE;
#endif

#ifdef _RAMPE_AUTONOME
    P_L_BT_spp_pair_rampe = FALSE;
    P_L_BT_spp_connect_rampe = FALSE;
    P_L_BT_spp_start_rampe = FALSE;
    P_L_BT_spp_rampe_state = BT_disconnected;
    P_L_BT_spp_rampe_state_memo = P_L_BT_spp_rampe_state;

    BT_TIMER_INIT_LOCK(P_L_BT_spp_Connect_timer,S_S_1MS_U32_TIME,P_L_BT_spp_Connect_lock);
    P_L_BT_spp_Connect_lock = TRUE;
#endif

}




