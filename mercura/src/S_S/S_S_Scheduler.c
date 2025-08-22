/******************** (C) COPYRIGHT 2008 SYNERGIE ********************
* File Name          : main.c
* Author             : Synergie software dept.
* Version            : V1.0
* Date               : 10/01/2008
* Description        : Project Scheduler
*******************************************************************************/

/********************************************************************/
/*   Include Files                                                  */
/********************************************************************/
#include "hwi.h"
#include "S_S_Scheduler.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_hal_system_clock.h"
#include "hwi_hal_timer.h"
#include "hwi_hal_spi.h"
#include "hwi_watchdog.h"
#include "p_l_rpg.h"
#include "app_adresse.h"
#include "hwi_hal_can.h"
#include "hwi_hal_gpio.h"
#include "p_l_rpg.h"
#include "p_l_button.h"
#include "p_l_check_io.h"
#include "app_manage_on_off.h"
#include "app_manage_wifi.h"
#include "app_manage_sd.h"
#include "p_l_bx310x.h"
#include "p_l_rx_can1.h"
#include "p_l_tx_can1.h"

#ifdef _PROGRAMATION
#include "app_manage_flashage.h"
#endif

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
#include "hwi_hal_i2s.h"
#endif
#endif

#include "p_l_data_flash.h"
#include "app_manage_BT.h"
#include "p_l_data_flash.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern BOOL_TYPE P_L_Rpg_applicatif_mode;

//flag indiquant qu'une carte SD autorisant le mode testeur
extern BOOL_TYPE P_L_CarteSd_Testeur_vue;
//flag indiquant qu'une carte SD autorisant le mode carte en test
extern BOOL_TYPE P_L_CarteSd_Atester_vue;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
U32     	S_S_1MS_U32_TIME;
BOOL_TYPE 	S_S_Led_flip;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static void S_S_Scheduler_Init(void);
static void S_S_Scheduler(void);
static void S_S_Background(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
*
*  Function Name   	: S_S_Scheduler_Start
*  Description     	: initialisation des differentes taches de l'applicatif
*	Param			:
*
*******************************************************************************/
void S_S_Scheduler_Start(void)
{
	HAL_FLASH_Lock();

    //a laisser en 1er
    HWI_SystemClock_Config();

    HWI_Hal_Gpio_Init();

    APP_Adresse_Node_Init();

    P_L_Data_Flash_Init();
    APP_Manage_BT_Init();

#ifndef _UART_ONLY
    HWI_Hal_Can_Init(HWI_CBR500000);

    P_L_Can_Rx_Data_Init();
    P_L_Can_Send_Data_Can_Init();

    P_L_Button_Init();

    P_L_Check_Input_Init();
#endif

    APP_Gestion_Alim_Carte_Init();

    APP_Manage_Wifi_Init();

#ifndef _UART_ONLY
    Hwi_Hal_Spi_Init();
    HWI_Timer_Sd_Delay_Init();
    APP_Manage_SD_Init();

#ifdef _PROGRAMATION
    APP_Manage_Flashage_Init();
#endif

#endif

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
    HWI_I2S_Init();
    HWI_Timer_PWM_Audio_Init();
#endif
#endif

#ifndef _UART_ONLY
    P_L_Rpg_Init();
#endif

    P_L_Bx310x_Init(P_L_Rpg_applicatif_mode);

    S_S_Scheduler_Init();

#ifdef _PROGRAMATION
    HWI_Timer_1MS_Reprog_init();
#endif

    HWI_Start_watchdog();

    HWI_Timer_1ms_Init();
}


/*******************************************************************************
*
*  Function Name   	: main
*  Description     	: debut du code
*	Param					:
*
*******************************************************************************/
void S_S_Scheduler_Callback(void)
{
    S_S_1MS_U32_TIME = S_S_1MS_U32_TIME + 1;
    HWI_Reload_watchdog();
    S_S_Scheduler();
}


/*******************************************************************************
*
*  Function Name   	: s_s_sheduler
*  Description     	: Ordonanceur
*	Param					:
*
*******************************************************************************/
static void S_S_Scheduler(void)
{
    HWI_Reload_watchdog();

#ifdef _PROGRAMATION
    APP_Manage_Flashage();
#endif

    if(TRUE == P_L_Rpg_applicatif_mode)
    {

    	////////////////////////////////////////////////
    	//              2 ms
    	///////////////////////////////////////////////
#ifdef _WIFI_INIT_BT_PLUS_RAPIDE
    	switch(S_S_1MS_U32_TIME % 2)               // tt les 8ms
    	{
    		case 1:
    			P_L_Bx310x_Manage_State();
    		break;
    	}
#endif

#ifndef _UART_ONLY
////////////////////////////////////////////////
//              8 ms
///////////////////////////////////////////////
		switch(S_S_1MS_U32_TIME % 8)               // tt les 8ms
		{
/*
			case 2:
			 P_L_Can_Send_Debug();
			break;
*/
			case 3:
				APP_Gestion_Wifi_TestSend();
				APP_Gestion_Wifi_Send();
			break;

			case 5:
			   P_L_Check_Input();
			break;

			case 7 :
				APP_BT_Manage_Send_Data();
			break;


			default:
			break;
		}

#endif
////////////////////////////////////////////////
//              32 ms
///////////////////////////////////////////////
		switch(S_S_1MS_U32_TIME % 32)               // tt les 32ms
		{
			case 15:
				APP_Gestion_Alim_Carte();
			break;

#ifndef _UART_ONLY
			case 20:
				P_L_Button_Manage();
			break;

			case 30:
				P_L_Can_Send_Data_Can();
			break;


#endif
			case 31:
				  //APP_Gestion_Wifi_Send();
			break;


			default:
			break;
		}

////////////////////////////////////////////////
//              128 ms
///////////////////////////////////////////////
#ifndef _WIFI_INIT_BT_PLUS_RAPIDE
		switch(S_S_1MS_U32_TIME % 128)
		{
			case 121:
				P_L_Bx310x_Manage_State();
			break;

			default:
			break;

		}
#endif

#ifndef _UART_ONLY
///////////////////////////////////////////////
//              512 ms
///////////////////////////////////////////////
		switch(S_S_1MS_U32_TIME % 512)
		{

			case 150:
				P_L_Data_Flash_Wr();
			break;

			case 300:
				if(TRUE == P_L_CarteSd_Testeur_vue)
				{
					HWI_Led_1_Rouge_Cmd(S_S_Led_flip);
					HWI_Led_2_Rouge_Cmd(S_S_Led_flip);
					HWI_Led_1_Verte_Cmd(!S_S_Led_flip);
					HWI_Led_2_Verte_Cmd(!S_S_Led_flip);
					S_S_Led_flip= !S_S_Led_flip;
				}
				else  if(TRUE == P_L_CarteSd_Atester_vue)
				{
					HWI_Reveil_Cmd(S_S_Led_flip);
					S_S_Led_flip= !S_S_Led_flip;
				}
			break;

#ifdef _RAMPE_SOLAIRE
			case 400:
				APP_Manage_Perte_Tablette();
			break;
#endif

			case 420:
#ifdef _PROGRAMATION
				APP_Manage_Add_Module_Check();
#endif

			case 402:
				APP_Test_WIFI_Connection();
			break;

			case 500:
				APP_Manage_Wifi_Client();
			break;

			case 505:
				P_L_Bx310x_Test_Timeout_Cmd();
			break;

			case 510:
				P_L_Wifi_Test_Init_Wep();
			break;

			case 511:
				P_L_Manage_Perte_Reseau();
			break;

			default:
			break;
		}
#endif
    }
    else
    {
    	//do nothing
    }
}

/*******************************************************************************
*
*  Function Name   	: S_S_Background
*  Description     	: boucle de fond infinit
*	Param					:
*
*******************************************************************************/
static void S_S_Background(void)
 {
   // boucle de fond
 	while(1)
 	{
 		P_L_Rpg_Saut_Boot(); // Surveille la demande de retour en boot
 	    P_L_Rpg_Calc_Checksum(); // Calcul des checksums
    }
}

/*******************************************************************************
*
*  Function Name   	: S_S_Scheduler_Init
*  Description     	: Initialisation de l'ordonanceur
*	Param					:
*
*******************************************************************************/
static void S_S_Scheduler_Init(void)
{
    S_S_1MS_U32_TIME = 0;
    S_S_Led_flip = FALSE;
}

/*******************************************************************************
*
*  Function Name   	: main
*  Description     	: debut du code
*	Param					:
*
*******************************************************************************/
int main(void)
{
	S_S_Scheduler_Start();
    S_S_Background();
return(1);
}

/*******************************************************************************
*
*  Function Name   	: Error_Handler
*  Description     	:
*	Param			:
*
*******************************************************************************/
void _Error_Handler(char *file, int line)
{

}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}
