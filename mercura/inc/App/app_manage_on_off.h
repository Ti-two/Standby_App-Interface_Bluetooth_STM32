/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_ON_OFF_H_
#define _APP_MANAGE_ON_OFF_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define APP_TEMPO_AV_ANALYSE                ((U32)1000) //ben le 9/11/2012 ((U32)2000) pour accéléré le demarrage
#define APP_TEMPO_BEFORE_SHUTDOWN           ((U32)3000)
#define APP_TEMPO_BEFORE_SHUTDOWNCAN        ((U32)1000)
#define APP_TEMPO_BEFORE_STOP_WATCHDOG      ((U32)4000)
#define APP_TEMPO_STOP_REQUEST              ((U32)2000)
#define APP_FILTRAGE_ARRET_STOP_RESEAU_CAN  ((U32)1000)
#define APP_TIME_OUT_STOP_REQUEST           ((U32)5000)

/*******************************************************************************
*  definition des differents etat du systeme
*******************************************************************************/
typedef enum
{
APP_RUN,
APP_WAIT_STOP_REQUEST,
APP_STOP_REQUEST,
APP_STOP_WATCHDOG,
APP_WAIT_RESTAST
}APP_STATE_ARRET_TYPE;



/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void APP_Gestion_Alim_Carte(void);
extern void APP_Gestion_Veille_Et_Arret_Systeme(void);
extern void APP_Gestion_Alim_Carte_Init(void);
extern void HWI_Gestion_reveil(void);
extern void HWI_Commande_alim_bloc(BOOL_TYPE cmd);
extern void HWI_Commande_alim_facade(BOOL_TYPE cmd);
extern void HWI_Commande_reveil(BOOL_TYPE cmd);

#ifdef _CAN_BLOC_I2C_DEF2_9
extern void APP_Gestion_demarrage(void);
#endif

#endif

