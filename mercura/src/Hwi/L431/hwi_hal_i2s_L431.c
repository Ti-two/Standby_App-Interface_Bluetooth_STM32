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
#include "hwi_hal_i2s.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "S_S_Scheduler.h"
#include "hwi_nvic_priority.h"

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
SAI_HandleTypeDef 			SAIHandle;
U8                  		HWI_I2S_Value;
U16							HWI_I2S_ValueU16;
BOOL_TYPE					HWI_I2S_running;
U8 							HWI_AIS_Buffer[HWI_SAI_DATA_SIZE+2];
U16 						HWI_InternalBufferAudio[HWI_SAI_IN_NB_CHANNELS];
U8 							offset_decalage;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
 *
 *  Function Name   : HWI_I2S_Init
 *  Description     : initialisation de I2S
 *
 *
 *******************************************************************************/
void HWI_I2S_Init(void)
{
	offset_decalage = 0;

	SAIHandle.Instance = SAI1_Block_A;
	SAIHandle.Init.AudioMode = SAI_MODESLAVE_RX;
	SAIHandle.Init.Synchro = SAI_ASYNCHRONOUS;
	SAIHandle.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	SAIHandle.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	SAIHandle.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	SAIHandle.Init.MonoStereoMode = SAI_MONOMODE;
	SAIHandle.Init.CompandingMode = SAI_NOCOMPANDING;
	SAIHandle.Init.TriState = SAI_OUTPUT_RELEASED;
#ifdef _PCM
	if (HAL_SAI_InitProtocol(&SAIHandle, SAI_PCM_LONG, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
	{
		Error_Handler();
	}
#else
	if (HAL_SAI_InitProtocol(&SAIHandle, SAI_I2S_MSBJUSTIFIED, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
	{
		Error_Handler();
	}
#endif

    HWI_I2S_Stop();
}

/*******************************************************************************
 *
 *  Function Name   : HWI_I2S_Start
 *  Description     : This function start I2S
 *
 *
 *******************************************************************************/
void HWI_I2S_Start(void)
{
	if (FALSE == HWI_I2S_running)
	{
		HWI_I2S_running = TRUE;
		HAL_SAI_Receive_IT(&SAIHandle,HWI_AIS_Buffer,HWI_SAI_IN_NB_CHANNELS);
	}

}

/*******************************************************************************
 *
 *  Function Name   : HWI_I2S_Stop
 *  Description     : This function stop I2S
 *
 *
 *******************************************************************************/
void HWI_I2S_Stop(void)
{
	HWI_I2S_running = FALSE;
}

/*******************************************************************************
 *
 *  Function Name   : HAL_SAI_MspInit
 *  Description     : This function configures the hardware resources used
 *
 *
 *******************************************************************************/
static uint32_t SAI1_client = 0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{
GPIO_InitTypeDef GPIO_InitStruct;
RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

// SAI1
	if(hsai->Instance==SAI1_Block_A)
	{
		// Peripheral clock enable
		// Initializes the peripherals clock

		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
		PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
		PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
		PeriphClkInit.PLLSAI1.PLLSAI1M = 2;
		PeriphClkInit.PLLSAI1.PLLSAI1N = 9;
		PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
		PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
		PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
		PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
		  Error_Handler();
		}

		if (SAI1_client == 0)
		{
		   __HAL_RCC_SAI1_CLK_ENABLE();

		   //Peripheral interrupt init
		   HAL_NVIC_SetPriority(SAI1_IRQn,HAL_NVIC_PREEMPT_PRIORITY_I2S, HAL_NVIC_SUB_PRIORITY_I2S);
		   HAL_NVIC_EnableIRQ(SAI1_IRQn);
		}
		SAI1_client ++;

		// SAI1_A_Block_A GPIO Configuration
		// PB12     ------> SAI1_FS_A
		// PB13     ------> SAI1_SCK_A
		// PB15     ------> SAI1_SD_A

		GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

/*******************************************************************************
 *
 *  Function Name   : HAL_I2S_MspDeInit
 *  Description     : This function freeze the hardware resources used
 *
 *
 *******************************************************************************/
void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{
	// SAI1
	if(hsai->Instance==SAI1_Block_A)
	{
		SAI1_client --;
		if (SAI1_client == 0)
		{
			// Peripheral clock disable
			__HAL_RCC_SAI1_CLK_DISABLE();
			// SAI1 interrupt DeInit
			HAL_NVIC_DisableIRQ(SAI1_IRQn);
		}

		// SAI1_A_Block_A GPIO Configuration
		// PB12     ------> SAI1_FS_A
		// PB13     ------> SAI1_SCK_A
		// PB15     ------> SAI1_SD_A
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15);

	}
}

/*******************************************************************************
 *
 *  Function Name   : HAL_SAI_RxCpltCallback
 *  Description     : Rx Transfer completed callback.
 *
 *
 *******************************************************************************/
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	if(hsai->Instance==SAI1_Block_A)
	{
		if (TRUE == HWI_I2S_running)
		{

			HWI_InternalBufferAudio[0] = HWI_AIS_Buffer[1] + HWI_AIS_Buffer[0]*256;
			HWI_I2S_ValueU16  = ( HWI_InternalBufferAudio[0]  + 0x8000);
			HWI_I2S_Value = (U8)((HWI_I2S_ValueU16 >> (DECALAGE_AUDIO+offset_decalage))&0x00FF);
			//unlock HAL_SAI_Receive_IT
			__HAL_UNLOCK(&SAIHandle);
			//ask new data
			HAL_SAI_Receive_IT(&SAIHandle,HWI_AIS_Buffer,HWI_SAI_IN_NB_CHANNELS);
		}
	}
}

/*******************************************************************************
 *
 *  Function Name   : HAL_SAI_ErrorCallback
 *  Description     : HAL_SAI_ErrorCallback
 *
 *
 *******************************************************************************/
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
	if(hsai->Instance==SAI1_Block_A)
	{
		if (TRUE == HWI_I2S_running)
		{
			 __HAL_SAI_DISABLE(hsai);
			//unlock HAL_SAI_Receive_IT
			__HAL_UNLOCK(&SAIHandle);
			//ask new data
			HAL_SAI_Receive_IT(&SAIHandle,HWI_AIS_Buffer,HWI_SAI_IN_NB_CHANNELS);
		}
	}
}

