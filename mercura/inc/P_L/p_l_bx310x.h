/*******************************************************************************
*  Synergie Ing�nierie Indu#endif
#endif

// TIMEOUT MODIFIÉ POUR BLUETOOTH SPP - Augmenté de 10s à 30s pour éviter déconnexions
#define APP_TIMEOUT_RESET_CMD   (U32)30000  // 30000ms (était 10000ms)elle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_BX310X_H_
#define _P_L_BX310X_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
//#define APP_TIME_INTER_SPP_CONNECT 				(U32)2000
//ADD
#define _RAMPE_AUTONOME
#define _PROJECTEUR_BT


#define APP_TIME_INTER_SPP_CONNECT 				(U32)5000

typedef struct bd_addr_t
{
    unsigned char addr[6];
}bd_addr;


typedef bd_addr hw_addr;


typedef struct
{
    U8 ip[4];
}IP_TYPE;


//on enregistre un client par rapport a son IP
typedef struct
{
    BOOL_TYPE used;
    IP_TYPE	client_ip;
    U32 client_id;
    BOOL_TYPE bluetooth;
}P_L_Wifi_AP_TYPE;


/** States of the internal state machine. */
typedef enum  {
    Bx310x_state_RESET,
    Bx310x_state_ATTENTE,
	Bx310x_state_VERSION,
    Bx310x_state_SRBTSYSTEM_0,
    Bx310x_state_RESET_module,
    Bx310x_state_ATTENTE_RESET_module,
	Bx310x_state_CONTROL_FLUX,
    Bx310x_state_ATI,
	Bx310x_state_RWCFG,
	Bx310x_state_ASK_RWCFG,
	Bx310x_state_RWAPCFG,
	Bx310x_state_RWAPCFGNU,
	Bx310x_state_RWAPNETCFG,
    Bx310x_state_KTCPCFG,
	Bx310x_state_KTCPCNX,
    Bx310x_state_SRWSTACFG,
    Bx310x_state_WAIT_WIFI_TEST,
#ifdef _BLUETOOTH
    Bx310x_state_SRBTSYSTEM_CFG,
    Bx310x_state_SRBTADDR,
    Bx310x_state_SRBTNAME,
    Bx310x_state_SRBTCSECPARAMS,
    Bx310x_state_SRBTCPROFILES,
	Bx310x_state_SRBTSTATE,
#ifdef _GESTION_AUDIO_BT
	Bx310x_state_KI2SCFG,
#endif
#ifdef _TELECOMMANDE_BT
    Bx310x_state_start_general_inquiry,
    Bx310x_state_start_pair_request,
#endif
#endif
    Bx310x_state_WAIT_WIFI_AirVantage,
    Bx310x_state_WDSI,
    Bx310x_state_WDSS,
#ifdef _PAIR_TO_DEVICE
	Bx310x_state_LIST_BT_DEVICE,
	Bx310x_state_PAIR_AUDIO,
#endif
#ifdef _RAMPE_AUTONOME
	Bx310x_state_SRBTUNPAIR,
	Bx310x_state_BT_SSP_CONNECT,
	Bx310x_state_BT_SSP_START,
#endif
	Bx310x_state_FINIT
}P_L_BX310X_STATE;



#ifdef _BLUETOOTH
typedef enum  {
    BT_disconnected='0',
    BT_connected='1',
}P_L_BT_STATE_TYPE;

#ifdef _GESTION_AUDIO_BT
typedef enum  {
    audio_stream_suspended='0',
    audio_stream_stopped='1',
    audio_stream_started='2',
}P_L_BT_AUDIO_STATE_TYPE;
#endif
#endif

//#define APP_TIMEOUT_RESET_CMD   (U32)10000
#define APP_TIMEOUT_RESET_CMD   (U32)30000

//////////////////////////////////////////////////////////////////
#define P_L_WIFI_MAX_CLIENTS 		5
#define P_L_Wifi_SSID_MAX			20
#define P_L_WIFI_CLEF_WEP_SIZE      30
#define P_L_Wifi_Open_security		0
#define P_L_Wifi_WPA_security		1
#define P_L_Wifi_WPA2_security		2
#define P_L_Wifi_WEP_security		3

#define P_L_Wifi_SSID_MAC			17

#define P_L_BT_MAC_SIZE             17
 #define P_L_BT_PIN_CODE_SIZE       6

#define P_L_FILTRAGE_WEP_ERASE               ((U32)3000)

#define P_L_TAILLE_SEND_BX310       ((U8)242)
#define P_L_TAILLE_SEND_CMD         ((U8)11)
#define P_L_TAILLE_SEND_MOT_CODE    ((U8)4)
#define P_L_TAILLE_SEND_MAX         ((P_L_TAILLE_SEND_BX310-P_L_TAILLE_SEND_CMD-P_L_TAILLE_SEND_MOT_CODE)/2)+P_L_TAILLE_SEND_MOT_CODE


#define P_L_RPG_PASS             0x01
#define P_L_RPG_FAIL             0x02


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Bx310x_Rx(U16 ptr_received_string,U8 * received_string);
extern void P_L_Wifi_Tx_data(U8 client,U8 data_len,U8* data);
extern void P_L_Wifi_Tx_dataDirect(U8 endpoint,U8 data_len,U8* data);
extern void P_L_Parametre_Wifi_Init(void);
extern void P_L_Bx310x_Module_Init_Ask(void);
extern void P_L_Bx310x_Init(BOOL_TYPE init_clef_wep);
extern void P_L_Bx310x_Test_Timeout_Cmd(void);
extern void P_L_Bx310x_Manage_State(void);
extern void P_L_Wifi_Wep_Wr(void);
extern void P_L_Wifi_Test_Init_Wep(void);
extern U32 APP_HexbinX(const U8 *Hexa,U8 size);
extern U8 APP_Hex2bin(const U8 *Hexa);
extern void  APP_2binHexa(U8 Value,U8 *Hexa);
#ifdef _RAMPE_AUTONOME
extern void P_L_Bx310x_Test_Pair_RX(U8* rxPair);
extern void P_L_BT_Tx_dataDirect(U32 client_id,U8 data_len,U8* data);
extern void P_L_BT_Tx_data(U32 client_id,U8 data_len,U8* data);
#endif
#ifdef _BLUETOOTH
BOOL_TYPE P_L_Wifi_Connect_Bluetooth(U8 endpoint);
#endif
#endif


