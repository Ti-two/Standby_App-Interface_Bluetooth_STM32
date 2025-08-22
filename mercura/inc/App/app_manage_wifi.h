/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_WIFI_H_
#define _APP_MANAGE_WIFI_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"


/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define WRD_KEY_ASK			FCC('k','e','y','?')
#define WRD_BTN_ASK 		FCC('b','t','n','?')
#define WRD_MESS_ASK		FCC('m','e','s','?')
#define WRD_MESS			FCC('m','e','s','s')
#define WRD_WEP			    FCC('w','e','p',' ')
#define WRD_MESS_PLUS		FCC('m','e','s','+')
#define WRD_CFG			    FCC('c','f','g',' ')
#define WRD_BTN				FCC('b','t','n',' ')
#define WRD_STS				FCC('s','t','s','?')
#define WRD_BAT				FCC('b','a','t','?')

#define WRD_ODO			    FCC('o','d','o','=')
#define WRD_ODO_ASK			FCC('o','d','o','?')

#define WRD_FILE_OPEN_WR	    FCC('f','i','w','o')
#define WRD_FILE_CLOSE_WR	    FCC('f','i','w','c')
#define WRD_FILE_TRANSFERT_WR	FCC('f','i','w','t')

#define WRD_FILE_OPEN_RD	    FCC('f','i','r','o')
#define WRD_FILE_CLOSE_RD	    FCC('f','i','r','c')
#define WRD_FILE_TRANSFERT_RD	FCC('f','i','r','t')

#define WRD_FLASHAGE	    FCC('f','l','a','s')
#define WRD_LISTE_MODULE    FCC('l','i','s','t')
#define WRD_CHANGE_ADR      FCC('a','d','r','c')
#define WRD_HISTORIQUE_MODULE    FCC('h','i','s','t')
#define WRD_HISTORIQUE_MODULE_ACQ    FCC('h','i','s','=')
#define WRD_DIAG            FCC('d','i','a','g')
#define WRD_CAN             FCC('c','a','n','=')
#define WRD_VERSION_FILE    FCC('v','e','r','s')

#ifdef _TEST_ETHERNET
#define WRD_CMD_ETHERNET    FCC('e','t','h','=')
#endif

#define WRD_JUMP_BOOT_ASK	FCC('J','m','p','W')
#define WRD_TEST_ASK		FCC('T','e','s','t')

#define BUFF_KEY(ptr)			FFC_BUFF(ptr,'k','e','y',' ')
#define BUFF_BTN(ptr)			FFC_BUFF(ptr,'b','t','n',' ')
#define BUFF_MESS(ptr)			FFC_BUFF(ptr,'m','e','s','s')
#define BUFF_MESS_REP(ptr)		FFC_BUFF(ptr,'m','e','s','=')
#define BUFF_WEP(ptr)			FFC_BUFF(ptr,'w','e','p',' ')
#define BUFF_CFG(ptr)			FFC_BUFF(ptr,'c','f','g',' ')
#define BUFF_STS(ptr)			FFC_BUFF(ptr,'s','t','s','=')
#define BUFF_BIP(ptr)			FFC_BUFF(ptr,'b','i','p','=')
#define BUFF_BAT(ptr)			FFC_BUFF(ptr,'b','a','t','=')






#define BUFF_JUMP_APLI(ptr)		FFC_BUFF(ptr,'J','m','p','W')
#define BUFF_TEST(ptr)			FFC_BUFF(ptr,'A','p','l','i')

#define BUFF_ODO(ptr)		    FFC_BUFF(ptr,'o','d','o','=')
#define BUFF_ODO_REP(ptr)		FFC_BUFF(ptr,'o','d','o',' ')

#define BUFF_FILE_OPEN_WR_REP(ptr)		FFC_BUFF(ptr,'f','i','w','o')
#define BUFF_FILE_CLOSE_WR_REP(ptr)	FFC_BUFF(ptr,'f','i','w','c')
#define BUFF_FILE_TRANSFERT_WR_REP(ptr)	FFC_BUFF(ptr,'f','i','w','t')

#define BUFF_FILE_OPEN_RD_REP(ptr)		FFC_BUFF(ptr,'f','i','r','o')
#define BUFF_FILE_CLOSE_RD_REP(ptr)	FFC_BUFF(ptr,'f','i','r','c')
#define BUFF_FILE_TRANSFERT_RD_REP(ptr)	FFC_BUFF(ptr,'f','i','r','t')

#define BUFF_FLASHAGE_REP(ptr)	    FFC_BUFF(ptr,'f','l','a','=')
#define BUFF_LISTE_MODULE_REP(ptr)	FFC_BUFF(ptr,'l','i','s','=')
#define BUFF_CHANGE_ADR_REP(ptr)	FFC_BUFF(ptr,'a','d','r','=')
#define BUFF_HISTORIQUE_REP(ptr)	FFC_BUFF(ptr,'h','i','s','=')
#define BUFF_CAN(ptr)	             FFC_BUFF(ptr,'c','a','n','=')
#define BUFF_VERSION_FILE_REP(ptr)	FFC_BUFF(ptr,'v','e','r','=')

#define BUFF_CMD_ETHERNET_REP(ptr)	FFC_BUFF(ptr,'e','t','h','=')


#define WRD_OFF				FCC('0','0',0,0)
#define WRD_FIXE			FCC('0','0',0,1)
#define WRD_CLIGN			FCC('0','0',0,2)
#define WRD_MESS_2			FCC('0','0',0,3)
#define WRD_MESS_3			FCC('0','0',0,4)

#define  APP_TAILLE_WIFI_KEY                (U8)8
#define  APP_TAILLE_WIFI_UNLOCKEDKEY        (U8)8

#define  APP_TAILLE_TRANSFERT_STEP              (U8)2

#define P_L_CAN_TAILLE_1_ENVOIE_HISTORIQUE  ((U8)100)
#define P_L_CAN_TAILLE_1_ENVOIE_DATA        ((U8)100)

#define REQUEST_SIZE				4
#define FIN_CHAINE_SIZE				1
#define FIN_CHAINE					'\n'
#define FCC(c1,c2,c3,c4)			(((U32)c4<<24)+((U32)c3<<16)+((U16)c2<<8)+(U8)c1)	/* FourCC */
#define FFC_BUFF(ptr,c1,c2,c3,c4) 	*(U8*)(ptr)=(U8)(c1); *(U8*)((ptr)+1)=(U8)(c2); *(U8*)((ptr)+2)=(U8)(c3); *(U8*)((ptr)+3)=(U8)(c4)


#define	LD_WORD(ptr)				(U16)(((U16)*(U8*)((ptr)+1)<<8)|(U16)*(U8*)(ptr))
#define	ST_WORD(ptr,val)			*(U8*)(ptr)=(U8)(val); *(U8*)((ptr)+1)=(U8)((U16)(val)>>8)
#define	LD_DWORD(ptr)				(U32)(((U32)*(U8*)((ptr)+3)<<24)|((U32)*(U8*)((ptr)+2)<<16)|((U16)*(U8*)((ptr)+1)<<8)|*(U8*)(ptr))
#define	ST_DWORD(ptr,val)			*(U8*)(ptr)=(U8)(val); *(U8*)((ptr)+1)=(U8)((U16)(val)>>8); *(U8*)((ptr)+2)=(U8)((U32)(val)>>16); *(U8*)((ptr)+3)=(U8)((U32)(val)>>24)

#define APP_FILTRAGE_USE_KEY        ((U32)5000)//((U32)2000)
#define APP_FILTRAGE_CLIENT_OK      ((U32)20000)

#define P_L_RESET_VECTOR_OFFSET       4
#define P_L_FIRST_WIFI_FLASH_ADR      0x0801A800  //


#define APP_NB_MAX_LIGNE_WIFI        ((U8)6)
#define APP_TAILLE_MAX_MESS          ((U8)16)

#define APP_FILTRAGE_LED_CHANGE      ((U32)500)

#define APP_TIMEOUT_BROADCAST        ((U32)3000)
typedef enum
{
APP_MESSAGE_PMV_TYPE_NON_UTILISE = 0,
APP_MESSAGE_PMV_TYPE_FIXE = 1,
APP_MESSAGE_PMV_TYPE_CLIGNOTANT = 2,
APP_MESSAGE_PMV_TYPE_CHAINE_2 = 3,
APP_MESSAGE_PMV_TYPE_CHAINE_3 = 4,
APP_MESSAGE_PMV_TYPE_DEFILEMENT = 5,
APP_MESSAGE_PMV_TYPE_6 = 6,
APP_MESSAGE_PMV_TYPE_DEFILEMENT_MAX = 7
}APP_PMV_TYPE;


typedef struct {
	BOOL_TYPE   used;
	U8          endpoint;
	BOOL_TYPE   wifi_key_ask;
	U32         timer_key_ask;
	BOOL_TYPE   lock_key_ask;
    U8          wifi_key[APP_TAILLE_WIFI_KEY];
    U8          wifi_Unlockkey[APP_TAILLE_WIFI_UNLOCKEDKEY];
    BOOL_TYPE   gestion_com_ok;
    BOOL_TYPE   send_Sts;
    //U8   		send_Sts_Count;
    BOOL_TYPE   send_Bip;
    //-------------------------------------------//
    BOOL_TYPE   send_Key;
    BOOL_TYPE   send_Bat;
    BOOL_TYPE   send_OK;
    BOOL_TYPE   send_NOK;
    U8			data_ok_nok[4];
}APP_WIFI_CLIENTS_TYPE;




#define APP_MODE_GESTION_WIFI_TAB  0x02


// Si APP_MODE_GESTION_WIFI_TAB alors defint dans nouvelle zone est nouveau type
typedef enum
{
APP_MODE_MESSAGE_PMV = 0xFF,
APP_MODE_BTN_ET_MESSAGE_PMV = 0x01,
APP_MODE_ODOMETRE = 0x02,
APP_MODE_AUDIO = 0x03,
MODE_CMD_RAMPE_AUTONOME_BLEU = 0x04,
MODE_RAMPE_AUTONOME = 0x05,
MODE_RAMPE_AUTONOME_WIFI = 0x06,
MODE_PROJECTEUR_BT = 0x07,
MODE_CMD_RAMPE_AUTONOME_ORANGE = 0x08
}APP_MODE_GES_WIFI_BLT_TYPE;

#define APP_FILTRAGE_APC_ODOMETRE   ((U32)2000)
#define APP_VALEUR_MAX_ODOMETRE     ((F64)0x0F423F)


#define APP_FILTRAGE_PERTE_BLOC_PMV_CAN    ((U32)3000)


#ifdef _TEST_ETHERNET
#define PL_CAN_ID_ETHERNET_ASK              ((U32)0x101)
#define PL_CAN_ASK_CHGT_MODULE_VUE          ((U8)0x01)
#define PL_CAN_ASK_MODULE_VUE               ((U8)0x02)


#define PL_CAN_ID_ETHERNET_ADR_MODULE       ((U32)0x102)
#define PL_CAN_ID_ETHERNET_READ             ((U32)0x103)
#define PL_CAN_ID_ETHERNET_WRITE            ((U32)0x104)

#define PL_CAN_ID_CMD                ((U8)0x30)

#endif

//#define APP_FILTRAGE_PERTE_TABLETTE         ((U32)60000)
#define APP_FILTRAGE_PERTE_TABLETTE         ((U32)120000)

#define PICTO_TOUCHE_ON_OFF                 ((U8)0x01)
#define PICTO_PAS_DE_TOUCHE                 ((U8)0xFF)
#define PICTO_TOUCHE_NU                     ((U8)0x00)


/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void APP_Manage_Wifi_Init(void);
extern void APP_Manage_Wifi_Client(void);
extern void APP_Gestion_Wifi_Data(U8 endpoint, U16 data_len,U8* data/*,BOOL_TYPE gestion_data_size*/);
extern void APP_Gestion_Wifi_Send();
extern void APP_Gestion_Wifi_TestSend();
extern void APP_Manage_Wifi_Find_Client_Leave(U8 endpoint);
extern BOOL_TYPE APP_Manage_Wifi_Find_Client(U8 endpoint,U8 *position_client);
#ifdef _PROGRAMATION
extern void APP_SendTrame_Flashage_OK(void);
extern void APP_SendTrame_Flashage_Error(void);
extern void APP_SendTrame_Flashage_RUNNING(U8 APP_Rpg_all_module_step,U32 APP_Flashage_Adr_module_reprog,U8 APP_Module_reprog,U8 APP_Module_nb_reprog,U8 P_L_Can_step_can_reprog,U32 P_L_Can_donnes_en_cours,U32 P_L_Can_nb_donnee);
extern void APP_SendTrame_Liste_Module_RUNNING(void);
extern void APP_SendTrame_Historique_RUNNING(U8 P_L_Can_step_historique);
extern void APP_SendTrame_Historique_Error(void);
extern void APP_SendTrame_Historique_OK(void);
extern void APP_SendTrame_Change_adresse_OK(void);
extern void APP_SendTrame_Change_adresse_Error(void);
extern void APP_SendTrame_Change_adresse_RUNNING(U8 P_L_Can_step_adr_chgt);
extern void APP_SendTrame_CAN_wifi(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght);
#ifdef _TEST_ETHERNET
extern void APP_SendTrame_CAN_ethernet(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght);
#endif
extern void APP_SendTrame_Version_File_Error(void);
extern void APP_SendTrame_Version_File_OK(void);
extern void APP_SendTrame_Version_File_RUNNING(U8 file_encours,S8 nb_file);
#endif

#ifdef _RAMPE_SOLAIRE
extern void APP_Manage_Perte_Tablette(void);
#endif
extern void APP_Test_WIFI_Connection();

#endif

