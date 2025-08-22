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
#include "p_l_data_flash.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l.h"
#include "p_l_bx310x.h"
#include "p_l_rpg.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U8      			   P_L_BT_spp_mac_pair_rampe[P_L_BT_MAC_SIZE];
extern hw_addr             P_L_BT_Spp_Mac;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
//demande d ecritue de la mac du module appairait en spp
BOOL_TYPE	P_L_Flash_Mac_spp_Wr;
U8			P_L_Flash_Mac_spp_rx[P_L_SSP_MAC_PAIR_FLASH_SIZE];

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name  : P_L_Data_Flash_Wr
*  Description    : gestion de ecriture des donnée en flash
*	Param		  :
*
*******************************************************************************/
void P_L_Data_Flash_Wr(void)
{
U8 result_flash;
U8 data[1];

	if (TRUE == P_L_Flash_Mac_spp_Wr)
	{
		result_flash = P_L_Rpg_Param_Flash(P_L_SSP_MAC_PAIR_FLASH_ADR,P_L_Flash_Mac_spp_rx,P_L_SSP_MAC_PAIR_FLASH_SIZE);
		if (P_L_RPG_TEST_PASS == result_flash)
		{
			/*
			data[0] = 1;
			result_flash = P_L_Rpg_Param_Flash(P_L_SSP_MAX_PAIR_RX_OK_FLASH_ADR,data,P_L_SSP_MAX_PAIR_RX_OK_FLASH_SIZE);
			if (P_L_RPG_TEST_PASS == result_flash)
			{
				P_L_Flash_Mac_spp_Wr = FALSE;
			}
			*/
			P_L_Flash_Mac_spp_Wr = FALSE;
		}
		else
		{
			/*
			data[0] = 0;
			result_flash = P_L_Rpg_Param_Flash(P_L_SSP_MAX_PAIR_RX_OK_FLASH_ADR,data,P_L_SSP_MAX_PAIR_RX_OK_FLASH_SIZE);
			*/
		}
	}
}

/*******************************************************************************
*
*  Function Name  : P_L_Data_Flash_Init
*  Description    :
*	Param		  :
*
*******************************************************************************/
void P_L_Data_Flash_Init(void)
{
const U8 *ptr;
#ifdef _RAMPE_AUTONOME
	P_L_Flash_Mac_spp_Wr = FALSE;

#ifdef _SANSBOOT
	ptr = (const U8 *) (((P_L_SSP_MAC_PAIR_FLASH_ADR-P_L_FIRST_PARAM_FLASH_ADR)+P_L_FIRST_CONF_FLASH_ADR)-P_L_PARAM_FLASH_SIZE);
#else
	ptr = (const U8 *) P_L_SSP_MAC_PAIR_FLASH_ADR;
#endif

	P_L_BT_Spp_Mac.addr[0] = *ptr;
	P_L_BT_Spp_Mac.addr[1] = *(ptr+1);
	P_L_BT_Spp_Mac.addr[2] = *(ptr+2);
	P_L_BT_Spp_Mac.addr[3] = *(ptr+3);
	P_L_BT_Spp_Mac.addr[4] = *(ptr+4);
	P_L_BT_Spp_Mac.addr[5] = *(ptr+5);


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



#endif
}

