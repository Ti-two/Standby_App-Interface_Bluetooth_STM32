/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "app_manage_grp_file.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "app_manage_wifi.h"
#include "lib_macros.h"
#include "ff.h"
#include "user_diskio.h"
#include "fatfs.h"
#include "string.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/*NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32      S_S_1MS_U32_TIME;
extern S8       APP_Flashage_version_nb_file;
extern U8       APP_Flashage_version_file_encours;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
FATFS   fs_grp;
U8      APP_Nb_file_grp;
U8      APP_File_encours_grp;

U32     APP_Nb_data_file_grp;
U32     APP_File_data_encours_grp;

FATFS       fs_wifigrp;
FIL         APP_FileWifi;
U32         APP_File_data_encours_wifi;
BOOL_TYPE   APP_FileWifi_Open;
U32         APP_FileWifi_Open_timer;
BOOL_TYPE   APP_FileWifi_Open_lock;

FIL         APP_FileVersion;
DIR         APP_DirectoryVersion;

const TCHAR APP_INFO_FILE_NAME[] = "0:/prog/sys/infoFile.txt";
const TCHAR  APP_DIR_CCM_FILE_NAME[] = "0:/prog/ccm/";
const TCHAR  APP_DIR_GCM_FILE_NAME[] = "0:/prog/gcm/";
const TCHAR  APP_DIR_GPR_FILE_NAME[] = "0:/prog/grp/";
const TCHAR  APP_DIR_GPR_NAME[] = "0:/prog/grp";
const TCHAR  APP_INFO_FLASH_NAME[] = "0:/prog/sys/infoFlas.txt";
const TCHAR  APP_INFO_USER_NAME[] = "0:/prog/sys/infoUser.txt";
const TCHAR APP_INFO_BLOC_NAME[] = "0:/prog/data/infoBloc.cfg";
const TCHAR  APP_PAS_DE_FICHIER_NAME[] = "0:/prog/grp/NA.grp";
const TCHAR APP_VERSION_FILE_NAME[] = "0:/prog/sys/versFile.txt";
/*
static const TCHAR  APP_INFO_TIME_NAME[] = "0:/prog/sys/infoTime.txt";




static const TCHAR  APP_DIR_CCM_FILE_NAME[] = "0:/prog/ccm/";
static const TCHAR  APP_DIR_GCM_FILE_NAME[] = "0:/prog/gcm/";
static const TCHAR  APP_DIR_GPR_FILE_NAME[] = "0:/prog/grp/";
*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static U32 APP_get_nb_line(FIL *file);
static BOOL_TYPE APP_Line02_Decod(U8 *line, U16 *high_adress);
static BOOL_TYPE APP_Line00_decod(U8* line, U16 *adr, U8* Datas, U8 *pNbDatas);

static BOOL_TYPE APP_Analyse_ALL_Line(U32 mfilePosGPR,U32 mfilePosOS,BOOL_TYPE CCM_File,U8 TypeGcm,U32 *Taille_GCM_old_Sans_Logo,U32 *Taille_GCM_new_CCM_Avec_Logo,FIL *fileGPR,FIL *fileOS);
static BOOL_TYPE APP_Generation_1_GPR(U8 *path,U8 *path_gpr,BOOL_TYPE fichierCCM);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
 *
 *  Function Name   : APP_Generation_File_Version
 *  Description     : Creation du fichier des versions
 *
 *
 *******************************************************************************/
APP_FLASHAGE_STATE  APP_Generation_File_Version(void)
{
APP_FLASHAGE_STATE state;
FILINFO     fno;
U8          nb_file_dir;
FIL         fileRead;
TCHAR       Path_GPR_File_read[25];
U8          Buff[LgMaxLigne];
U32         nb_data_write;


    fno.lfname = NULL;

    state = FL_RUNNIG;


    if (APP_Flashage_version_nb_file == (-1))
    {
        if (FR_OK == f_mount(&fs_grp, "", 0))
        {
            if (FR_OK == f_opendir(&APP_DirectoryVersion,APP_DIR_GPR_NAME))
            {
                nb_file_dir = 0;
                //attention j ai modifier le retours de F_readdir pour pouvoir avoir FR_NO_FILE;
                while (FR_OK == f_readdir(&APP_DirectoryVersion,&fno))
                {
                    nb_file_dir++;
                }

                f_closedir(&APP_DirectoryVersion);

                APP_Flashage_version_nb_file = nb_file_dir;
                APP_Flashage_version_file_encours = 0;

                /* faux car sd peut etre vide
                if (0 == APP_Flashage_version_nb_file)
                {
                   state = FL_ERROR;
                }
                */
                //ouverture du code source gcm
                if (FR_OK != f_open(&APP_FileVersion,APP_VERSION_FILE_NAME,FA_WRITE|FA_CREATE_ALWAYS))
                {
                    state = FL_ERROR;
                }
                else
                {
                    f_lseek(&APP_FileVersion,0);
                    f_opendir(&APP_DirectoryVersion,APP_DIR_GPR_NAME);
                }
            }
            else
            {
                state = FL_ERROR;
            }
        }
        else
        {
            state = FL_ERROR;
        }
    }
    else
    {
        if (FR_OK == f_readdir(&APP_DirectoryVersion,&fno))
        {
            strcpy((char *)Path_GPR_File_read,APP_DIR_GPR_FILE_NAME);
            strcpy((char *)&(Path_GPR_File_read[12]),(const char*)fno.fname);

            if (FR_OK == f_open(&fileRead,Path_GPR_File_read,FA_READ))
            {
                f_gets((TCHAR*)Buff,(int)LgMaxLigne,&fileRead);
                f_gets((TCHAR*)Buff,(int)LgMaxLigne,&fileRead);
                f_close(&fileRead);
                APP_Flashage_version_file_encours++;
                f_write(&APP_FileVersion,Buff,(U32)strlen((TCHAR*)Buff),&nb_data_write);
            }
        }
        else
        {
            f_close(&APP_FileVersion);
            f_closedir(&APP_DirectoryVersion);
            state = FL_OK;
        }
    }

    if ((FL_ERROR == state)||(FL_OK == state))
    {
        f_mount(NULL, "", 1);
    }


return state;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Generation_GPR
 *  Description     : cree les fichiers de repogramation
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Generation_GPR(void)
{
U8          Buff[LgMaxLigne];
U8          Buff1[LgMaxLigne];
U8          Buff2[LgMaxLigne];
U32         nb_data_lue;
U32         nb_data_write;

FIL        fileInfo;
U32        fileInfoPos;

BOOL_TYPE   init_a_faire;
BOOL_TYPE   result;
U32         line_en_cours;
FILINFO     fno;
DIR         dp;
U8          nb_file_dir;

    APP_FileWifi_Open = FALSE;

    result = TRUE;
    fno.lfname = NULL;

    if (FR_OK == f_mount(&fs_grp, "", 0))
    {
        fileInfoPos = 0;
        nb_data_lue = 1;
        init_a_faire = TRUE;

        if (FR_OK == f_open(&fileInfo,APP_INFO_FILE_NAME,FA_READ))
        {
            APP_Nb_file_grp = APP_get_nb_line(&fileInfo) - 2;
            f_close(&fileInfo);
        }

        APP_File_encours_grp = 0;

        while (TRUE == init_a_faire)
        {
            //ouverture du code source gcm
            if (FR_OK == f_open(&fileInfo,APP_INFO_FILE_NAME,FA_READ))
            {
                f_lseek(&fileInfo,fileInfoPos);

                APP_Read_String_line(&fileInfo,Buff,LgMaxLigne,&nb_data_lue,&fileInfoPos,&line_en_cours);

                if (nb_data_lue == 0)
                {
                   init_a_faire = FALSE;
                   f_close(&fileInfo);
                }
                else
                {
                    if (nb_data_lue == APP_TAILLE_MAKE_INFO)
                    {
                        if ('0' == Buff[APP_POS_MAKE_INFO])
                        {
                            init_a_faire = TRUE;
                        }
                        else
                        {
                            init_a_faire = FALSE;
                            APP_File_encours_grp = APP_Nb_file_grp;
                            nb_file_dir = 0;

                            if (FR_OK == f_opendir(&dp,APP_DIR_GPR_NAME))
                            {
                                //attention j ai modifier le retours de F_readdir pour pouvoir avoir FR_NO_FILE;
                                while (FR_OK == f_readdir(&dp,&fno))
                                {
                                    nb_file_dir++;
                                }

                                f_closedir(&dp);
/*ben test plus a faire si envouye par apli
                                if (APP_Nb_file_grp > nb_file_dir)
                                {
                                    result = FALSE;
                                }
*/
                            }
                            else
                            {
                                result = FALSE;
                            }

                        }
                        f_close(&fileInfo);
                    }
                    else if (0 == strncmp((char*)"FIN",(char*)Buff,3))
                    {
                        if (TRUE == init_a_faire)
                        {
                            f_close(&fileInfo);
                            f_open(&fileInfo,APP_INFO_FILE_NAME,FA_WRITE);
                            f_lseek(&fileInfo,0);
                            Buff[0] = '1';
                            Buff[1] = 0x0D;
                            Buff[2] = 0x0A;

                            f_write(&fileInfo,Buff,3,&nb_data_write);
                            f_close(&fileInfo);
                            APP_File_encours_grp = APP_Nb_file_grp;
                            init_a_faire = FALSE;
                        }
                    }
                    else if (0 == strncmp((char*)".gcm",(char*)&Buff[nb_data_lue-APP_TAILLE_EXTENSION],APP_TAILLE_EXTENSION))
                    {
                        strcpy((char *)Buff1,APP_DIR_GCM_FILE_NAME);
                        strcpy((char *)Buff2,APP_DIR_GPR_FILE_NAME);
                        strncpy((char *)&Buff1[APP_DIR_SIZE],(char *)Buff,nb_data_lue);
                        strncpy((char *)&Buff2[APP_DIR_SIZE],(char *)Buff,3);
                        strcpy((char *)&Buff2[APP_DIR_SIZE+3],".grp");
                        Buff1[APP_DIR_SIZE+nb_data_lue] = 0x0A;
                        Buff2[APP_DIR_SIZE+3+APP_TAILLE_EXTENSION] = 0x0A;
                        f_close(&fileInfo);
                        APP_File_encours_grp++;

                        if (FALSE == APP_Generation_1_GPR(Buff1,Buff2,FALSE))
                        {
                            nb_data_lue = 0;
                            result = FALSE;
                            init_a_faire = FALSE;
                        }
                    }
                    else if (0 == strncmp((char*)".ccm",(char*)&Buff[nb_data_lue-APP_TAILLE_EXTENSION],APP_TAILLE_EXTENSION))
                    {
                        strcpy((char *)Buff1,APP_DIR_CCM_FILE_NAME);
                        strcpy((char *)Buff2,(char *)APP_DIR_GPR_FILE_NAME);
                        strncpy((char *)&Buff1[APP_DIR_SIZE],(char *)Buff,nb_data_lue);
                        strncpy((char *)&Buff2[APP_DIR_SIZE],(char *)Buff,nb_data_lue-3);
                        strcpy((char *)&Buff2[APP_DIR_SIZE+nb_data_lue-APP_TAILLE_EXTENSION],".grp");
                        Buff1[APP_DIR_SIZE+nb_data_lue] = 0x0A;
                        Buff2[APP_DIR_SIZE+nb_data_lue] = 0x0A;
                        f_close(&fileInfo);
                        APP_File_encours_grp++;

                        if (FALSE == APP_Generation_1_GPR(Buff1,Buff2,TRUE))
                        {
                            nb_data_lue = 0;
                            result = FALSE;
                            init_a_faire = FALSE;
                        }
                    }
                }
            }
            else
            {
                nb_data_lue = 0;
                //error_fileinfo = TRUE;
                init_a_faire = FALSE;
                result = FALSE;
            }
        }

    // Unmount SDCARD
    f_mount(NULL, "", 1);
    }
    return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Generation_1_GPR
 *  Description     : cree un fichier de repogramation
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_Generation_1_GPR(U8 *path,U8 *path_gpr,BOOL_TYPE fichierCCM)
{
FIL         fileOS;
U32         filePosOS;
FIL         fileGPR;
U32         filePosGPR;
U8          Buff[LgMaxLigne];
U32         nb_data_write;
U32         nb_data_lue;
U8          type[4];
U8          cheksum_old[6];
U8          cheksum_new[6];
U8          taille[6];
U32         Taille_GCM_old_CCM_Sans_Logo;
U32         Taille_GCM_new_CCM_Avec_Logo;
U32         filePosGPRMemo;
U32         filePosOSMemo;
U8          TypeGcm;
BOOL_TYPE   result;

    result = TRUE;

    if (FR_OK == f_open(&fileOS,(const TCHAR*)path,FA_READ))
    {

        APP_File_data_encours_grp = 0;
        APP_Nb_data_file_grp = f_size(&fileOS);

        //ouverture deuxieme fichier
        //creation fichier gpr
        if (FR_OK == f_open(&fileGPR,(const TCHAR*)path_gpr,FA_WRITE|FA_CREATE_ALWAYS))
        {

            filePosOS = 0;
            filePosGPR = 0;

            f_lseek(&fileOS,0);
            f_lseek(&fileGPR,0);

            // I00INFORMATIONS SUR LE SW :
            // I01 Nom du SW : CBC20109
            // I02   Version : V01.09
            // I03  Checksum : 3D7E
            // soit:
            //    ->  rien(0 data)
            //    ->  I04  Type GCM : 01
            //        I05  Checksum new : 02A7
            //    ->  I10INFORMATIONS DE COMPILATION :
            // I11 Compilé le : 04/02/2016
            // I12          A : 15:21:09
            // I13        Par : bdelporte
            // I14  C.Version : V2.12A
            // I15  L.Version : V4.56F
            // rien(0 data)
            // si soft bloc->
            //            I20INFORMATION DE TRANSMISSION
            //            I21   Protocole : PRC01.00
            //            I22 Vitesse CAN : 0500000
            //            I23       Id Tx : 0x001
            //            I24     Rx Mask : 0x40
            //            rien(0 data)

            //ecriture du type de fichier CCM ou GCM

            if (FALSE == fichierCCM)
            {
                Buff[0] = 'G';
                Buff[1] = 'C';
                Buff[2] = 'M';
                Buff[3] = 0x0D;
                Buff[4] = 0x0A;
            }
            else
            {
                Buff[0] = 'C';
                Buff[1] = 'C';
                Buff[2] = 'M';
                Buff[3] = 0x0D;
                Buff[4] = 0x0A;
            }
            f_write(&fileGPR,&Buff,5,&nb_data_write);
            filePosGPR = nb_data_write;

            //I00INFORMATIONS SUR LE SW :
            APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

            //I01 Nom du SW :
            APP_Read_Data_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            if (FALSE == fichierCCM)
            {
                //ecriture non du fichier
                f_write(&fileGPR,&Buff[16],nb_data_lue-14,&nb_data_write);
                filePosGPR = filePosGPR+nb_data_write;
            }

            //I02   Version :
            APP_Read_Data_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            if (TRUE == fichierCCM)
            {
                //ecriture non du fichier
                f_write(&fileGPR,&Buff[16],nb_data_lue-14,&nb_data_write);
                filePosGPR = filePosGPR+nb_data_write;
            }
            //I03  Checksum :
            APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

            type[0] = 0x30;
            type[1] = 0x30;
            type[2] = 0x0D;
            type[3] = 0x0A;

            cheksum_old[0] = Buff[16];
            cheksum_old[1] = Buff[17];
            cheksum_old[2] = Buff[18];
            cheksum_old[3] = Buff[19];
            cheksum_old[4] = 0x0D;
            cheksum_old[5] = 0x0A;

            cheksum_new[0] = Buff[16];
            cheksum_new[1] = Buff[17];
            cheksum_new[2] = Buff[18];
            cheksum_new[3] = Buff[19];
            cheksum_new[4] = 0x0D;
            cheksum_new[5] = 0x0A;


            //rien
            //ou I04  Type GCM : 01
            //ou I10INFORMATIONS DE COMPILATION
            APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            if (0 == nb_data_lue)
            {//rien

            }
            else if (18 == nb_data_lue)
            {
                //I04  Type GCM
                TypeGcm = APP_Hex2bin(&Buff[16]);
                type[0] = Buff[16];
                type[1] = Buff[17];

                if (('0' == type[0])&&('1' == type[1]))
                {
                    //I05 Checksum new :
                    APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

                    cheksum_new[0] = Buff[20];
                    cheksum_new[1] = Buff[21];
                    cheksum_new[2] = Buff[22];
                    cheksum_new[3] = Buff[23];
                }

                //espace
                APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            }
            else if (25 == nb_data_lue)
            {//I06  Checksum Logo :
                cheksum_new[0] = Buff[21];
                cheksum_new[1] = Buff[22];
                cheksum_new[2] = Buff[23];
                cheksum_new[3] = Buff[24];
            }
            else
            {//I10INFORMATIONS DE COMPILATION

            }
            //ecriture du type
            f_write(&fileGPR,type,4,&nb_data_write);
            filePosGPR = filePosGPR + nb_data_write;
            //ecriture GCM cheksum old ou CCM sans logo
            f_write(&fileGPR,cheksum_old,6,&nb_data_write);
            filePosGPR = filePosGPR + nb_data_write;
            //ecriture GCM cheksum new ou CCM avec logo
            f_write(&fileGPR,cheksum_new,6,&nb_data_write);
            filePosGPR = filePosGPR + nb_data_write;

            Buff[0] = 0;
            while(('I' != Buff[0])||('1' != Buff[1])||('5' != Buff[2]))
            {
                APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            }
            //rien
            APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

            filePosOSMemo = filePosOS;

            //data ou I20
            APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
            if (('I' == Buff[0])&&('2' == Buff[1])&&('0' == Buff[2]))
            {// I20
                Buff[0] = 0;
                while(('I' != Buff[0])||('2' != Buff[1])||('4' != Buff[2]))
                {
                    APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
                }
                //rien
                APP_Read_String_line(&fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
                filePosOSMemo = filePosOS;
            }
            else
            {
                f_lseek(&fileOS,filePosOSMemo);
            }

            filePosGPRMemo = filePosGPR;

            taille[0] = 0x00;
            taille[1] = 0x00;
            taille[2] = 0x00;
            taille[3] = 0x00;
            taille[4] = 0x0D;
            taille[5] = 0x0A;

            //ecriture taille Gcm old ou CCM Sans Logo
            f_write(&fileGPR,taille,6,&nb_data_write);
            filePosGPR = filePosGPR + nb_data_write;
            //ecriture taille Gcm new ou CCM Avec Logo
            f_write(&fileGPR,taille,6,&nb_data_write);
            filePosGPR = filePosGPR + nb_data_write;

            Taille_GCM_old_CCM_Sans_Logo = 0;
            Taille_GCM_new_CCM_Avec_Logo = 0;
            //analyse data hex
            APP_Analyse_ALL_Line(filePosGPR,filePosOSMemo,fichierCCM,TypeGcm,&Taille_GCM_old_CCM_Sans_Logo,&Taille_GCM_new_CCM_Avec_Logo,&fileGPR,&fileOS);

            //retour a ecriture des tailles
            f_lseek(&fileGPR,filePosGPRMemo);

            //ecriture taille Gcm old ou CCM Sans Logo
            taille[0] = (U8)((Taille_GCM_old_CCM_Sans_Logo >> 24) & 0x000000FF);
            taille[1] = (U8)((Taille_GCM_old_CCM_Sans_Logo >> 16) & 0x000000FF);
            taille[2] = (U8)((Taille_GCM_old_CCM_Sans_Logo >> 8) & 0x000000FF);
            taille[3] = (U8)(Taille_GCM_old_CCM_Sans_Logo & 0x000000FF);
            f_write(&fileGPR,taille,6,&nb_data_write);

            //ecriture taille Gcm new ou CCM Avec Logo
            taille[0] = (U8)((Taille_GCM_new_CCM_Avec_Logo >> 24) & 0x000000FF);
            taille[1] = (U8)((Taille_GCM_new_CCM_Avec_Logo >> 16) & 0x000000FF);
            taille[2] = (U8)((Taille_GCM_new_CCM_Avec_Logo >> 8) & 0x000000FF);
            taille[3] = (U8)(Taille_GCM_new_CCM_Avec_Logo & 0x000000FF);
            f_write(&fileGPR,taille,6,&nb_data_write);

            f_close(&fileGPR);
        }
        else
        {
            result = FALSE;
        }
        f_close(&fileOS);
    }
    else
    {
        result = FALSE;
    }
    return result;
}

/*******************************************************************************
*
* Function Name   : P_L_Analyse_ALL_Line
* Description     : Analyse de tous le fichier a flasher
*
*******************************************************************************/
static BOOL_TYPE APP_Analyse_ALL_Line(U32 mfilePosGPR,U32 mfilePosOS,BOOL_TYPE CCM_File,U8 TypeGcm,U32 *Taille_GCM_old_Sans_Logo,U32 *Taille_GCM_new_CCM_Avec_Logo,FIL *fileGPR,FIL *fileOS)
{
BOOL_TYPE result;
U8 NbfinVue;
U32 MaxAdrSansLogo;
U32 MaxAdrAvecLogo;
BOOL_TYPE first_line_Ok;
U32 filePosOS;
U8 Buff[50];
U32 nb_data_lue;
U32 nb_data_write;


BOOL_TYPE result_decode;
U32 SegmentStartAdr;
BOOL_TYPE ext_adresse_O4_nO2;
U32 ram_adresse;
BOOL_TYPE RamAdresse;
U16 higher_adress;
U32 temp_adr;
U32 temp_low;
U16 lower_adress;
U8 LineDatas[S2_NB_DATAS_MAX];
U8 NbLineDatas;
U32 Adr;
#ifdef _GRP_SHORT
U32 SegmentStartAdr_Memo;
U8 Buff_FF[1];
U32 nb_data_write_FF;
#else
U32 position;
U32 taille_zone;
U32 filePosGPR;
#endif

    MaxAdrSansLogo = 0;
    MaxAdrAvecLogo = 0;
    NbfinVue = 0;
    first_line_Ok = FALSE;
    filePosOS  = mfilePosOS;

    Buff[0] = 0xFF;
    result = TRUE;
#ifdef _GRP_SHORT
    SegmentStartAdr_Memo = 0;
    Buff_FF[0] = 0xFF;

//retour au debut des datas
   f_lseek(fileOS,filePosOS);

   // récupère la ligne
   //P_L_F_read_line(&MyfileOS,Buff,&nb_data_lue,&filePosOS);
    APP_Read_String_line(fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

    //Tpanel
    if (2 == TypeGcm)
    {
        RamAdresse = FALSE;
    }

    do
    {
        if ((':' == Buff[0]) && ('0' == Buff[7]))
        {
            ////////////////////////////////////////////////
            // 04 - EXTENDED LINERA ADDRESS RECORD
            ////////////////////////////////////////////////
            if('4' == Buff[8])
            {
                first_line_Ok = TRUE;
                // extended linear address record
                result_decode = APP_Line02_Decod(Buff, &higher_adress);
                if (FALSE == result_decode)
                {
                    return(FALSE);
                }

                SegmentStartAdr = (U32)(higher_adress << 16);
                ext_adresse_O4_nO2 = TRUE;
                //test si Tpanel
                if (2 == TypeGcm)
                {
                    ram_adresse = P_L_ADRESSE_RAM_T_PANEL;
                }
                else
                {
                    ram_adresse = P_L_ADRESSE_RAM;
                }

                if (ram_adresse <= SegmentStartAdr)
                {
                   RamAdresse = TRUE;
                }
                else
                {
                   RamAdresse = FALSE;
                }
            }
            ////////////////////////////////////////////////
            // 02 - champ d'adresse étendue
            ////////////////////////////////////////////////
            else if ('2' == Buff[8])
            {// L'adresse spécifiée par le champ 02 est multipliée par 16 (décalée de 4 bits vers la gauche) et additionnée aux adresses contenues dans les champs de type 00.

                result_decode = APP_Line02_Decod(Buff, &higher_adress);
                if (FALSE == result_decode)
                {
                   return(FALSE);
                }

                 SegmentStartAdr = (U32)(higher_adress << 4);
                 ext_adresse_O4_nO2 = FALSE;

                //test si tpanel
                if (2 == TypeGcm)
                {
                   ram_adresse = P_L_ADRESSE_RAM_T_PANEL;
                }
                else
                {
                    ram_adresse = P_L_ADRESSE_RAM;
                }

                if (ram_adresse <= SegmentStartAdr)
                {
                  RamAdresse = TRUE;
                }
                else
                {
                   RamAdresse = FALSE;
                }
            }
            ////////////////////////////////////////////////
            // 00 - DATA RECORD
            ////////////////////////////////////////////////
            else if ('0' == Buff[8])
            {
                if (FALSE == first_line_Ok)
                {
                    //test si tpanel
                    if (2 == TypeGcm)
                    {
                      higher_adress =  (U32)(0x00);
                      SegmentStartAdr = higher_adress;
                      first_line_Ok = TRUE;
                      ext_adresse_O4_nO2 = FALSE;
                    }
                    else
                    {
                        return(FALSE);
                    }
                }

                if (FALSE == RamAdresse)
                {
                    // data record
                    // Récupère les octets de code codés en ASCCI en octet et stock le tout dans un buffer tampon.

                    result_decode = APP_Line00_decod(Buff, &lower_adress, LineDatas, &NbLineDatas);
                    if (FALSE == result_decode)
                    {
                        return(FALSE);
                    }

                    if (TRUE == ext_adresse_O4_nO2)
                    {
                        SegmentStartAdr = (U32)(higher_adress << 16) + (U32)(lower_adress);
                    }
                    else
                    {
                        SegmentStartAdr = (U32)(higher_adress << 4) + (U32)(lower_adress);
                    }

                    if (FALSE == CCM_File)
                    {
                    ///////////////////////////////////////////////////////////////////////////////
                    // GCM   Appli
                    ///////////////////////////////////////////////////////////////////////////////

                        temp_low = P_L_ADRESSE_START_GCM_OLD;

                        //Facade XXL new
                        if (1 == TypeGcm)
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_NEW;
                        }
                        //Tpanel
                        else if (2 == TypeGcm)
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_T_PANEL;
                            temp_low = P_L_ADRESSE_START_GCM_T_PANEL;
                        }
                        else
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_OLD;
                        }

                        if((temp_low <= SegmentStartAdr) &&  (SegmentStartAdr + NbLineDatas <= temp_adr))
                        {
                             // do nothing
                             // les adresses sont valide
                        }
                        else
                        {
                             // les adresses sont en dehors de la zone prevue
                             return(FALSE);
                        }

                        //Tpanel
                        if (2 == TypeGcm)
                        {
                            SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_GCM_T_PANEL;
                        }
                        else
                        {
                            SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_GCM_OLD;
                        }

                    }
                    else
                    {
                    ///////////////////////////////////////////////////////////////////////////////
                    // CCM  Config
                    ///////////////////////////////////////////////////////////////////////////////

                        if((P_L_ADRESSE_START_CCM_OLD <= SegmentStartAdr) &&  (SegmentStartAdr + NbLineDatas <= P_L_ADRESSE_END_CCM_LOGO))
                        {
                             // do nothing
                             // les adresses sont valide
                        }
                        else
                        {
                            // les adresses sont en dehors de la zone prevue
                            return(FALSE);
                        }

                        SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_CCM_OLD;
                    }

                    if ( SegmentStartAdr_Memo != SegmentStartAdr)
                    {
                        while (SegmentStartAdr_Memo != SegmentStartAdr)
                        {
                          f_write(fileGPR,Buff_FF,1,&nb_data_write_FF);
                          SegmentStartAdr_Memo = SegmentStartAdr_Memo + nb_data_write_FF;
                        }
                    }

                    //ecriture data Gcm ou CCM
                    f_write(fileGPR,LineDatas,NbLineDatas,&nb_data_write);

                    SegmentStartAdr_Memo = SegmentStartAdr_Memo + nb_data_write;

                    Adr = SegmentStartAdr+NbLineDatas;

                    if (Adr > MaxAdrAvecLogo)
                    {
                        MaxAdrAvecLogo = Adr;

                        if (MaxAdrAvecLogo < (P_L_ADRESSE_END_CCM_OLD-P_L_ADRESSE_START_CCM_OLD+1))
                        {
                            MaxAdrSansLogo =  MaxAdrAvecLogo;
                        }
                    }
                }
                else
                {
                   // do nothind
                }
            }
            ////////////////////////////////////////////////
            // 5
            ////////////////////////////////////////////////
            else if ('5' == Buff[8])
            {
                // do nothing
            }
            ////////////////////////////////////////////////
            // 1 end of file
            ////////////////////////////////////////////////
            else if ('1' == Buff[8])
            {
                // end of file
                if (FALSE == CCM_File)
                {
                ///////////////////////////////////////////////////////////////////////////////
                // GCM   Appli
                ///////////////////////////////////////////////////////////////////////////////
                    if (0 == NbfinVue)
                    {
                        *Taille_GCM_old_Sans_Logo = MaxAdrAvecLogo+1;
                        NbfinVue++;
                        result = TRUE;
                    }
                    else if (1 == NbfinVue)
                    {
                        *Taille_GCM_new_CCM_Avec_Logo = MaxAdrAvecLogo-P_L_OFFSET_PROGRAMATION_GCM+1;
                        NbfinVue++;
                        result = TRUE;
                    }
                    else
                    {
                        NbfinVue++;
                        result = FALSE;
                    }
                }
                else
                {
                ///////////////////////////////////////////////////////////////////////////////
                // CCM  Config
                ///////////////////////////////////////////////////////////////////////////////
                    *Taille_GCM_old_Sans_Logo = MaxAdrSansLogo+1;
                    *Taille_GCM_new_CCM_Avec_Logo = MaxAdrAvecLogo+1;

                    result = TRUE;
                }
            }
        }
        else
        {
            result = FALSE;
        }
      // récupère la ligne
     // P_L_F_read_line(&MyfileOS,Buff,&nb_data_lue,&filePosOS);
      APP_Read_String_line(fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
   }
   while((':' == Buff[0]) && (0 != nb_data_lue));


#else
    filePosGPR = mfilePosGPR;

    if (FALSE == CCM_File)
    {
        //XXL
        if (1 == TypeGcm)
        {
            taille_zone = P_L_TAILLE_ZONE_GCM_NEW;
        }
        //Tpanel
        else if (2 == TypeGcm)
        {
            taille_zone = P_L_TAILLE_ZONE_GCM_T_PANEL;
        }
        else
        {
            taille_zone = P_L_TAILLE_ZONE_GCM_OLD;
        }

        for (position=0;position<taille_zone;position++)
        {
            f_write(fileGPR,Buff,1,&nb_data_write);
        }
    }
    else
    {
      for (position=0;position<P_L_TAILLE_ZONE_CCM;position++)
      {
         f_write(fileGPR,Buff,1,&nb_data_write);
      }
   }

   //retour au debut des datas
   f_lseek(fileOS,filePosOS);

   // récupère la ligne
   //P_L_F_read_line(&MyfileOS,Buff,&nb_data_lue,&filePosOS);
    APP_Read_String_line(fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);

    //Tpanel
    if (2 == TypeGcm)
    {
        RamAdresse = FALSE;
    }

    do
    {
        if ((':' == Buff[0]) && ('0' == Buff[7]))
        {
            ////////////////////////////////////////////////
            // 04 - EXTENDED LINERA ADDRESS RECORD
            ////////////////////////////////////////////////
            if('4' == Buff[8])
            {
                first_line_Ok = TRUE;
                // extended linear address record
                result_decode = APP_Line02_Decod(Buff, &higher_adress);
                if (FALSE == result_decode)
                {
                    return(FALSE);
                }

                SegmentStartAdr = (U32)(higher_adress << 16);
                ext_adresse_O4_nO2 = TRUE;
                //test si Tpanel
                if (2 == TypeGcm)
                {
                    ram_adresse = P_L_ADRESSE_RAM_T_PANEL;
                }
                else
                {
                    ram_adresse = P_L_ADRESSE_RAM;
                }

                if (ram_adresse <= SegmentStartAdr)
                {
                   RamAdresse = TRUE;
                }
                else
                {
                   RamAdresse = FALSE;
                }
            }
            ////////////////////////////////////////////////
            // 02 - champ d'adresse étendue
            ////////////////////////////////////////////////
            else if ('2' == Buff[8])
            {// L'adresse spécifiée par le champ 02 est multipliée par 16 (décalée de 4 bits vers la gauche) et additionnée aux adresses contenues dans les champs de type 00.

                result_decode = APP_Line02_Decod(Buff, &higher_adress);
                if (FALSE == result_decode)
                {
                   return(FALSE);
                }

                 SegmentStartAdr = (U32)(higher_adress << 4);
                 ext_adresse_O4_nO2 = FALSE;

                //test si tpanel
                if (2 == TypeGcm)
                {
                   ram_adresse = P_L_ADRESSE_RAM_T_PANEL;
                }
                else
                {
                    ram_adresse = P_L_ADRESSE_RAM;
                }

                if (ram_adresse <= SegmentStartAdr)
                {
                  RamAdresse = TRUE;
                }
                else
                {
                   RamAdresse = FALSE;
                }
            }
            ////////////////////////////////////////////////
            // 00 - DATA RECORD
            ////////////////////////////////////////////////
            else if ('0' == Buff[8])
            {
                if (FALSE == first_line_Ok)
                {
                    //test si tpanel
                    if (2 == TypeGcm)
                    {
                      higher_adress =  (U32)(0x00);
                      SegmentStartAdr = higher_adress;
                      first_line_Ok = TRUE;
                      ext_adresse_O4_nO2 = FALSE;
                    }
                    else
                    {
                        return(FALSE);
                    }
                }

                if (FALSE == RamAdresse)
                {
                    // data record
                    // Récupère les octets de code codés en ASCCI en octet et stock le tout dans un buffer tampon.

                    result_decode = APP_Line00_decod(Buff, &lower_adress, LineDatas, &NbLineDatas);
                    if (FALSE == result_decode)
                    {
                        return(FALSE);
                    }

                    if (TRUE == ext_adresse_O4_nO2)
                    {
                        SegmentStartAdr = (U32)(higher_adress << 16) + (U32)(lower_adress);
                    }
                    else
                    {
                        SegmentStartAdr = (U32)(higher_adress << 4) + (U32)(lower_adress);
                    }

                    if (FALSE == CCM_File)
                    {
                    ///////////////////////////////////////////////////////////////////////////////
                    // GCM   Appli
                    ///////////////////////////////////////////////////////////////////////////////

                        temp_low = P_L_ADRESSE_START_GCM_OLD;

                        //Facade XXL new
                        if (1 == TypeGcm)
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_NEW;
                        }
                        //Tpanel
                        else if (2 == TypeGcm)
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_T_PANEL;
                            temp_low = P_L_ADRESSE_START_GCM_T_PANEL;
                        }
                        else
                        {
                            temp_adr = P_L_ADRESSE_END_GCM_OLD;
                        }

                        if((temp_low <= SegmentStartAdr) &&  (SegmentStartAdr + NbLineDatas <= temp_adr))
                        {
                             // do nothing
                             // les adresses sont valide
                        }
                        else
                        {
                             // les adresses sont en dehors de la zone prevue
                             return(FALSE);
                        }

                        //Tpanel
                        if (2 == TypeGcm)
                        {
                            SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_GCM_T_PANEL;
                        }
                        else
                        {
                            SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_GCM_OLD;
                        }

                    }
                    else
                    {
                    ///////////////////////////////////////////////////////////////////////////////
                    // CCM  Config
                    ///////////////////////////////////////////////////////////////////////////////

                        if((P_L_ADRESSE_START_CCM_OLD <= SegmentStartAdr) &&  (SegmentStartAdr + NbLineDatas <= P_L_ADRESSE_END_CCM_LOGO))
                        {
                             // do nothing
                             // les adresses sont valide
                        }
                        else
                        {
                            // les adresses sont en dehors de la zone prevue
                            return(FALSE);
                        }

                        SegmentStartAdr = SegmentStartAdr-P_L_ADRESSE_START_CCM_OLD;
                    }

                    //on se deplace sur le debut des data
                    f_lseek(fileGPR,filePosGPR+SegmentStartAdr);

                    //ecriture data Gcm ou CCM
                    f_write(fileGPR,LineDatas,NbLineDatas,&nb_data_write);

                    Adr = SegmentStartAdr+NbLineDatas;

                    if (Adr > MaxAdrAvecLogo)
                    {
                        MaxAdrAvecLogo = Adr;

                        if (MaxAdrAvecLogo < (P_L_ADRESSE_END_CCM_OLD-P_L_ADRESSE_START_CCM_OLD+1))
                        {
                            MaxAdrSansLogo =  MaxAdrAvecLogo;
                        }
                    }
                }
                else
                {
                   // do nothind
                }
            }
            ////////////////////////////////////////////////
            // 5
            ////////////////////////////////////////////////
            else if ('5' == Buff[8])
            {
                // do nothing
            }
            ////////////////////////////////////////////////
            // 1 end of file
            ////////////////////////////////////////////////
            else if ('1' == Buff[8])
            {
                // end of file
                if (FALSE == CCM_File)
                {
                ///////////////////////////////////////////////////////////////////////////////
                // GCM   Appli
                ///////////////////////////////////////////////////////////////////////////////
                    if (0 == NbfinVue)
                    {
                        *Taille_GCM_old_Sans_Logo = MaxAdrAvecLogo+1;
                        NbfinVue++;
                        result = TRUE;
                    }
                    else if (1 == NbfinVue)
                    {
                        *Taille_GCM_new_CCM_Avec_Logo = MaxAdrAvecLogo-P_L_OFFSET_PROGRAMATION_GCM+1;
                        NbfinVue++;
                        result = TRUE;
                    }
                    else
                    {
                        NbfinVue++;
                        result = FALSE;
                    }
                }
                else
                {
                ///////////////////////////////////////////////////////////////////////////////
                // CCM  Config
                ///////////////////////////////////////////////////////////////////////////////
                    *Taille_GCM_old_Sans_Logo = MaxAdrSansLogo+1;
                    *Taille_GCM_new_CCM_Avec_Logo = MaxAdrAvecLogo+1;

                    result = TRUE;
                }
            }
        }
        else
        {
            result = FALSE;
        }
      // récupère la ligne
     // P_L_F_read_line(&MyfileOS,Buff,&nb_data_lue,&filePosOS);
      APP_Read_String_line(fileOS,Buff,LgMaxLigne,&nb_data_lue,&filePosOS,&APP_File_data_encours_grp);
   }
   while((':' == Buff[0]) && (0 != nb_data_lue));
#endif
   return(result);
}



/*******************************************************************************
*
*  Function Name   :  APP_Line00_decod
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : decodage des lignes hex id 00
*******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// void Line00_decod(U8 00Ligne[], U32 *adr, U8 *pDatas, U8 *pNbDatas)
//------------------------------------------------------------------------------
// Décode une ligne -data record-
// Entrée(s):
//  - Chaine de caratère de type -data record-static U32 APP_get_nb_line(FIL *file)
//    :llaaaatt[dd...]cc
//        ll is the record-length field that represents the number of data bytes (dd) in the record.
//        aaaa is the address field that represents the starting address for subsequent data in the record.
//        tt == 00 --> is the field that represents the HEX record type, which may be one of the following:
//        dd is a data field that represents one byte of data. A record may have multiple data bytes. The number of data bytes in the record must match the number specified by the ll field.
//        cc is the checksum field that represents the checksum of the record.
//
// Sortie(s):
//  - l'adresse de départ
//  - les octets de données lues dans la ligne
//  - le nombre d'octets de données dans la ligne
//------------------------------------------------------------------------------
static BOOL_TYPE APP_Line00_decod(U8* line, U16 *adr, U8* Datas, U8 *pNbDatas)
{
U8 NbOctet;
U8 NbDatas;
U8 TableIndex = 0;
U8 adr2,adr1;
U8 Cheksum;
U8 ReadCheksum;

    for(TableIndex = 0; TableIndex<S2_NB_DATAS_MAX; TableIndex++)
    {
        Datas[TableIndex] = 0;
    }

    NbOctet = APP_Hex2bin(&line[NB_OCTET_HEX_MSB]);  // Décodage du nombre d'octets dans la ligne.
    NbDatas = NbOctet;

    adr2 = APP_Hex2bin(&line[HEX_ADR_2_MSB]);     // Décodage de l'adresse (port forts).
    adr1 = APP_Hex2bin(&line[HEX_ADR_1_MSB]);     // Décodage de l'adresse (port moyens).

    Cheksum = NbDatas + adr2 + adr1;

    for(TableIndex=0;TableIndex<NbDatas;TableIndex++)
    {
        Datas[TableIndex] = APP_Hex2bin(&line[2*TableIndex+HEX_FIRST_DATA_MSB]);
        Cheksum = Cheksum + Datas[TableIndex];
    }
    TableIndex++;

    *adr = (U16)(adr2<<8) + (U16)adr1;                                      // Remise en forme de l'adresse.
    *pNbDatas = NbDatas;

    Cheksum = 0xFF - Cheksum + 1 ;
    // verification cheksum

    ReadCheksum = APP_Hex2bin(&line[(2*NbDatas)+9]);
    if (Cheksum ==  ReadCheksum)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}
/*******************************************************************************
*
*  Function Name   :  APP_Line02_Decod
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : decodage des lignes hex id 02
*******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// void Line02Decod(U8 02line[], U32 *adr, U8 *pDatas, U8 *pNbDatas)
//------------------------------------------------------------------------------
// Décode une ligne hex 02 et renvoie
//  - l'adresse etendue de départ
//------------------------------------------------------------------------------
static BOOL_TYPE APP_Line02_Decod(U8* line, U16 *high_adress)
{
U8 adr2,adr1;
U8 boucle;
U8 Cheksum;
U8 ReadCheksum;

    adr2 = APP_Hex2bin(&line[HEX_ADR_4_MSB]);   // Décodage de l'adresse.
    adr1 = APP_Hex2bin(&line[HEX_ADR_3_MSB]);   // Décodage de l'adresse.

    *high_adress = (U16)(adr2<<8) + (U16)adr1;

    ReadCheksum = APP_Hex2bin(&line[HEX_02_CHKSUM_MSB]);
    Cheksum = 0;
    for (boucle=0;boucle<6;boucle++)
    {
        Cheksum = Cheksum + APP_Hex2bin(&line[(2*boucle)+1]);
    }

    Cheksum = 0xFF - Cheksum + 1;

    if (Cheksum ==  ReadCheksum)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/*******************************************************************************
*
*  Function Name  : APP_Hex2bin
*  Description    :
*  Param		  :
*
*******************************************************************************/
U8 APP_Hex2bin(const U8 *Hexa)
{
    return (U8)APP_HexbinX(Hexa,2);
}

/*******************************************************************************
*
*  Function Name  : APP_HexbinX
*  Description    :
*  Param		  :
*
*******************************************************************************/
U32 APP_HexbinX(const U8 *Hexa,U8 size)
{
U32 Result;
U8 boucle;

    Result = 0;
	for(boucle=0;boucle<size;boucle++)
    {
        if (( Hexa[boucle] >= '0' && Hexa[boucle] <= '9') || ( Hexa[boucle] >= 'A' && Hexa[boucle] <= 'F' ))
        {
            // Si le premier caractère est valide
            // Conversion des poids forts
            if(( Hexa[boucle] - '0' ) <= 9 )
            {
                Result = Result*16 + (( Hexa[boucle] - '0' ) & 0x0F);
            }
            else
            {
                Result = Result*16 + ( ( Hexa[boucle] - 'A' + 10 )& 0x0F);
            }
        }
        else
        {
        }
	}

	// Retour du résultat
	return (Result);
}
/*******************************************************************************
 *
 *  Function Name   : APP_Read_String_line
 *  Description     : read one string line
 *
 *
 *******************************************************************************/
void APP_Read_String_line(FIL *file,U8 *buff,U16 taille_max,U32 *nb_data_lue,U32 *filePos,U32 *data_en_cours)
{

    if (f_gets((TCHAR*)buff,(int)taille_max,file))
    {
        *nb_data_lue = (U32)strlen((TCHAR*)buff) - 1;
        *filePos = *filePos + (*nb_data_lue) + 2;
    }
    else
    {
        *nb_data_lue = 0;
    }
    *data_en_cours = (*data_en_cours) + (*nb_data_lue);

/*
U32 boucle;
U32 taille_read;

    if (FR_OK != f_read(file,buff,taille_max,&taille_read))
    {
        *nb_data_lue = 0;
    }

    if (0 != taille_read)
    {
        for (boucle=0;boucle<(taille_read-1);boucle++)
        {
            if ((0x0D == buff[boucle])&&(0x0A == buff[boucle+1]))
            {
                *nb_data_lue = boucle;
                *filePos = *filePos+boucle+2;
                f_lseek(file,*filePos);
                break;
            }
        }
    }
    else
    {
        *nb_data_lue = 0;
    }
    *data_en_cours = (*data_en_cours) + (*nb_data_lue);
*/
}
/*******************************************************************************
 *
 *  Function Name   : APP_Read_Data_line
 *  Description     : read one data line
 *
 *
 *******************************************************************************/
void APP_Read_Data_line(FIL *file,U8 *buff,U16 taille_max,U32 *nb_data_lue,U32 *filePos,U32 *data_en_cours)
{
U32 boucle;
U32 taille_read;

    if (FR_OK != f_read(file,buff,taille_max,&taille_read))
    {
        *nb_data_lue = 0;
    }

    if (0 != taille_read)
    {
        for (boucle=0;boucle<(taille_read-1);boucle++)
        {
            if ((0x0D == buff[boucle])&&(0x0A == buff[boucle+1]))
            {
                *nb_data_lue = boucle;
                *filePos = *filePos+boucle+2;
                f_lseek(file,*filePos);
                break;
            }
        }
    }
    else
    {
        *nb_data_lue = 0;
    }
    *data_en_cours = (*data_en_cours) + (*nb_data_lue);
}
/*******************************************************************************
 *
 *  Function Name   : APP_get_nb_line
 *  Description     : read one line
 *
 *
 *******************************************************************************/
static U32 APP_get_nb_line(FIL *file)
{
U32 nb_line;
TCHAR buff[255];

    nb_line = 0;
    f_lseek(file,0);

    while (f_gets(buff,sizeof(buff),file))
    {
        nb_line++;
    }

    f_lseek(file,0);

return nb_line;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Open_GPR
 *  Description     : ouvre ou cree un fichier demande via wifi
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Open_WIFI_File(U8 *path_file,U8 mode)
{
BOOL_TYPE result;
S8 boucle;
U8 taille;
U8 FileName[200];
FRESULT res;

    result = FALSE;
    res = FR_OK;

    if (FR_OK == f_mount(&fs_wifigrp, "", 0))
    {
        if (FA_WRITE == mode)
        {
            //ouverture deuxieme fichier
            //creation fichier gpr
            if (FR_OK == f_open(&APP_FileWifi,(const TCHAR*)path_file,FA_WRITE|FA_CREATE_ALWAYS))
            {
                APP_FileWifi_Open = TRUE;
                APP_File_data_encours_wifi = 0;
                result = TRUE;
            }
            else
            {
                taille = (U8) (strlen((const char *)path_file));

                for(boucle=taille;boucle>0;boucle--)
                {
                    if (path_file[boucle] == '\\')
                    {
                        taille = boucle+1;
                        boucle = -1;
                    }
                }

                for(boucle=1;boucle<taille;boucle++)
                {
                    if (path_file[boucle] == '\\')
                    {
                        strncpy((char *)FileName,(const char *)path_file,boucle);
                        FileName[boucle] = '\n';

                        if (res == f_mkdir((const char *)FileName))
                        {
                            if ((res != FR_EXIST)&&(res != FR_OK))
                            {
                                boucle = taille;
                            }
                        }
                    }
                }

                if (FR_OK == f_open(&APP_FileWifi,(const TCHAR*)path_file,FA_WRITE|FA_CREATE_ALWAYS))
                {
                    APP_FileWifi_Open = TRUE;
                    APP_File_data_encours_wifi = 0;
                    result = TRUE;
                }
            }
        }
        else
        {
            if (FR_OK == f_open(&APP_FileWifi,(const TCHAR*)path_file,FA_READ))
            {
                APP_FileWifi_Open = TRUE;
                APP_File_data_encours_wifi = 0;
                result = TRUE;
            }
        }
    }

    if (TRUE == APP_FileWifi_Open)
    {
        BT_TIMER_INIT_LOCK(APP_FileWifi_Open_timer,S_S_1MS_U32_TIME,APP_FileWifi_Open_lock);
    }
return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Read_WIFI_File
 *  Description     : ecrit un fichier via le wifi
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Read_WIFI_File(U8 offset ,U8 *data,U8 *data_size)
{
BOOL_TYPE result;
U32 nb_data_lue;

    result = FALSE;
    if (TRUE == APP_FileWifi_Open)
    {
        //retour a donne en cours a lire
        f_lseek(&APP_FileWifi,(offset*P_L_CAN_TAILLE_1_ENVOIE_DATA));

        //lecture des donnees
        if (FR_OK == f_read(&APP_FileWifi,data,P_L_CAN_TAILLE_1_ENVOIE_DATA,&nb_data_lue))
        {
            *data_size = (U8)nb_data_lue;
            result = TRUE;
        }
    }
return result;
}
/*******************************************************************************
 *
 *  Function Name   : APP_Write_GPR
 *  Description     : ecrit un fichier via le wifi
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Write_WIFI_File(U8 *data,U32 data_size)
{
BOOL_TYPE result;
U32 nb_data_write;

    result = FALSE;
    if (TRUE == APP_FileWifi_Open)
    {
        if (FR_OK == f_write(&APP_FileWifi,data,data_size,&nb_data_write))
        {
            if (data_size == nb_data_write)
            {
                result = TRUE;
            }
        }
    }
return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Close_WIFI_File
 *  Description     : ferme un fichier ouvert via le wifi
 *
 *
 *******************************************************************************/
BOOL_TYPE APP_Close_WIFI_File(void)
{
BOOL_TYPE result;

    result = FALSE;
    if (TRUE == APP_FileWifi_Open)
    {
        if (FR_OK == f_close(&APP_FileWifi))
        {
            f_mount(NULL, "", 1);
            result = TRUE;
        }
    }
    else
    {
        result = TRUE;
    }

return result;
}

