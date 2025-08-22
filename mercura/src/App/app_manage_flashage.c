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
#include "app_manage_flashage.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_macros.h"
#include "app_manage_sd.h"
#include "app_manage_wifi.h"
#include "app_manage_grp_file.h"
#include "hwi_hal_gpio.h"
#include "p_l_can_flashage_tx.h"
#include "p_l_can_flashage_rx.h"
#include "ff.h"
#include "user_diskio.h"
#include "fatfs.h"
#include "p_l_can1_rx.h"
#include "string.h"
#include "p_l_bx310x.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
//flag indiquant qu'une carte SD autorisant la reprogramation de module
extern BOOL_TYPE            P_L_CarteSd_Programation_vue;
extern U8                   APP_Nb_file_grp;
extern U8                   APP_File_encours_grp;

//etapes de la reprogramation d un module
extern P_L_CAN_REPROG_MOD_TYPE     P_L_Can_step_can_reprog;
//etapes du chagement adresse
extern P_L_CAN_CHGT_ADR_STEP       P_L_Can_step_adr_chgt;

//nombre de donnee a reprogrammer
extern U32                         P_L_Can_state_donnee;
//nombre de donnee a reprogrammer
extern U32                         P_L_Can_nb_donnee;

extern U32                  APP_Nb_data_file_grp;
extern U32                  APP_File_data_encours_grp;

//etapes de la lecture d un historique
extern P_L_CAN_HISTORIQUE_TYPE     P_L_Can_step_historique;

extern U32                  S_S_1MS_U32_TIME;

extern BOOL_TYPE            APP_FileWifi_Open;

extern U32                  APP_FileWifi_Open_timer;
extern BOOL_TYPE            APP_FileWifi_Open_lock;



extern const TCHAR          APP_INFO_FLASH_NAME[];
extern const TCHAR          APP_INFO_USER_NAME[];
extern const TCHAR          APP_INFO_BLOC_NAME[];
extern const TCHAR          APP_DIR_GPR_FILE_NAME[];
extern const TCHAR          APP_PAS_DE_FICHIER_NAME[];

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
//step for programming module
APP_FLASHAGE_STEP           APP_Flashage_state;
U16                         APP_Timer_Led;
BOOL_TYPE                   APP_Flip_Flop_Led;
BOOL_TYPE                   APP_Flip_Flop_Reveil;
//endpoint ask flashage
U8                          APP_Flashage_endpoint;

//ask module adresse change
BOOL_TYPE                   APP_Flashage_chgt_adr_module_ask;
//adresse du module ciblé
U32                         APP_Adr_module_chgt_adr;
//adresse du module ciblé
U8                          APP_Adr_log_module_chgt_adr;


//ask send module liste
BOOL_TYPE                   APP_Flashage_liste_module_ask;

/////////////////////////////////////////////////////////////////////
//   REPORG ALL MODULE
////////////////////////////////////////////////////////////////////

//suivit
//numero du module en cours reprog
U8                          APP_Module_reprog;
//nombre de module a reprogrammer
U8                          APP_Module_nb_reprog;
//step for programming all module
APP_RPG_ALL_STEP_TYPE       APP_Rpg_all_module_step;

//ask module reprog all
BOOL_TYPE                   APP_Flashage_reprog_all_module_ask;


FATFS                       APP_Flashage_fs;

//temps de reférence au moment du flashage
APP_FLASHAGE_TIME_TYPE      APP_Flashage_time;

//mon du fichier a flasher OS
U8                          APP_Flashage_user_reprog[APP_TAILLE_INFO_USER];

//mon du fichier a flasher Cfg
U8                          APP_Flashage_societe_reprog[APP_TAILLE_INFO_SOCIETE];

//adresse du module a flasher
U32                         APP_Flashage_Adr_module_reprog;

//mon du fichier a flasher OS
U8                          APP_Flashage_file_os[APP_CHEMIN_SIZE];

//mon du fichier a flasher Cfg
U8                          APP_Flashage_file_cfg[APP_CHEMIN_SIZE];


//flag indiquant si Os ou Cfg en cours de reprog
BOOL_TYPE                   APP_Module_reprog_os_not_cfg;

//position dans le fichier du module en cours de flashage
U32                         APP_Flashage_pose_file_flash;

//liste des blocs vue sur le reseau CAN
APP_MODULE_VUE_TYPE         APP_Flashage_module[APP_NB_MAX_FLASHAGE_MODULE];
//memorise les adresse vue entre chaque demande
U32                         APP_Flashage_module_memo_adr[APP_NB_MAX_FLASHAGE_MODULE];

APP_FLASHAGE_OSCFG_TYPE     APP_Flashage_OS_CFG;
/////////////////////////////////////////////////////////////////////
//   HISTORIQUE MODULE
////////////////////////////////////////////////////////////////////
//ask module historique
BOOL_TYPE                   APP_Flashage_historique_module_ask;
//adresse du module ciblé
U32                         APP_Adr_module_historique;
//taille de historique a lire
BOOL_TYPE                   APP_Full_historique;

// etatpes denvoie des donneee historique lue
U8                          P_L_Wifi_send_historique;



/////////////////////////////////////////////////////////////////////
//   DIAGNOSTIQUE MODULE
////////////////////////////////////////////////////////////////////

//flag indiquant que l on est en mode diag wifi
BOOL_TYPE                   P_L_Wifi_CAN_diag_on;
//endpoint diagnostique
U8                          P_L_Wifi_CAN_diag_endpoint;
//index de tableau ecriture trame CAN
U8                          APP_Wifi_count_trame_can;
//tableau de flag indiquant buffer de reception CAN plein
BOOL_TYPE                   APP_Wifi_Send_BuffeSendCan[APP_Wifi_CAN_NB_BUFFER];
//index du tableau ou l on doit mettre les trame can recue
U8                          APP_Wifi_BuffeSendCan_write_num;
//tableau des donnee can recue
U8                          APP_Wifi_BuffeSendCan[APP_Wifi_CAN_NB_BUFFER][APP_Wifi_TAILLE_DONNE_CAN];
//index ecriture des donnee CAN recue
U8                          APP_Wifi_BuffeSendCan_index;
//index du tableau que l on doit envoye
U8                          APP_Wifi_BuffeSendCan_Tx_num;

/////////////////////////////////////////////////////////////////////
//   MISE A JOUR FILE VERSION
////////////////////////////////////////////////////////////////////
//ask mise a jour version file
BOOL_TYPE                   APP_Flashage_version_ask;
S8                          APP_Flashage_version_nb_file;
U8                          APP_Flashage_version_file_encours;



//time out envoie CAN
BOOL_TYPE                   P_L_Can_Lock_timer_OS_CFG;
U32                         P_L_Can_Timer_timer_OS_CFG;
/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static APP_FLASHAGE_STATE APP_Flashage_Reprog_ALL(void);
static void APP_Manage_Add_Module_Vue_init(void);
static void APP_Manage_Diagnostique_Init(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Flashage
 *  Description     : gestion
 *
 *
 *******************************************************************************/
void APP_Manage_Flashage(void)
{
APP_FLASHAGE_STATE result;
BOOL_TYPE res;

    if (TRUE == P_L_CarteSd_Programation_vue)
    {
        switch(APP_Flashage_state)
        {
            case FLASHAGE_INIT_FILE:
                if (TRUE == APP_Generation_GPR())
                {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
                else
                {
                    APP_Flashage_state = FLASHAGE_ERROR_FILE;
                }
            break;

            case FLASHAGE_WAIT_EVENT:
                if (TRUE == APP_Flashage_chgt_adr_module_ask)
                {
                    APP_Flashage_state = FLASHAGE_CHANGE_ADR;

                    P_L_Can_Manage_Change_Adr_Init(APP_Adr_module_chgt_adr,APP_Adr_log_module_chgt_adr);
                }
                else if (TRUE == APP_Flashage_reprog_all_module_ask)
                {
                    APP_Flashage_state = FLASHAGE_REPROG_ALL;

                    APP_Rpg_all_module_step = RGP_ALL_INIT;
                }
                else if (TRUE == APP_Flashage_historique_module_ask)
                {
                   APP_Flashage_state = FLASHAGE_HISTORIQUE;
                   P_L_Can_Manage_Historique_Init(APP_Adr_module_historique,APP_Full_historique);
                }
                else if (TRUE == P_L_Wifi_CAN_diag_on)
                {
                    APP_Manage_Diagnostique_Init();
                    APP_Flashage_state = FLASHAGE_DIAGNOSTIQUE;
                }
                else if (TRUE == APP_Flashage_version_ask)
                {
                    APP_Flashage_version_nb_file = -1;
                    APP_Flashage_version_file_encours = 0;

                    APP_Flashage_state = FLASHAGE_VERSION_FILE;
                }
            break;

            case FLASHAGE_CHANGE_ADR:
                result = P_L_Can_Manage_Change_Adr();
                if (FL_OK == result)
                {
                    APP_Flashage_state = FLASHAGE_CHANGE_OK;
                }
                else if (FL_ERROR == result)
                {
                    APP_Flashage_state = FLASHAGE_CHANGE_ERROR;
                }
            break;

            case FLASHAGE_CHANGE_OK:
                 if (FALSE == APP_Flashage_chgt_adr_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_CHANGE_ERROR:
                 if (FALSE == APP_Flashage_chgt_adr_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_REPROG_ALL:
                if (FALSE == APP_Flashage_reprog_all_module_ask)
                {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                    APP_Rpg_all_module_step = RGP_ALL_INIT;
                    P_L_Can_Manage_Reprog_Init(0x00,NULL);
                }
                else
                {
                    result = APP_Flashage_Reprog_ALL();
                    if (FL_OK == result)
                    {
                        APP_Flashage_state = FLASHAGE_REPROG_ALL_OK;
                    }
                    else if (FL_ERROR == result)
                    {
                        APP_Flashage_state = FLASHAGE_REPROG_ALL_ERROR;
                    }
                }
            break;

            case FLASHAGE_REPROG_ALL_OK:
                 if (FALSE == APP_Flashage_reprog_all_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_REPROG_ALL_ERROR:
                 if (FALSE == APP_Flashage_reprog_all_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_ERROR_FILE:
                if (TRUE == APP_Flashage_chgt_adr_module_ask)
                {
                   APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
                else if (TRUE == APP_Flashage_historique_module_ask)
                {
                   APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
                else if (TRUE == P_L_Wifi_CAN_diag_on)
                {
                   APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
                else if (TRUE == APP_Flashage_version_ask)
                {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
            break;

            case FLASHAGE_ERROR:
            break;

            case FLASHAGE_HISTORIQUE:
                result = P_L_Can_Manage_Read_Historique();
                if (FL_OK == result)
                {
                    APP_Flashage_state = FLASHAGE_HISTORIQUE_OK;
                }
                else if (FL_ERROR == result)
                {
                    APP_Flashage_state = FLASHAGE_HISTORIQUE_ERROR;
                }
            break;

            case FLASHAGE_HISTORIQUE_OK:
                 if (FALSE == APP_Flashage_historique_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_HISTORIQUE_ERROR:
                 if (FALSE == APP_Flashage_historique_module_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_DIAGNOSTIQUE:
                if (FALSE == P_L_Wifi_CAN_diag_on)
                {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                }
            break;

             case FLASHAGE_VERSION_FILE:
                result = APP_Generation_File_Version();
                if (FL_OK == result)
                {
                    APP_Flashage_state = FLASHAGE_VERSION_FILE_OK;
                }
                else if (FL_ERROR == result)
                {
                    APP_Flashage_state = FLASHAGE_VERSION_FILE_ERROR;
                }
            break;

            case FLASHAGE_VERSION_FILE_OK:
                 if (FALSE == APP_Flashage_version_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            case FLASHAGE_VERSION_FILE_ERROR:
                 if (FALSE == APP_Flashage_version_ask)
                 {
                    APP_Flashage_state = FLASHAGE_WAIT_EVENT;
                 }
            break;

            default:
                APP_Flashage_state = FLASHAGE_INIT_FILE;
            break;
        }

        if (TRUE == APP_FileWifi_Open)
        {
            BT_TIMER_LOCK(APP_FileWifi_Open_timer,S_S_1MS_U32_TIME,APP_TIMEOUT_OUVERTURE_FICHIER_WIFI,APP_FileWifi_Open_lock,res);
            if(TRUE == res)
            {
                APP_Close_WIFI_File();
            }
        }
    }
}


/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Reprog_ALL
 *  Description     : reprogramaton d un module
 *
 *
 *******************************************************************************/
static APP_FLASHAGE_STATE APP_Flashage_Reprog_ALL(void)
{
APP_FLASHAGE_STATE state;
APP_FLASHAGE_STATE result;
U32         line_en_cours;

FIL         fileInfo;
U32         PosFileInfo;

FIL         fileFlash;
U8          buff_Flash[LgMaxLigne];
U32         nb_data_lue_flash;

FIL         fileBloc;
U32         PosFileBloc;
 U8         buff_Bloc[LgMaxLigne];
U32         nb_data_lue_bloc;

U8          boucle;
BOOL_TYPE   module_found;
U32         adr_module;
U16         Phy_adresse;
U8          id_type;
BOOL_TYPE   double_adresse;
U8          adr_logique;
//position dans le fichier du module en cours de flashage
U32         APP_Temp_U32;

    state = FL_RUNNIG;

    switch (APP_Rpg_all_module_step)
    {
        case RGP_ALL_INIT:
            APP_Flashage_pose_file_flash = 0;
            APP_Module_reprog = 0;

            if (FR_OK == f_mount(&APP_Flashage_fs, "", 0))
            {
                APP_Rpg_all_module_step = RGP_INFO_CHECK;
            }
            else
            {
                APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
            }
        break;

        case RGP_INFO_CHECK:
            //ouverture de la sequence de flashage
            if (FR_OK == f_open(&fileInfo,APP_INFO_USER_NAME,FA_READ))
            {
                PosFileInfo = 0;
                f_lseek(&fileInfo,PosFileInfo);

                //lecture ligne info user
                //P_L_F_read_line(&MyfileInfo,Buff_flash,&nb_data_lue_flash,&MyfilePosInfo);
                APP_Read_String_line(&fileInfo,buff_Flash,LgMaxLigne,&nb_data_lue_flash,&PosFileInfo,&line_en_cours);
                for(boucle=0;boucle<APP_TAILLE_INFO_USER;boucle++)
                {
                    if (boucle<nb_data_lue_flash)
                    {
                        APP_Flashage_user_reprog[boucle] = buff_Flash[boucle];
                    }
                    else
                    {
                        APP_Flashage_user_reprog[boucle] = ' ';
                    }
                }

                //lecture ligne info societe
                //P_L_F_read_line(&MyfileInfo,Buff_flash,&nb_data_lue_flash,&MyfilePosInfo);
                APP_Read_String_line(&fileInfo,buff_Flash,LgMaxLigne,&nb_data_lue_flash,&PosFileInfo,&line_en_cours);
                for(boucle=0;boucle<APP_TAILLE_INFO_SOCIETE;boucle++)
                {
                    if (boucle<nb_data_lue_flash)
                    {
                        APP_Flashage_societe_reprog[boucle] = buff_Flash[boucle];
                    }
                    else
                    {
                        APP_Flashage_societe_reprog[boucle] = ' ';
                    }
                }

                f_close(&fileInfo);

            }

            APP_Module_nb_reprog = 0;

            //ouverture de la sequence de flashage
            if (FR_OK == f_open(&fileFlash,APP_INFO_FLASH_NAME,FA_READ))
            {
                nb_data_lue_flash = 1;

                while (0 != nb_data_lue_flash)
                {
                    APP_Read_String_line(&fileFlash,buff_Flash,LgMaxLigne,&nb_data_lue_flash,&APP_Temp_U32,&line_en_cours);

                    if (0 != nb_data_lue_flash)
                    {
                        if (0 == strncmp((char*)"FIN",(char*)buff_Flash,3))
                        {
                            nb_data_lue_flash = 0;
                        }
                        else
                        {
                            if (('A'<= buff_Flash[nb_data_lue_flash-1])&&('F'>= buff_Flash[nb_data_lue_flash-1]))
                            {
                                APP_Module_nb_reprog++;
                            }
                        }
                    }
                }

                f_close(&fileFlash);
                APP_Rpg_all_module_step = RGP_ALL_CHECK;
            }
            else
            {
                APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
            }
        break;



        case RGP_ALL_CHECK:
            //ouverture de la sequence de flashage
            if (FR_OK == f_open(&fileFlash,APP_INFO_FLASH_NAME,FA_READ))
            {
                f_lseek(&fileFlash,APP_Flashage_pose_file_flash);

                //ouverture des info sur les modules

                if (FR_OK == f_open(&fileBloc,APP_INFO_BLOC_NAME,FA_READ))
                {
                    PosFileBloc = 0;
                    f_lseek(&fileBloc,PosFileBloc);

                    //lecture ligne module a flasher
                    //P_L_F_read_line(&MyfileFlash,Buff_flash,&nb_data_lue_flash,&MyfilePosFlash);
                    APP_Read_String_line(&fileFlash,buff_Flash,LgMaxLigne,&nb_data_lue_flash,&APP_Flashage_pose_file_flash,&line_en_cours);
                    if (0 == nb_data_lue_flash)
                    {
                        APP_Rpg_all_module_step = RGP_ALL_END_OK;
                    }
                    else if (0 == strncmp((char*)"FIN",(char*)buff_Flash,3))
                    {
                        APP_Rpg_all_module_step = RGP_ALL_END_OK;
                    }
                    else if (('A'<= buff_Flash[nb_data_lue_flash-1])&&('F'>= buff_Flash[nb_data_lue_flash-1]))
                    {
                        module_found = FALSE;
                        while (FALSE == module_found)
                        {
                            //lecture info module
                            //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                            APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                            //if (nb_data_lue_bloc > APP_FIN_SIZE)
                            if (0 != strncmp((char*)"FIN",(char*)buff_Bloc,3))
                            {
                                module_found = TRUE;

                                //test du nom sans son adresse
                                for(boucle=0;boucle<(nb_data_lue_flash-2);boucle++)
                                {
                                    if (buff_Bloc[boucle] != buff_Flash[boucle])
                                    {
                                        module_found = FALSE;
                                        boucle = nb_data_lue_flash;
                                    }
                                }

                                if (FALSE == module_found)
                                {
                                    //adresse can
                                    //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                    APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                    //fichier OS a flasher
                                    //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                    APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                    //fichier cfg a flasher
                                    //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                    APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                    //ligne vide
                                    //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                    APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                }
                            }
                            else
                            {
                               break;
                            }
                        }

                        if (TRUE == module_found)
                        {
                            //adresse can (exemple "0x80" "0x100")
                            //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                            APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                            if ( ((nb_data_lue_bloc == 4)||(nb_data_lue_bloc == 5))&&(buff_Bloc[0]='0')&&(buff_Bloc[1]='x'))
                            {
                                adr_module = APP_HexbinX(&buff_Bloc[2],nb_data_lue_bloc-2);
                                id_type = (U8)((adr_module & P_L_CC_MASK_TYPE ) >> 4);
                                Phy_adresse = (U16)((adr_module & P_L_CC_MASK_PHYSIQUE_ADR) >> 6);

                                double_adresse = FALSE;

                                if (MODULE_CONTROLPANEL ==  id_type)
                                {
                                    if (PHY_FACADE_GRAPH_DISPLAY == Phy_adresse)
                                    {
                                        double_adresse = TRUE;
                                    }
                                    else if (PHY_FACADE_TPANEL_DISPLAY == Phy_adresse)
                                    {
                                        double_adresse = TRUE;
                                    }
                                }

                                adr_logique = (buff_Flash[nb_data_lue_flash-1] - 'A' + 1);
                                if (TRUE == double_adresse)
                                {
                                    adr_logique = ((adr_logique*2)-1);
                                }

                                APP_Flashage_Adr_module_reprog = adr_module+adr_logique;

                                for(boucle=0;boucle<APP_DIR_SIZE;boucle++)
                                {
                                    APP_Flashage_file_os[boucle] = APP_DIR_GPR_FILE_NAME[boucle];
                                    APP_Flashage_file_cfg[boucle] = APP_DIR_GPR_FILE_NAME[boucle];
                                }

                                //fichier OS a flasher
                                //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                for(boucle=0;boucle<(nb_data_lue_bloc+2);boucle++)
                                {
                                    APP_Flashage_file_os[boucle+APP_DIR_SIZE] = buff_Bloc[boucle];
                                }
                                //fichier cfg a flasher
                                //P_L_F_read_line(&MyfileBloc,Buff_bloc,&nb_data_lue_bloc,&MyfilePosBloc);
                                APP_Read_String_line(&fileBloc,buff_Bloc,LgMaxLigne,&nb_data_lue_bloc,&PosFileBloc,&line_en_cours);
                                for(boucle=0;boucle<(nb_data_lue_bloc+2);boucle++)
                                {
                                    APP_Flashage_file_cfg[boucle+APP_DIR_SIZE] = buff_Bloc[boucle];
                                }
                                APP_Module_reprog++;
                                APP_Rpg_all_module_step = RGP_ALL_OS;

                            }
                            else
                            {
                                APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
                            }
                        }
                        else
                        {
                            APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
                        }
                    }
                    else
                    {
                        APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
                    }

                    f_close(&fileBloc);
                }
                else
                {
                    APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
                }

                f_close(&fileFlash);
            }
            else
            {
               APP_Rpg_all_module_step = RGP_ALL_ERROR_FILE;
            }
        break;

        case RGP_ALL_OS:
            if ((APP_Flashage_OS_CFG == OS_CFG) || (APP_Flashage_OS_CFG == OS_SEUL))
            {
                P_L_Can_Manage_Reprog_Init(APP_Flashage_Adr_module_reprog,APP_Flashage_file_os);
                APP_Module_reprog_os_not_cfg = TRUE;
                APP_Rpg_all_module_step = RGP_ALL_OS_WAIT;
            }
            else
            {
                APP_Rpg_all_module_step = RGP_ALL_CFG;
            }
        break;

        case RGP_ALL_OS_WAIT:
            result = P_L_Can_Manage_Reprog_One();
            if (FL_ERROR == result)
            {
                APP_Rpg_all_module_step = RGP_ALL_ERROR_OS;
            }
            else if (FL_OK == result)
            {
                APP_Rpg_all_module_step = RGP_ALL_CFG;
             //   BT_TIMER_INIT_LOCK(P_L_Can_Timer_timer_OS_CFG,S_S_1MS_U32_TIME,P_L_Can_Lock_timer_OS_CFG);
            }
        break;

        case RGP_ALL_CFG:
            if ((APP_Flashage_OS_CFG == OS_CFG) || (APP_Flashage_OS_CFG == CFG_SEUL))
            {
                module_found = FALSE;

                //test du nom si non applicable
                for(boucle=0;boucle<13;boucle++)
                {
                    if (APP_Flashage_file_cfg[boucle] != APP_PAS_DE_FICHIER_NAME[boucle])
                    {
                        module_found = TRUE;
                        boucle = 13;
                    }
                }

                if (TRUE == module_found)
                {
                    P_L_Can_Manage_Reprog_Init(APP_Flashage_Adr_module_reprog,APP_Flashage_file_cfg);
                    APP_Module_reprog_os_not_cfg = FALSE;
                    APP_Rpg_all_module_step = RGP_ALL_CFG_WAIT;
                }
                else
                {
                    APP_Rpg_all_module_step = RGP_ALL_CHECK;
                }
            }
            else
            {
                APP_Rpg_all_module_step = RGP_ALL_CHECK;
            }
        break;

        case RGP_ALL_CFG_WAIT:
            result = P_L_Can_Manage_Reprog_One();
            if (FL_ERROR == result)
            {
                APP_Rpg_all_module_step = RGP_ALL_ERROR_CFG;
            }
            else if (FL_OK == result)
            {
                APP_Rpg_all_module_step = RGP_ALL_CHECK;
            }
        break;

        case RGP_ALL_ERROR_OS:
        case RGP_ALL_ERROR_CFG:
        case RGP_ALL_ERROR_FILE:
            state = FL_ERROR;
            f_mount(NULL, "", 1);
            break;

        case RGP_ALL_END_OK:
            f_mount(NULL, "", 1);
            state = FL_OK;
            break;
    }
return state;
}


/*******************************************************************************
 *
 *  Function Name   : APP_Manage_LED_Flashage
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_1ms_Flashage(void)
{
U8 boucle;
U8 position_client;


    if (TRUE == P_L_CarteSd_Programation_vue)
    {
        APP_Timer_Led++;

////////////////////////////////////////////////
//              8 ms
///////////////////////////////////////////////
        if ( 5 == (APP_Timer_Led % 8) )              // tt les 8ms
        {
            if (APP_Flashage_state == FLASHAGE_DIAGNOSTIQUE)
            {
                if (TRUE == APP_Manage_Wifi_Find_Client(P_L_Wifi_CAN_diag_endpoint,&position_client))
                {
                    for (boucle=0;boucle<APP_Wifi_CAN_NB_BUFFER;boucle++)
                    {
                        if (TRUE == APP_Wifi_Send_BuffeSendCan[APP_Wifi_BuffeSendCan_Tx_num])
                        {
                            P_L_Wifi_Tx_data(P_L_Wifi_CAN_diag_endpoint,4+2+(APP_Wifi_CAN_TRAME_MAX*APP_Wifi_CAN_TAILLE_TRAME),APP_Wifi_BuffeSendCan[boucle]);
                            APP_Wifi_Send_BuffeSendCan[APP_Wifi_BuffeSendCan_Tx_num] = FALSE;
                            APP_Wifi_BuffeSendCan_Tx_num++;
                            if (APP_Wifi_BuffeSendCan_Tx_num >= APP_Wifi_CAN_NB_BUFFER)
                            {
                               APP_Wifi_BuffeSendCan_Tx_num = 0;
                            }

                            boucle = APP_Wifi_CAN_NB_BUFFER;
                        }
                    }
                }
            }
        }

/////////////////////////////////////////////////////////////////////
//          512 ms
/////////////////////////////////////////////////////////////////////
        if(APP_Timer_Led>= 512)
        {
            APP_Timer_Led = 0;

            if (APP_Flashage_state == FLASHAGE_REPROG_ALL_ERROR)
            {
               APP_SendTrame_Flashage_Error();
            }
            else if (APP_Flashage_state == FLASHAGE_REPROG_ALL_OK)
            {
               APP_SendTrame_Flashage_OK();
            }
            else if (APP_Flashage_state == FLASHAGE_REPROG_ALL)
            {
               APP_SendTrame_Flashage_RUNNING(APP_Rpg_all_module_step,APP_Flashage_Adr_module_reprog,APP_Module_reprog,APP_Module_nb_reprog,P_L_Can_step_can_reprog,P_L_Can_state_donnee,P_L_Can_nb_donnee);
            }
            else if (APP_Flashage_state == FLASHAGE_CHANGE_ERROR)
            {
               APP_SendTrame_Change_adresse_Error();
            }
            else if (APP_Flashage_state == FLASHAGE_CHANGE_OK)
            {
               APP_SendTrame_Change_adresse_OK();
            }
            else if (APP_Flashage_state == FLASHAGE_CHANGE_ADR)
            {
               APP_SendTrame_Change_adresse_RUNNING(P_L_Can_step_adr_chgt);
            }
            else if (APP_Flashage_state == FLASHAGE_HISTORIQUE_ERROR)
            {
               APP_SendTrame_Historique_Error();
            }
            else if (APP_Flashage_state == FLASHAGE_HISTORIQUE_OK)
            {
               APP_SendTrame_Historique_OK();
            }
            else if (TRUE == APP_Flashage_historique_module_ask)
            {
                APP_SendTrame_Historique_RUNNING(P_L_Can_step_historique);
            }
            else if (APP_Flashage_state == FLASHAGE_VERSION_FILE_ERROR)
            {
               APP_SendTrame_Version_File_Error();
            }
            else if (APP_Flashage_state == FLASHAGE_VERSION_FILE_OK)
            {
               APP_SendTrame_Version_File_OK();
            }
            else if (TRUE == APP_Flashage_version_ask)
            {
                APP_SendTrame_Version_File_RUNNING(APP_Flashage_version_file_encours,APP_Flashage_version_nb_file);
            }
            // a laisser en dernier
            else if (TRUE == APP_Flashage_liste_module_ask)
            {
                APP_SendTrame_Liste_Module_RUNNING();
            }

            APP_Flip_Flop_Reveil = !APP_Flip_Flop_Reveil;
            HWI_Reveil_Cmd(APP_Flip_Flop_Reveil);

            if (APP_Flashage_state == FLASHAGE_WAIT_EVENT)
            {
                P_L_Can_Send_Wake_UP();
            }

            switch(APP_Flashage_state)
            {
                case FLASHAGE_INIT_FILE:
                    APP_Flip_Flop_Led =!APP_Flip_Flop_Led;
                    HWI_Led_2_Rouge_Cmd(APP_Flip_Flop_Led);
                break;

                case FLASHAGE_WAIT_EVENT:
                    HWI_Led_2_Rouge_Cmd(FALSE);
                    HWI_Led_2_Verte_Cmd(TRUE);
                break;

                case FLASHAGE_REPROG_ALL:
                case FLASHAGE_CHANGE_ADR:
                    APP_Flip_Flop_Led =!APP_Flip_Flop_Led;
                    HWI_Led_2_Verte_Cmd(APP_Flip_Flop_Led);
                break;

                case FLASHAGE_REPROG_ALL_ERROR:
                case FLASHAGE_CHANGE_ERROR:
                case FLASHAGE_ERROR_FILE:
                case FLASHAGE_ERROR:
                    HWI_Led_2_Rouge_Cmd(TRUE);
                    HWI_Led_2_Verte_Cmd(FALSE);
                break;

                default:
                break;
            }
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Add_Module_Vue
 *  Description     : gestion des module vue sur le reseau can
 *
 *
 *******************************************************************************/
void APP_Manage_Add_Module_Vue(U32 adresse)
{
BOOL_TYPE module_vue;
U8  vide_vue;
U8 boucle;

    if ((adresse > 0x0000000F) && (0x00000000 != (adresse & 0x00000400)))
    {
        module_vue = FALSE;
        vide_vue = APP_NB_MAX_FLASHAGE_MODULE;

        for(boucle=0;boucle<APP_NB_MAX_FLASHAGE_MODULE;boucle++)
        {
            if (adresse == APP_Flashage_module[boucle].adresse)
            {
                // re initialisation de la tempo vue
                BT_TIMER_INIT_LOCK(APP_Flashage_module[boucle].timer,S_S_1MS_U32_TIME,APP_Flashage_module[boucle].lock);
                module_vue = TRUE;
                boucle = APP_NB_MAX_FLASHAGE_MODULE;
            }
            else if ((ADR_NU == APP_Flashage_module[boucle].adresse)&&(vide_vue == APP_NB_MAX_FLASHAGE_MODULE))
            {
                vide_vue = boucle;
            }
        }

        if ((FALSE == module_vue)&&(vide_vue < APP_NB_MAX_FLASHAGE_MODULE))
        {
            APP_Flashage_module[vide_vue].adresse = adresse;
            // re initialisation de la tempo vue
            BT_TIMER_INIT_LOCK(APP_Flashage_module[vide_vue].timer,S_S_1MS_U32_TIME,APP_Flashage_module[vide_vue].lock);
        }
    }
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Add_Module_Vue
 *  Description     : gestion des module vue sur le reseau can
 *
 *
 *******************************************************************************/
void APP_Manage_Add_Module_Check(void)
{
U8 boucle;
BOOL_TYPE result;

    if (TRUE == P_L_CarteSd_Programation_vue)
    {
        for(boucle=0;boucle<APP_NB_MAX_FLASHAGE_MODULE;boucle++)
        {
            if (ADR_NU != APP_Flashage_module[boucle].adresse)
            {
                BT_TIMER_LOCK(APP_Flashage_module[boucle].timer,S_S_1MS_U32_TIME,APP_FILTRAGE_PRTE_MODULE_CAN_CAN,APP_Flashage_module[boucle].lock,result);
                if(TRUE == result)
                {
                    APP_Flashage_module[boucle].adresse = ADR_NU;
                }
            }
        }
    }
}
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Add_Module_Vue_init
 *  Description     : initialisation des module sur le reseau can
 *
 *
 *******************************************************************************/
static void APP_Manage_Add_Module_Vue_init(void)
{
U8 boucle;

    for(boucle=0;boucle<APP_NB_MAX_FLASHAGE_MODULE;boucle++)
    {
       APP_Flashage_module[boucle].adresse = ADR_NU;
       APP_Flashage_module_memo_adr[boucle] = ADR_NU;
    }
}
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Diagnostique_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
static void APP_Manage_Diagnostique_Init(void)
{
U8 boucle;

    APP_Wifi_count_trame_can = 0;

    for(boucle=0;boucle<APP_Wifi_CAN_NB_BUFFER;boucle++)
    {
        APP_Wifi_Send_BuffeSendCan[boucle] = FALSE;
    }

    APP_Wifi_BuffeSendCan_write_num = 0;
    APP_Wifi_BuffeSendCan_index = 0;
    APP_Wifi_BuffeSendCan_Tx_num = 0;
}
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Flashage_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_Flashage_Init(void)
{
    APP_Flashage_state = FLASHAGE_INIT_FILE;
    APP_Nb_file_grp = 0;
    APP_File_encours_grp = 0;

    APP_Nb_data_file_grp = 0;
    APP_File_data_encours_grp = 0;

    APP_Timer_Led = 0;

    APP_Flashage_chgt_adr_module_ask = FALSE;
    APP_Flashage_reprog_all_module_ask = FALSE;
    APP_Flashage_liste_module_ask = FALSE;
    APP_Flashage_historique_module_ask = FALSE;
    APP_Flashage_version_ask = FALSE;
    APP_Manage_Add_Module_Vue_init();

    APP_Manage_Diagnostique_Init();

}




