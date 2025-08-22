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
#include "hwi_hal_timer.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_nvic_priority.h"
#include "S_S_Scheduler.h"

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
extern U8 						HWI_I2S_Value;
extern P_L_BT_AUDIO_STATE_TYPE 	P_L_BT_audio_state;
#endif
#endif

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
TIM_HandleTypeDef    	TimSDDelayHandle;			//TIM 1

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
TIM_HandleTypeDef    	TimPWMAudioHandle;			//TIM 2
#endif
#endif

#ifdef _PROGRAMATION
TIM_HandleTypeDef    	Tim1msReprogHandle;	    	//TIM 4
#endif

TIM_HandleTypeDef    	Tim1msHandle;			    //TIM 5

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/* NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/


/********************************************************************************
*
*  Function Name   : HWI_Timer_1ms_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption
*
*
*******************************************************************************/
void HWI_Timer_1ms_Init(void)
{
	// initialize hardware timer
	U32 uwPrescalerValue = 0;
	U32 uwPeriodValue = 0;

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};

	// (1/((APB1TimerClock)/uwPrescalerValue))*uwPeriodValue = IT toutes les x secondes
	// (uwPrescalerValue/APB1TimerClock)*uwPeriodValue = IT toutes les x secondes
	// (uwPrescalerValue*uwPeriodValue)/APB1TimerClock = IT toutes les x secondes

	// (uwPrescalerValue*uwPeriodValue) = (IT toutes les x secondes)*APB1TimerClock
	// uwPrescalerValue =   ((IT toutes les x secondes)*APB1TimerClock)/uwPeriodValue
	// it at 1ms
	// APB1TimerClock = 90 000 000 = 2* HAL_RCC_GetPCLK1Freq()
	// uwPeriodValue = PERIOD_VALUE_1MS = 1000
	// uwPrescalerValue =   ((1/1000)*2* HAL_RCC_GetPCLK1Freq())/uwPeriodValue
	// uwPrescalerValue =  ( 2* HAL_RCC_GetPCLK1Freq() *( 1/1000))/uwPeriodValue
	// uwPrescalerValue =  ( 2* HAL_RCC_GetPCLK1Freq())/(uwPeriodValue*1000)
	uwPeriodValue = PERIOD_VALUE_1MS;//1000;
	uwPrescalerValue = (uint32_t) ( ((HAL_RCC_GetPCLK1Freq())*2)/(uwPeriodValue*1000));

	//##-1- Configure the TIM peripheral #######################################
	// Initialize TIMx peripheral as follow:
	//+ Prescaler = (SystemCoreClock/2)/18000000
	//+ Period = 18000  (to have an output frequency = Prescaler/Period equal to 1 KHz = 1ms)
	//+ ClockDivision = 0
	//+ Counter direction = Up

	Tim1msHandle.Instance = TIM5;
	Tim1msHandle.Init.Period = uwPeriodValue-1;
	Tim1msHandle.Init.Prescaler = uwPrescalerValue-1;			// incrementation tous les TIMxCLK divis� par uwPrescalerValue
	Tim1msHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;						// Not dividing
	Tim1msHandle.Init.CounterMode = TIM_COUNTERMODE_UP;		// mode comptage  configuration
	Tim1msHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	 if(HAL_TIM_Base_Init(&Tim1msHandle) != HAL_OK)
	 {
		// Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	 }

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&Tim1msHandle, &sClockSourceConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}


	 sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
	 sSlaveConfig.InputTrigger = TIM_TS_ITR0;
	 if (HAL_TIM_SlaveConfigSynchro(&Tim1msHandle, &sSlaveConfig) != HAL_OK)
	 {
		 Error_Handler();
	 }

	 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	 if (HAL_TIMEx_MasterConfigSynchronization(&Tim1msHandle, &sMasterConfig) != HAL_OK)
	 {
		_Error_Handler(__FILE__, __LINE__);
	 }

	 //##-2- Start the TIM Base generation in interrupt mode ####################
	 // Start Channel1
	 if(HAL_TIM_Base_Start_IT(&Tim1msHandle) != HAL_OK)
	 {
		// Starting Error
		_Error_Handler(__FILE__, __LINE__);
	 }
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_1ms_Start
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption
*
*
*
*******************************************************************************/
void HWI_Timer_1ms_Start(void)
{
    if(HAL_TIM_Base_Start_IT(&Tim1msHandle) != HAL_OK)
    {
        // Starting Error
        _Error_Handler(__FILE__, __LINE__);
    }
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_1ms_Stop
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption
*
*
*
*******************************************************************************/
void HWI_Timer_1ms_Stop(void)
{
    if(HAL_TIM_Base_Stop_IT(&Tim1msHandle) != HAL_OK)
    {
        // Starting Error
        _Error_Handler(__FILE__, __LINE__);
    }
}

#ifdef _PROGRAMATION
/********************************************************************************
*
*  Function Name   : HWI_Timer_1MS_Reprog_init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption timer 1
*
*	Timer TIM2
*
*******************************************************************************/
void HWI_Timer_1MS_Reprog_init(void) {
// initialize hardware timer
U32 uwPrescalerValue = 0;
U32 uwPeriodValue = 0;

TIM_ClockConfigTypeDef sClockSourceConfig = {0};

	uwPeriodValue = PERIOD_VALUE_1MS;//1000;
	uwPrescalerValue = (uint32_t) ( ((HAL_RCC_GetPCLK1Freq())*2)/(uwPeriodValue*1000));

	/*##-1- Configure the TIM peripheral #######################################*/
	/* Initialize TIMx peripheral as follow:
	+ Prescaler = (SystemCoreClock/2)/18000000
	+ Period = 18000  (to have an output frequency = Prescaler/Period equal to 1 KHz = 1ms)
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	Tim1msReprogHandle.Instance = TIM4;
	Tim1msReprogHandle.Init.Period = uwPeriodValue;
	Tim1msReprogHandle.Init.Prescaler = uwPrescalerValue;			// incrementation tous les TIMxCLK divisé par uwPrescalerValue
	Tim1msReprogHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;						// Not dividing
	Tim1msReprogHandle.Init.CounterMode = TIM_COUNTERMODE_UP;		// mode comptage  configuration
	Tim1msReprogHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	 if(HAL_TIM_Base_Init(&Tim1msReprogHandle) != HAL_OK)
	 {
	    /* Initialization Error */
	    _Error_Handler(__FILE__, __LINE__);
	 }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&Tim1msReprogHandle, &sClockSourceConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

	 /*##-2- Start the TIM Base generation in interrupt mode ####################*/
	 /* Start Channel1 */
	 if(HAL_TIM_Base_Start_IT(&Tim1msReprogHandle) != HAL_OK)
	 {
	    /* Starting Error */
	    _Error_Handler(__FILE__, __LINE__);
	 }
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_1MS_Reprog_start
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption timer 1
*
*	Timer TIM2
*
*******************************************************************************/
void HWI_Timer_1MS_Reprog_start(void)
{
	 if(HAL_TIM_Base_Start_IT(&Tim1msReprogHandle) != HAL_OK)
     {
        /* Starting Error */
        _Error_Handler(__FILE__, __LINE__);
     }
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_1MS_Reprog_stop
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : interruption timer 1
*
*	Timer TIM2
*
*******************************************************************************/
void HWI_Timer_1MS_Reprog_stop(void)
{
	 if(HAL_TIM_Base_Stop_IT(&Tim1msReprogHandle) != HAL_OK)
     {
        /* Starting Error */
        _Error_Handler(__FILE__, __LINE__);
     }
}

#endif

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
/********************************************************************************
*
*  Function Name   : HWI_Timer_PWM_Audio_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : PWM audio
*
*
*
*******************************************************************************/
void HWI_Timer_PWM_Audio_Init(void)
{
	 HWI_Pwm_IT_Pont_H(FREQUENCE_20kHz);
}

/*******************************************************************************
*
*  Function Name   :  HWI_Pwm_IT_Puissance funtion
*  Model Reference :
*  Description     : Gere l' arret le signal de PMW puissance
*
*******************************************************************************/
void HWI_Pwm_IT_Pont_H(HWI_AUDIO_FREQUENCE audio_frequence)
{
U32 uwPrescalerValue = 0;
U32 uwPeriodValue = 0;
TIM_MasterConfigTypeDef sMasterConfig = {0};
TIM_SlaveConfigTypeDef sSlaveConfig = {0};
TIM_OC_InitTypeDef      sConfigOC = {0};


	//##-1- Configure the TIM peripheral #######################################
	// Initialize TIMx peripheral as follow:
	//+ Prescaler = (SystemCoreClock/2)/18000000
	//+ Period = 18000  (to have an output frequency = Prescaler/Period equal to 1 KHz = 1ms)
	//+ ClockDivision = 0
	//+ Counter direction = Up

	// (1/((APB1TimerClock)/uwPrescalerValue))*uwPeriodValue = IT toutes les x secondes
	// (uwPrescalerValue/APB1TimerClock)*uwPeriodValue = IT toutes les x secondes
	// (uwPrescalerValue*uwPeriodValue)/APB1TimerClock = IT toutes les x secondes

	uwPeriodValue = PERIOD_VALUE_PWM; 		 //0xFF  //valeur max de comptage =  valeur max ADC
													  // (uwPrescalerValue*2047)/APB1TimerClock = IT toutes les x secondes
	//valeur pour avoir 50ys de periode par rapport a la valeur max de comptage
	// (1*2047)/32000000 = 64ys
	if (FREQUENCE_40kHz == audio_frequence)
	{
		uwPrescalerValue = PRESCALER_VALUE_PWM_40KHz;
	}
	else
	{
		uwPrescalerValue = PRESCALER_VALUE_PWM_20KHz;
	}

	TimPWMAudioHandle.Instance = TIM2;
	TimPWMAudioHandle.Init.Period = uwPeriodValue-1;
	TimPWMAudioHandle.Init.Prescaler =  uwPrescalerValue-1;
	TimPWMAudioHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimPWMAudioHandle.Init.CounterMode = TIM_COUNTERMODE_UP;		// mode comptage  configuration
	TimPWMAudioHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	if(HAL_TIM_PWM_Init(&TimPWMAudioHandle) != HAL_OK)
	{
		// Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
	sSlaveConfig.InputTrigger = TIM_TS_ITR0;
	if (HAL_TIM_SlaveConfigSynchro(&TimPWMAudioHandle, &sSlaveConfig) != HAL_OK)
	{
	    Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&TimPWMAudioHandle, &sMasterConfig) != HAL_OK)
	{
		 // Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWMAudioHandle, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		 // Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	if (HAL_TIM_PWM_ConfigChannel(&TimPWMAudioHandle, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		// Initialization Error
		_Error_Handler(__FILE__, __LINE__);
	}

////////////////////////////////////////////////////////////
	// Start channel 1
	if(HAL_TIM_PWM_Start(&TimPWMAudioHandle, TIM_CHANNEL_1) != HAL_OK)
	{
		// Starting Error
		_Error_Handler(__FILE__, __LINE__);
	}

	// Start channel 2
	if(HAL_TIM_PWM_Start(&TimPWMAudioHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		// Starting Error
		_Error_Handler(__FILE__, __LINE__);
	}
////////////////////////////////////////////////////////////

	HWI_Timer_PWM_Audio_Start();
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_PWM_Audio_Start
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*
*
*******************************************************************************/
void HWI_Timer_PWM_Audio_Start(void)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

	TIM2->CCR1 = MIDLE_VALUE_AUDIO;
	TIM2->CCR2 = MIDLE_VALUE_AUDIO;

	APP_Timer_Memo_CCR1 = MIDLE_VALUE_AUDIO;
	APP_Timer_Memo_CCR2 = MIDLE_VALUE_AUDIO;


	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	if(HAL_TIM_Base_Start_IT(&TimPWMAudioHandle) != HAL_OK)
	{
		// Starting Error
		_Error_Handler(__FILE__, __LINE__);
	}
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_PWM_Audio_Stop
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*
*
*******************************************************************************/
void HWI_Timer_PWM_Audio_Stop(void)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(HAL_TIM_Base_Stop_IT(&TimPWMAudioHandle) != HAL_OK)
	{
		// Starting Error
		_Error_Handler(__FILE__, __LINE__);
	}

	TIM2->CCR1 = REPOS_VALUE_AUDIO_CCR1;
	TIM2->CCR2 = REPOS_VALUE_AUDIO_CCR2;

	APP_Timer_Memo_CCR1 = REPOS_VALUE_AUDIO_CCR1;
	APP_Timer_Memo_CCR2 = REPOS_VALUE_AUDIO_CCR2;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure Py.x as Output push-pull
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,(GPIO_PinState) GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(GPIO_PinState) GPIO_PIN_RESET);
}

/*******************************************************************************
*
*  Function Name   :  HWI_Pwm_output_enable funtion
*  Model Reference :
*  Description     : Gere l' arret du siganl PMW audio
*
*******************************************************************************/
void HWI_Pwm_IT_Son(FunctionalState etat)
{
    if (etat == DISABLE)
    {
    	HWI_Timer_PWM_Audio_Stop();
    }
    else
    {
    	HWI_Timer_PWM_Audio_Start();
    }
}
#endif
#endif

/********************************************************************************
*
*  Function Name   : HAL_TIM_Base_MspInit
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : hal initialisation
*
*********************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM5_IRQn, HAL_NVIC_PREEMPT_PRIORITY_T_1_MS, HAL_NVIC_SUB_PRIORITY_T_1_MS);
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
	}
#ifdef _PROGRAMATION
  else if(htim_base->Instance==TIM4)
  {
        __HAL_RCC_TIM4_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM4_IRQn, HAL_NVIC_PREEMPT_PRIORITY_T_1_MS_REPROG, HAL_NVIC_SUB_PRIORITY_T_1_MS_REPROG);
	    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }
#endif
}

/********************************************************************************
*
*  Function Name   : HAL_TIM_Base_MspDeInit
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : hal initialisation
*
*********************************************************************************/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

	if(htim_base->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(TIM5_IRQn);
	}
#ifdef _PROGRAMATION
    else if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(TIM4_IRQn);
	}
#endif
}

/********************************************************************************
*
*  Function Name   : HAL_TIM_PWM_MspInit
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : hal initialisation
*
*********************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
    if(htim_pwm->Instance==TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        // TIM2 interrupt Init
        HAL_NVIC_SetPriority(TIM2_IRQn, HAL_NVIC_PREEMPT_PRIORITY_AUDIO, HAL_NVIC_SUB_PRIORITY_AUDIO);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}



/********************************************************************************
*
*  Function Name   : HAL_TIM_PWM_MspDeInit
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : hal initialisation
*
*********************************************************************************/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
    if(htim_pwm->Instance==TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}

/*******************************************************************************
*
*  HAL_TIM_PeriodElapsedCallback
* @brief  Period elapsed callback in non blocking mode
* @param  htim: TIM handle
* @retval None
*
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
U8 	value;

	if(htim->Instance==TIM2)
	{
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
		if (audio_stream_started == P_L_BT_audio_state)
		{
			value = HWI_I2S_Value;
		}
		else
		{
			value = 0x7F;
		}

		//   DISABLE the TIM Counter
		TIM2->CR1 &= ((uint16_t)0x03FE);

		TIM2->CCR1 = value;
		TIM2->CCR2 = value;

		//remise en route du compteur
		TIM2->CR1 |= 0x0001;

		// on reste le pending bit de l IT
		__HAL_TIM_CLEAR_IT(&TimPWMAudioHandle, TIM_IT_UPDATE);
#endif
#endif

	}
#ifdef _PROGRAMATION
    else if(htim->Instance==TIM4)
	{//it a 1ms de scheduler
		 APP_Manage_1ms_Flashage();
	}
#endif
	else if(htim->Instance==TIM5)
	{//it a 1ms de scheduler
		S_S_Scheduler_Callback();
	}

}


/********************************************************************************
*
*  Function Name   : HWI_Timer_Sd_Delay_Init
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*
*
*
*******************************************************************************/
void HWI_Timer_Sd_Delay_Init(void)
{
	// Enable Timer Clock
	__HAL_RCC_TIM1_CLK_ENABLE();
	TimSDDelayHandle.Instance = TIM1;
	TimSDDelayHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	// RCC_Clocks/1000 for 1ms /1000000 for 1us, !MAx value 0xFFFF = 65535 !;
	TimSDDelayHandle.Init.Prescaler = (HAL_RCC_GetHCLKFreq()/1000000);
	TimSDDelayHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TimSDDelayHandle.Init.Period = 65535;
	TimSDDelayHandle.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&TimSDDelayHandle);
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_Sd_Delay_µs
*  Description     : delay in µs
*
*******************************************************************************/
void HWI_Timer_Sd_Delay_us(U16 delay_time)
{
	__HAL_TIM_SET_COUNTER(&TimSDDelayHandle,0);
	HAL_TIM_Base_Start(&TimSDDelayHandle);
	// wait
	while(__HAL_TIM_GET_COUNTER(&TimSDDelayHandle) != delay_time)
	{

	};
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_Sd_Delay_ms
*  Description     : delay in ms
*
*******************************************************************************/
void HWI_Timer_Sd_Delay_ms(U32 delay_time)
{
	if(delay_time > 0 )
	{
		do
		{
			HWI_Timer_Sd_Delay_us(1000);
		}
		while(--delay_time);
	}
}

/********************************************************************************
*
*  Function Name   : HWI_Timer_Sd_Delay_s
*  Description     : delay in s
*
*******************************************************************************/
void HWI_Timer_Sd_Delay_s(U32 delay_time)
{
	if(delay_time > 0 )
	{
		do
		{
			HWI_Timer_Sd_Delay_ms(1000);
		}
		while(--delay_time);
	}
}
