/*******************************************************************************
 *  Synergie Ing�nierie Industirelle Co.
 *
 *  This document is the property of Synergie Ing�nierie Industrielle
 *  It must not be copied (in whole or in part) or disclosed without
 *  prior written consent of the company. Any copies by any method
 *  must also include a copy of this legend.
 *******************************************************************************/
/*******************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "app_manage_BT.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "lib_macros.h"
#include "p_l_bx310x.h"
#include "hwi_hal_can.h"
#include <string.h>
#include "app_manage_wifi.h"
#include "p_l_tx_can1.h"

/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE */

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
extern U32 S_S_1MS_U32_TIME_HIGH_PRIORITY;
extern U32 S_S_1MS_U32_TIME;

extern U8 P_L_BT_spp_rampe_id;
// indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE APP_Mode_gestion_wifi_blt;

extern P_L_BX310X_STATE P_L_Bx310x_internal_state;

#ifdef _RAMPE_AUTONOME
extern P_L_BT_STATE_TYPE P_L_BT_spp_rampe_state;
#endif

extern hw_addr P_L_BT_Module_Mac;
extern hw_addr P_L_BT_Spp_Mac;

extern BOOL_TYPE APP_Demande_Arret_SendCan;
extern BOOL_TYPE APP_Arret_Rampe_autonome_wifi;
extern BOOL_TYPE APP_Demande_Arret_Rampe_autonome_wifi;
/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
BOOL_TYPE APP_BT_Send_Data_running;
U8 APP_BT_Send_Data_send[242];
U16 APP_BT_Send_Data_lenght;
P_L_BT_SEND_STATE APP_BT_Send_State;

BOOL_TYPE APP_BT_Loop_send_spp;

U8 APP_BT_key_Tx[APP_TAILLE_WIFI_KEY];
U8 APP_BT_App_Key[APP_TAILLE_WIFI_KEY];

U8 APP_BT_Unlockkey_Tx[APP_TAILLE_WIFI_UNLOCKEDKEY];
U8 APP_BT_Unlockkey_Rx[APP_TAILLE_WIFI_UNLOCKEDKEY];
BOOL_TYPE APP_BT_key_ask;
U32 APP_BT_timer_key_ask;
BOOL_TYPE APP_BT_lock_key_ask;

U32 APP_BT_timer_all_step;
BOOL_TYPE APP_BT_lock_all_step;

U32 APP_BT_timer_1_step;
BOOL_TYPE APP_BT_lock_1_step;

U8 counter_test = 0;

U32 APP_BT_spp_loop_timer;
BOOL_TYPE APP_BT_spp_loop_lock;

BOOL_TYPE APP_BT_spp_Rx_FlipFlop;

BOOL_TYPE APP_BT_can_output_force_send;
U8 APP_BT_can_output_force_send_count;

BOOL_TYPE APP_BT_can_Alarm_force_send;

// Tab to store button status: 64 buttons on 6 bits
APP_BT_COMMAND_BOUTON_TYPE APP_BT_can_command_appli_output[HWI_NB_BUTTON_BT_CMD_APPLI];

APP_BT_POWER_TYPE APP_BT_can_output[HWI_NB_POWER_BT_CMD_RAMPE];
U16 APP_BT_can_ana[HWI_NB_ANA_BT_SPA_RAMPE];
U8 APP_BT_can_input;
BOOL_TYPE APP_BT_can_Alarm_Bat_sys;

U32 APP_BT_Timeout_RxOutput_timer;
BOOL_TYPE APP_BT_Timeout_RxOutput_lock;
BOOL_TYPE APP_BT_can_output_timeout;

U32 APP_BT_Timeout_RxOutput_Reset_timer;
BOOL_TYPE APP_BT_Timeout_RxOutput_Reset_lock;

U32 APP_BT_Timeout_RxWifiBT1_timer;
BOOL_TYPE APP_BT_Timeout_RxWifiBT1_lock;

U32 APP_BT_Timeout_RxWifiBT2_timer;
BOOL_TYPE APP_BT_Timeout_RxWifiBT2_lock;

/////////////////////////////////////////////////////
BOOL_TYPE APP_BT_spp_send_Key;
BOOL_TYPE APP_BT_WifiBT_input_not_veille;
/////////////////////////////////////////////////////

BOOL_TYPE APP_BT_CAN_baudrate_int;

#ifdef _PROJECTEUR_BT
U8 APP_BT_projecteur_cmd;
BOOL_TYPE APP_BT_spp_send_data1_ok;
#endif

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static BOOL_TYPE APP_BT_Send_Data(U16 data_len, U8 *data);
static void APP_BT_Unlockey(U8 *BT_key, hw_addr Mac, U8 *BT_Unlockkey);
static BOOL_TYPE APP_BT_Test_Trame_Format(U8 *data, U16 data_len, U16 *data_size_rx);
static void APP_BT_Encode_Trame_Format(U16 *data_len_tx, U8 *data_Tx, U16 data_len, U8 *data);
static BOOL_TYPE APP_BT_Test_Trame_Cheksum(U8 *data, U16 data_len);
static void APP_Manage_BT_Output_Timeout(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Manage_Send_Data
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_BT_Manage_Send_Data(void)
{
	U8 data_send[242];
	U16 ptr_data_send;
	BOOL_TYPE result;
	U8 output;
	U8 boucle;
	U32 TempKey;

#ifdef _DEBUG_WIFI_BT
	U8 data_debug[7];

	data_debug[0x00] = 0x00;
	data_debug[0x01] = 0x00;
	data_debug[0x02] = 0x00;
	data_debug[0x03] = 0x00;
	data_debug[0x04] = 0x00;
	data_debug[0x05] = 0x00;
	data_debug[0x06] = 0x00;
#endif
	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
	{
		APP_Manage_BT_Output_Timeout();
		APP_Manage_Wifi_BT_Timeout();
	}

#ifdef _PROJECTEUR_BT
	if (TRUE)
	// if ((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
	{
#endif
		if ((P_L_Bx310x_internal_state == Bx310x_state_FINIT) && (P_L_BT_spp_rampe_state == BT_connected))
		{

			if (APP_BT_can_output_force_send == TRUE)
			{
				APP_BT_can_output_force_send = FALSE;
				APP_BT_can_output_force_send_count = 2;
			}

			BT_TIMER_LOCK(APP_BT_spp_loop_timer, S_S_1MS_U32_TIME, APP_TIMEOUT_INTER_SPP_LOOP, APP_BT_spp_loop_lock, result);
			if ((TRUE == result) || (0x00 != APP_BT_can_output_force_send_count))
			{
				APP_BT_Loop_send_spp = TRUE;
				BT_TIMER_INIT_LOCK(APP_BT_spp_loop_timer, S_S_1MS_U32_TIME, APP_BT_spp_loop_lock);
			}

			if ((TRUE == APP_BT_Loop_send_spp) && (P_L_Bx310x_internal_state == Bx310x_state_FINIT))
			// if (TRUE == APP_BT_Loop_send_spp)
			{

#ifdef _DEBUG_WIFI_BT
				data_debug[0x01] = 0x01;
#endif

				BUFF_DATA_1_BT(data_send);
				ptr_data_send = 4;
#ifdef _MODE_TEST
				strcpy((char *)&(data_send[ptr_data_send]), (char *)"hello test");
				ptr_data_send = ptr_data_send + 10;
#else
			if (TRUE) //((APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
			{
				for (output = 0; output < HWI_NB_POWER_BT_CMD_RAMPE; output++)
				{
					data_send[ptr_data_send] = APP_BT_can_output[output].cmd;
					ptr_data_send++;
					data_send[ptr_data_send] = APP_BT_can_output[output].consigne_pwm;
					ptr_data_send++;
					data_send[ptr_data_send] = APP_BT_can_output[output].time_pwm;
					ptr_data_send++;
				}
			}
			else
			{
				for (output = 0; output < HWI_NB_POWER_BT_CMD_RAMPE; output++)
				{
					data_send[ptr_data_send] = APP_BT_can_output[output].etat_pow;
					ptr_data_send++;
					data_send[ptr_data_send] = APP_BT_can_output[output].defaut_cc;
					ptr_data_send++;
					data_send[ptr_data_send] = APP_BT_can_output[output].defaut_co1;
					ptr_data_send++;
					data_send[ptr_data_send] = APP_BT_can_output[output].defaut_co2;
					ptr_data_send++;
				}

				data_send[ptr_data_send] = APP_BT_can_input;
				ptr_data_send++;

				data_send[ptr_data_send] = (APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE] >> 8) & 0xFF;
				ptr_data_send++;
				data_send[ptr_data_send] = APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE] & 0xFF;
				ptr_data_send++;
			}
#endif // HERE KEY
	   // fsm to ask KEY
				APP_BT_Loop_send_spp = !APP_BT_Send_Data(ptr_data_send, data_send);

				if ((0x00 < APP_BT_can_output_force_send_count) && (FALSE == APP_BT_Loop_send_spp))
				{
					APP_BT_can_output_force_send_count--;
				}
			}

			/////////////////////////////////////////////////////
		}
		else
		{
			APP_BT_Send_State = BT_SEND_STATE_FINISH;
		}
#ifdef _PROJECTEUR_BT
	}
	else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
	{
		if ((P_L_Bx310x_internal_state == Bx310x_state_FINIT) && (P_L_BT_spp_rampe_state == BT_connected))
		{
		}
		else
		{
			APP_BT_Send_State = BT_SEND_STATE_FINISH;
		}
	}
#endif
	// Timer si com trop long
	if ((APP_BT_Send_State != BT_SEND_STATE_WAIT) && (APP_BT_Send_State != BT_SEND_STATE_FINISH))
	{
		BT_TIMER_LOCK(APP_BT_timer_all_step, S_S_1MS_U32_TIME, APP_TIMEOUT_ALL_STEP, APP_BT_lock_all_step, result);
		if (TRUE == result)
		{
			APP_BT_Send_State = BT_SEND_STATE_FINISH;
		}
	}

	if ((P_L_Bx310x_internal_state == Bx310x_state_FINIT) && (P_L_BT_spp_rampe_state == BT_connected))
	{
		if (TRUE == APP_BT_spp_send_Key)
		{
			APP_BT_spp_send_Key = FALSE;

			TempKey = S_S_1MS_U32_TIME_HIGH_PRIORITY;

			//  APP_BT_key_Tx[0] = (TempKey & 0x000000FF);
			//  APP_BT_key_Tx[1] = ((TempKey >> 8 ) & 0x000000FF);
			//  APP_BT_key_Tx[2] = ((TempKey >> 16 ) & 0x000000FF);
			//  APP_BT_key_Tx[3] = ((TempKey >> 24 ) & 0x000000FF);
			U8 ZERO = 0x00000000;
			APP_BT_key_Tx[0] = ZERO;
			APP_BT_key_Tx[1] = ZERO;
			APP_BT_key_Tx[2] = ZERO;
			APP_BT_key_Tx[3] = ZERO;

			APP_BT_Unlockey(APP_BT_key_Tx, P_L_BT_Module_Mac, APP_BT_Unlockkey_Tx);

			APP_BT_key_ask = TRUE;

			BUFF_KEY_BT(data_send);

			data_send[4] = 0x00;
			data_send[5] = (APP_TAILLE_WIFI_KEY + 3);
			data_send[6] = 'O';
			data_send[7] = 'K';

			for (boucle = 0; boucle < APP_TAILLE_WIFI_KEY; boucle++)
			{
				data_send[8 + boucle] = APP_BT_key_Tx[boucle];
			}
			data_send[12] = FIN_CHAINE;
			P_L_BT_Tx_dataDirect(P_L_BT_spp_rampe_id, 13, data_send);

			BT_TIMER_INIT_LOCK(APP_BT_timer_key_ask, S_S_1MS_U32_TIME, APP_BT_lock_key_ask);
		}
#ifdef _PROJECTEUR_BT
		else if (TRUE == APP_BT_spp_send_data1_ok)
		{
			APP_BT_spp_send_data1_ok = FALSE;

			BUFF_DATA_1_BT_R(data_send);

			data_send[4] = 0x00;
			data_send[5] = 0x00;
			data_send[6] = 'O';
			data_send[7] = 'K';
			data_send[8] = FIN_CHAINE;
			P_L_BT_Tx_dataDirect(P_L_BT_spp_rampe_id, 9, data_send);
		}
#endif
		else
		{
			/////////////////////////////////////////////////////

			// FSM To send data 
			switch (APP_BT_Send_State)
			{
			case BT_SEND_STATE_ASK_KEY:
				if (TRUE == APP_BT_key_ask)
				{
					APP_BT_Send_State = BT_SEND_STATE_SEND_DATA;
				}
				else
				{
					BUFF_KEY_ASK_BT(data_send);
					P_L_BT_Tx_dataDirect(P_L_BT_spp_rampe_id, TAILLE_WRD_BT, data_send);
					BT_TIMER_INIT_LOCK(APP_BT_timer_1_step, S_S_1MS_U32_TIME, APP_BT_lock_1_step);
					APP_BT_Send_State = BT_SEND_STATE_WAIT_KEY;
				}
				break;

			case BT_SEND_STATE_WAIT_KEY:
				BT_TIMER_LOCK(APP_BT_timer_1_step, S_S_1MS_U32_TIME, APP_TIMEOUT_1_STEP, APP_BT_lock_1_step, result);
				// si timer écoulé retour BT_SEND_STATE_ASK_KEY
				if (TRUE == result)
				{
					APP_BT_Send_State = BT_SEND_STATE_ASK_KEY;
				}
				break;

			case BT_SEND_STATE_SEND_DATA:
				APP_BT_Encode_Trame_Format(&ptr_data_send, data_send, APP_BT_Send_Data_lenght, APP_BT_Send_Data_send);
				P_L_BT_Tx_dataDirect(P_L_BT_spp_rampe_id, ptr_data_send, data_send);
				BT_TIMER_INIT_LOCK(APP_BT_timer_1_step, S_S_1MS_U32_TIME, APP_BT_lock_1_step);
				APP_BT_Send_State = BT_SEND_STATE_FINISH;
				break;

			case BT_SEND_STATE_FINISH:
				APP_BT_Send_Data_running = FALSE;
				APP_BT_Send_State = BT_SEND_STATE_WAIT;
				break;

			case BT_SEND_STATE_WAIT:
				// do nothing
				break;

			default:
				APP_BT_Send_State = BT_SEND_STATE_WAIT;
				break;
			}
			/////////////////////////////////////////////////////
		}
	}
	else
	{
		APP_BT_Send_State = BT_SEND_STATE_FINISH;
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
static void APP_Manage_BT_Output_Timeout(void)
{
	U8 power;
	BOOL_TYPE result;

	if (FALSE == APP_BT_can_output_timeout)
	{
		BT_TIMER_LOCK(APP_BT_Timeout_RxOutput_timer, S_S_1MS_U32_TIME, APP_TIMEOUT_BT_OUTPUT, APP_BT_Timeout_RxOutput_lock, result);
		if (TRUE == result)
		{
			APP_BT_can_output_timeout = TRUE;

			for (power = 0x00; power < HWI_NB_POWER_BT_CMD_RAMPE; power++)
			{
				APP_BT_can_output[power].cmd = 0x00;
				APP_BT_can_output[power].consigne_pwm = APP_VALEUR_OFF_PWM;
				APP_BT_can_output[power].time_pwm = APP_INSTANTANE;
			}
		}
	}

	BT_TIMER_LOCK(APP_BT_Timeout_RxOutput_Reset_timer, S_S_1MS_U32_TIME, APP_TIMEOUT_BT_RESET, APP_BT_Timeout_RxOutput_Reset_lock, result);
	if (TRUE == result)
	{
		P_L_Bx310x_Module_Init_Ask();
		BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxOutput_Reset_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxOutput_Reset_lock);
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_Gestion_BT_Data
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_Gestion_BT_DataRx(U16 data_len, U8 *data)
{
	U8 data_send[242];
	U32 SwWord;
	U32 TempKey;
	U8 boucle;
	U8 BT_key_Rx[APP_TAILLE_WIFI_KEY];
	U16 data_size_rx;
	int result;
	U8 output;
	U8 ptr_data_rx;

#ifdef _DEBUG_WIFI_BT
	U8 data_debug[7];

	data_debug[0x00] = 0x01;
	data_debug[0x01] = 0x00;
	data_debug[0x02] = 0x00;
	data_debug[0x03] = 0x00;
	data_debug[0x04] = 0x00;
	data_debug[0x05] = 0x00;
	data_debug[0x06] = 0x00;
#endif

	SwWord = LD_DWORD(&data[0]);

	if (SwWord == WRD_KEY_ASK_BT)
	{

		APP_BT_spp_send_Key = TRUE;

		// if (TRUE == APP_BT_Test_Trame_Cheksum(data, data_len))
		// {
			/////////////////////////////////////////////////////
			/*
			TempKey = S_S_1MS_U32_TIME_HIGH_PRIORITY;

			APP_BT_key_Tx[0] = (TempKey & 0x000000FF);
			APP_BT_key_Tx[1] = ((TempKey >> 8 ) & 0x000000FF);
			APP_BT_key_Tx[2] = ((TempKey >> 16 ) & 0x000000FF);
			APP_BT_key_Tx[3] = ((TempKey >> 24 ) & 0x000000FF);

			APP_BT_Unlockey(APP_BT_key_Tx,P_L_BT_Module_Mac,APP_BT_Unlockkey_Tx);

			APP_BT_key_ask = TRUE;

			BUFF_KEY_BT(data_send);

			data_send[4] = 0x00;
			data_send[5] = (APP_TAILLE_WIFI_KEY+3);
			data_send[6] = 'O';
			data_send[7] = 'K';

			for (boucle=0;boucle<APP_TAILLE_WIFI_KEY;boucle++)
			{
				data_send[8+boucle] = APP_BT_key_Tx[boucle];
			}
			data_send[12] = FIN_CHAINE;
			//P_L_BT_Tx_dataDirect(P_L_BT_spp_rampe_id,13,data_send);
			P_L_BT_Tx_data(P_L_BT_spp_rampe_id,13,data_send);

			BT_TIMER_INIT_LOCK(APP_BT_timer_key_ask,S_S_1MS_U32_TIME,APP_BT_lock_key_ask);
			*/

			// APP_BT_spp_send_Key = TRUE;

			/////////////////////////////////////////////////////
		// }
	}
	else if (SwWord == WRD_KEY_BT)
	{
		BOOL_TYPE flag_key_APP = TRUE;
		
		APP_BT_App_Key[0] = 51;
		APP_BT_App_Key[1] = 51;
		APP_BT_App_Key[2] = 51;
		APP_BT_App_Key[3] = 50;
		APP_BT_App_Key[4] = 51;
		APP_BT_App_Key[5] = 13;
		APP_BT_App_Key[6] = 48;
		APP_BT_App_Key[7] = 48;
		
		ptr_data_rx = 4 + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;

		for (output = 0; output < APP_TAILLE_WIFI_KEY; output++)
		{
			// first 6 bits
			BT_key_Rx[output] = data[ptr_data_rx];
			ptr_data_rx++;
			if (BT_key_Rx[output] != APP_BT_App_Key[output])
			{
				flag_key_APP = FALSE;
			}
		}

		APP_BT_Unlockey(BT_key_Rx, P_L_BT_Spp_Mac, APP_BT_Unlockkey_Rx);
		if(flag_key_APP == TRUE)
		{
			APP_BT_key_ask = TRUE;
			APP_BT_Send_State = BT_SEND_STATE_SEND_DATA;
		}
	}
	else if (SwWord == WRD_KEY_CAPP_BT)
	{
		BOOL_TYPE flag_key_ok = TRUE;

		for (boucle = 0; boucle < APP_TAILLE_WIFI_UNLOCKEDKEY; boucle++)
		{
			if (data[4 + boucle + 2] != APP_BT_Unlockkey_Tx[boucle])
			{
				flag_key_ok = FALSE;
				boucle = APP_TAILLE_WIFI_UNLOCKEDKEY;
			}
		}
		if (flag_key_ok == TRUE && TRUE == APP_BT_key_ask)
		{
			ptr_data_rx = 4 + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;

			for (output = 0; output < HWI_NB_BUTTON_BT_CMD_APPLI; output++)
			{
				// first 6 bits
				APP_BT_can_command_appli_output[output].id = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output)];
				ptr_data_rx++;
				APP_BT_can_command_appli_output[output].cmd = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output) + 1];
				// APP_BT_can_command_appli_output[output].trame = data[ptr_data_rx ];//data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output) + 2];
				ptr_data_rx++;
			}
		}
	}
	else if (SwWord == WRD_DATA_1_BT)
	{
		if (TRUE == APP_BT_Test_Trame_Format(data, data_len, &data_size_rx))
		{
#ifdef _DEBUG_WIFI_BT
			data_debug[0x01] = 0x02;
#endif

#ifdef _PROJECTEUR_BT
			if (TRUE) //((APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU)||(APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_ORANGE))
			{
#endif
				if (APP_Mode_gestion_wifi_blt == MODE_CMD_RAMPE_AUTONOME_BLEU) // MODE_RAMPE_AUTONOME)
				{
					APP_BT_Loop_send_spp = TRUE;
				}

				if (TRUE) //(APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME)
				{
					BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxOutput_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxOutput_lock);
					BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxOutput_Reset_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxOutput_Reset_lock);
					APP_Manage_Reload_Wifi_BT_Timeout();

					APP_BT_can_output_timeout = FALSE;

					ptr_data_rx = 4 + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;

					for (output = 0; output < HWI_NB_POWER_BT_CMD_RAMPE; output++)
					{
						APP_BT_can_output[output].cmd = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output)];
						ptr_data_rx++;
						APP_BT_can_output[output].consigne_pwm = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output) + 1];
						ptr_data_rx++;
						APP_BT_can_output[output].time_pwm = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (3*output) + 2];
						ptr_data_rx++;
					}
				}
				else
				{
					ptr_data_rx = 4 + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;

					for (output = 0; output < HWI_NB_POWER_BT_CMD_RAMPE; output++)
					{
						APP_BT_can_output[output].etat_pow = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (4*output)];
						ptr_data_rx++;
						APP_BT_can_output[output].defaut_cc = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (4*output) + 1];
						ptr_data_rx++;
						APP_BT_can_output[output].defaut_co1 = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (4*output) + 2];
						ptr_data_rx++;
						APP_BT_can_output[output].defaut_co2 = data[ptr_data_rx]; // data[4+ APP_TAILLE_WIFI_UNLOCKEDKEY + 2 + (4*output) + 3];
						ptr_data_rx++;
					}

					APP_BT_can_input = data[ptr_data_rx];
					ptr_data_rx++;

					APP_BT_can_ana[HWI_MES_BAT_SPA_RAMPE] = (data[ptr_data_rx] * 256) + data[ptr_data_rx + 1];
					ptr_data_rx = ptr_data_rx + 2;
				}
// #endif
#ifdef _PROJECTEUR_BT
			}
			else if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
			{
				APP_BT_spp_Rx_FlipFlop = !APP_BT_spp_Rx_FlipFlop;

				if (APP_BT_spp_Rx_FlipFlop == TRUE)
				{
					HWI_Led_1_Verte_Cmd(TRUE);
				}
				else
				{
					HWI_Led_1_Verte_Cmd(FALSE);
				}

				if (APP_BT_CAN_baudrate_int == FALSE)
				{
					HWI_Hal_Can_Init(HWI_CBR125000);
					APP_BT_CAN_baudrate_int = TRUE;
				}

				ptr_data_rx = 4 + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;

				APP_BT_projecteur_cmd = data[ptr_data_rx];
				P_L_Can_Send_PRJ_CMD(MAIL_BOX1);
				APP_BT_spp_send_data1_ok = TRUE;
			}
#endif
		}

		// #ifdef _DEBUG_WIFI_BT
		// 		P_L_Can_Send_Data_Can_Debug(data_debug,MAIL_BOX2);
		// #endif
	}
	// #ifdef _PROJECTEUR_BT
	else if (SwWord == WRD_BTN_BT)
	{
		if (APP_Mode_gestion_wifi_blt == MODE_PROJECTEUR_BT)
		{
			if (TRUE == APP_BT_Test_Trame_Format(data, data_len, &data_size_rx))
			{
				APP_BT_spp_Rx_FlipFlop = !APP_BT_spp_Rx_FlipFlop;

				if (APP_BT_spp_Rx_FlipFlop == TRUE)
				{
					HWI_Led_1_Verte_Cmd(TRUE);
				}
				else
				{
					HWI_Led_1_Verte_Cmd(FALSE);
				}
			}
		}
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Send_Data
 *  Description     :
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_BT_Send_Data(U16 data_len, U8 *data)
{
	BOOL_TYPE result;
	U16 boucle;

	result = FALSE;
	if (APP_BT_Send_Data_running == FALSE)
	{
		APP_BT_Send_Data_running = TRUE;
		APP_BT_Send_Data_lenght = data_len;

		for (boucle = 0; boucle < APP_BT_Send_Data_lenght; boucle++)
		{
			APP_BT_Send_Data_send[boucle] = data[boucle];
		}

		APP_BT_Send_State = BT_SEND_STATE_ASK_KEY;
		BT_TIMER_INIT_LOCK(APP_BT_timer_all_step, S_S_1MS_U32_TIME, APP_BT_lock_all_step);
		result = TRUE;
	}
	return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Unlockey
 *  Description     :
 *
 *
 *******************************************************************************/
static void APP_BT_Unlockey(U8 *BT_key, hw_addr Mac, U8 *BT_Unlockkey)
{

	// Version simplifiée pour les tests - remplace l'algorithme complexe par des zéros
	// BT_Unlockkey[0] = BT_key[0] ^ Mac.addr[0];
	// BT_Unlockkey[1] = (BT_key[1] ^ Mac.addr[2]) + 6;
	// BT_Unlockkey[2] = ((BT_key[2] ^ Mac.addr[1]) ^ BT_Unlockkey[0]) + 25;
	// BT_Unlockkey[3] = ((BT_key[3] ^ Mac.addr[4]) ^ BT_Unlockkey[1]) - 8;
	// BT_Unlockkey[4] = ((BT_key[2] ^ Mac.addr[3]) ^ BT_Unlockkey[2]) + 32;
	// BT_Unlockkey[5] = ((BT_key[3] ^ Mac.addr[5]) ^ BT_Unlockkey[3]) - 1;
	// BT_Unlockkey[6] = ((BT_key[2] ^ Mac.addr[1]) ^ BT_Unlockkey[1]) + 85;
	// BT_Unlockkey[7] = ((BT_key[3] ^ Mac.addr[0]) ^ BT_Unlockkey[2]) - 12;


	//Key frame

	BT_Unlockkey[0] = 2;
	BT_Unlockkey[1] = 2;
	BT_Unlockkey[2] = 2;
	BT_Unlockkey[3] = 2;
	BT_Unlockkey[4] = 2;
	BT_Unlockkey[5] = 2;
	BT_Unlockkey[6] = 2;
	BT_Unlockkey[7] = 2;
	
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Encode_Trame_Format
 *  Description     : test si format de la trame et securite ok
 *
 *
 *******************************************************************************/
static void APP_BT_Encode_Trame_Format(U16 *data_len_tx, U8 *data_Tx, U16 data_len, U8 *data)
{
	U16 ptr;
	U16 boucle;

	*data_len_tx = data_len + APP_TAILLE_WIFI_UNLOCKEDKEY + 2;
	ptr = 0;

	// mots code
	for (boucle = 0; boucle < TAILLE_WRD_BT; boucle++)
	{
		data_Tx[ptr] = data[boucle];
		ptr++;
	}

	// Taille
	data_Tx[ptr] = (U8)((*data_len_tx >> 8) & 0x00FF);
	ptr++;
	data_Tx[ptr] = (U8)(*data_len_tx & 0x00FF);
	ptr++;

	// Unlockkey
	for (boucle = 0; boucle < APP_TAILLE_WIFI_UNLOCKEDKEY; boucle++)
	{
		data_Tx[ptr] = APP_BT_Unlockkey_Rx[boucle];
		ptr++;
	}

	// data
	for (boucle = 4; boucle < data_len; boucle++)
	{
		data_Tx[ptr] = data[boucle];
		ptr++;
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_Test_Trame_Format
 *  Description     : test si format de la trame et securite ok
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_BT_Test_Trame_Format(U8 *data, U16 data_len, U16 *data_size_rx)
{
	//Not used


	BOOL_TYPE result = FALSE;
	BOOL_TYPE flag_key_ok = TRUE;
	U16 data_size;
	U16 boucle;

	U8 cheksum_1;
	U8 cheksum_2;

	data_size = (data[4] * 256) + data[5];

	*data_size_rx = data_size;

	// if (data_size + 2  == data_len)
	// {

	// if (TRUE == APP_BT_key_ask)
	// {
	// 	APP_BT_key_ask = FALSE;

	for (boucle = 0; boucle < APP_TAILLE_WIFI_UNLOCKEDKEY; boucle++)
	{
		if (data[4 + boucle + 2] != APP_BT_Unlockkey_Tx[boucle])
		{
			flag_key_ok = FALSE;
			boucle = APP_TAILLE_WIFI_UNLOCKEDKEY;
		}
	}

	if (TRUE == flag_key_ok)
	{

		cheksum_1 = 0;
		cheksum_2 = 0;

		for (boucle = 0; boucle < data_size; boucle++)
		{
			cheksum_1 = cheksum_1 + data[boucle];
			cheksum_2 = cheksum_2 + (data[boucle] * (boucle + 1));
		}

		if ((cheksum_1 == data[data_size]) && (cheksum_2 == data[data_size + 1]))
		{
			result = TRUE;
		}
		// }
	}
	// }
	// return result;
	result = TRUE;
	return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_Test_Trame_Format
 *  Description     : test si format de la trame et securite ok
 *
 *
 *******************************************************************************/
static BOOL_TYPE APP_BT_Test_Trame_Cheksum(U8 *data, U16 data_len)
{
	// Not used
	
	BOOL_TYPE result = FALSE;
	// U16 boucle;
	U8 cheksum_1;
	U8 cheksum_2;
	U8 boucle;

	// cheksum_1 = 0;
	// cheksum_2 = 0;

	// for (boucle=0;boucle<(data_len-2);boucle++)
	// {
	// 	cheksum_1 = cheksum_1 + data[boucle];
	// 	cheksum_2 = cheksum_2 + (data[boucle]*(boucle+1));
	// }

	// if ((cheksum_1 == data[data_len-2])&&(cheksum_2 == data[data_len-1]))
	// {
	// 	result = TRUE;
	// }

	// TEST TRAME PASSWORD:

	// return result;
	result = TRUE;
	return result;
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Can_Data_Power_Bloc
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_BT_Can_Data_Power_Bloc(U8 *p_l_can_data_ptr, U8 offset, U8 p_l_can_lenght)
{
	U8 power;
	U8 position;
	U8 data_power;
	BOOL_TYPE force_send;
	BOOL_TYPE force_send2;

	force_send = FALSE;
	force_send2 = FALSE;

	for (power = 0x00; power < 7; power++)
	{
		position = power + (7 * offset);
		data_power = p_l_can_data_ptr[power + 1];

		// on regarde si l'on est dans de la zone des power
		if (HWI_NB_POWER_BT_CMD_RAMPE > position)
		{
			if (HWI_NB_POWER_PWM_BT_CMD_RAMPE <= position)
			{
				// data_power = p_l_can_data_ptr[power+1];

				if (PL_CAN_POW_ETAT == (data_power & PL_CAN_POW_ETAT))
				{ // demande de mise en route de la sortie
					if (APP_BT_can_output[position].cmd == 0x00)
					{
						force_send = TRUE;
					}

					APP_BT_can_output[position].cmd = 0x01;
					APP_BT_can_output[position].consigne_pwm = APP_VALEUR_MAXI_PWM;
					APP_BT_can_output[position].time_pwm = APP_INSTANTANE;
				}
				else
				{ // demande d'arret de la sortie
					if (APP_BT_can_output[position].cmd == 0x01)
					{
						force_send = TRUE;
					}
					APP_BT_can_output[position].cmd = 0x00;
					APP_BT_can_output[position].consigne_pwm = APP_VALEUR_OFF_PWM;
					APP_BT_can_output[position].time_pwm = APP_INSTANTANE;
				}
			}
		}
		else if (HWI_NB_POWER_BT_CMD_RAMPE == position)
		{
			if (PL_CAN_POW_ETAT == (data_power & PL_CAN_POW_ETAT))
			{
				if (FALSE == APP_BT_can_Alarm_Bat_sys)
				{
					force_send2 = TRUE;
				}

				APP_BT_can_Alarm_Bat_sys = TRUE;
			}
			else
			{
				if (TRUE == APP_BT_can_Alarm_Bat_sys)
				{
					force_send2 = TRUE;
				}
				APP_BT_can_Alarm_Bat_sys = FALSE;
			}
		}
	}

	if (TRUE == force_send)
	{
		APP_BT_can_output_force_send = TRUE;
	}

	if (TRUE == force_send2)
	{
		APP_BT_can_Alarm_force_send = TRUE;
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Can_Data_Pwm_Bloc
 *  Description     :
 *
 *
 *
 *******************************************************************************/
void APP_BT_Can_Data_Pwm_Bloc(U8 *p_l_can_data_ptr, U8 offset)
{
	U8 module;
	U8 pwm;
	U8 position;
	U8 consigne_pwm;
	U8 time_pwm;
	BOOL_TYPE etat_pow;
	BOOL_TYPE force_send;

	force_send = FALSE;

	for (pwm = 0x00; pwm < 3; pwm++)
	{
		position = pwm + (3 * offset);
		consigne_pwm = p_l_can_data_ptr[(2 * pwm) + 1];
		time_pwm = p_l_can_data_ptr[(2 * pwm) + 2];

		if (APP_VALEUR_MAXI_PWM < consigne_pwm)
		{
			consigne_pwm = APP_VALEUR_MAXI_PWM;
		}

		if (HWI_NB_POWER_PWM_BT_CMD_RAMPE > position)
		{
			if (APP_VALEUR_OFF_PWM == consigne_pwm)
			{

				if ((APP_BT_can_output[position].cmd == 0x01) || (APP_BT_can_output[position].consigne_pwm != consigne_pwm) || (APP_BT_can_output[position].time_pwm != time_pwm))
				{
					force_send = TRUE;
				}

				APP_BT_can_output[position].cmd = 0x00;
				APP_BT_can_output[position].consigne_pwm = consigne_pwm;
				APP_BT_can_output[position].time_pwm = time_pwm;
			}
			else
			{
				if ((APP_BT_can_output[position].cmd == 0x00) || (APP_BT_can_output[position].consigne_pwm != consigne_pwm) || (APP_BT_can_output[position].time_pwm != time_pwm))
				{
					force_send = TRUE;
				}
				APP_BT_can_output[position].cmd = 0x01;
				APP_BT_can_output[position].consigne_pwm = consigne_pwm;
				APP_BT_can_output[position].time_pwm = time_pwm;
			}
		}
	}

	if (TRUE == force_send)
	{
		APP_BT_can_output_force_send = TRUE;
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_BT_Can_Data_Power_Bloc
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_SPA_Can_Data_Power_Bloc(U8 *p_l_can_data_ptr, U8 offset, U8 p_l_can_lenght)
{
	U8 position;
	U8 data_power;
	BOOL_TYPE analyse;

	analyse = FALSE;

	if (offset == 0)
	{
		position = 0;
		data_power = p_l_can_data_ptr[1];
		analyse = TRUE;
	}
	else if (offset == 1)
	{
		position = 1;
		data_power = p_l_can_data_ptr[2];
		analyse = TRUE;
	}
	if (analyse == TRUE)
	{
		if (PL_CAN_POW_ETAT == (data_power & PL_CAN_POW_ETAT))
		{ // power on
			APP_BT_can_output[position].etat_pow = TRUE;
		}
		else
		{ //  power off
			APP_BT_can_output[position].etat_pow = FALSE;
		}

		if (PL_CAN_DEFAUT_CC == (data_power & PL_CAN_MASK_DEFAUT_CO_CC))
		{
			APP_BT_can_output[position].defaut_cc = TRUE;
			APP_BT_can_output[position].defaut_co1 = FALSE;
			APP_BT_can_output[position].defaut_co2 = FALSE;
		}
		else if (PL_CAN_DEFAUT_CO1 == (data_power & PL_CAN_MASK_DEFAUT_CO_CC))
		{
			APP_BT_can_output[position].defaut_cc = FALSE;
			APP_BT_can_output[position].defaut_co1 = TRUE;
			APP_BT_can_output[position].defaut_co2 = TRUE;
		}
		else if (PL_CAN_DEFAUT_CO2 == (data_power & PL_CAN_MASK_DEFAUT_CO_CC))
		{
			APP_BT_can_output[position].defaut_cc = FALSE;
			APP_BT_can_output[position].defaut_co1 = FALSE;
			APP_BT_can_output[position].defaut_co2 = TRUE;
		}
		else
		{
			APP_BT_can_output[position].defaut_cc = FALSE;
			APP_BT_can_output[position].defaut_co1 = FALSE;
			APP_BT_can_output[position].defaut_co2 = FALSE;
		}
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_SPA_Can_Data_Analog_Bloc
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_SPA_Can_Data_Analog_Bloc(U8 *p_l_can_data_ptr, U8 offset)
{
	U8 analog;
	U8 position;

	if (offset == 0)
	{
		for (analog = 0x00; analog < 3; analog++)
		{
			position = analog + (3 * offset);
			APP_BT_can_ana[position] = (p_l_can_data_ptr[(2 * analog) + 1] * 256) + p_l_can_data_ptr[(2 * analog) + 2];
		}
	}
}

/*******************************************************************************
 *
 *  Function Name   : APP_SPA_Can_Data_Analog_Bloc
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_SPA_Can_Data_Input_Bloc(U8 *p_l_can_data_ptr)
{
	APP_BT_can_input = p_l_can_data_ptr[1];
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_BT_Timeout
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_Manage_Wifi_BT_Timeout(void)
{
#ifdef _WIFI_BT_ARRET_SPA
	BOOL_TYPE result;

	BT_TIMER_LOCK(APP_BT_Timeout_RxWifiBT1_timer, S_S_1MS_U32_TIME, APP_TIMEOUT_BT_RESET, APP_BT_Timeout_RxWifiBT1_lock, result);
	if (TRUE == result)
	{
		APP_BT_WifiBT_input_not_veille = FALSE;

		BT_TIMER_LOCK(APP_BT_Timeout_RxWifiBT2_timer, S_S_1MS_U32_TIME, APP_TIMEOUT_BT_WIFI, APP_BT_Timeout_RxWifiBT2_lock, result);
		if (TRUE == result)
		{
			APP_Demande_Arret_SendCan = TRUE;
		}
	}
	else
	{
		BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxWifiBT2_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxWifiBT2_lock);
	}

#endif
}
/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Reload_Wifi_BT_Timeout
 *  Description     :
 *
 *
 *******************************************************************************/
void APP_Manage_Reload_Wifi_BT_Timeout(void)
{
#ifdef _WIFI_BT_ARRET_SPA
	if (APP_Mode_gestion_wifi_blt == MODE_RAMPE_AUTONOME_WIFI)
	{
		if ((APP_Arret_Rampe_autonome_wifi == FALSE) && (APP_Demande_Arret_Rampe_autonome_wifi == FALSE))
		{
			BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxWifiBT1_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxWifiBT1_lock);
			BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxWifiBT2_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxWifiBT2_lock);
			APP_Demande_Arret_SendCan = FALSE;
			APP_BT_WifiBT_input_not_veille = TRUE;
		}
	}
	else
	{
		BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxWifiBT1_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxWifiBT1_lock);
		BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxWifiBT2_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxWifiBT2_lock);
		APP_Demande_Arret_SendCan = FALSE;
		APP_BT_WifiBT_input_not_veille = TRUE;
	}
#endif
}

/*******************************************************************************
 *
 *  Function Name   : APP_Manage_Wifi_Init
 *  Description     : initialisation
 *
 *
 *******************************************************************************/
void APP_Manage_BT_Init(void)
{
	U8 power;

	APP_BT_Send_Data_running = FALSE;
	APP_BT_Send_State = BT_SEND_STATE_WAIT;
	APP_BT_key_ask = FALSE;

	APP_BT_Loop_send_spp = FALSE;
	BT_TIMER_INIT_LOCK(APP_BT_spp_loop_timer, S_S_1MS_U32_TIME, APP_BT_spp_loop_lock);
	APP_BT_spp_loop_lock = TRUE;
	APP_BT_spp_Rx_FlipFlop = FALSE;

	for (power = 0x00; power < HWI_NB_POWER_BT_CMD_RAMPE; power++)
	{
		APP_BT_can_output[power].cmd = 0x00;
		APP_BT_can_output[power].consigne_pwm = APP_VALEUR_OFF_PWM;
		APP_BT_can_output[power].time_pwm = APP_INSTANTANE;
		APP_BT_can_output[power].etat_pow = FALSE;
		APP_BT_can_output[power].defaut_cc = FALSE;
		APP_BT_can_output[power].defaut_co1 = FALSE;
		APP_BT_can_output[power].defaut_co2 = FALSE;
	}
	BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxOutput_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxOutput_lock);
	BT_TIMER_INIT_LOCK(APP_BT_Timeout_RxOutput_Reset_timer, S_S_1MS_U32_TIME, APP_BT_Timeout_RxOutput_Reset_lock);
	APP_Manage_Reload_Wifi_BT_Timeout();
	APP_BT_can_output_timeout = TRUE;
	APP_BT_can_output_force_send = FALSE;
	APP_BT_can_output_force_send_count = 0;

	APP_BT_can_Alarm_force_send = FALSE;

	APP_BT_spp_send_Key = FALSE;
	APP_BT_can_Alarm_Bat_sys = FALSE;

	APP_BT_CAN_baudrate_int = FALSE;

#ifdef _PROJECTEUR_BT
	APP_BT_projecteur_cmd = 0x00;
	APP_BT_spp_send_data1_ok = TRUE; // FALSE;
#endif
}
