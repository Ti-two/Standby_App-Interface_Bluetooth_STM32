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
#include "lib_macros.h"
#include "app_manage_on_off.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_macros.h"
#include "hwi_hal_gpio.h"
#include "hwi_hal_can.h"
#include "hwi_watchdog.h"
#include "app_manage_wifi.h"


/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NOTHING*/


/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
//demande via le reseau can d'arreter l'analyse des differents arret
extern BOOL_TYPE				    APP_Arret_analyse;

extern U32                          S_S_1MS_U32_TIME;

extern U32							APP_Timer_filtrage_perte_reseau_can_stop;
extern BOOL_TYPE					APP_Lock_filtrage_perte_reseau_can_stop;

extern BOOL_TYPE                    APP_Adresse_can_valid;

//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE           APP_Mode_gestion_wifi_blt;

//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE                            P_L_CarteSd_Atester_vue;
//flag indiquant qu'une carte SD autorisant le mode testeur
extern BOOL_TYPE                             P_L_CarteSd_Testeur_vue;


#ifdef _RAMPE_SOLAIRE
extern BOOL_TYPE                             APP_Stop_Rampe;
#endif


/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
// demande du reseau can arreter le systeme
extern BOOL_TYPE				            APP_Can_stop_request;


// demande d arrete du systeme sur perte du reseau CAN
BOOL_TYPE				            APP_Can_stop_perte_reseau;

// gestion des phase d'arret du systeme, differents etat du systeme
APP_STATE_ARRET_TYPE 	            APP_State_arret;

//demande d'arret d envoie des trame CAN
BOOL_TYPE                           APP_Demande_Arret_SendCan;

//Temporisation des phase d'arret
U32                                 APP_Timer_arret_systeme;
BOOL_TYPE                           APP_Lock_Timer_arret_systeme;

BOOL_TYPE                            APP_Arret_Rampe_autonome_wifi;
BOOL_TYPE                            APP_Demande_Arret_Rampe_autonome_wifi;
U32                                 APP_Timer_Arret_Rampe_autonome_wifi;
BOOL_TYPE                           APP_Lock_Arret_Rampe_autonome_wifi;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static BOOL_TYPE APP_Test_Condition_Arret(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name  : APP_Gestion_Alim_Carte
*  Description    : Gere les phases de mise en route et d'arret de la carte
*  Param			:
*
*******************************************************************************/
void APP_Gestion_Alim_Carte(void)
{
BOOL_TYPE result;

        switch(APP_State_arret)
        {
            // le systeme est en marche
            case APP_RUN:
            	if ((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME) || (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI))
            	{

            		if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
					{
						if (APP_Demande_Arret_Rampe_autonome_wifi == TRUE)
						{
							 BT_TIMER_LOCK(APP_Timer_Arret_Rampe_autonome_wifi,S_S_1MS_U32_TIME,2000,APP_Lock_Arret_Rampe_autonome_wifi,result);
							 if (TRUE == result)
							 {
								 APP_Arret_Rampe_autonome_wifi = TRUE;
							 }
						}
						else
						{
							APP_Arret_Rampe_autonome_wifi = FALSE;
						}
					}

            		if (TRUE == APP_Test_Condition_Arret())
					{
						APP_Demande_Arret_SendCan = TRUE;

					}
					else
					{// on est pas en phase d'arret

					}

            	}
            	else
            	{
					// test s il y a une demmande d'arret en cours
					if (TRUE == APP_Test_Condition_Arret())
					{// demande d'arret en cours
						// on passe en phase d arret
						APP_State_arret = APP_WAIT_STOP_REQUEST;
						APP_Demande_Arret_SendCan = TRUE;

					}
					else
					{// on est pas en phase d'arret

					}
            	}

                // re initialisation de la tempo des etapes d arret
                BT_TIMER_INIT_LOCK(APP_Timer_arret_systeme,S_S_1MS_U32_TIME,APP_Lock_Timer_arret_systeme);
            break;

            case APP_WAIT_STOP_REQUEST:
                //on arrete les crenaux de reveil
                APP_Demande_Arret_SendCan = TRUE;

                BT_TIMER_LOCK(APP_Timer_filtrage_perte_reseau_can_stop,S_S_1MS_U32_TIME,APP_FILTRAGE_ARRET_STOP_RESEAU_CAN,APP_Lock_filtrage_perte_reseau_can_stop,result);
                if(TRUE == result)
                {
                   // tempo d attente plus de communication CAN
                    // on passe a la phase arret
                    APP_State_arret = APP_STOP_REQUEST;
                }

                // attente de quelque secondes
                BT_TIMER_LOCK(APP_Timer_arret_systeme,S_S_1MS_U32_TIME,APP_TIME_OUT_STOP_REQUEST,APP_Lock_Timer_arret_systeme,result);
                if (TRUE == result)
                {// tempo d attente ecoule
                    // on passe a la phase arret
                    APP_State_arret = APP_STOP_REQUEST;
                }
            break;

            // arret de la carte
            case APP_STOP_REQUEST:
                HAL_CAN_Write_NSTB(FALSE);
                HAL_CAN_Write_EN(TRUE);

                // on coupe l'alimentation de la carte
                HWI_Commande_alim_bloc(FALSE);

                // re initialisation de la tempo des etapes d arret
                BT_TIMER_INIT_LOCK(APP_Timer_arret_systeme,S_S_1MS_U32_TIME,APP_Lock_Timer_arret_systeme);

                // on passe a l etape watchdog si jamais la coupure d alimentation
                // de la carte n a pas fonctionne
                APP_State_arret = APP_STOP_WATCHDOG;
            break;

            //watchdog non coupure alimentation de la carte
            case APP_STOP_WATCHDOG:
                HAL_CAN_Write_NSTB(FALSE);
                HAL_CAN_Write_EN(FALSE);

                // attente de la tempo de watchdog d arret de la carte
                BT_TIMER_LOCK(APP_Timer_arret_systeme,S_S_1MS_U32_TIME,APP_TEMPO_BEFORE_STOP_WATCHDOG,APP_Lock_Timer_arret_systeme,result);
                if (TRUE == result)
                {// tempo de watchdog ecoule

                    // on passe a l etape attente redemarrage du systeme
                    APP_State_arret = APP_WAIT_RESTAST;
                        // on force le watchdog du microprocesseur a se declenche
                    HWI_Arret_watchdog();
                }
                else
                {//pas encore ecoule
                    // on reste dans cette etape
                    APP_State_arret = APP_STOP_WATCHDOG;
                    // on recharge et ou verifie que le watchdog est en route
                    HWI_Start_watchdog();
                }
            break;

            // attente que le watchdog microprocessuer se declenche
            case APP_WAIT_RESTAST:
                // on reste dans cette etape
                APP_State_arret = APP_WAIT_RESTAST;
                // on force le watchdog du microprocesseur a se declenche
                HWI_Arret_watchdog();
            break;

            default:
                // do nothing
                // on reforce le mode RUN
                APP_State_arret = APP_RUN;
                // on confirme l'alimentation de la carte
                HWI_Commande_alim_bloc(TRUE);
            break;
        }
}

/*******************************************************************************
*
*  Function Name  : APP_Test_Condition_Arret
*  Description    : revoie true si une demande d arret est en cours
*  Param		  :
*                   renvoie true si:
*                   demande d arret via le reseau can
*                   demande d arret via ecoulement de la tempo arret
*                   demande d arret via le coeur d analyse
*                   ou tension basse et pas de temoin de charge
*******************************************************************************/
static BOOL_TYPE APP_Test_Condition_Arret(void)
{
#ifndef _SANSBOOT
//1-1
#ifndef _DEBUG_RELEASE
//2-1
    if ((TRUE == APP_Adresse_can_valid)&&(FALSE == P_L_CarteSd_Atester_vue)&&(FALSE == P_L_CarteSd_Testeur_vue))
    {
    	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    	{
    		 return (APP_Can_stop_request);
    	}
    	else if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
    	{
    		 return (APP_Can_stop_request || APP_Arret_Rampe_autonome_wifi);
    	}
    	else if ((APP_MODE_ODOMETRE == APP_Mode_gestion_wifi_blt)||
        	/*(MODE_RAMPE_AUTONOME == APP_Mode_gestion_wifi_blt)||*/
			(MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt))
        {
            return (FALSE);
        }
        else
        {
#ifdef _RAMPE_SOLAIRE
//2-2
            if (FALSE == APP_Stop_Rampe)
            {
               return (APP_Can_stop_request);
            }
            else
            {
              return(FALSE);
            }
#else
//2-2
            return (APP_Can_stop_request);
            //return((TRUE == APP_Can_stop_request)||(TRUE == APP_Can_stop_perte_reseau));
#endif
//2-2
//2-1
        }
    }
    else
    {
        return(FALSE);
    }
//2-1
#else
//2-1
#ifdef _RAMPE_SOLAIRE
//2-2
    if (FALSE == APP_Stop_Rampe)
    {
       return (APP_Can_stop_request);
    }
    else
    {
      return(FALSE);
    }
//2-2
#else
//2-2
    return(APP_Can_stop_request);
//2-2
#endif
//2-2
//2-1
#endif
//1-1
#else
//1-1
#ifdef _RAMPE_SOLAIRE
//1-2
    if (FALSE == APP_Stop_Rampe)
    {
       return (APP_Can_stop_request);
    }
    else
    {
      return(FALSE);
    }
//1-2
#else
//1-2
    if(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
    {
        return (APP_Can_stop_request);
    }
    else if ((APP_MODE_ODOMETRE == APP_Mode_gestion_wifi_blt)||
         /*(MODE_RAMPE_AUTONOME == APP_Mode_gestion_wifi_blt)||*/
    	 (MODE_RAMPE_AUTONOME_WIFI == APP_Mode_gestion_wifi_blt))
     {
          return (FALSE);
     }
     else
     {
    	 return(APP_Can_stop_request);
     }
//1-2
#endif
//1-2
//1-1
#endif
}



/*******************************************************************************
*
*  Function Name  : HWI_Commande_alim
*  Description    :
*  Param		  :
*
*******************************************************************************/
void HWI_Commande_alim_bloc(BOOL_TYPE cmd)
{
    HWI_Rst_Latch_WU_Cmd(!cmd);
}

/*******************************************************************************
*
*  Function Name  : APP_Gestion_Alim_Carte_Init
*  Description    :
*  Param		  :
*
*******************************************************************************/
void APP_Gestion_Alim_Carte_Init(void)
{
    //reset de la demande d arret via le reseau can
    APP_Can_stop_request = FALSE;
    //reset de la demande sur perte du reseau CAN
    APP_Can_stop_perte_reseau = FALSE;

    // initialisation de l'etape d arret du systeme
    APP_State_arret = APP_RUN;

     // commande alimantation de la carte
    HWI_Commande_alim_bloc(TRUE);

    // initialisation du filtrage des etapes d arret du systeme
    BT_TIMER_INIT_LOCK(APP_Timer_arret_systeme,((U32)0x00000000),APP_Lock_Timer_arret_systeme);

    //arret de la demande d'arret d envoie des trame CAN
    APP_Demande_Arret_SendCan = FALSE;

    APP_Arret_Rampe_autonome_wifi = FALSE;
    APP_Demande_Arret_Rampe_autonome_wifi = FALSE;
}
