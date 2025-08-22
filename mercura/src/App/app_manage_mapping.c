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
#include "app_manage_mapping.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/


/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/


#ifdef _SANSCFG
// pour le test

//*******************************************************************************************************************************
//                      CONFIG NEUTRE DE FLASHAGE
//*******************************************************************************************************************************

/////
// nombre de carte led
U8_CAL  __attribute__((section(".Code_Config_2.APP_TFT_NB_PMV_CARTE_LED")))   APP_TFT_NB_PMV_CARTE_LED_INIT    __attribute__ ((aligned (1)));

//liste des adresses can des facades APP_LISTE_FACADE_CFG_FACADE           ((U32)0x0801C804)
U32_CAL __attribute__((section(".Code_Config_2.APP_LISTE_FACADE")))           APP_LISTE_FACADE_INIT[APP_NB_MAX_FACADE] __attribute__ ((aligned (1))) = {0x51,0x52};

//Choix du type de gestion a faire wifi BT
U8_CAL __attribute__((section(".Code_Config_2.APP_TYPE_WIFI_BT_TAB"))) 		APP_TYPE_WIFI_BT_TAB[APP_NB_MAX_BLOC]  __attribute__ ((aligned (1)));
#endif
