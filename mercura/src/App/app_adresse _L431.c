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
#include "app_adresse.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l.h"
#include "app_manage_mapping.h"
#include "app_manage_wifi.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/


/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern BOOL_TYPE                P_L_Rpg_applicatif_mode;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
// adresse globale du module de communication sur le reseaux CAN
U32 							APP_Adresse_module;

//validitï¿½ de l'adresse can du module
BOOL_TYPE 						APP_Adresse_can_valid;

//flags indiquant si une config bloc ou facade a ete vue
BOOL_TYPE       				APP_Config_bloc_vue;

//indique quel gestion wifi blt faire
APP_MODE_GES_WIFI_BLT_TYPE      APP_Mode_gestion_wifi_blt;

//definit si la position du module dans la configuration
U8								APP_Position_module;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static U8 APP_Find_Bloc(U32 adresse_module,BOOL_TYPE APP_Config_bloc_vue);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
*
*  Function Name   : Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : init function
*******************************************************************************/
void APP_Adresse_Node_Init(void)
{
// adresse logique temporaire
U8 p_l_adresse_logique_mod;
//adresse phisyque temporaire
U32 p_l_adresse_physique_mod;

APP_Position_module = 0;

/////////////////////////////////////////////////
// code normal
/////////////////////////////////////////////////
#ifndef _SANSBOOT
U8 module;

const U32  *ptr_app_liste_facade;
const U8   *ptr_app_gestion_wifi_blt;

    //adresse logique et physique lue dans le boot
	p_l_adresse_physique_mod = APP_ADRESSE_PHYSIQUE_DEFAUT;
	p_l_adresse_logique_mod = *(U8_CAL*)(P_L_ADR_LOGIQUE_FLASH_ADR);

    // l'adresse can du module est defini comme l'adresse physique plus l'adresse logique
    if ((0x00 < p_l_adresse_logique_mod) && (p_l_adresse_logique_mod <= 0x0f))
    {
        // on mets a jour l'adresse du module can avec la valeur lue
        APP_Adresse_module =   (U32)p_l_adresse_physique_mod + (U32)p_l_adresse_logique_mod;
        // on set le flag indiquant que l'adresse est valide
        APP_Adresse_can_valid = TRUE;
    }
    else
    {
        // on mets a jour l'adresse du module can avec la valeur par defaut
        APP_Adresse_module = (U32)p_l_adresse_physique_mod + APP_ADRESSE_LOGIQUE_DEFAUT;
        // on reset le flag indiquant que l'adresse est valide
        APP_Adresse_can_valid = FALSE;
    }


    ptr_app_liste_facade = (const U32 *)APP_LISTE_FACADE_CFG_BLOC;

    if (*ptr_app_liste_facade != 0xFFFFFFFF)
    {
        APP_Config_bloc_vue = FALSE;

        for(module=0x00;module<APP_NB_MAX_FACADE;module++)
        {
            if (*(ptr_app_liste_facade+module) != 0xFFFFFFFF)
            {
                if ((*(ptr_app_liste_facade+module) & 0x000000FF) != 0x000000FF)
                {
                    if ((*(ptr_app_liste_facade+module) & 0x00000030) == 0x00000010)
                    {
                        APP_Config_bloc_vue = TRUE;
                    }
                }
            }
        }
    }
    else
    {
        APP_Config_bloc_vue = TRUE;
    }

	// on recherche sa position dans la config
	APP_Position_module = APP_Find_Bloc(APP_Adresse_module,APP_Config_bloc_vue);

    if (TRUE == APP_Config_bloc_vue)
    {
        ptr_app_gestion_wifi_blt = (const U8 *) APP_GESTION_WIFI_BLT_CFG_BLOC;

        if (*ptr_app_gestion_wifi_blt == APP_MODE_GESTION_WIFI_TAB)
        {
        	ptr_app_gestion_wifi_blt =  (const U8 *) APP_GESTION_WIFI_BT_TAB_CFG_BLOC;
        	APP_Mode_gestion_wifi_blt = *(ptr_app_gestion_wifi_blt+APP_Position_module);
        }
        else
        {
        	APP_Mode_gestion_wifi_blt = *ptr_app_gestion_wifi_blt;
        }
    }
    else
    {
    	 ptr_app_gestion_wifi_blt = (const U8 *) APP_GESTION_WIFI_BLT_CFG_FACADE;

    	 if (*ptr_app_gestion_wifi_blt == APP_MODE_GESTION_WIFI_TAB)
    	 {
    		 ptr_app_gestion_wifi_blt =  (const U8 *) APP_GESTION_WIFI_BT_TAB_CFG_FACADE;
    		 APP_Mode_gestion_wifi_blt = *(ptr_app_gestion_wifi_blt+APP_Position_module);
    	 }
    	 else
    	 {
    		 APP_Mode_gestion_wifi_blt = *ptr_app_gestion_wifi_blt;
    	 }
    }


#else
/////////////////////////////////////////////////
// code sans boot
/////////////////////////////////////////////////
    // on mets a jour l'adresse du module can avec la valeur par defaut
	p_l_adresse_physique_mod = APP_ADRESSE_PHYSIQUE_DEFAUT;



    p_l_adresse_logique_mod = (U32)1;

    APP_Adresse_module =   (U32)p_l_adresse_physique_mod + (U32)p_l_adresse_logique_mod;

    // on reset le flag indiquant que l'adresse est valide
    APP_Adresse_can_valid = TRUE;
    P_L_Rpg_applicatif_mode = TRUE;

    APP_Config_bloc_vue = TRUE;

    //APP_Config_bloc_vue = FALSE;
#ifdef _TEST_PROJECTEUR_BT
    APP_Mode_gestion_wifi_blt = MODE_PROJECTEUR_BT;
    APP_Config_bloc_vue = FALSE;
#else
#ifdef _TEST_RAMPE_AUTONOME_WIFI
    APP_Mode_gestion_wifi_blt = MODE_RAMPE_AUTONOME_WIFI;
    APP_Config_bloc_vue = FALSE;
#else
#ifdef _TEST_CMD_RAMPE_AUTONOME_BLEU
    APP_Mode_gestion_wifi_blt = MODE_CMD_RAMPE_AUTONOME_BLEU;
    APP_Config_bloc_vue = FALSE;
#else
#ifdef _TEST_CMD_RAMPE_AUTONOME_ORANGE
    APP_Mode_gestion_wifi_blt = MODE_CMD_RAMPE_AUTONOME_ORANGE;
    APP_Config_bloc_vue = FALSE;
#else
#ifdef  _TEST_RAMPE_AUTONOME
    APP_Mode_gestion_wifi_blt = MODE_RAMPE_AUTONOME;
    p_l_adresse_logique_mod = (U32)2;
    APP_Adresse_module =   (U32)p_l_adresse_physique_mod + (U32)p_l_adresse_logique_mod;
    APP_Config_bloc_vue = TRUE;
#else
#ifdef  _TEST_MODE_MESSAGE_PMV
    APP_Mode_gestion_wifi_blt = APP_MODE_MESSAGE_PMV;
#else
    APP_Mode_gestion_wifi_blt = APP_MODE_BTN_ET_MESSAGE_PMV;
#endif
#endif
#endif
#endif
#endif
#endif
#endif
}

/*******************************************************************************
*
*  Function Name   : APP_Find_Bloc
*  Description     : on recherche la bloc dans la configuration et
*                  retourne sa position
*
*
*******************************************************************************/
static U8 APP_Find_Bloc(U32 adresse_module,BOOL_TYPE APP_Config_bloc_vue)
{
const U32  *ptr_app_liste_bloc;
U8 bloc;
U8 module;

    // on initialise module a aucun vue
    module = 0xFF;
    if (APP_Config_bloc_vue == TRUE)
    {
    	ptr_app_liste_bloc = (const U32 *)APP_LISTE_BLOC_CFG_BLOC;
    }
    else
    {
    	ptr_app_liste_bloc = (const U32 *)APP_LISTE_BLOC_CFG_FACADE;
    }

    // on passe en revue la listes des blocs configure
    for (bloc=0x00;bloc<APP_NB_MAX_BLOC;bloc++)
    {
        // on test si l adresse du bloc configure correspond a adresse_module
    	if (*(ptr_app_liste_bloc+bloc) == adresse_module)
        {// oui
            // on memorise la position de ce module
            module = bloc;
            // on sort de la boucle for
            bloc = APP_NB_MAX_BLOC;
        }
    }
return(module);
}


