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
#include "app_manage_sd.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_macros.h"
#include "ff.h"
#include "hwi_hal_gpio.h"
#include "hwi_hal_can.h"
#include "S_S_Scheduler.h"
#include "p_l_bx310x.h"
#include "string.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
//flag indiquant qu'une carte SD autorisant le flashage a ete vue
BOOL_TYPE                P_L_CarteSd_Flashage_vue;
//flag indiquant qu'une carte SD autorisant le mode carte en test
BOOL_TYPE                P_L_CarteSd_Atester_vue;
//flag indiquant qu'une carte SD autorisant le mode testeur
BOOL_TYPE                P_L_CarteSd_Testeur_vue;
//flag indiquant qu'une carte SD autorisant la reprogramation de module
BOOL_TYPE               P_L_CarteSd_Programation_vue;
//flag indiquant qu'une carte SD autorise aucun mode particulier
BOOL_TYPE               P_L_CarteSd_No_Mode_vue;


//flag indiquant qu'une carte SD autorisant la mise a jour du module wifi
BOOL_TYPE                P_L_CarteSd_MiseAJour_vue;
U8                       P_L_CarteSd_Clef_wep_MAJ[P_L_WIFI_CLEF_WEP_SIZE+1];
U8                       P_L_CarteSd_ssid_mac_MAJ[P_L_Wifi_SSID_MAX];



/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/




/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
FATFS fs_SD;
FIL fil;
DIR dir;
FRESULT fres;
DWORD fre_clust;
//uint32_t total, free;
//char buffer[100];

static const TCHAR  FLASHAGE_FILE_NAME[] = "Flashage.txt";
static const TCHAR  A_TESTER_FILE_NAME[] = "ATester.txt";
static const TCHAR  TESTEUR_FILE_NAME[] = "Testeur.txt";
static const TCHAR  MISE_A_JOUR_FILE_NAME[] = "MiseAJour.txt";

#ifdef _PROGRAMATION
static const TCHAR PROGRAMATION_DIR[] = "prog";
#endif

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/* NONE */
/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*NONE*/

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_SD_Init(void)
{
FRESULT result;
BOOL_TYPE SSID_OK;
BOOL_TYPE WEP_OK;
TCHAR buffer[300];
U8 size_rx;

    if (TRUE == HWI_Presence_SD_read())
    {
        //##-1- Register the file system object to the FatFs module ##############
        if(f_mount(&fs_SD, "", 0) != FR_OK)
        {
            //##-3- Create a FAT file system (format) on the logical drive #########
            // WARNING: Formatting the uSD card will delete all content on the device
            if (f_mkfs(0,0,FF_MAX_SS,NULL,0) != FR_OK)
            {
                // FatFs Format Error
                _Error_Handler(__FILE__, __LINE__);
            }
        }

        P_L_CarteSd_No_Mode_vue = TRUE;

        // try to open  file to read if file exist
        result = f_open(&fil,FLASHAGE_FILE_NAME,FA_READ);
        if(result != FR_OK)
        {
            P_L_CarteSd_Flashage_vue = FALSE;
        }
        else
        {
            P_L_CarteSd_Flashage_vue = TRUE;
            P_L_CarteSd_No_Mode_vue = FALSE;
        }

        // try to open  file to read if file exist
        result = f_open(&fil,A_TESTER_FILE_NAME,FA_READ);
        if(result != FR_OK)
        {
            P_L_CarteSd_Atester_vue = FALSE;
        }
        else
        {
            P_L_CarteSd_Atester_vue = TRUE;
            P_L_CarteSd_No_Mode_vue = FALSE;
        }


         // try to open  file to read if file exist
        result = f_open(&fil,TESTEUR_FILE_NAME,FA_READ);
        if(result != FR_OK)
        {
            P_L_CarteSd_Testeur_vue = FALSE;
        }
        else
        {
            P_L_CarteSd_Testeur_vue = TRUE;
            P_L_CarteSd_No_Mode_vue = FALSE;
        }

        // try to open  file to read if file exist
        result = f_open(&fil,MISE_A_JOUR_FILE_NAME,FA_READ);
        if(result != FR_OK)
        {
            P_L_CarteSd_MiseAJour_vue = FALSE;
        }
        else
        {
            SSID_OK = FALSE;
            WEP_OK = FALSE;

            while(f_gets(buffer, sizeof(buffer), &fil))
            {
                size_rx = strlen(buffer);

                if (0 == strncmp((char*)"SSID: ",(char*)buffer,6))
                {
                    if ((size_rx-6) < P_L_Wifi_SSID_MAX)
                    {
                        strcpy((char *)P_L_CarteSd_ssid_mac_MAJ,(char *)&buffer[6]);
                        SSID_OK = TRUE;
                    }
                }
                else if (0 == strncmp((char*)"WEP: ",(char*)buffer,5))
                {
                   if ((size_rx-5) < P_L_WIFI_CLEF_WEP_SIZE)
                    {
                        strcpy((char *)P_L_CarteSd_Clef_wep_MAJ,(char *)&buffer[5]);
                        WEP_OK = TRUE;
                    }
                }
            }

            if ((TRUE == SSID_OK)&& (TRUE == WEP_OK))
            {
                P_L_CarteSd_MiseAJour_vue = TRUE;
                P_L_CarteSd_No_Mode_vue = FALSE;
            }
            else
            {
                P_L_CarteSd_MiseAJour_vue = FALSE;
            }
        }
#ifdef _PROGRAMATION
        result = f_opendir(&dir,PROGRAMATION_DIR);
        if(result != FR_OK)
        {
            P_L_CarteSd_Programation_vue = FALSE;
        }
        else
        {
            f_closedir(&dir);
            P_L_CarteSd_Programation_vue = TRUE;
            P_L_CarteSd_No_Mode_vue = FALSE;
        }
#endif

        HWI_Can_FilterConfig();

        // Close file
        f_close(&fil);
        // Unmount SDCARD
        f_mount(NULL, "", 1);
    }
}

/*
        // Open file to write
        if(f_open(&fil,"test.txt",FA_OPEN_ALWAYS | FA_CREATE_NEW | FA_READ | FA_WRITE) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);

        // Check free space
        if(f_getfree("", &fre_clust, &pfs) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);

        total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
        free = (uint32_t)(fre_clust * pfs->csize * 0.5);

        // Free space is less than 1kb
        if(free < 1)
            _Error_Handler(__FILE__, __LINE__);

        // Writing text
        f_puts("STM32 SD Card I/O Example via SPI\n", &fil);
        f_puts("Save the world!!!", &fil);

        // Close file
        if(f_close(&fil) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);

        // Open file to read
        if(f_open(&fil, "first.txt", FA_READ) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);

        while(f_gets(buffer, sizeof(buffer), &fil))
        {
            //printf("%s", buffer);
        }

        // Close file
        if(f_close(&fil) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);

        // Unmount SDCARD
        if(f_mount(NULL, "", 1) != FR_OK)
            _Error_Handler(__FILE__, __LINE__);
    }
*/


