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
/* NONE*/

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
//	SystemCoreClockUpdate();

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
	  /* Configure Flash prefetch, Instruction cache, Data cache */
	  /* Default configuration at reset is:                      */
	  /* - Prefetch disabled                                     */
	  /* - Instruction cache enabled                             */
	  /* - Data cache enabled                                    */
#if (INSTRUCTION_CACHE_ENABLE == 0)
   __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
#endif /* INSTRUCTION_CACHE_ENABLE */

#if (DATA_CACHE_ENABLE == 0)
   __HAL_FLASH_DATA_CACHE_DISABLE();
#endif /* DATA_CACHE_ENABLE */

#if (PREFETCH_ENABLE != 0)
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */


	// Set Interrupt Group Priority
 	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUPE);

 	// Use systick as time base source and configure 1ms tick (default clock after Reset is HSI)
	//Configure the SysTick to have interrupt in 1ms time basis
	 HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

     HAL_NVIC_SetPriority(SysTick_IRQn, HAL_NVIC_PREEMPT_PRIORITY_STICK ,HAL_NVIC_SUB_PRIORITY_STICK);

     uwTickPrio = HAL_NVIC_PREEMPT_PRIORITY_STICK;

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

 //   __HAL_AFIO_REMAP_SWJ_NOJTAG(); pas dans la Lib L431

    // Set the Vector Table base location
#ifdef _SANSBOOT
    SCB->VTOR = P_L_BOOT_VECTOR_ADR; // Vector Table Relocation in Internal FLASH
#else
    SCB->VTOR = P_L_FIRST_USER_FLASH_ADR;
#endif


     //pour ne pas avoir de hard fault avec la division par zero
     SCB->CCR =  SCB->CCR & (!SCB_CCR_DIV_0_TRP_Msk);


	// Configure the main internal regulator output voltage
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	// Initializes the RCC Oscillators according to the specified parameters
	// in the RCC_OscInitTypeDef structure.
	//

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 2;
	RCC_OscInitStruct.PLL.PLLN = 14;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	//Initializes the CPU, AHB and APB buses clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}

    SystemCoreClockUpdate();
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
}

