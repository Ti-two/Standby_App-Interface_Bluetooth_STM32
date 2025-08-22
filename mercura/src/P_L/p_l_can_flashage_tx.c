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
#include "p_l_can_flashage_tx.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l_can_flashage_rx.h"
#include "lib_macros.h"
#include "hwi_hal_can.h"
#include "app_manage_flashage.h"
#include "ff.h"
#include "user_diskio.h"
#include "fatfs.h"
#include "app_manage_grp_file.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32                  S_S_1MS_U32_TIME;
//step for programming module
extern APP_FLASHAGE_STEP    APP_Flashage_state;

/////////////////////////////////////////////////////////////////////
//   REPORG INFO
////////////////////////////////////////////////////////////////////
//temps de reférence au moment du flashage
extern APP_FLASHAGE_TIME_TYPE            APP_Flashage_time;
//mon du fichier a flasher OS
extern U8                   APP_Flashage_user_reprog[APP_TAILLE_INFO_USER];

//mon du fichier a flasher Cfg
extern U8                   APP_Flashage_societe_reprog[APP_TAILLE_INFO_SOCIETE];

// adresse globale du module de communication sur le reseaux CAN
extern U32                   APP_Adresse_module;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/

/////////////////////////////////////////////////////////////////////
//   CHAGEMENT ADRESSE
////////////////////////////////////////////////////////////////////
//etapes du chagement adresse
P_L_CAN_CHGT_ADR_STEP       P_L_Can_step_adr_chgt;
// nombre essaie envoie de la trame can
U8                          P_L_Can_nb_essai_send_chgt_adr;
//time out envoie CAN
BOOL_TYPE                   P_L_Can_Lock_send_chgt_adr;
U32                         P_L_Can_Timer_send_chgt_adr;
//adresse du module ciblé
U32                         P_L_Can_adr_module;
//adresse logique a changer
U8                          P_L_Can_adr_logique;

//flag indiquant que l on reprogramme une NEX XXL
BOOL_TYPE                   P_L_Can_reprog_New_Facade_XXL;
//flag indiquant que l on reprogramme une T PANEL
BOOL_TYPE                   P_L_Can_reprog_New_Facade_T_PANEL;

/////////////////////////////////////////////////////////////////////
//   REPORG ONE MODULE
////////////////////////////////////////////////////////////////////
//etapes de la reprogramation d un module
P_L_CAN_REPROG_MOD_TYPE     P_L_Can_step_can_reprog;

// suivit

U16                         P_L_Can_nb_transfert_next_data;
U16                         P_L_Can_nb_transfert_lot;


// nombre essaie envoie de la trame can
U8                          P_L_Can_nb_essai_send_reprog;
//time out envoie CAN
BOOL_TYPE                   P_L_Can_Lock_send_reprog;
U32                         P_L_Can_Timer_send_reprog;

//cheksum du fichier a flasher
U8                          P_L_Can_cheksum_MSB;
U8                          P_L_Can_cheksum_LSB;

//flag indiquent si c est un CCM a flasher
BOOL_TYPE                   P_L_Can_fichierCCM;

//nom du fichier a flasher
U8                          P_L_Can_Name_File[8];

//flags indiquant les zones CCM a flasher
BOOL_TYPE                   P_L_Can_zone1_XXL;
BOOL_TYPE                   P_L_Can_zone2_XXL;

//flags indiquant une interruption de data
BOOL_TYPE                   P_L_Can_flag_not_data;

//taille du segement en cours envoie;
U16                         P_L_Can_taille_max_data_send;
//donne de se segement en cours envoie
U16                         P_L_Can_data_flash_segement;
//donnee en cours de flashage
U32                         P_L_Can_donnes_en_cours;
//nombre de donnee a reprogrammer
U32                         P_L_Can_nb_donnee;

//nombre de donnee a reprogrammer
U32                         P_L_Can_state_donnee;

//adresse de debut reprogramation
U32                         P_L_Can_adresse_start;

//taille du buffeur de reception de celui que l on va flasher
U16                         P_L_Can_taille_buffer_ECU;

//tableau data a flasher
U8                          P_L_Can_Data_Segement[P_L_TAILLE_BUFFER_SEGMENT];

//donnee en cours d ecriture
U32                         P_L_Can_data_flash_running;

//adresse de dubut du segement a flasher
U32                         P_L_Can_start_segment_adresse;

//taille du segement a programme de 0 à P_L_TAILLE_BUFFER_SEGMENT
U16                         P_L_Can_taille_segment;

//index de transfert consecutif 3x
U16                         P_L_Can_TransfertIndex;

//indique le monbre max de paquet que l on peut envoyer en 2x
U8                          P_L_Can_nb_paquets;

//flag indiquant que c est le dernier segemen,t envoye autorise le saut en apli
BOOL_TYPE                   P_L_Can_last_segment;

//tableau de reception des donnee de l historique
U8                          P_L_Can_received_historique[P_L_CAN_TAILLE_BUFFER_HISTORIQUE];
//Nombre historique vue
U8                          P_L_Can_Nb_historique;
//index ecriture dans le tableau des historiques
U16                         P_L_Can_index_historique;

FATFS                       P_L_Can_reprog_fs;
//objet contenant les elements du fichier ouvert
FIL                         P_L_Can_file_GPR;
//position dans le fichier
U32                         P_L_Can_pos_file_GPR;

//fichier a programme
U8                          *P_L_Can_file_reprog;

/////////////////////////////////////////////////////////////////////
//   HISTORIQUE
////////////////////////////////////////////////////////////////////
//taille de la lecture de l historique
BOOL_TYPE                   P_L_Can_full_historique;
//etapes de la lecture d un historique
P_L_CAN_HISTORIQUE_TYPE     P_L_Can_step_historique;

// nombre essaie envoie de la trame can
U8                          P_L_Can_nb_essai_send_historique;
//time out envoie CAN
BOOL_TYPE                   P_L_Can_Lock_send_historique;
U32                         P_L_Can_Timer_send_historique;


//////////////////////////////////////////////////////////////////////////
//gestion semaphore envoie wifi
BOOL_TYPE                   P_L_Can_semaphore_SS;
BOOL_TYPE                   P_L_Can_semaphore_IT;

//time out envoie CAN
BOOL_TYPE                   P_L_Can_Lock_send_IT;
U32                         P_L_Can_Timer_send_IT;


/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static APP_FLASHAGE_STATE P_L_Can_Manage_Reprog_One_Schedule(void);

static BOOL_TYPE P_L_Can_Send_Requete_2x(void);
static void P_L_Can_Send_Requete_30(U8 adresse_logique);
static void P_L_Can_Send_Requete_31(void);
static void P_L_Can_Send_Requete_33(U32 adresse_module);
static void P_L_Can_Send_Requete_34(void);
static void P_L_Can_Send_Requete_36(void);
static void P_L_Can_Send_Requete_37(void);
static void P_L_Can_Send_Requete_38(void);
static void P_L_Can_Send_Requete_39(void);
static void P_L_Can_Send_Requete_40(void);
static void P_L_Can_Send_Requete_41(void);
static void P_L_Can_Send_Requete_42(BOOL_TYPE recopie_first);
static void P_L_Can_Send_Requete_43(void);
static void P_L_Can_Send_Requete_50(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Read_Historique
 *  Description     : lecture de l historique d un module
 *
 *
 *******************************************************************************/
APP_FLASHAGE_STATE  P_L_Can_Manage_Read_Historique(void)
{
APP_FLASHAGE_STATE state;
BOOL_TYPE result;

    state = FL_RUNNIG;
    switch(P_L_Can_step_historique)
    {
       case CAN_START_READ_HISTORIQUE:
            P_L_Can_nb_essai_send_historique = 0;
            P_L_Can_step_historique = CAN_RETOUR_EN_BOOT_HISTORIQUE_TX;

        //break;

        case CAN_RETOUR_EN_BOOT_HISTORIQUE_TX:
             //REQUETE $33 demande de retour en boot
             P_L_Can_step_historique = CAN_RETOUR_EN_BOOT_HISTORIQUE_RX;
             P_L_Can_Send_Requete_33(P_L_Can_adr_module);
             BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_historique,S_S_1MS_U32_TIME,P_L_Can_Lock_send_historique);
        break;

        case CAN_READ_READ_HISTORIQUE_TX:
            P_L_Can_step_historique = CAN_READ_READ_HISTORIQUE_RX;
            P_L_Can_Send_Requete_43();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_historique,S_S_1MS_U32_TIME,P_L_Can_Lock_send_historique);
        break;

        case CAN_SORTIE_BOOT_HISTORIQUE_TX:
            //REQUETE $50 demande saut dans applicatif
            P_L_Can_step_historique = CAN_SORTIE_BOOT_HISTORIQUE_RX;
            P_L_Can_Send_Requete_50();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_SEND_HISTORIQUE:
           // attente de la fin envoie de toutes les information de l'historique
        break;

        case CAN_END_READ_HISTORIQUE:
            P_L_Can_step_historique = CAN_START_READ_HISTORIQUE;
            state = FL_OK;
        break;

        case CAN_ERROR_READ_HISTORIQUE:
             state = FL_ERROR;
             P_L_Can_step_historique = CAN_START_READ_HISTORIQUE;
            break;

        default:
            BT_TIMER_LOCK(P_L_Can_Timer_send_historique,S_S_1MS_U32_TIME,P_L_CAN_TIMEOUT_SEND,P_L_Can_Lock_send_historique,result);
            if (TRUE == result)
            {
                P_L_Can_nb_essai_send_historique++;
                if (P_L_CAN_NB_ESSAI_MAX > P_L_Can_nb_essai_send_historique)
                {
                    P_L_Can_step_historique = P_L_Can_step_historique-1;
                }
                else
                {
                    P_L_Can_step_historique = CAN_ERROR_READ_HISTORIQUE;
                }
            }
        break;
    }
    return state;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Change_Adr
 *  Description     : change adresse of module
 *
 *
 *******************************************************************************/
APP_FLASHAGE_STATE P_L_Can_Manage_Change_Adr(void)
{
APP_FLASHAGE_STATE state;
BOOL_TYPE result;

    state = FL_RUNNIG;

    switch(P_L_Can_step_adr_chgt)
    {
       case CAN_START_CHGT_ADRESSE:
            P_L_Can_nb_essai_send_chgt_adr = 0;
            P_L_Can_step_adr_chgt = CAN_RETOUR_EN_BOOT_TX;

        //break;

        case CAN_RETOUR_EN_BOOT_TX:
             //REQUETE $33 demande de retour en boot
             P_L_Can_step_adr_chgt = CAN_RETOUR_EN_BOOT_RX;
             P_L_Can_Send_Requete_33(P_L_Can_adr_module);
             BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_chgt_adr,S_S_1MS_U32_TIME,P_L_Can_Lock_send_chgt_adr);
        break;

        case CAN_CHGT_LOG_ADR_TX:
            //REQUETE $30 demande de changement adresse
            P_L_Can_step_adr_chgt = CAN_CHGT_LOG_ADR_RX;
            P_L_Can_Send_Requete_30(P_L_Can_adr_logique);
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_chgt_adr,S_S_1MS_U32_TIME,P_L_Can_Lock_send_chgt_adr);
        break;

        case CAN_QUITER_BOOT_TX:
            //REQUETE $50 demande saut dans applicatif
            P_L_Can_nb_essai_send_chgt_adr = 0;
            P_L_Can_step_adr_chgt = CAN_FIN_CHGT_ADR;
            P_L_Can_Send_Requete_50();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_chgt_adr,S_S_1MS_U32_TIME,P_L_Can_Lock_send_chgt_adr);
        break;

        case CAN_FIN_CHGT_ADR:
            P_L_Can_step_adr_chgt = CAN_START_CHGT_ADRESSE;
            state = FL_OK;
        break;

        case CAN_ERROR_CHGT_ADR:
             state = FL_ERROR;
             P_L_Can_step_adr_chgt = CAN_START_CHGT_ADRESSE;
            break;

        default:
            BT_TIMER_LOCK(P_L_Can_Timer_send_chgt_adr,S_S_1MS_U32_TIME,P_L_CAN_TIMEOUT_SEND,P_L_Can_Lock_send_chgt_adr,result);
            if (TRUE == result)
            {
                P_L_Can_nb_essai_send_chgt_adr++;
                if (P_L_CAN_NB_ESSAI_MAX > P_L_Can_nb_essai_send_chgt_adr)
                {
                    P_L_Can_step_adr_chgt = P_L_Can_step_adr_chgt-1;
                }
                else
                {
                    P_L_Can_step_adr_chgt = CAN_ERROR_CHGT_ADR;
                }
            }
        break;
    }
    return state;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_flashage_Tx_IT
 *  Description     : fin de transmission trame CAN
 *
 *
 *******************************************************************************/
void P_L_Can_flashage_Tx_IT(void)
{
    if (APP_Flashage_state == FLASHAGE_REPROG_ALL)
    {
        if ((CAN_TRANSFERT_NEXT_DATA == P_L_Can_step_can_reprog) || (CAN_WRITE_HISTORIQUE_1 == P_L_Can_step_can_reprog) || (CAN_WRITE_HISTORIQUE == P_L_Can_step_can_reprog))
        {
            if (FALSE == P_L_Can_semaphore_SS)
            {
                P_L_Can_semaphore_IT = TRUE;
                BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_IT,S_S_1MS_U32_TIME,P_L_Can_Lock_send_IT);
                P_L_Can_Manage_Reprog_One_Schedule();
                P_L_Can_semaphore_IT = FALSE;
            }
        }
        else
        {
            P_L_Can_semaphore_IT = FALSE;
        }
    }
    else
    {
       P_L_Can_semaphore_IT = FALSE;
    }
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Reprog_One
 *  Description     : reprogramaton d un module
 *
 *
 *******************************************************************************/
APP_FLASHAGE_STATE P_L_Can_Manage_Reprog_One(void)
{
APP_FLASHAGE_STATE state;
BOOL_TYPE result;

    state = FL_RUNNIG;

    if ((CAN_TRANSFERT_NEXT_DATA == P_L_Can_step_can_reprog) || (CAN_WRITE_HISTORIQUE_1 == P_L_Can_step_can_reprog) || (CAN_WRITE_HISTORIQUE == P_L_Can_step_can_reprog))
    {


        BT_TIMER_LOCK(P_L_Can_Timer_send_IT,S_S_1MS_U32_TIME,P_L_CAN_TIMEOUT_NEXT,P_L_Can_Lock_send_IT,result);
        if (TRUE == result)
        {
            P_L_Can_semaphore_SS = TRUE;

            if (FALSE == P_L_Can_semaphore_IT)
            {
                state = P_L_Can_Manage_Reprog_One_Schedule();
                P_L_Can_semaphore_SS = FALSE;
            }

            if ((P_L_Can_index_historique != 0) || (CAN_TRANSFERT_NEXT_DATA == P_L_Can_step_can_reprog))
            {
                BT_TIMER_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_CAN_TIMEOUT_SEND,P_L_Can_Lock_send_reprog,result);
                if (TRUE == result)
                {
                    P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                }
            }
        }

    }
    else
    {
        P_L_Can_semaphore_SS = TRUE;
        state = P_L_Can_Manage_Reprog_One_Schedule();
        P_L_Can_semaphore_SS = FALSE;
    }
    return state;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Reprog_ALL
 *  Description     : schedule la reprogramatin d un module
 *
 *
 *******************************************************************************/
static APP_FLASHAGE_STATE P_L_Can_Manage_Reprog_One_Schedule(void)
{
APP_FLASHAGE_STATE state;
BOOL_TYPE   result;
BOOL_TYPE   FlashageOK;
BOOL_TYPE   DonneeOK;
U32         boucle;
U32         data_en_cours;
U8          Buff[LgMaxLigne];
U32         nb_data_lue;
U8          cheksum_MSB_1;
U8          cheksum_LSB_1;
U32         taille_1;
U8          cheksum_MSB_2;
U8          cheksum_LSB_2;
U32         taille_2;

    state = FL_RUNNIG;

    switch(P_L_Can_step_can_reprog)
    {
        case CAN_START_REPROG:
            P_L_Can_nb_essai_send_reprog = 0;
            P_L_Can_step_can_reprog = CAN_RETOUR_EN_BOOT_REPROG_TX;
            //break;

        case CAN_RETOUR_EN_BOOT_REPROG_TX:
            //REQUETE $33 demande de retour en boot
            P_L_Can_step_can_reprog = CAN_RETOUR_EN_BOOT_REPROG_RX;
            P_L_Can_Send_Requete_33(P_L_Can_adr_module);
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

        case CAN_ANALYSE_MODULE_REPROG:
            //if (FR_OK == f_mount(&fs_reprog, "", 0))
            //{
                P_L_Can_pos_file_GPR = 0;
                if (FR_OK == f_open(&P_L_Can_file_GPR,(const TCHAR*)P_L_Can_file_reprog,FA_READ))
                {


                    //en Ascii
                    //Nom du SW exemple: CBC20109
                    //Type data 00->classique 01->new XXL 02 ->Tpanel
                    //Cheksum GCM old ou CCM sans logo
                    //Cheksum GCM new ou CCM avec logo

                    //en decicmal
                    //Taille Gcm old ou CCM Sans Logo
                    //Taille Gcm new ou CCM Avec Logo
                    //data a flasher

                    P_L_Can_pos_file_GPR = 0;
                    f_lseek(&P_L_Can_file_GPR,P_L_Can_pos_file_GPR);

                    //lecture du type de fichier CCM ou GCM
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_String_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);
                    if ('G' == Buff[0])
                    {
                        P_L_Can_fichierCCM = FALSE;
                    }
                    else
                    {
                        P_L_Can_fichierCCM = TRUE;
                    }

                    //Nom du SW exemple: CBC20109
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_String_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);
                    for(boucle=0;boucle<8;boucle++)
                    {
                        P_L_Can_Name_File[boucle] = Buff[boucle];
                    }

                    //Type data
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_String_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);

                    //Cheksum GCM classique ou CCM sans logo
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_String_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);

                    cheksum_MSB_1 = APP_Hex2bin(&Buff[0]);
                    cheksum_LSB_1 = APP_Hex2bin(&Buff[2]);

                    //Cheksum GCM new XXL ou CCM avec logo
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_String_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);

                    cheksum_MSB_2 = APP_Hex2bin(&Buff[0]);
                    cheksum_LSB_2 = APP_Hex2bin(&Buff[2]);

                    //Taille Gcm classique ou CCM Sans Logo
                    //P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_Data_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);
                    taille_1 = Buff[0];
                    taille_1 = (taille_1 << 8) + Buff[1];
                    taille_1 = (taille_1 << 8) + Buff[2];
                    taille_1 = (taille_1 << 8) + Buff[3];

                    //Taille Gcm new XXL ou CCM Avec Logo
                    // P_L_F_read_line(&MyfileGPR,Buff,&nb_data_lue,&MyfilePosGPR);
                    APP_Read_Data_line(&P_L_Can_file_GPR,Buff,LgMaxLigne,&nb_data_lue,&P_L_Can_pos_file_GPR,&data_en_cours);
                    taille_2 = Buff[0];
                    taille_2 = (taille_2 << 8) + Buff[1];
                    taille_2 = (taille_2 << 8) + Buff[2];
                    taille_2 = (taille_2 << 8) + Buff[3];

                    //mise a jour du cheksum par defaut atendue en fin de flashage
                        // -> GCM classique ou CCM sans logo
                    P_L_Can_cheksum_MSB = cheksum_MSB_1;
                    P_L_Can_cheksum_LSB = cheksum_LSB_1;

                    //mise a jour de la taille  par defaut du fichier a flasher
                        // -> GCM classique ou CCM sans logo
                    P_L_Can_nb_donnee = taille_1;

                    //reset des donne en cours de programation
                    P_L_Can_donnes_en_cours = 0x00;

                    P_L_Can_state_donnee = 0x00;

                    //test si fichier de config ou apli
                    if(FALSE == P_L_Can_fichierCCM)
                    {
                        ///////////////////////////////////////////////////////////////////////////////
                        // GCM   Appli
                        ///////////////////////////////////////////////////////////////////////////////

                        //on position adresse de debut de flashage par defaut
                        P_L_Can_adresse_start = P_L_ADRESSE_START_GCM_OLD;

                        //test type produit
                        if (TRUE ==  P_L_Can_reprog_New_Facade_XXL)
                        {// nouvelle facade XXL

                            //donnee en partie 2 du fichier
                            //correction du cheksum par defaut attendue en fin de flashage
                            P_L_Can_cheksum_MSB = cheksum_MSB_2;
                            P_L_Can_cheksum_LSB = cheksum_LSB_2;
                            //correction de la taille des donne a flasher
                            P_L_Can_nb_donnee = taille_2;
                            //ajout d un offset sur la taille
                            P_L_Can_nb_donnee = P_L_Can_nb_donnee + P_L_OFFSET_PROGRAMATION_GCM;
                            //ajout d un offset sur les premieres donne a programme
                            P_L_Can_donnes_en_cours =  P_L_OFFSET_PROGRAMATION_GCM;
                        }
                        else if (TRUE ==  P_L_Can_reprog_New_Facade_T_PANEL)
                        {// Tpanel
                            //mise a jour de l adresse de debut de flashage
                            P_L_Can_adresse_start = P_L_ADRESSE_START_GCM_T_PANEL;
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    else
                    {
                        ///////////////////////////////////////////////////////////////////////////////
                        // CCM  Config
                        ///////////////////////////////////////////////////////////////////////////////

                        //on position adresse de debut de flashage par defaut
                        P_L_Can_adresse_start = P_L_ADRESSE_START_CCM_OLD;

                        //test type produit
                        switch(P_L_Can_adr_module&0x0FF0)
                        {
                            // facade XXL
                            case HWI_ADR_FAV_GRAPH_DISPLAY_BASE_WR:
                                // test si nouvelle facade XXL
                                if (TRUE ==  P_L_Can_reprog_New_Facade_XXL)
                                {//oui
                                    //correction du cheksum par defaut atendue en fin de flashage
                                    P_L_Can_cheksum_MSB = cheksum_MSB_2;
                                    P_L_Can_cheksum_LSB = cheksum_LSB_2;
                                    //correction de la taille des donnees a flasher
                                    P_L_Can_nb_donnee = taille_2;
                                }

                                break;

                            case HWI_ADR_FAV_TPANEL_DISPLAY_BASE_WR:
                                //correction du cheksum par defaut atendue en fin de flashage
                                P_L_Can_cheksum_MSB = cheksum_MSB_2;
                                P_L_Can_cheksum_LSB = cheksum_LSB_2;
                                //correction de la taille des donnees a flasher
                                P_L_Can_nb_donnee = taille_2;
                                break;

                            default:
                                //do nothing
                                break;
                        }
                    }
                    // set du flag zone 1
                    P_L_Can_zone1_XXL = TRUE;
                    // set du flag zone 2
                    P_L_Can_zone2_XXL = TRUE;
                    // reset du flag pas de data vue precedement
                    P_L_Can_flag_not_data = FALSE;
                    //demarrage de la reprogramation
                    P_L_Can_step_can_reprog = CAN_SEGEMENT_REPROG;

                    P_L_Can_nb_transfert_lot = 0;
                }
                else
                {
#ifdef _MODE_REFLASH_TONALITE
                    P_L_Can_step_can_reprog = CAN_SORTIE_BOOT_TX;
#else
                    P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
#endif
                }
            break;

        case CAN_SEGEMENT_REPROG:
            //test si encore donne a flasher
            if (P_L_Can_donnes_en_cours < P_L_Can_nb_donnee)
            {
                //set du flag donnee a flasher trouve
                DonneeOK = TRUE;
                //test si donnee vide vue au flashage presedent et pas facade T panel
                if ((TRUE == P_L_Can_flag_not_data)&&(FALSE == P_L_Can_reprog_New_Facade_T_PANEL))
                {
                    //reset du flag donnee a flasher trouve
                    DonneeOK = FALSE;

                    //tanqu il y a des donnee a flasher et que des donnee non pas ete trouve
                    while((P_L_Can_donnes_en_cours<P_L_Can_nb_donnee)&&(FALSE == DonneeOK))
                    {
                        //retour a donne en cours a lire
                        f_lseek(&P_L_Can_file_GPR,P_L_Can_pos_file_GPR+P_L_Can_donnes_en_cours);

                        //lecture des donnees
                        f_read(&P_L_Can_file_GPR,P_L_Can_Data_Segement,P_L_TAILLE_BUFFER_SEGMENT,&nb_data_lue);

                        if ( 0 == nb_data_lue)
                        {
                            //reset du flag donnee a flasher trouve
                            DonneeOK = FALSE;
                        }
                        else
                        {
                            //test des donnee lue
                            for (boucle=0;boucle<nb_data_lue;boucle++)
                            {   //test si pas de donnee
                                if (0xFF == P_L_Can_Data_Segement[boucle])
                                {//pas de donnee

                                    //test si on ne depasse pas le nombre de donnee a reprogramer
                                    if (P_L_Can_donnes_en_cours < P_L_Can_nb_donnee)
                                    {//non
                                        //on incremente les donnee en cours
                                        P_L_Can_donnes_en_cours++;
                                    }
                                    else
                                    {//oui
                                        //fin de la boucle for
                                        boucle = nb_data_lue;
                                    }
                                }
                                else
                                {//donne trouve
                                    //fin de la boucle for
                                    boucle = nb_data_lue;
                                    //set du flag donnee a flasher trouve
                                    DonneeOK = TRUE;
                                }
                            }
                        }
                    }

                    //test si chagement de zone de flashage XXL
                    if ((P_L_Can_donnes_en_cours >= (P_L_ADRESSE_START_CCM_ZONE_2_OLD-P_L_ADRESSE_START_CCM_OLD))&&(TRUE == P_L_Can_zone1_XXL))
                    {//set du flag zone 1
                        P_L_Can_zone1_XXL = FALSE;
                    }
                }

                // test s il y a des donne a flasher on ete trouve
                if (TRUE == DonneeOK)
                {
                    //adresse de debut de flashage envoye sur le can
                    P_L_Can_start_segment_adresse = P_L_Can_adresse_start + P_L_Can_donnes_en_cours;

                    //reset du flag indiquent une absence de donnee trouvee
                    P_L_Can_flag_not_data = FALSE;

                    //set du flag autorisant le flashage des donnee
                    FlashageOK = TRUE;

                    //retour a donne en cours a lire
                    f_lseek(&P_L_Can_file_GPR,P_L_Can_pos_file_GPR+P_L_Can_donnes_en_cours);

                    //lecture des donnees
                    f_read(&P_L_Can_file_GPR,P_L_Can_Data_Segement,P_L_TAILLE_BUFFER_SEGMENT,&nb_data_lue);

                    //2 type de traitement en fonction du type de fichier


                    //test si facade XXL et fichier CCM
                    if ((TRUE ==  P_L_Can_reprog_New_Facade_XXL)&&(TRUE == P_L_Can_fichierCCM))
                    {//test du zone de zone pour ce produit

                        if (P_L_Can_donnes_en_cours < (P_L_ADRESSE_START_CCM_ZONE_2_OLD-P_L_ADRESSE_START_CCM_OLD))
                        {//zone 1 (Message)
                          //adresse de debut de flashage envoye sur le can
                          P_L_Can_start_segment_adresse = P_L_Can_start_segment_adresse - P_L_OFFSET_PROGRAMATION_ZONE_1;
                        }
                        else
                        {//zone 2 (Cfg)
                          //adresse de debut de flashage envoye sur le can
                          P_L_Can_start_segment_adresse = P_L_Can_start_segment_adresse - P_L_OFFSET_PROGRAMATION_ZONE_2;
                        }
                    }

                    if ( 0 == nb_data_lue)
                    {
                        //reset du flag autorisant le flashage des donnee
                        FlashageOK = FALSE;
                        //plus de segement a flasher
                        P_L_Can_step_can_reprog = CAN_CALCUL_CHEKSUM_TX;
                    }
                    else
                    {

                        //je pense que le code ci-dessous ne sert a rien
                        // a part le test de zone qui doit pouvoir se faire sans la boucle
                        //anci que le calcule de P_L_Can_donnes_en_cours et P_L_Can_taille_segment


                        //test des donnee lue
                        for (boucle=0;boucle<nb_data_lue;boucle++)
                        {
                            //test si fichier CCM  Config car
                            //il faut arreter le segement lors du changement de zone
                            //ou du saut pour le logo du tpanel
                            if ((TRUE == P_L_Can_fichierCCM)&&
                                ( ((P_L_Can_donnes_en_cours >= (P_L_ADRESSE_START_CCM_ZONE_2_OLD-P_L_ADRESSE_START_CCM_OLD))&&(TRUE == P_L_Can_zone1_XXL)&&(TRUE ==  P_L_Can_reprog_New_Facade_XXL)) ||
                                  ((P_L_Can_donnes_en_cours >= (P_L_ADRESSE_END_CCM_OLD-P_L_ADRESSE_START_CCM_OLD))&&(TRUE == P_L_Can_zone1_XXL)&&(TRUE ==  P_L_Can_reprog_New_Facade_T_PANEL)) ||
                                  ((P_L_Can_donnes_en_cours >= (P_L_ADRESSE_START_CCM_LOGO-P_L_ADRESSE_START_CCM_OLD))&&(TRUE == P_L_Can_zone2_XXL)&&(TRUE ==  P_L_Can_reprog_New_Facade_T_PANEL)) ) )

                            {
                                //fin de la boucle for
                                boucle = nb_data_lue;

                                //test si zone 1 vue
                                if( TRUE == P_L_Can_zone1_XXL)
                                {//reset du flage
                                    P_L_Can_zone1_XXL = FALSE;
                                }
                                else
                                {//passage en zone 2
                                    P_L_Can_zone2_XXL =FALSE;
                                    //reset du flag autorisant le flashage des donnee
                                    FlashageOK = FALSE;
                                }
                            }
                            else
                            {   //ben  test flashage
                                /* ben pour etre plus rapide
                                //test si abscence de donnee trouve et pas facade tpanel
                                if (( 0xFF == P_L_Can_Data_Segement[boucle]) &&
                                  ( 0xFF == P_L_Can_Data_Segement[boucle+1]) &&
                                  ( 0xFF == P_L_Can_Data_Segement[boucle+2]) &&
                                  ( 0xFF == P_L_Can_Data_Segement[boucle+3]) &&
                                  (FALSE == P_L_Can_reprog_New_Facade_T_PANEL) &&
                                  (FALSE == P_L_Can_reprog_New_Facade_XXL))
                                {
                                    //abscence de donnee trouve
                                    //fin de la boucle for
                                    boucle = nb_data_lue;
                                    //set du flag indiquent une absence de donnee trouvee
                                    P_L_Can_flag_not_data = TRUE;
                                }
                                else */if (P_L_Can_donnes_en_cours >= P_L_Can_nb_donnee)
                                {//fin des donnees a flasher
                                    //fin de la boucle for
                                    boucle = nb_data_lue;
                                    //set du flag indiquent une absence de donnee trouvee
                                    P_L_Can_flag_not_data = TRUE;
                                }
                                else
                                {//donne a flasher ok on continue analyse
                                    //on incremente la posotion des donnee en cours
                                    P_L_Can_donnes_en_cours++;
                                    //idem pour la taille des donnée de flashage envoye sur le can
                                    P_L_Can_taille_segment = boucle+1;
                                }
                            }
                        }
                    }
                    //test du flag autorisant le flashage des donnee
                    if (TRUE == FlashageOK)
                    {
                        //pas gere pour l instant on dit toujour que c est le dernier segement(comme para CCS2010)
                        P_L_Can_last_segment = TRUE;
                        //demùande de programation du segement
                        P_L_Can_step_can_reprog = CAN_START_TELECHARGEMENT_TX;
                    }
                }
            }
            else
            {
                //plus de segement a flasher
                P_L_Can_step_can_reprog = CAN_CALCUL_CHEKSUM_TX;
            }
            break;

        case CAN_START_TELECHARGEMENT_TX:
            //REQUETE $34 Requête de téléchargement
            P_L_Can_step_can_reprog = CAN_START_TELECHARGEMENT_RX;
            P_L_Can_Send_Requete_34();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

        case CAN_TRANSFERT_FIRST_DATA_TX:
            //REQUETE $36 transfert premiere donnee
            if (P_L_Can_taille_segment <= P_L_Can_data_flash_running)
            {
               P_L_Can_step_can_reprog =  CAN_TRANSFERT_END_TX;
            }

            P_L_Can_step_can_reprog = CAN_TRANSFERT_FIRST_DATA_RX;
            P_L_Can_nb_transfert_next_data = 0;
            P_L_Can_nb_transfert_lot++;
            P_L_Can_Send_Requete_36();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

        case CAN_TRANSFERT_NEXT_DATA:
            //REQUETE $2x transfert next donnee
            if (FL_ERROR == P_L_Can_Send_Requete_2x())
            {
                state = FL_ERROR;
            }
            P_L_Can_nb_essai_send_reprog = 0;
            P_L_Can_nb_transfert_next_data++;
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

        case CAN_TRANSFERT_END_TX:
            //REQUETE $37 fin de transfert
            P_L_Can_step_can_reprog =  CAN_TRANSFERT_END_RX;
            P_L_Can_Send_Requete_37();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

         case CAN_CALCUL_CHEKSUM_TX:
             P_L_Can_step_can_reprog = CAN_CALCUL_CHEKSUM_RX;
            //test si fichier config ou appli
             if(FALSE == P_L_Can_fichierCCM)
             {
                //REQUETE $38 demande de calcul de cheksum apli
                P_L_Can_Send_Requete_38();
             }
             else
             {
                //REQUETE $39 demande de calcul de cheksum config
                P_L_Can_Send_Requete_39();
             }
             BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
         break;

        case CAN_WRITE_CHEKSUM_TX:
            P_L_Can_step_can_reprog = CAN_WRITE_CHEKSUM_RX;

            //test si fichier config ou appli
            if(FALSE == P_L_Can_fichierCCM)
            {
                //REQUETE $40 demande d ecriture du cheksum apli
                P_L_Can_Send_Requete_40();
            }
            else
            {
                //REQUETE $41 demande d ecriture du cheksum config
                P_L_Can_Send_Requete_41();
            }
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_READ_HISTORIQUE_TX:
            //REQUETE $43 demande de lecture de l historique
            P_L_Can_step_can_reprog = CAN_READ_HISTORIQUE_RX;
            P_L_Can_Send_Requete_43();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_ERASE_HISTORIQUE_TX:
            //REQUETE $31 demande effacement de historique
            P_L_Can_step_can_reprog = CAN_ERASE_HISTORIQUE_RX;
            P_L_Can_Send_Requete_31();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_WRITE_HISTORIQUE_1:
            //REQUETE $42 demande ecriture de historique
            P_L_Can_Send_Requete_42(TRUE);
            P_L_Can_nb_essai_send_reprog = 0;
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_WRITE_HISTORIQUE:
            //REQUETE $42 demande ecriture de historique
            P_L_Can_Send_Requete_42(FALSE);
            P_L_Can_nb_essai_send_reprog = 0;
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
        break;

        case CAN_SORTIE_BOOT_TX:
            //REQUETE $50 demande saut dans applicatif
            P_L_Can_step_can_reprog = CAN_SORTIE_BOOT_RX;
            P_L_Can_Send_Requete_50();
            BT_TIMER_INIT_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_Can_Lock_send_reprog);
            break;

        case CAN_END_REPROG:
            f_close(&P_L_Can_file_GPR);
            state = FL_OK;
            break;

         case CAN_ERROR_REPROG:
             f_close(&P_L_Can_file_GPR);
            state = FL_ERROR;
            break;

         default:
            if ((CAN_START_TELECHARGEMENT_RX == P_L_Can_step_can_reprog)&&(TRUE ==  P_L_Can_reprog_New_Facade_XXL))
            {
                BT_TIMER_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_TIMEOUT_SEND_ERASE_XXL,P_L_Can_Lock_send_reprog,result);
            }
            else
            {
                BT_TIMER_LOCK(P_L_Can_Timer_send_reprog,S_S_1MS_U32_TIME,P_L_CAN_TIMEOUT_SEND,P_L_Can_Lock_send_reprog,result);
            }

            if (TRUE == result)
            {
                P_L_Can_nb_essai_send_reprog++;
                if (P_L_CAN_NB_ESSAI_MAX > P_L_Can_nb_essai_send_reprog)
                {
                    P_L_Can_step_can_reprog = P_L_Can_step_can_reprog-1;
                }
                else
                {
                    P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                }
            }
        break;
    }
    return state;
}


/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_2x
* Description     : REQUETE $3x transfert next donnee
*
*******************************************************************************/
static BOOL_TYPE P_L_Can_Send_Requete_2x(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;

U16 nb_data;
U16 nb_data_segement;
U8 boucle;

BOOL_TYPE result = FL_ERROR;

    if (P_L_Can_taille_max_data_send > P_L_Can_data_flash_segement)
    {
        result = FL_OK;

        Can_buffer_send[0] = (U8)0x00;
        Can_buffer_send[1] = (U8) ((P_L_Can_TransfertIndex & 0x0F) | 0x20);

        nb_data = P_L_Can_taille_segment-P_L_Can_data_flash_running;

        if (nb_data > 6)
        {
            nb_data = 6;
        }

        nb_data_segement = P_L_Can_taille_max_data_send -P_L_Can_data_flash_segement;
        if (nb_data > nb_data_segement)
        {
            nb_data = nb_data_segement;
        }

        for (boucle=0;boucle<nb_data;boucle++)
        {
            Can_buffer_send[boucle+2] = (U8) P_L_Can_Data_Segement[P_L_Can_data_flash_running+boucle];
        }

        Can_objet.HWI_CAN_LENGTH = nb_data+2;

        // envoie de la trame can sur CAN1 MAIL BOX 1
        Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
        P_L_Can_TransfertIndex++;
        P_L_Can_data_flash_running = P_L_Can_data_flash_running+nb_data;
        P_L_Can_data_flash_segement = P_L_Can_data_flash_segement+nb_data;
        P_L_Can_state_donnee = P_L_Can_state_donnee+nb_data;
    }
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_30
* Description     : demande de changement adresse
*
*******************************************************************************/
static void P_L_Can_Send_Requete_30(U8 adresse_logique)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = 0x00;
    Can_buffer_send[1] = 0x30;
    Can_buffer_send[2] = adresse_logique;
    Can_buffer_send[3] = 0x00;
    Can_buffer_send[4] = 0x00;
    Can_buffer_send[5] = 0x00;
    Can_buffer_send[6] = 0x00;
    Can_buffer_send[7] = 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_31
* Description     : REQUETE $31 demande d effacement de historique
*
*******************************************************************************/
static void P_L_Can_Send_Requete_31(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;


    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x31;
    Can_buffer_send[2] = (U8) 0x00; // demande de tout l'historique
    Can_buffer_send[3] = (U8) 0x00;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_33
* Description     : demande de retour en boot
*
*******************************************************************************/
static void P_L_Can_Send_Requete_33(U32 adresse_module)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = 0x00;
    Can_buffer_send[1] = 0x33;
    Can_buffer_send[2] = (U8)(adresse_module>>8);
    Can_buffer_send[3] = (U8)adresse_module;
    Can_buffer_send[4] = (U8)0x00;
    Can_buffer_send[5] = (U8)0x00;
    Can_buffer_send[6] = (U8)0x00;
    Can_buffer_send[7] = (U8)0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}


/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_34
* Description     : REQUETE $34 Requête de téléchargement
*                   //P_L_Can_start_segment_adresse
*                   // P_L_Can_taille_segment
*******************************************************************************/
static void P_L_Can_Send_Requete_34(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8)0x00;
    Can_buffer_send[1] = (U8)0x34;
    Can_buffer_send[2] = (U8) (P_L_Can_start_segment_adresse>>24);
    Can_buffer_send[3] = (U8) (P_L_Can_start_segment_adresse>>16);
    Can_buffer_send[4] = (U8) (P_L_Can_start_segment_adresse>>8);
    Can_buffer_send[5] = (U8) P_L_Can_start_segment_adresse;
    Can_buffer_send[6] = (U8) (P_L_Can_taille_segment>>8);
    Can_buffer_send[7] = (U8) (P_L_Can_taille_segment);
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_36
* Description     : REQUETE $36 transfert premiere donnee
*
*******************************************************************************/
static void P_L_Can_Send_Requete_36(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    P_L_Can_taille_max_data_send = P_L_Can_taille_segment-P_L_Can_data_flash_running;


    if(P_L_Can_taille_max_data_send > P_L_Can_taille_buffer_ECU)
    {
        P_L_Can_taille_max_data_send = P_L_Can_taille_buffer_ECU;
    }

    Can_buffer_send[0] = (U8)0x00;
    Can_buffer_send[1] = (U8) (((P_L_Can_taille_max_data_send >> 8) & 0x0F) | 0x10);
    Can_buffer_send[2] = (U8) (P_L_Can_taille_max_data_send);
    Can_buffer_send[3] = (U8)0x36;
    Can_buffer_send[4] = (U8) P_L_Can_Data_Segement[P_L_Can_data_flash_running];
    Can_buffer_send[5] = (U8) P_L_Can_Data_Segement[P_L_Can_data_flash_running+1];
    Can_buffer_send[6] = (U8) P_L_Can_Data_Segement[P_L_Can_data_flash_running+2];
    Can_buffer_send[7] = (U8) P_L_Can_Data_Segement[P_L_Can_data_flash_running+3];

    P_L_Can_data_flash_running = P_L_Can_data_flash_running+4;
    P_L_Can_data_flash_segement = 4;
    P_L_Can_state_donnee=P_L_Can_state_donnee+4;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}


/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_37
* Description     : REQUETE $37 fin de transfert
*
*******************************************************************************/
static void P_L_Can_Send_Requete_37(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;


    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x37;

    if (TRUE == P_L_Can_last_segment)
    {
        Can_buffer_send[2] = (U8) 0xFF;
    }
    else
    {
        Can_buffer_send[2] = (U8) 0x00;
    }
    Can_buffer_send[3] = (U8) 0x00;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_38
* Description     : REQUETE $38 demande de calcul de cheksum apli
*
*******************************************************************************/
static void P_L_Can_Send_Requete_38(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x38;
    Can_buffer_send[2] = (U8) 0x00;
    Can_buffer_send[3] = (U8) 0x00;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_39
* Description     : REQUETE $38 demande de calcul de cheksum config
*
*******************************************************************************/
static void P_L_Can_Send_Requete_39(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x39;
    Can_buffer_send[2] = (U8) 0x00;
    Can_buffer_send[3] = (U8) 0x00;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_40
* Description     : REQUETE $40 demande d ecriture du cheksum apli
*
*******************************************************************************/
static void P_L_Can_Send_Requete_40(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x40;
    Can_buffer_send[2] = (U8) P_L_Can_cheksum_MSB;
    Can_buffer_send[3] = (U8) P_L_Can_cheksum_LSB;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_41
* Description     : REQUETE $41 demande d ecriture du cheksum config
*
*******************************************************************************/
static void P_L_Can_Send_Requete_41(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x41;
    Can_buffer_send[2] = (U8) P_L_Can_cheksum_MSB;
    Can_buffer_send[3] = (U8) P_L_Can_cheksum_LSB;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_42
* Description     : REQUETE $42 demande ecriture de historique
*
*******************************************************************************/
static void P_L_Can_Send_Requete_42(BOOL_TYPE recopie_first)
{
U8 boucle;
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;


    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x42;
    Can_buffer_send[2] = (U8) 0x05;

    if (FALSE == recopie_first)
    {
        switch (P_L_Can_index_historique)
        {
            // REQUET 0
            case 0:
                Can_buffer_send[3] = P_L_Can_Name_File[0];
                Can_buffer_send[4] = P_L_Can_Name_File[1];
                Can_buffer_send[5] = P_L_Can_Name_File[2];
                Can_buffer_send[6] = P_L_Can_Name_File[3];
                Can_buffer_send[7] = P_L_Can_Name_File[4];
                break;

            // REQUET 1
            case 1:
                Can_buffer_send[3] = P_L_Can_Name_File[5];
                Can_buffer_send[4] = P_L_Can_Name_File[6];
                Can_buffer_send[5] = P_L_Can_Name_File[7];
                Can_buffer_send[6] = ((U8)APP_Flashage_time.Date);
                Can_buffer_send[7] = ((U8)APP_Flashage_time.Month);
                break;

            // REQUET 2
            case 2:
                Can_buffer_send[3] = ((U8)APP_Flashage_time.Year-1900);
                Can_buffer_send[4] = ((U8)APP_Flashage_time.Hours);
                Can_buffer_send[5] = ((U8)APP_Flashage_time.Minutes);
                Can_buffer_send[6] = ((U8)APP_Flashage_time.Seconds);
                Can_buffer_send[7] = ((U8)'F');
                break;

            // REQUET 3
            case 3:
                Can_buffer_send[3] = ((U8)'1');
                Can_buffer_send[4] = ((U8)'2');
                Can_buffer_send[5] = ((U8)'T');
                Can_buffer_send[6] = ((U8)'V');
                Can_buffer_send[7] = ((U8)'1');
                break;

            // REQUET 4
            case 4:
                Can_buffer_send[3] = ((U8)'.');
                Can_buffer_send[4] = ((U8)'0');
                Can_buffer_send[5] = ((U8)0);
                Can_buffer_send[6] = APP_Flashage_user_reprog[0];
                Can_buffer_send[7] = APP_Flashage_user_reprog[1];
                break;

            // REQUET 5
            case 5:
            // REQUET 6
            case 6:
                for(boucle=3;boucle<8;boucle++)
                {
                     Can_buffer_send[boucle] = APP_Flashage_user_reprog[boucle-3+2+(5*(P_L_Can_index_historique-5))];
                }

                if (6 ==P_L_Can_index_historique)
                {
                    Can_buffer_send[7] = ' ';
                }
                break;

            // REQUET 7
            case 7:
           // REQUET 8
            case 8:
                for(boucle=3;boucle<8;boucle++)
                {
                     Can_buffer_send[boucle] = APP_Flashage_societe_reprog[boucle-3+(5*(P_L_Can_index_historique-7))];
                }
                break;

            case 12:
                Can_buffer_send[2] = (U8) 0x04;
            case 9:
            case 10:
            case 11:
                Can_buffer_send[3] = ((U8)0);
                Can_buffer_send[4] = ((U8)0);
                Can_buffer_send[5] = ((U8)0);
                Can_buffer_send[6] = ((U8)0);
                Can_buffer_send[7] = ((U8)0);
                break;
        }
    }
    else
    {
        for(boucle=3;boucle<8;boucle++)
        {
            Can_buffer_send[boucle] = P_L_Can_received_historique[(P_L_Can_index_historique*5)+(boucle-3)];
        }

        if (12 == P_L_Can_index_historique)
        {
            Can_buffer_send[2] = (U8) 0x04;
        }
    }

    if ( P_L_CAN_NB_LOT_CAN_HISTORIQUE > P_L_Can_index_historique)
    {
         // envoie de la trame can sur CAN1 MAIL BOX 1
        Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
        P_L_Can_index_historique++;
    }
}


/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_43
* Description     : REQUETE $43 demande lecture de historique
*
*******************************************************************************/
static void P_L_Can_Send_Requete_43(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    P_L_Can_index_historique = 0;

    Can_buffer_send[0] = (U8) 0x00;
    Can_buffer_send[1] = (U8) 0x43;
    Can_buffer_send[2] = (U8) 0x01; // demande de tout l'historique
    Can_buffer_send[3] = (U8) 0x00;
    Can_buffer_send[4] = (U8) 0x00;
    Can_buffer_send[5] = (U8) 0x00;
    Can_buffer_send[6] = (U8) 0x00;
    Can_buffer_send[7] = (U8) 0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
*
* Function Name   : P_L_Can_Send_Requete_50
* Description     : demande quiter boot
*
*******************************************************************************/
static void P_L_Can_Send_Requete_50(void)
{
HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = ((U32)0x01);
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8)0x00;
    Can_buffer_send[1] = (U8)0x50;
    Can_buffer_send[2] = (U8)0x00;
    Can_buffer_send[3] = (U8)0x00;
    Can_buffer_send[4] = (U8)0x00;
    Can_buffer_send[5] = (U8)0x00;
    Can_buffer_send[6] = (U8)0x00;
    Can_buffer_send[7] = (U8)0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Send_Wake_UP
 *  Description     : maintient les blocs reveille
 *
 *
 *******************************************************************************/
void P_L_Can_Send_Wake_UP(void)
{
    HWI_CAN_OBJECT_TYPE Can_objet;
U8 Can_buffer_send[P_L_CAN_LENGHT_DATA];

    Can_objet.HWI_CAN_MSG_ID = APP_Adresse_module;
    Can_objet.HWI_ID_FORMAT=HWI_STD;
    Can_objet.HWI_CAN_LENGTH = 0x08;

    Can_buffer_send[0] = (U8)PL_CAN_ID_INFO_COEUR;
    Can_buffer_send[1] = (U8)0xFF;
    Can_buffer_send[2] = (U8)0x00;
    Can_buffer_send[3] = (U8)0x00;
    Can_buffer_send[4] = (U8)0x00;
    Can_buffer_send[5] = (U8)0x00;
    Can_buffer_send[6] = (U8)0x00;
    Can_buffer_send[7] = (U8)0x00;
    // envoie de la trame can sur CAN1 MAIL BOX 1
    Hwi_Can_Send_Message(CAN1, &Can_objet, Can_buffer_send,MAIL_BOX1);

}
/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Change_Adr_Init
 *  Description     : change adresse init
 *
 *
 *******************************************************************************/
void P_L_Can_Manage_Change_Adr_Init(U32 adresse,U8 log_adresse)
{
    P_L_Can_adr_module = adresse & 0xFFFFF3FF;
    P_L_Can_adr_logique = log_adresse;
    P_L_Can_step_adr_chgt = CAN_START_CHGT_ADRESSE;
    P_L_Can_semaphore_SS = FALSE;
    P_L_Can_semaphore_IT = FALSE;
}

/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Historique_Init
 *  Description     : lecture de historique d un module
 *
 *
 *******************************************************************************/
void P_L_Can_Manage_Historique_Init(U32 adresse,BOOL_TYPE full_historique)
{
    P_L_Can_adr_module = adresse & 0xFFFFF3FF;
    P_L_Can_full_historique = full_historique;
    P_L_Can_step_historique = CAN_START_READ_HISTORIQUE;
    P_L_Can_semaphore_SS = FALSE;
    P_L_Can_semaphore_IT = FALSE;
}
/*******************************************************************************
 *
 *  Function Name   : P_L_Can_Manage_Reprog_Init
 *  Description     : reprogramation init
 *
 *
 *******************************************************************************/
void P_L_Can_Manage_Reprog_Init(U32 adresse,U8  *file_reprog)
{
    P_L_Can_adr_module = adresse & 0xFFFFF3FF;
    P_L_Can_file_reprog = file_reprog;
    P_L_Can_step_can_reprog = CAN_START_REPROG;

    P_L_Can_reprog_New_Facade_XXL = FALSE;
    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
    P_L_Can_last_segment = TRUE;
    P_L_Can_semaphore_SS = FALSE;
    P_L_Can_semaphore_IT = FALSE;
}
