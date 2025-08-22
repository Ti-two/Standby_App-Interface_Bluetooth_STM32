/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_FLASHAGE_H_
#define _APP_MANAGE_FLASHAGE_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"


/*******************************************************************************
*  Application Definitions
*******************************************************************************/

#define APP_TAILLE_INFO_USER                ((U8)12)
#define APP_TAILLE_INFO_SOCIETE             ((U8)10)

#define APP_NB_MAX_FLASHAGE_MODULE          ((U8)30)
#define ADR_NU                              ((U32)0xFFFFFFFF)
#define APP_FILTRAGE_PRTE_MODULE_CAN_CAN    ((U32)1000)

#define APP_TIMEOUT_OUVERTURE_FICHIER_WIFI  ((U32)10*60*1000)

#define APP_Wifi_CAN_TRAME_MAX              ((U8)8)
#define APP_Wifi_CAN_TAILLE_TRAME           ((U8)13)
#define APP_Wifi_CAN_NB_BUFFER              ((U8)3)
#define APP_Wifi_TAILLE_DONNE_CAN           ((U8)4+2+(APP_Wifi_CAN_TRAME_MAX*APP_Wifi_CAN_TAILLE_TRAME))

typedef struct {
    U32 adresse;
    U32 timer;
    BOOL_TYPE  lock;
} APP_MODULE_VUE_TYPE;

typedef enum
{
    OS_CFG = 0x00,
    OS_SEUL = 0x01,
    CFG_SEUL = 0x02
}APP_FLASHAGE_OSCFG_TYPE;

/** States of the internal state machine. */
typedef enum
{
    FLASHAGE_INIT_FILE,
    FLASHAGE_WAIT_EVENT,
    FLASHAGE_ERROR_FILE,
    FLASHAGE_CHANGE_ADR,
    FLASHAGE_CHANGE_ERROR,
    FLASHAGE_CHANGE_OK,
    FLASHAGE_REPROG_ALL,
    FLASHAGE_REPROG_ALL_ERROR,
    FLASHAGE_REPROG_ALL_OK,
    FLASHAGE_ERROR,
    FLASHAGE_HISTORIQUE,
    FLASHAGE_HISTORIQUE_ERROR,
    FLASHAGE_HISTORIQUE_OK,
    FLASHAGE_DIAGNOSTIQUE,
    FLASHAGE_VERSION_FILE,
    FLASHAGE_VERSION_FILE_OK,
    FLASHAGE_VERSION_FILE_ERROR,
}APP_FLASHAGE_STEP;

typedef enum
{
    RGP_ALL_INIT = 0,
    RGP_ALL_CHECK = 1,
    RGP_INFO_CHECK = 2,
    RGP_ALL_OS = 3,
    RGP_ALL_OS_WAIT = 4,
    RGP_ALL_CFG = 5,
    RGP_ALL_CFG_WAIT = 6,
    RGP_ALL_ERROR_FILE = 7,
    RGP_ALL_ERROR_OS = 8,
    RGP_ALL_ERROR_CFG = 9,
    RGP_ALL_END_OK = 10
}APP_RPG_ALL_STEP_TYPE;

typedef enum
{
    FL_ERROR = 0,
    FL_RUNNIG = 1,
    FL_OK=2
}APP_FLASHAGE_STATE;


typedef struct {
        U16 Year;  /* 1..4095 */
        U8  Month; /* 1..12 */
        U8  Date;  /* 1.. 31 */
        U8  Hours;  /* 0..23 */
        U8  Minutes;   /* 0..59 */
        U8  Seconds;   /* 0..59 */
} APP_FLASHAGE_TIME_TYPE;

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void APP_Manage_Flashage(void);
extern void APP_Manage_1ms_Flashage(void);
extern void APP_Manage_Add_Module_Vue(U32 adresse);
extern void APP_Manage_Add_Module_Check(void);
extern void APP_Manage_Flashage_Init(void);

#endif

