/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _APP_MANAGE_GRP_FILE_H
#define _APP_MANAGE_GRP_FILE_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#include "ff.h"
#include "app_manage_flashage.h"

#define LgMaxLigne                  50

#define APP_TAILLE_MAKE_INFO        ((U8)1)
#define APP_POS_MAKE_INFO           ((U8)0)
#define APP_TAILLE_EXTENSION        ((U8)4)
#define APP_DIR_SIZE                ((U8)12)
#define S2_NB_DATAS_MAX             LgMaxLigne
#define HEX_NB_DATAS_MAX            LgMaxLigne

#define APP_EXTENSION_SIZE          ((U8)3)
#define APP_NAME_FACADE_CCM_SIZE    ((U8)11)
#define APP_CHEMIN_SIZE             APP_DIR_SIZE+APP_NAME_FACADE_CCM_SIZE+2

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define APP_PAS_DE_MODULE                  (U32) 0xFFFFFFFF

#define HWI_ADR_FAV_GRAPH_DISPLAY_BASE_WR		(U16)0x90
#define HWI_ADR_FAV_TPANEL_DISPLAY_BASE_WR		(U16)0x190

#define P_L_ADRESSE_START_GCM      (U32) 0x08002000
#define P_L_ADRESSE_END_GCM        (U32) 0x0801A7FC


#define P_L_ADRESSE_START_GCM_OLD      (U32) 0x08002000
#define P_L_ADRESSE_END_GCM_OLD        (U32) 0x0801A7FC

#define P_L_ADRESSE_START_GCM_NEW      (U32) 0x08020000
#define P_L_ADRESSE_END_GCM_NEW        (U32) 0x081FFFFF

#define P_L_OFFSET_PROGRAMATION_GCM     (U32)(P_L_ADRESSE_START_GCM_NEW-P_L_ADRESSE_START_GCM_OLD)

#define P_L_ADRESSE_START_GCM_T_PANEL  (U32) 0x00003000
#define P_L_ADRESSE_END_GCM_T_PANEL    (U32) 0x00080000
#define P_L_TAILLE_ZONE_GCM_T_PANEL    (U32)(P_L_ADRESSE_END_GCM_T_PANEL-P_L_ADRESSE_START_GCM_T_PANEL)

#define P_L_ADRESSE_START_CCM_OLD      (U32) 0x0801A800
#define P_L_ADRESSE_END_CCM_OLD        (U32) 0x0801F7FE


#define P_L_ADRESSE_START_CCM_NEW      (U32) 0x0800C000
#define P_L_ADRESSE_END_CCM_NEW        (U32) 0x0801FFFF

#define P_L_ADRESSE_START_CCM_ZONE_1_OLD  (U32) 0x0801A800
#define P_L_ADRESSE_START_CCM_ZONE_2_OLD  (U32) 0x0801C800

#define P_L_ADRESSE_START_CCM_ZONE_NEW_1  (U32) 0x0800C000
#define P_L_ADRESSE_START_CCM_ZONE_NEW_2  (U32) 0x08010000

#define P_L_ADRESSE_START_CCM_LOGO       (U32) 0x08020000
#define P_L_ADRESSE_END_CCM_LOGO         (U32) 0x0802FFFF

#define P_L_TAILLE_ZONE_CCM              (U32)(P_L_ADRESSE_END_CCM_LOGO-P_L_ADRESSE_START_CCM_NEW)
#define P_L_TAILLE_ZONE_GCM_OLD          (U32)(P_L_ADRESSE_END_GCM_OLD-P_L_ADRESSE_START_GCM_OLD)
#define P_L_TAILLE_ZONE_GCM_NEW          (U32)(P_L_ADRESSE_END_GCM_NEW-P_L_ADRESSE_START_GCM_NEW)

#define P_L_OFFSET_PROGRAMATION_ZONE_1   (U32)(P_L_ADRESSE_START_CCM_ZONE_1_OLD-P_L_ADRESSE_START_CCM_ZONE_NEW_1)
#define P_L_OFFSET_PROGRAMATION_ZONE_2   (U32)(P_L_ADRESSE_START_CCM_ZONE_2_OLD-P_L_ADRESSE_START_CCM_ZONE_NEW_2)

#define P_L_TAILLE_SEGMENT_T_PANEL       512

#define P_L_TAILLE_BUFFER_SEGMENT       (40*P_L_TAILLE_SEGMENT_T_PANEL)

#define P_L_ADRESSE_RAM           (U32) 0x20000000
#define P_L_ADRESSE_RAM_T_PANEL   (U32) 0x10000000


  /************************************************************
  * Définition des positions des octets et demi octets dans la trame 02
  ************************************************************/
  #define HEX_ADR_4_MSB          9     //  Position du MSB de l'octet de poids fort de l'adresse
  #define HEX_ADR_4_LSB          10     //  Position du LSB de l'octet de poids fort de l'adresse
  #define HEX_ADR_3_MSB          11     //  Position du MSB de l'octet de poids moyen de l'adresse
  #define HEX_ADR_3_LSB          12     //  Position du LSB de l'octet de poids moyen de l'adresse
  #define HEX_ADR_2_MSB          3     //  Position du MSB de l'octet de poids moyen de l'adresse
  #define HEX_ADR_2_LSB          4     //  Position du LSB de l'octet de poids moyen de l'adresse
  #define HEX_ADR_1_MSB          5    //  Position du MSB de l'octet de poids moyen de l'adresse
  #define HEX_ADR_1_LSB          6    //  Position du LSB de l'octet de poids moyen de l'adresse

  #define NB_OCTET_HEX_MSB       1
  #define NB_OCTET_HEX_LSB       2

  #define HEX_FIRST_DATA_MSB     9
  #define HEX_FIRST_DATA_LSB     10

  #define HEX_02_CHKSUM_MSB      13
  #define HEX_02_CHKSUM_LSB      14

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern BOOL_TYPE APP_Generation_GPR(void);
extern void APP_Read_String_line(FIL *file,U8 *buff,U16 taille_max,U32 *nb_data_lue,U32 *filePos,U32 *data_en_cours);
extern void APP_Read_Data_line(FIL *file,U8 *buff,U16 taille_max,U32 *nb_data_lue,U32 *filePos,U32 *data_en_cours);
extern U8 APP_Hex2bin(const U8 *Hexa);
extern U32 APP_HexbinX(const U8 *Hexa,U8 size);
extern BOOL_TYPE APP_Open_WIFI_File(U8 *path_file,U8 mode);
extern BOOL_TYPE APP_Write_WIFI_File(U8 *data,U32 data_size);
extern BOOL_TYPE APP_Read_WIFI_File(U8 offset ,U8 *data,U8 *data_size) ;
extern BOOL_TYPE APP_Close_WIFI_File(void);
extern APP_FLASHAGE_STATE  APP_Generation_File_Version(void);
#endif

