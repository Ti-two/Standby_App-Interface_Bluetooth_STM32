/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/

#ifndef _HWI_H_
#define _HWI_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "types.h"


#if defined(STM32F105xC)
  #include "stm32f1xx_hal.h"
#elif defined(STM32L431xx)
  #include "stm32l4xx_hal.h"
#else
erreur choisir un processeaur
#endif


/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define HWI_DISABLE_INTERRUPTS
#define HWI_ENABLE_INTERRUPTS


#define _WIFI_INIT_BT_PLUS_RAPIDE

#define _WIFI_BT_ARRET_SPA

//#define _TEST_PROJECTEUR_BT

// pour pouvoir utiliser le debug en jtag est uniquement dans ce cas
//#define _SANSCFG
// pour pouvoir utiliser le code sans le boot
//#define _SANSBOOT

//#define _DEBUG_WIFI_BT

//NEW
// Configuration Bluetooth SPP
#ifndef _DATA_BT_CALSSIQUE_SPP
#define _DATA_BT_CALSSIQUE_SPP
#endif

#ifndef _BT_CLASSIQUE  
#define _BT_CLASSIQUE
#endif
//NEW
/*******************************************************************************
*  Feature Macro Definitions
*******************************************************************************/



#endif

