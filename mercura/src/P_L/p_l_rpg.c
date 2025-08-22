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
#include "p_l_rpg.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "app.h"
#include "hwi_hal_can.h"
#include "p_l.h"
#include "hwi_hal_flash.h"
#include "p_l_rx_can1.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U8 P_L_Rpg_boot_demande[P_L_MAX_DATA_RAM_COM];
extern S8_CAL * P_L_Rpg_hardware_version_adress;

extern U8 APP_Rpg_Flag_dem_a_froid2[P_L_MAX_DATA_RAM_INIT];
extern U8 APP_Rpg_Flag_dem_a_froid[P_L_MAX_DATA_RAM_INIT];
extern U8 P_L_Rpg_boot_demande[P_L_MAX_DATA_RAM_COM];
extern U32 APP_Adresse_module;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
BOOL_TYPE P_L_Rpg_retour_boot;
BOOL_TYPE P_L_Rpg_applicatif_mode;
U16 P_L_Rpg_checksum_applicatif;
U16 P_L_Rpg_checksum_config;
U8_CAL* P_L_Memory_Code_ptr;
BOOL_TYPE P_L_Rpg_calc_checksum_ready;
U8 rpg_send_buffer[8];


/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE */

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Saut_Boot
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
void P_L_Rpg_Saut_Boot(void)
{

    if(TRUE == P_L_Rpg_retour_boot)
    {
        APP_Rpg_Flag_dem_a_froid[0] = 0x5C;
        APP_Rpg_Flag_dem_a_froid[1] = 0xC5;

    	APP_Rpg_Flag_dem_a_froid2[0] = 0xAA;
    	APP_Rpg_Flag_dem_a_froid2[1] = 0x55;

        // Reset propre !!!
        NVIC_SystemReset();
    }
    else
    {
        // do nothing
    }
}

/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Calc_Checksum
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : calcul du checksum applicatif et configuration
*******************************************************************************/
void P_L_Rpg_Calc_Checksum(void)
{
    //verifie si le checksum a été claculé
    if (FALSE == P_L_Rpg_calc_checksum_ready)
    {
        // inserer ici la fonction de calcul de checksum
//        for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_USER_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_END_USER_FLASH_ADR;P_L_Memory_Code_ptr++)
#if defined(STM32F105xC)
    	for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_USER_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_END_USER_FLASH_ADR_OLD;P_L_Memory_Code_ptr++)
#elif defined(STM32L431xx)
		for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_USER_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_END_USER_FLASH_ADR_NEW;P_L_Memory_Code_ptr++)
#else
erreur choisir un processeur
#endif
    	{
            if(0xFF != *P_L_Memory_Code_ptr)
            {
                P_L_Rpg_checksum_applicatif += *P_L_Memory_Code_ptr;
                P_L_Rpg_checksum_applicatif ++;
            }
        }

//        for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_CONF_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_END_CONF_FLASH_ADR;P_L_Memory_Code_ptr++)
#if defined(STM32F105xC)
		for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_CONF_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_CHECKSUM_CONF_FLASH_ADR_OLD;P_L_Memory_Code_ptr++)
#elif defined(STM32L431xx)
		for (P_L_Memory_Code_ptr=(U8_CAL*)P_L_FIRST_CONF_FLASH_ADR;P_L_Memory_Code_ptr<(U8_CAL*)P_L_CHECKSUM_CONF_FLASH_ADR_NEW;P_L_Memory_Code_ptr++)
#else
erreur choisir un processeur
#endif
    	{
            if(0xFF != *P_L_Memory_Code_ptr)
            {
                P_L_Rpg_checksum_config += *P_L_Memory_Code_ptr;
                P_L_Rpg_checksum_config ++;
            }
        }
        // le checksum est calculé
        P_L_Rpg_calc_checksum_ready = TRUE;
    }
    else
    {
        // population de la zone d'echange de données avec les valeur de checksum
		if(P_L_DMD_CHECKSUM_APL == P_L_Rpg_boot_demande[1])
		{
			P_L_Rpg_boot_demande[2] = (U8)((P_L_Rpg_checksum_applicatif/256)& 0x00FF);
			P_L_Rpg_boot_demande[3] = (U8)(P_L_Rpg_checksum_applicatif & 0x00FF);
		}
		else if(P_L_DMD_CHECKSUM_CONFIG == P_L_Rpg_boot_demande[1])
        {
            P_L_Rpg_boot_demande[2] = (U8)((P_L_Rpg_checksum_config/256)& 0x00FF);
            P_L_Rpg_boot_demande[3] = (U8)(P_L_Rpg_checksum_config & 0x00FF);
        }
        else
        {
            P_L_Rpg_boot_demande[2] = 0xFF;
            P_L_Rpg_boot_demande[3] = 0xFF;
        }

        // verifie si il y a une demande de retour en boot après calcul de checksum
        if((P_L_SERVICE_CHECKSUM + 0x40) == P_L_Rpg_boot_demande[0])
        {
            P_L_Rpg_retour_boot = TRUE;
        }
        else
        {
            // do nothing
        }
    }
}


/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : init function
*******************************************************************************/
void P_L_Rpg_Init(void)
{
HWI_CAN_OBJECT_TYPE  can_objet;

   P_L_Rpg_checksum_applicatif = 0;
   P_L_Rpg_checksum_config = 0;
   P_L_Rpg_calc_checksum_ready = FALSE;
   P_L_Rpg_retour_boot= FALSE;


   if(P_L_SERVICE_MISE_SOUS_TENSION == P_L_Rpg_boot_demande[0])
   {
      // On a reçu une demande pour rester dans l'applicatif
      P_L_Rpg_applicatif_mode = TRUE;
   }
   else if(P_L_SERVICE_CHECKSUM == P_L_Rpg_boot_demande[0])
   {

      if(P_L_DMD_CHECKSUM_APL == P_L_Rpg_boot_demande[1])
      {
         // crée la réponse retour suite demande calcul checksum conf.
         P_L_Rpg_boot_demande[0] = 0x40 + P_L_SERVICE_CHECKSUM;
         P_L_Rpg_boot_demande[1] = P_L_DMD_CHECKSUM_APL;
         P_L_Rpg_boot_demande[2] = 0xFF;
         P_L_Rpg_boot_demande[3] = 0xFF;
         P_L_Rpg_applicatif_mode = FALSE;
      }
      else if(P_L_DMD_CHECKSUM_CONFIG == P_L_Rpg_boot_demande[1])
      {
         // crée la réponse retour suite demande calcul checksum conf.
         P_L_Rpg_boot_demande[0] = 0x40 + P_L_SERVICE_CHECKSUM;
         P_L_Rpg_boot_demande[1] = P_L_DMD_CHECKSUM_CONFIG;
         P_L_Rpg_boot_demande[2] = 0xFF;
         P_L_Rpg_boot_demande[3] = 0xFF;
         P_L_Rpg_applicatif_mode = FALSE;
      }
    }
   else if(P_L_SERVICE_RETOUR_APL_BOOT == P_L_Rpg_boot_demande[0])
   {
        if(P_L_SAUT_SUITE_REPROG_DMD == P_L_Rpg_boot_demande[1])
        {
        	can_objet.HWI_CAN_MSG_ID = (APP_Adresse_module | P_L_ID_SLAVE_TO_MASTER);
        	can_objet.HWI_CAN_LENGTH = 0x08;
        	can_objet.HWI_ID_FORMAT = HWI_STD;


            rpg_send_buffer[7] = 0x00;
            rpg_send_buffer[6] = 0x00;
            rpg_send_buffer[5] = 0x00;
            rpg_send_buffer[4] = 0x00;
            rpg_send_buffer[3] = 0x00;
            rpg_send_buffer[2] = 0x00;
            rpg_send_buffer[1] = 0x90;
            rpg_send_buffer[0] = 0x00;
            //Réponse positive à la requête $50 suite saut après flashage.
            Hwi_Can_Send_Message(CAN1,&can_objet, rpg_send_buffer,MAIL_BOX3);

            P_L_Rpg_applicatif_mode = TRUE;

            P_L_Rpg_boot_demande[0] = P_L_SERVICE_MISE_SOUS_TENSION;
        }
   }
   APP_Rpg_Flag_dem_a_froid[0] = 0x00;
   APP_Rpg_Flag_dem_a_froid[1] = 0x00;

   APP_Rpg_Flag_dem_a_froid2[0] = 0x00;
   APP_Rpg_Flag_dem_a_froid2[1] = 0x00;
}

/*******************************************************************************
*
*  Function Name   : P_L_Rpg_Param_Flash
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*
*******************************************************************************/
U8 P_L_Rpg_Param_Flash(U32 flash_adresse,U8* data,U16 data_size)
{
U8 status;
U8 data_temp[P_L_PARAM_FLASH_SIZE];
U16 boucle;
U8_CAL *P_L_Param_ptr;
U16 offset;

U32 first_param_flash;
U32 last_param_flash;
	status = P_L_RPG_TEST_PASS;

#ifdef _SANSBOOT
	first_param_flash = P_L_FIRST_CONF_FLASH_ADR-P_L_PARAM_FLASH_SIZE;
	last_param_flash = P_L_FIRST_CONF_FLASH_ADR;
	flash_adresse = (flash_adresse-P_L_FIRST_PARAM_FLASH_ADR)+first_param_flash;
#else
	first_param_flash = P_L_FIRST_PARAM_FLASH_ADR;
	last_param_flash = P_L_LAST_PARAM_FLASH_ADR;

#endif

	if ((flash_adresse >= first_param_flash) && ((flash_adresse+data_size) < last_param_flash))

	{
		//copy Flash data to RAM
		P_L_Param_ptr = (U8_CAL*)first_param_flash;
		for(boucle=0;boucle<P_L_PARAM_FLASH_SIZE;boucle++)
		{
			data_temp[boucle] = *P_L_Param_ptr;
			P_L_Param_ptr = P_L_Param_ptr + 1;
		}

		//calcul offset of first data write
		offset = (U16)(flash_adresse - first_param_flash);

		//update Ram data with new value
		for(boucle=0;boucle<data_size;boucle++)
		{
			data_temp[boucle+offset] = data[boucle];
		}

		//erase flash memory zone
		if (HAL_OK == HWI_Hal_Flash_Erase_Zone((U32)first_param_flash,((U32)last_param_flash-(U32)0x00000001)))
		{
			if (HAL_OK !=  HWI_Hal_Flash_Program((U32)first_param_flash,(U8*)data_temp,P_L_PARAM_FLASH_SIZE))
			{
				status = P_L_RPG_TEST_FAIL;
			}
		}
		else
		{
			status = P_L_RPG_TEST_FAIL;
		}
	}
	else
	{
		status = P_L_RPG_TEST_FAIL;
	}

	return status;
}
