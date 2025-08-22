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
#include "hwi_hal_system_clock.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "S_S_Scheduler.h"
#include "p_l.h"
#include "hwi_nvic_priority.h"
#include "app_manage_boot_version.h"


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
//U32 HSE_VALUE;


/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static void ResetSysClock(void);

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
 *
 *  Function Name   : HWI_SystemClock_Config
 *  Description     : SystemClock
 *
 *
 *******************************************************************************/
void HWI_SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	HAL_RCC_DeInit();
//	HSE_VALUE   = HWI_QUARTZ_20_MHz;
	APP_Test_Version_Boot();
//	ResetSysClock();
//	SystemCoreClockUpdate();

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

	// Configure Flash prefetch
#if (PREFETCH_ENABLE != 0)
#if defined(STM32F101x6) || defined(STM32F101xB) || defined(STM32F101xE) || defined(STM32F101xG) || \
  defined(STM32F102x6) || defined(STM32F102xB) || \
  defined(STM32F103x6) || defined(STM32F103xB) || defined(STM32F103xE) || defined(STM32F103xG) || \
  defined(STM32F105xC) || defined(STM32F107xC)

// Prefetch buffer is not available on value line devices
	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif
#endif// PREFETCH_ENABLE

	// Set Interrupt Group Priority
 	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUPE);

 	// Use systick as time base source and configure 1ms tick (default clock after Reset is HSI)
	//Configure the SysTick to have interrupt in 1ms time basis
	 HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

     HAL_NVIC_SetPriority(SysTick_IRQn, HAL_NVIC_PREEMPT_PRIORITY_STICK ,HAL_NVIC_SUB_PRIORITY_STICK);

     uwTickPrio = HAL_NVIC_PREEMPT_PRIORITY_STICK;

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    // Set the Vector Table base location
#ifdef _SANSBOOT
    SCB->VTOR = P_L_BOOT_VECTOR_ADR;
#else
    SCB->VTOR = P_L_FIRST_USER_FLASH_ADR;
#endif

     //pour ne pas avoir de hard fault avec la division par zero
     SCB->CCR =  SCB->CCR & (!SCB_CCR_DIV_0_TRP_Msk);

	// Initializes the RCC Oscillators according to the specified parameters
	// in the RCC_OscInitTypeDef structure.

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;
	RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	// Initializes the CPU, AHB and APB buses clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2S2;
	PeriphClkInit.I2s2ClockSelection = RCC_I2S2CLKSOURCE_PLLI2S_VCO;
    PeriphClkInit.PLLI2S.PLLI2SMUL = RCC_PLLI2S_MUL8;
    PeriphClkInit.PLLI2S.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV8;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_RCC_PLLI2S_ENABLE();

	SystemCoreClockUpdate();
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
}

/*******************************************************************************
 *
 *  Function Name   : ResetSysClock
 *  Description     : ResetSysClock and unlock for old boot without HAL
 *
 *
 *******************************************************************************/
static void ResetSysClock(void)
{
__IO U32 StartUpCounter = 0,HSEStatus;

	// Reset the RCC clock configuration to the default reset state(for debug purpose)
	// Set HSION bit
	RCC->CR |= (uint32_t)0x00000001;

	// Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits
	RCC->CFGR &= (uint32_t)0xF8FF0000;

	// Reset HSEON, CSSON and PLLON bits
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	// Reset HSEBYP bit
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	// Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits
	RCC->CFGR &= (uint32_t)0xFF80FFFF;

	// Disable all interrupts and clear pending bits
	RCC->CIR = 0x009F0000;

	// SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------
	// Enable HSE
	RCC->CR |= ((U32)RCC_CR_HSEON);

	// Wait till HSE is ready and if Time out is reached exit
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	}
	while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        // Configure PLLs ------------------------------------------------------
        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);


        //Clock sans passer par la PLL2
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV1SRC_HSE);
    }
}
