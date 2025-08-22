
/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/


/*******************************************************************************
*  Library Include Files
*******************************************************************************/
#include "hwi.h"
#include "p_l.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "p_l_rpg.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
/* NONE*/

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/

const U8 P_L_MASK_READ[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const U8 P_L_MASK_SET[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const U8 P_L_MASK_RESET[8] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
const U32 P_L_MASK_READ32[32] = {0x00000001,0x00000002,0x00000004,0x00000008,
                                 0x00000010,0x00000020,0x00000040,0x00000080,
                                 0x00000100,0x00000200,0x00000400,0x00000800,
                                 0x00001000,0x00002000,0x00004000,0x00008000,
                                 0x00010000,0x00020000,0x00040000,0x00080000,
                                 0x00100000,0x00200000,0x00400000,0x00800000,
                                 0x01000000,0x02000000,0x04000000,0x08000000,
                                 0x10000000,0x20000000,0x40000000,0x80000000};


/*******************************************************************************
*  Application Level Variables: Outputs to the Application
*******************************************************************************/
U8 __attribute__((section(".Ram_Com.P_L_Rpg_boot_reserved")))   APP_Rpg_Flag_dem_a_froid[P_L_MAX_DATA_RAM_INIT]    __attribute__ ((aligned (1))); // ne pas utiliser
U8 __attribute__((section(".Ram_Com.P_L_Rpg_boot_demande")))    P_L_Rpg_boot_demande[P_L_MAX_DATA_RAM_COM]      __attribute__ ((aligned (1)));
U8 __attribute__((section(".Ram_Com.P_L_Ram_bloc_adr_low")))    P_L_Ram_bloc_adr_low                            __attribute__ ((aligned (1)));   //Adresse physique du bloc.
U8 __attribute__((section(".Ram_Com.APP_Boot_Can_Speed")))      APP_Boot_Can_Speed                              __attribute__ ((aligned (1))); //Reservé! sourtout ne pas utiliser!!!
U8 __attribute__((section(".Ram_Com.P_L_Ram_bloc_adr_high")))   P_L_Ram_bloc_adr_high                           __attribute__ ((aligned (1)));   //Adresse physique du bloc.
U8 __attribute__((section(".Ram_Com.P_L_Ram_bloc_adr_low2")))   P_L_Ram_bloc_adr_low2                           __attribute__ ((aligned (1)));   //Adresse physique du bloc.
U8 __attribute__((section(".Ram_Com.P_L_Ram_bloc_adr_high2")))  P_L_Ram_bloc_adr_high2                          __attribute__ ((aligned (1)));   //Adresse physique du bloc.
U8 __attribute__((section(".Ram_Com.P_L_Rpg_boot_reserved2")))  APP_Rpg_Flag_dem_a_froid2[P_L_MAX_DATA_RAM_INIT] __attribute__ ((aligned (1))); // ne pas utiliser

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*NONE*/

