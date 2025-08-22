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
#include "p_l_can_flashage_rx.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l_can_flashage_tx.h"
#include "app_manage_flashage.h"
#include "p_l.h"
#include "lib_macros.h"
#include "lib_filter.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
//adresse du module ciblé
extern U32                      P_L_Can_adr_module;
//step for programming module
extern APP_FLASHAGE_STEP        APP_Flashage_state;
//etapes du chagement adresse
extern P_L_CAN_CHGT_ADR_STEP    P_L_Can_step_adr_chgt;
// nombre essaie envoie de la trame can
extern U8                       P_L_Can_nb_essai_send_chgt_adr;

//flag indiquant que l on reprogramme une NEX XXL
extern BOOL_TYPE               P_L_Can_reprog_New_Facade_XXL;
//flag indiquant que l on reprogramme une T PANEL
extern BOOL_TYPE               P_L_Can_reprog_New_Facade_T_PANEL;


//etapes de la reprogramation d un module
extern P_L_CAN_REPROG_MOD_TYPE  P_L_Can_step_can_reprog;
// nombre essaie envoie de la trame can
extern U8                       P_L_Can_nb_essai_send_reprog;
//taille du buffeur de reception de celui que l on va flasher
extern U16                      P_L_Can_taille_buffer_ECU;
//donnee en cours d ecriture
extern U32                      P_L_Can_data_flash_running;
//indique le monbre max de paquet que l on peut envoyer en 2x
extern U8                       P_L_Can_nb_paquets;
//taille du segement a programme de 0 à P_L_TAILLE_BUFFER_SEGMENT
extern U16                      P_L_Can_taille_segment;
//index de transfert consecutif 3x
extern U16                      P_L_Can_TransfertIndex;
//flag indiquent si c est un CCM a flasher
extern BOOL_TYPE                P_L_Can_fichierCCM;
//cheksum du fichier a flasher
extern U8                       P_L_Can_cheksum_MSB;
extern U8                       P_L_Can_cheksum_LSB;

//tableau de reception des donnee de l historique
extern U8                       P_L_Can_received_historique[P_L_CAN_TAILLE_BUFFER_HISTORIQUE];
//Nombre historique vue
extern U8                       P_L_Can_Nb_historique;
//index ecriture dans le tableau des historiques
extern U16                      P_L_Can_index_historique;

//etapes de la lecture d un historique
extern P_L_CAN_HISTORIQUE_TYPE  P_L_Can_step_historique;
// nombre essaie envoie de la trame can
extern U8                       P_L_Can_nb_essai_send_historique;


/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
/* NONE*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
 *
 *  Function Name   : APP_Can_rx_frame_flashage
 *  Description     : reception trame can flashage
 *
 *
 *******************************************************************************/
void P_L_Can_flashage_rx_frame(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght)
{
U8 cheksum_rx_MSB;
U8 cheksum_rx_LSB;
U16 nb_octet;
U8 data_index;
U16 tmp_index;
BOOL_TYPE RecievedHisto;
U8 boucle;


    APP_Manage_Add_Module_Vue(p_l_can_msg_id);

    if ((P_L_Can_adr_module+((U32)0x400)) == p_l_can_msg_id)
    {
//////////////////////////////////////////////////////////////
//
//          Reception changement adresse
//
//////////////////////////////////////////////////////////////
        if (APP_Flashage_state == FLASHAGE_CHANGE_ADR)
        {
            switch(P_L_Can_step_adr_chgt)
            {
                case CAN_RETOUR_EN_BOOT_RX:
                    if ( (0x00 == p_l_can_data_ptr[0]) && (0x73 == p_l_can_data_ptr[1]))
                    {
                        if (0x00 == p_l_can_data_ptr[2])
                        {
                            P_L_Can_reprog_New_Facade_XXL = FALSE;
                            P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                            P_L_Can_nb_essai_send_chgt_adr = 0;
                            P_L_Can_step_adr_chgt = CAN_CHGT_LOG_ADR_TX;
                        }
                        else if (0x01 == p_l_can_data_ptr[2])
                        {
                            switch(p_l_can_data_ptr[3])
                            {
                                case 0:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 1:
                                    P_L_Can_reprog_New_Facade_XXL = TRUE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 2:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = TRUE;
                                    break;

                                default:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;
                            }
                            P_L_Can_nb_essai_send_chgt_adr = 0;
                            P_L_Can_step_adr_chgt = CAN_CHGT_LOG_ADR_TX;
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    break;

                case CAN_CHGT_LOG_ADR_RX:
                    if ( (0x00 == p_l_can_data_ptr[0])&&
                        (0x70 == p_l_can_data_ptr[1])&&
                        (0x00 == p_l_can_data_ptr[2])&&
                        (0x00 == p_l_can_data_ptr[3])&&
                        (0x00 == p_l_can_data_ptr[4])&&
                        (0x00 == p_l_can_data_ptr[5])&&
                        (0x00 == p_l_can_data_ptr[6])&&
                        (0x00 == p_l_can_data_ptr[7]))
                    {
                        P_L_Can_nb_essai_send_chgt_adr = 0;
                        P_L_Can_step_adr_chgt = CAN_QUITER_BOOT_TX;
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x30 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_adr_chgt = CAN_ERROR_CHGT_ADR;
                    }
                    break;

                case CAN_QUITER_BOOT_RX:
                    if ( (0x00 == p_l_can_data_ptr[0])&&
                        (0x90 == p_l_can_data_ptr[1])&&
                        (0x00 == p_l_can_data_ptr[2])&&
                        (0x00 == p_l_can_data_ptr[3])&&
                        (0x00 == p_l_can_data_ptr[4])&&
                        (0x00 == p_l_can_data_ptr[5])&&
                        (0x00 == p_l_can_data_ptr[6])&&
                        (0x00 == p_l_can_data_ptr[7]))
                    {
                        P_L_Can_nb_essai_send_chgt_adr = 0;
                        P_L_Can_step_adr_chgt = CAN_FIN_CHGT_ADR;
                    }
                    break;

                default:
                    break;
            }
        }
//////////////////////////////////////////////////////////////
//
//          Reception reprogramation
//
//////////////////////////////////////////////////////////////
        else if (APP_Flashage_state == FLASHAGE_REPROG_ALL)
        {
            switch(P_L_Can_step_can_reprog)
            {
                case CAN_RETOUR_EN_BOOT_REPROG_RX:
                    if ( (0x00 == p_l_can_data_ptr[0]) && (0x73 == p_l_can_data_ptr[1]))
                    {
                        if (0x00 == p_l_can_data_ptr[2])
                        {
                            P_L_Can_reprog_New_Facade_XXL = FALSE;
                            P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                            P_L_Can_step_can_reprog = CAN_ANALYSE_MODULE_REPROG;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else if (0x01 == p_l_can_data_ptr[2])
                        {
                            switch(p_l_can_data_ptr[3])
                            {
                                case 0:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 1:
                                    P_L_Can_reprog_New_Facade_XXL = TRUE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 2:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = TRUE;
                                    break;

                                default:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;
                            }
                            P_L_Can_step_can_reprog = CAN_ANALYSE_MODULE_REPROG;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    break;

                case CAN_START_TELECHARGEMENT_RX:
                    //REQUETE $34 Requête de téléchargement
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x74 == p_l_can_data_ptr[1]))
                    {
                        P_L_Can_taille_buffer_ECU = (p_l_can_data_ptr[2] & 0x0F);
                        P_L_Can_taille_buffer_ECU = P_L_Can_taille_buffer_ECU<<8;
                        P_L_Can_taille_buffer_ECU += p_l_can_data_ptr[3];
                        if (P_L_TAILLE_BUFFER_SEGMENT < P_L_Can_taille_buffer_ECU)
                        {
                            P_L_Can_taille_buffer_ECU = P_L_TAILLE_BUFFER_SEGMENT;
                        }
                        P_L_Can_data_flash_running = 0;
                        P_L_Can_step_can_reprog = CAN_TRANSFERT_FIRST_DATA_TX;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x34 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                    break;

              case CAN_TRANSFERT_FIRST_DATA_RX:
                    //REQUETE $36 transfert premiere donnee
                    // 2 reponses:

                    //1- reponse en  3x controle de flux si plus de 4 donnee a envoye
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x30 == (p_l_can_data_ptr[1] & 0xF0)) )
                    {
                        P_L_Can_nb_paquets = (p_l_can_data_ptr[1] & 0x0F);
                        P_L_Can_nb_paquets = P_L_Can_nb_paquets<<8;
                        P_L_Can_nb_paquets += p_l_can_data_ptr[2];

                        if (P_L_Can_taille_segment > P_L_Can_data_flash_running)
                        {
                            P_L_Can_step_can_reprog = CAN_TRANSFERT_NEXT_DATA;
                            P_L_Can_nb_essai_send_reprog = 0;
                            P_L_Can_TransfertIndex = 0;
                        }
                        else
                        {
                            P_L_Can_step_can_reprog = CAN_TRANSFERT_END_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                    }
                    //break;
                    //2- reponse en  2x transfert next donnee si moins de 4 donnee a envoye

              case CAN_TRANSFERT_NEXT_DATA:
                    //REQUETE $2x transfert next donnee reponse en $36
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x76 == p_l_can_data_ptr[1]))
                    {
                        if (P_L_Can_taille_segment <= P_L_Can_data_flash_running)
                        {
                            P_L_Can_step_can_reprog =  CAN_TRANSFERT_END_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else
                        {
                            P_L_Can_step_can_reprog = CAN_TRANSFERT_FIRST_DATA_TX;
                            P_L_Can_nb_essai_send_reprog = 0;

                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x36 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                    break;

               case CAN_TRANSFERT_END_RX:
                    //REQUETE $37 fin de transfert
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x77 == p_l_can_data_ptr[1]))
                    {
                        P_L_Can_step_can_reprog = CAN_SEGEMENT_REPROG;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x37 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                    break;

              case CAN_CALCUL_CHEKSUM_RX:
                //test si fichier config ou appli
                if(FALSE == P_L_Can_fichierCCM)
                {
                    //REQUETE $38 demande de calcul de cheksum apli
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x78 == p_l_can_data_ptr[1]))
                    {
                        cheksum_rx_MSB = p_l_can_data_ptr[2];
                        cheksum_rx_LSB = p_l_can_data_ptr[3];

                        if ((P_L_Can_cheksum_MSB == cheksum_rx_MSB)&&(P_L_Can_cheksum_LSB == cheksum_rx_LSB))
                        {
                            P_L_Can_step_can_reprog = CAN_WRITE_CHEKSUM_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else
                        {
#ifdef _PAS_TEST_CHECKSUM_CAR_BUG
                            P_L_Can_step_can_reprog = CAN_WRITE_CHEKSUM_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
#else
                            P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
#endif
                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x38 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                }
                else
                {
                    //REQUETE $39 demande de calcul de cheksum config
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x79 == p_l_can_data_ptr[1]))
                    {
                        cheksum_rx_MSB = p_l_can_data_ptr[2];
                        cheksum_rx_LSB = p_l_can_data_ptr[3];
                        if ((P_L_Can_cheksum_MSB == cheksum_rx_MSB)&&(P_L_Can_cheksum_LSB == cheksum_rx_LSB))
                        {
                            P_L_Can_step_can_reprog = CAN_WRITE_CHEKSUM_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else
                        {
                            P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x38 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                }
                break;

                case CAN_WRITE_CHEKSUM_RX:
                    //test si fichier config ou appli
                    if(FALSE == P_L_Can_fichierCCM)
                    {
                        //REQUETE $40 demande d ecriture du cheksum apli
                        //reponse positive
                        if ((0x00 == p_l_can_data_ptr[0])&&
                            (0x80 == p_l_can_data_ptr[1]))
                        {
                            P_L_Can_step_can_reprog = CAN_READ_HISTORIQUE_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        //reponse negative
                        else if ((0x00 == p_l_can_data_ptr[0])&&
                                 (0x7F == p_l_can_data_ptr[1])&&
                                 (0x40 == p_l_can_data_ptr[2]))
                        {
                            P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                        }
                    }
                    else
                    {
                        //REQUETE $41 demande d ecriture du cheksum config
                        //reponse positive
                        if ((0x00 == p_l_can_data_ptr[0])&&
                            (0x81 == p_l_can_data_ptr[1]))
                        {
                            P_L_Can_step_can_reprog = CAN_READ_HISTORIQUE_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        //reponse negative
                        else if ((0x00 == p_l_can_data_ptr[0])&&
                                 (0x7F == p_l_can_data_ptr[1])&&
                                 (0x41 == p_l_can_data_ptr[2]))
                        {
                            P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                        }
                    }
                    break;

                case CAN_READ_HISTORIQUE_RX:
                    //REQUETE $43 lecture de historique
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x83 == p_l_can_data_ptr[1]))
                    {
                        //reception du nombre octets de donnee
                        nb_octet = (p_l_can_data_ptr[2]  << 8) + p_l_can_data_ptr[3];

                        data_index = 0;
                        //copie des donnee dans le tableau historique
                        for(P_L_Can_index_historique ; P_L_Can_index_historique < (U16)nb_octet; P_L_Can_index_historique++)
                        {
                            P_L_Can_received_historique[P_L_Can_index_historique] = p_l_can_data_ptr[data_index + 4];
                            data_index++;
                        }

                        //test si fin des donnee de historique
                        if(P_L_CAN_TAILLE_BUFFER_HISTORIQUE == nb_octet)
                        {//oui

                            P_L_Can_index_historique = 0x00;
                            P_L_Can_Nb_historique = 0;


                            //analyse des donnees pour trouver le nombre historique ecrit
                            do
                            {
                                //on se positionnne sur le debut d un historique
                                tmp_index = P_L_Can_Nb_historique*P_L_CAN_TAILLE_1_HISTORIQUE;

                                //test si on est pas arrivee au dernier historique
                                if ((P_L_CAN_NB_MAX_HISTORIQUE*P_L_CAN_TAILLE_1_HISTORIQUE) > tmp_index)
                                {//non
                                    //reset du flag historique vue
                                    RecievedHisto = FALSE;

                                    //passage en revue des donnee de l historique
                                    for(boucle=0;boucle<P_L_CAN_TAILLE_1_HISTORIQUE;boucle++)
                                    {
                                        //test si donneee ecrite
                                        if(0xFF != P_L_Can_received_historique[tmp_index + boucle])
                                        {//oui
                                            //set du flag historique vue
                                            RecievedHisto = TRUE;
                                            //arret de la boucle
                                            boucle = P_L_CAN_TAILLE_1_HISTORIQUE;
                                        }
                                    }
                                }
                                else
                                {//oui
                                   //reset du flag historique vue
                                   RecievedHisto = FALSE;
                                }

                                P_L_Can_Nb_historique++;
                            //on boucle tanque l on trouve un historique ecrit
                            }while(TRUE == RecievedHisto);

                            //compte un historique de trop
                            //supression de ctte ligne on testera max -1
                            //NbHistorique = NbHistorique-1;

                            //test si nombre maximal historique atteind
                            if((P_L_CAN_NB_MAX_HISTORIQUE-1) > P_L_Can_Nb_historique)
                            {//non
                                //demande ecriture de l historique
                                P_L_Can_index_historique = 0;
                                P_L_Can_step_can_reprog = CAN_WRITE_HISTORIQUE;
                                P_L_Can_nb_essai_send_reprog = 0;
                            }
                            else
                            {//oui
                                //demand effacement de historique
                                P_L_Can_step_can_reprog = CAN_ERASE_HISTORIQUE_TX;
                                P_L_Can_nb_essai_send_reprog = 0;
                            }
                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x43 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_WRITE_HISTORIQUE;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                    break;

                case CAN_ERASE_HISTORIQUE_RX:
                     //REQUETE $31 effacement de historique
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x71 == p_l_can_data_ptr[1]))
                    {
                        P_L_Can_index_historique = 0;
                        P_L_Can_step_can_reprog = CAN_WRITE_HISTORIQUE_1;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x31 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                    break;

                case CAN_WRITE_HISTORIQUE_1:
                case CAN_WRITE_HISTORIQUE:
                     //REQUETE $42 ecriture historique
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x82 == p_l_can_data_ptr[1]))
                    {
                        if (CAN_WRITE_HISTORIQUE_1 == P_L_Can_step_can_reprog)
                        {
                            P_L_Can_index_historique = 0;
                            P_L_Can_step_can_reprog = CAN_WRITE_HISTORIQUE;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                        else
                        {
                            P_L_Can_step_can_reprog = CAN_SORTIE_BOOT_TX;
                            P_L_Can_nb_essai_send_reprog = 0;
                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x42 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_ERROR_REPROG;
                    }
                    break;

                case CAN_SORTIE_BOOT_RX:
                    if ( (0x00 == p_l_can_data_ptr[0])&&
                        (0x90 == p_l_can_data_ptr[1])&&
                        (0x00 == p_l_can_data_ptr[2])&&
                        (0x00 == p_l_can_data_ptr[3])&&
                        (0x00 == p_l_can_data_ptr[4])&&
                        (0x00 == p_l_can_data_ptr[5])&&
                        (0x00 == p_l_can_data_ptr[6])&&
                        (0x00 == p_l_can_data_ptr[7]))
                    {
                        P_L_Can_step_can_reprog = CAN_END_REPROG;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                    break;

                default:
                    break;
            }
        }
//////////////////////////////////////////////////////////////
//
//          Reception historique
//
//////////////////////////////////////////////////////////////
        else if (APP_Flashage_state == FLASHAGE_HISTORIQUE)
        {
            switch(P_L_Can_step_historique)
            {
                case CAN_RETOUR_EN_BOOT_HISTORIQUE_RX:
                    if ( (0x00 == p_l_can_data_ptr[0]) && (0x73 == p_l_can_data_ptr[1]))
                    {
                        if (0x00 == p_l_can_data_ptr[2])
                        {
                            P_L_Can_reprog_New_Facade_XXL = FALSE;
                            P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                        }
                        else if (0x01 == p_l_can_data_ptr[2])
                        {
                            switch(p_l_can_data_ptr[3])
                            {
                                case 0:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 1:
                                    P_L_Can_reprog_New_Facade_XXL = TRUE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;

                                case 2:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = TRUE;
                                    break;

                                default:
                                    P_L_Can_reprog_New_Facade_XXL = FALSE;
                                    P_L_Can_reprog_New_Facade_T_PANEL = FALSE;
                                    break;
                            }
                        }
                        else
                        {
                            //do nothing
                        }
                        P_L_Can_nb_essai_send_historique = 0;
                        P_L_Can_step_historique = CAN_READ_READ_HISTORIQUE_TX;
                    }
                break;

                case CAN_READ_READ_HISTORIQUE_RX:
                    //REQUETE $43 lecture de historique
                    //reponse positive
                    if ((0x00 == p_l_can_data_ptr[0])&&
                        (0x83 == p_l_can_data_ptr[1]))
                    {
                        //reception du nombre octets de donnee
                        nb_octet = (p_l_can_data_ptr[2]  << 8) + p_l_can_data_ptr[3];

                        data_index = 0;
                        //copie des donnee dans le tableau historique
                        for(P_L_Can_index_historique ; P_L_Can_index_historique < (U16)nb_octet; P_L_Can_index_historique++)
                        {
                            P_L_Can_received_historique[P_L_Can_index_historique] = p_l_can_data_ptr[data_index + 4];
                            data_index++;
                        }

                        //test si fin des donnee de historique
                        if(P_L_CAN_TAILLE_BUFFER_HISTORIQUE == nb_octet)
                        {//oui

                            P_L_Can_index_historique = 0x00;
                            P_L_Can_Nb_historique = 0;


                            //analyse des donnees pour trouver le nombre historique ecrit
                            do
                            {
                                //on se positionnne sur le debut d un historique
                                tmp_index = P_L_Can_Nb_historique*P_L_CAN_TAILLE_1_HISTORIQUE;

                                //test si on est pas arrivee au dernier historique
                                if ((P_L_CAN_NB_MAX_HISTORIQUE*P_L_CAN_TAILLE_1_HISTORIQUE) > tmp_index)
                                {//non
                                    //reset du flag historique vue
                                    RecievedHisto = FALSE;

                                    //passage en revue des donnee de l historique
                                    for(boucle=0;boucle<P_L_CAN_TAILLE_1_HISTORIQUE;boucle++)
                                    {
                                        //test si donneee ecrite
                                        if(0xFF != P_L_Can_received_historique[tmp_index + boucle])
                                        {//oui
                                            //set du flag historique vue
                                            RecievedHisto = TRUE;
                                            //arret de la boucle
                                            boucle = P_L_CAN_TAILLE_1_HISTORIQUE;
                                        }
                                    }
                                }
                                else
                                {//oui
                                   //reset du flag historique vue
                                   RecievedHisto = FALSE;
                                }

                                P_L_Can_Nb_historique++;
                            //on boucle tanque l on trouve un historique ecrit
                            }while(TRUE == RecievedHisto);

                            //compte un historique de trop
                            P_L_Can_Nb_historique = P_L_Can_Nb_historique-1;

                            P_L_Can_step_historique = CAN_SORTIE_BOOT_HISTORIQUE_TX;
                        }
                    }
                    //reponse negative
                    else if ((0x00 == p_l_can_data_ptr[0])&&
                             (0x7F == p_l_can_data_ptr[1])&&
                             (0x43 == p_l_can_data_ptr[2]))
                    {
                        P_L_Can_step_can_reprog = CAN_WRITE_HISTORIQUE;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                break;


                case CAN_SORTIE_BOOT_HISTORIQUE_RX:
                    if ( (0x00 == p_l_can_data_ptr[0])&&
                        (0x90 == p_l_can_data_ptr[1])&&
                        (0x00 == p_l_can_data_ptr[2])&&
                        (0x00 == p_l_can_data_ptr[3])&&
                        (0x00 == p_l_can_data_ptr[4])&&
                        (0x00 == p_l_can_data_ptr[5])&&
                        (0x00 == p_l_can_data_ptr[6])&&
                        (0x00 == p_l_can_data_ptr[7]))
                    {
                        P_L_Can_step_historique = CAN_SEND_HISTORIQUE;
                        P_L_Can_nb_essai_send_reprog = 0;
                    }
                break;

                default:
                break;
            }
        }
//////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////
    }
}

