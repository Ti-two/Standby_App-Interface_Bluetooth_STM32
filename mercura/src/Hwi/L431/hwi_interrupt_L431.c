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
#include "hwi_interrupt.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/


/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
extern TIM_HandleTypeDef    	TimPWMAudioHandle;			//TIM 2
#endif
#endif

#ifdef _PROGRAMATION
extern TIM_HandleTypeDef    	Tim1msReprogHandle;	    	//TIM 7
#endif

extern TIM_HandleTypeDef    	TimSDDelayHandle;			//TIM 15

extern TIM_HandleTypeDef    	Tim1msHandle;			    //TIM 16

extern CAN_HandleTypeDef    	Can1Handle;
extern UART_HandleTypeDef   	RS232UartHandle;

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
extern SAI_HandleTypeDef    	SAIHandle;
#endif
#endif

extern U32						S_S_1MS_U32_TIME_HIGH_PRIORITY;

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{

}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
/* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
 /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
 /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
 /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{

}


/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}



/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{



}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
	HAL_FLASH_IRQHandler();
}


/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
	HAL_IncTick();
	S_S_1MS_U32_TIME_HIGH_PRIORITY++;
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TimPWMAudioHandle);
}
#endif
#endif

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef _PROGRAMATION
void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&Tim1msReprogHandle);
}
#endif

/*******************************************************************************
* Function Name  : TIM1_UP_TIM16_IRQHandler
* Description    : This function handles TIM1 update interrupt and TIM16 global interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_TIM16_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&Tim1msHandle);
}


/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : This function handles CAN1 RX0 interrupt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&Can1Handle);
}

/*******************************************************************************
* Function Name  : CAN1_TX_IRQHandler
* Description    : This function handles CAN1 TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&Can1Handle);
}

/*******************************************************************************
* Function Name  : SAI1_IRQHandler
* Description    : This function handles SAI interrupt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
void SAI1_IRQHandler(void)
{
	HAL_SAI_IRQHandler(&SAIHandle);
}
#endif
#endif

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
    //HWI_Uart_rx(&RS232UartHandle);
	//My_HAL_UART_IRQHandler(&RS232UartHandle);
	HAL_UART_IRQHandler(&RS232UartHandle);
}
