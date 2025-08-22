/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _P_L_RPG_
#define _P_L_RPG_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/

#define P_L_FIRST_RAM_PART_ADR       0x400   //
#define P_L_MAX_DATA_RAM_INIT        2
#define P_L_MAX_DATA_RAM_COM         4

#define MAX_ZONE_ECRITURE            32
#define MAX_NB_DATA_ECRITURE         32

//////////////////////////////////////////////

#define P_L_INT_RAM_BASE_ADDRESS      0x2000    //

// Offest de d�calage pour r�cup�rer l'adresse deb de fonction dans la table associ�e.
#define P_L_ERASEBLOCK_OFFSET	    0
#define P_L_PROGRAM_OFFSET	       1

// Test status
#define P_L_RPG_TEST_PASS             0x01
#define P_L_RPG_TEST_FAIL             0x02
#define P_L_PAGE_SIZE                 0x800 // 0x400 pour STM103


#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void P_L_Rpg_Init(void);
extern void P_L_Rpg_Saut_Boot(void);
extern void P_L_Rpg_Calc_Checksum(void);
extern U8 P_L_Rpg_Param_Flash(U32 flash_adresse,U8* data,U16 data_size);
#endif
