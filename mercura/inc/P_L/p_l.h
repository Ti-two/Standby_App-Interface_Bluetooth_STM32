/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_
#define _P_L_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"





/*******************************************************************************
*  Application Definitions
*******************************************************************************/
/*
#define P_L_BOOT_VECTOR_ADR           0x08000000  // D�but du boot
#define P_L_BOOT_NAME_FLASH_ADR       0x08001ff0  //
#define P_L_BOOT_VERSION_FLASH_ADR    0x08001ff8  //
#define P_L_FIRST_USER_FLASH_ADR      0x08002000 // D�but de la zone applicative
#define P_L_END_USER_FLASH_ADR        0x0801A7FB
#define P_L_CHECKSUM_APP_ADR          0x0801A7FC // Position checksum applicatif
#define P_L_FLAG_APP_VALID_ADR        0x0801A7FE// Position du flag applicatif en m�moire
#define P_L_LAST_USER_FLASH_ADR       0x0801A7FF  //

#define P_L_FIRST_CONF_FLASH_1_ADR    0x0801A800  //
#define P_L_END_CONF_FLASH_1_ADR      0x0801C7FF  //

#define P_L_FIRST_CONF_FLASH_2_ADR    0x0801C800  //
#define P_L_END_CONF_FLASH_2_ADR      0x0801F7FD

#define P_L_CHECKSUM_CONF_FLASH_ADR   0x0801F7FE  // Position checksum configuration
#define P_L_LAST_CONF_FLASH_ADR       0x0801F7FF  //


#define P_L_FIRST_CONF_FLASH_ADR	  P_L_FIRST_CONF_FLASH_1_ADR
#define P_L_END_CONF_FLASH_ADR		  P_L_CHECKSUM_CONF_FLASH_ADR

#define P_L_PARAM_FLASH_SIZE		  		(2*1024)    //2k de flash
#define P_L_ADR_LOGIQUE_FLASH_ADR     		0x08000800  //0x08000800  //
#define P_L_CFG_BIT_FLASH_ADR     	  		0x08000801  //0x08000801  //


#define P_L_ADR_LOGIQUE_MODULE_OLD    0x1FFFF804
*/
///////////////////Old definition		///////////////////////////
#define P_L_BOOT_FLASH_OLD_ADR            		0x08000000  // Debut adresse boot
#define P_L_BOOT_SW_NAME_FLASH_OLD_ADR          0x08001FEC
#define P_L_BOOT_HW_NAME_FLASH_OLD_ADR      	0x08001FF0
#define P_L_BOOT_SW_VERSION_FLASH_OLD_ADR   	0x08001FF8  //
#define P_L_BOOT_VERSION_TEXT_FLASH_OLD_ADR  	0x08001FFD

///////////////////New definition		///////////////////////////
#define P_L_BOOT_VECTOR_ADR			  		0x08000000  //

#define P_L_FIRST_PARAM_FLASH_ADR     		0x08000800  //0x08000800  //
#define P_L_LAST_PARAM_FLASH_ADR      		0x08001000  //0x08001000  //
#define P_L_PARAM_FLASH_SIZE		  		(2*1024)    //2k de flash
#define P_L_ADR_LOGIQUE_FLASH_ADR     		0x08000800  //0x08000800  //
#define P_L_CFG_BIT_FLASH_ADR     	  		0x08000801  //0x08000801  //
#define P_L_SSP_MAC_PAIR_FLASH_ADR     	  	0x08000810  //0x08000810  //
	#define P_L_SSP_MAC_PAIR_FLASH_SIZE     	6
//#define P_L_SSP_MAX_PAIR_RX_OK_FLASH_ADR     	  	0x08000818  //0x08000818  //
//	#define P_L_SSP_MAX_PAIR_RX_OK_FLASH_SIZE     	  	1

#define P_L_CHECKSUM_APP_ADR_NEW      0x0801A7F8  // Position checksum applicatif
#define P_L_CHECKSUM_APP_ADR_OLD      0x0801A7FC  // Position checksum applicatif
#define P_L_END_USER_FLASH_ADR_NEW    0x0801A7F7
#define P_L_END_USER_FLASH_ADR_OLD    0x0801A7FB

#define P_L_CHECKSUM_CONF_FLASH_ADR_NEW   0x0801F7F8  // Position checksum configuration
#define P_L_CHECKSUM_CONF_FLASH_ADR_OLD   0x0801F7FE  // Position checksum configuration
#define P_L_END_CONF_FLASH_ADR_NEW        0x0801F7F7
#define P_L_END_CONF_FLASH_ADR_OLD        0x0801F7FD

////////////////////////////////////////////////////////////////////////////////
#define P_L_FIRST_USER_FLASH_ADR      0x08002000  // D�but de la zone applicative


#define P_L_FLAG_APP_VALID_ADR        0x0801A7FE  // Position du flag applicatif en m�moire
#define P_L_LAST_USER_FLASH_ADR       0x0801A7FF  //

#define P_L_FIRST_CONF_FLASH_ADR      0x0801A800  //

#define P_L_FIRST_CONF_FLASH_1_ADR    0x0801A800  //
#define P_L_END_CONF_FLASH_1_ADR      0x0801C7FF  //

#define P_L_FIRST_CONF_FLASH_2_ADR    0x0801C800  //

#define P_L_FIRST_CONF_TONALITE_ADR   0x0801BE00  //
#define P_L_FIRST_CONF_WIFI_ADR       0x0801C000  //


#define P_L_END_CONF_WIFI_ADR         0x0801C7FF
#define P_L_FIRST_CONF_EQUA_ADR       0x0801CE00  //

#define P_L_END_CONF_FLASH_2_ADR      0x0801F7FD

#define P_L_LAST_CONF_FLASH_ADR       0x0801F7FF  //


///////////////////New definition		///////////////////////////
#define P_L_BOOT_FLASH_NEW_ADR            	 0x0803B000  // Debut adresse boot
#define P_L_BOOT_SW_NAME_FLASH_NEW_ADR       0x0803FFEE
#define P_L_BOOT_HW_NAME_FLASH_NEW_ADR       0x0803FFF2
#define P_L_BOOT_SW_VERSION_FLASH_NEW_ADR    0x0803FFFA  //
#define P_L_BOOT_VERSION_TEXT_FLASH_NEW_ADR  0x0803FFFF

////////////////////////////////////////////////////////////////////////////////
///////////////////Old definition		///////////////////////////
#define P_L_ADR_LOGIQUE_MODULE_OLD      0x1FFFF804
//#define P_L_ADR_PHYSIQUE_MODULE     0x08001FFE
///////////////////////////////////////////////////////////////////


#define P_L_RESET_VECTOR_OFFSET       4           // Offset pour recup�rer l'adresse de d�but de code

// Boot - applicatif identifiants
#define P_L_SERVICE_MISE_SOUS_TENSION 0x00 // pas de param�tre

//////////////////////////////////////////////
#define P_L_SERVICE_RETOUR_APL_BOOT   0x01 //
#define P_L_SAUT_SUITE_REPROG_DMD     0x01

//////////////////////////////////////////////

//////////////////////////////////////////////
#define P_L_SERVICE_CHECKSUM          0x02 // request 1octet 00 calcul sur l'appli 01 calcul config
                                             // response   1 octet appli ou config  2 octet LSB ckeksum 3 octet MSB checksum
#define P_L_DMD_CHECKSUM_APL          0x00
#define P_L_DMD_CHECKSUM_CONFIG       0x01
/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/



#endif
