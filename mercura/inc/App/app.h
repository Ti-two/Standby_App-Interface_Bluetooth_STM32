/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_
#define _APP_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define APP_SW_NAME_SIZE                4
#define APP_HW_NAME_SIZE                8
#define APP_SW_VERSION_TEXT_SIZE        5
#define APP_SW_VERSION_TEXT_CAN_SIZE    1

#define APP_PROM_ID_SIZE_CPV            50
#define APP_HARDWARE_VERSION_SIZE_CPV   8
#define APP_USER_NAME_DATA_SIZE_CPV     12
#define APP_USER_NAME_CODE_SIZE_CPV     12
#define APP_SW_VERSION_DATA_CPV         3
#define APP_SW_VERSION_SIZE_CPV         3


#define APP_CAN_CCP_FITTED_CPV         TRUE
#define APP_SECURITY_ACCESS_CPV        TRUE

#define APP_MANAGE_MAX_CASE_DEFIL1         11
#define APP_MANAGE_MAX_CASE_DEFIL2         16
#define APP_MANAGE_MAX_CASE_DEFIL3         16
#define APP_MANAGE_MAX_CASE_DEFIL4         16
#define APP_MANAGE_MAX_CASE_DEFIL5         16
#define APP_MANAGE_MAX_CASE_DEFIL6         16
#define APP_MANAGE_MAX_CASE_DEFIL7         16
#define APP_MANAGE_MAX_CASE_DEFIL8         16
#define APP_MANAGE_MAX_CASE_DEFIL9         16
#define APP_MANAGE_MAX_CASE_DEFIL10        16


#define APP_MODE_TEST_MAX_BIP 7

typedef enum APP_MODE_SPEAK_ADRESS_TYPE_
{
	APP_BIP_LONG,
	APP_BIP_SHORT,
	APP_NO_BIP
}
APP_MODE_SPEAK_ADRESS_TYPE;

typedef enum ATC_UNLOCK_SESSION_TYPE_
{
	ATC_RESULT_OK = 0x01,
	ATC_RESULT_NOK = 0x02,
	ATC_ANTI_SCANNING = 0x03
}
ATC_UNLOCK_SESSION_TYPE;

typedef enum APP_MANAGE_LED_TYPE_
{
    APP_MANAGE_ON = 0x01,
    APP_MANAGE_OFF,
    APP_MANAGE_BACKLIGHT,
    APP_MANAGE_RAMPE,
    APP_MANAGE_DEFIL1,
    APP_MANAGE_DEFIL2,
    APP_MANAGE_DEFIL3,
    APP_MANAGE_DEFIL4,
    APP_MANAGE_DEFIL5,
    APP_MANAGE_DEFIL6,
    APP_MANAGE_DEFIL7,
    APP_MANAGE_DEFIL8,
    APP_MANAGE_DEFIL9,
    APP_MANAGE_DEFIL10,
    APP_MANAGE_NOT_DEFINED
}
APP_MANAGE_LED_TYPE;


//-----------------------------------------------------------------


#define APP_VAL_PAS_LED_OFF             ((U16)341)
#define APP_PAS_LED_OFF                 ((U8)0)
#define APP_VAL_PAS_LED_1               ((U16)682)
#define APP_PAS_LED_1                   ((U8)1)
#define APP_VAL_PAS_LED_2               ((U16)1022)
#define APP_PAS_LED_2                   ((U8)2)
#define APP_VAL_PAS_LED_3               ((U16)1364)
#define APP_PAS_LED_3                   ((U8)3)
#define APP_VAL_PAS_LED_4               ((U16)1705)
#define APP_PAS_LED_4                   ((U8)4)
#define APP_VAL_PAS_LED_5               ((U16)2046)
#define APP_PAS_LED_5                   ((U8)5)
#define APP_VAL_PAS_LED_6               ((U16)2387)
#define APP_PAS_LED_6                   ((U8)6)
#define APP_VAL_PAS_LED_7               ((U16)2728)
#define APP_PAS_LED_7                   ((U8)7)
#define APP_VAL_PAS_LED_8               ((U16)3069)
#define APP_PAS_LED_8                   ((U8)8)
#define APP_VAL_PAS_LED_9               ((U16)3410)
#define APP_PAS_LED_9                   ((U8)9)
#define APP_VAL_PAS_LED_10              ((U16)3751)
#define APP_PAS_LED_10                  ((U8)10)
#define APP_VAL_PAS_LED_11              ((U16)4092)
#define APP_PAS_LED_11                  ((U8)11)
/*
#define APP_SEUIL_AFFICHE_E2POT         ((U8)20)

#define APP_TEMPS_AFFICH_E2POT          ((U32)3000)
*/

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void APP_Set_Backlight_Panel_Led(void);
extern void APP_Set_Rampe_Panel_Led(void);
extern void APP_Set_Rampe_Led(void);
extern void APP_Reset_Backlight_Panel_Led(void);
extern void APP_Set_Reset_Panel_Led(void);
extern void APP_Reset_Rampe_Panel_Led(void);
extern void APP_Check_Led_Bp_State(void);
extern void APP_Manage_e2pot_Led(void);
#endif

