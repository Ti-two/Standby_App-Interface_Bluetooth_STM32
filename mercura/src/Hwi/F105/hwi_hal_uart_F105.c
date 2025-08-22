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
#include "hwi_hal_uart.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_nvic_priority.h"
#include "hwi_bx310x.h"
#include "lib_macros.h"
#include "p_l_bx310x.h"

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
U8 	                	HWI_Uart_wifi_received_string[TAILLE_BUFFER_RX_RS]; // this will hold the recieved string

U16                 	HWI_Uart_wifi_ptr_received_string = 0; // this will hold the recieved string

UART_HandleTypeDef  	RS232UartHandle;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/

/*******************************************************************************
 *
 *  Function Name   : HWI_Uart_Fixe_Init
 *  Description     : initialisation de l uart fixe
 *
 *
 *******************************************************************************/
HAL_StatusTypeDef  HWI_Uart_Init(U32 BaudeRate,BOOL_TYPE HWControl_RTS_CTS)
{
HAL_StatusTypeDef result;

	RS232UartHandle.Instance        = USART1;
	RS232UartHandle.Init.BaudRate   = BaudeRate;
	RS232UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	RS232UartHandle.Init.StopBits   = UART_STOPBITS_1;
	RS232UartHandle.Init.Parity     = UART_PARITY_NONE;
	if (TRUE == HWControl_RTS_CTS)
	{
		RS232UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_RTS_CTS;
	}
	else
    {
        RS232UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    }

	RS232UartHandle.Init.Mode       = UART_MODE_TX_RX;
    RS232UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	result = HAL_UART_Init(&RS232UartHandle);

	__HAL_UART_CLEAR_FLAG(&RS232UartHandle,UART_IT_RXNE);
	__HAL_UART_CLEAR_FLAG(&RS232UartHandle,UART_FLAG_CTS);

	    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_DISABLE_IT(&RS232UartHandle, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_DISABLE_IT(&RS232UartHandle, UART_IT_ERR);

	HWI_Uart_wifi_ptr_received_string = 0;

	return result;
}


/*******************************************************************************
 *
 *  Function Name   : HWI_Reset_Rs232_Ext_Cmd
 *  Description     : commande du reset du module RS232 EXT
 *
 *******************************************************************************/
void HWI_EN_Rs232_Cmd(/*UART_HandleTypeDef* huart,*/BOOL_TYPE cmd)
{
//    if(huart->Instance==USART1)
//    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,(GPIO_PinState) cmd);
//    }
}

/*******************************************************************************
 *
 *  Function Name   : HWI_RTS_Rs232_Ext_Cmd
 *  Description     : commande du CTS du module RS232 EXT
 *
 *******************************************************************************/
void HWI_RTS_Rs232_Cmd(UART_HandleTypeDef* huart,BOOL_TYPE cmd)
{
//    if(huart->Instance==USART1)
//    {
        if  (RS232UartHandle.Init.HwFlowCtl == UART_HWCONTROL_NONE)
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,(GPIO_PinState) cmd);
        }
//    }
}

/*******************************************************************************
 *
 *  Function Name   : HAL_UART_MspInit
 *  Description     : initialisation de la hal
 *
 *
 *******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
GPIO_InitTypeDef GPIO_InitStruct;


    if(huart->Instance==USART1)
    {
       // Peripheral clock enable
		__HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**USART1 GPIO Configuration
        PA11     ------> USART1_CTS
        PA12     ------> USART1_RTS
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX

        */

        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        if  (huart->Init.HwFlowCtl == UART_HWCONTROL_NONE)
        {
            GPIO_InitStruct.Pin = GPIO_PIN_9;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_12;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        }
        else
        {
            GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_12;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        }

        /* Enable the UART Data Register not empty Interrupt */
		__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

		/* Peripheral interrupt init*/
		/* Sets the priority grouping field */
		HAL_NVIC_SetPriority(USART1_IRQn, HAL_NVIC_PREEMPT_PRIORITY_UART_RS232_NORMAL, HAL_NVIC_SUB_PRIORITY_UART_RS232_NORMAL);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

/////////////////////////////////////////////////////////////////
//              commande en RF

        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    }
}

/*******************************************************************************
 *
 *  Function Name   : HAL_UART_MspDeInit
 *  Description     : initialisation de la hal
 *
 *
 *******************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

    if(huart->Instance==USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        PA11     ------> USART1_CTS
        PA12     ------> USART1_RTS
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);

        // Peripheral interrupt Deinit
		HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}



/*******************************************************************************
*
*  Function Name   : HWI_Uart_Fixe_rx
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
void HWI_Uart_rx(UART_HandleTypeDef* huart)
{
U8 receive_data;


/*
    //test si it reception de trame
	if( __HAL_UART_GET_IT_SOURCE(huart,UART_IT_RXNE))
    {

        receive_data = (U8)(huart->Instance->DR & (uint16_t)0x00FF);
        //clear flag it reception de trame
        __HAL_UART_CLEAR_FLAG(huart,UART_IT_RXNE);
            __HAL_UART_CLEAR_PEFLAG(huart);

        if(huart->Instance==RS232UartHandle.Instance)
        {
             if (('\r' == receive_data)||('\n' == receive_data))
             {
                 if (('\r' == receive_data)&& (HWI_Uart_wifi_ptr_received_string !=0) )
                 {
                     P_L_Wifi_Rx(HWI_Uart_wifi_ptr_received_string,HWI_Uart_wifi_received_string);
                 }

                 HWI_Uart_wifi_ptr_received_string = 0;
             }
             else if(HWI_Uart_wifi_ptr_received_string < TAILLE_BUFFER_RX_RS)
             {
                HWI_Uart_wifi_received_string[HWI_Uart_wifi_ptr_received_string] = (U8)receive_data;
                HWI_Uart_wifi_ptr_received_string++;
             }
             else
             {
                HWI_Uart_wifi_ptr_received_string = 0;
             }
        }
    }
*/
/*
    __HAL_UART_CLEAR_PEFLAG(huart);

    if( __HAL_UART_GET_IT_SOURCE(huart,UART_IT_RXNE))
    {

        if(huart->Instance==RS232UartHandle.Instance)
        {
            receive_data = (U8)(huart->Instance->DR & (uint16_t)0x00FF);
            //clear flag it reception de trame
            __HAL_UART_CLEAR_FLAG(huart,UART_IT_RXNE);

            if ('\n' == receive_data)
             {
                 HWI_Uart_wifi_received_string[HWI_Uart_wifi_ptr_received_string] = (U8)receive_data;
                 HWI_Uart_wifi_ptr_received_string++;

                 if (HWI_Uart_wifi_ptr_received_string !=2)
                 {
                     P_L_Wifi_Rx(HWI_Uart_wifi_ptr_received_string,HWI_Uart_wifi_received_string);
                 }

                 HWI_Uart_wifi_ptr_received_string = 0;
             }
             else if(HWI_Uart_wifi_ptr_received_string < TAILLE_BUFFER_RX_RS)
             {
                HWI_Uart_wifi_received_string[HWI_Uart_wifi_ptr_received_string] = (U8)receive_data;
                HWI_Uart_wifi_ptr_received_string++;
             }
             else
             {
                HWI_Uart_wifi_ptr_received_string = 0;
             }
        }
    }
*/

    __HAL_UART_CLEAR_PEFLAG(huart);

    if( __HAL_UART_GET_IT_SOURCE(huart,UART_IT_RXNE))
    {

        if(huart->Instance==RS232UartHandle.Instance)
        {
            receive_data = (U8)(huart->Instance->DR & (uint16_t)0x00FF);


             if (0 == receive_data)
             {

             }
             else if ('\n' == receive_data)
             {
                 if(HWI_Uart_wifi_ptr_received_string < TAILLE_BUFFER_RX_RS)
                 {
                    HWI_Uart_wifi_received_string[HWI_Uart_wifi_ptr_received_string] = (U8)receive_data;
                    HWI_Uart_wifi_ptr_received_string++;
                 }

                 if (HWI_Uart_wifi_ptr_received_string !=2)
                 {
                     P_L_Bx310x_Rx(HWI_Uart_wifi_ptr_received_string,HWI_Uart_wifi_received_string);
                 }

                 HWI_Uart_wifi_ptr_received_string = 0;
             }
             else if(HWI_Uart_wifi_ptr_received_string < TAILLE_BUFFER_RX_RS)
             {
                HWI_Uart_wifi_received_string[HWI_Uart_wifi_ptr_received_string] = (U8)receive_data;
                HWI_Uart_wifi_ptr_received_string++;
             }
             else
             {
                HWI_Uart_wifi_ptr_received_string = 0;
             }

            //clear flag it reception de trame
            __HAL_UART_CLEAR_FLAG(huart,UART_IT_RXNE);

        }
    }
}


/*******************************************************************************
 *
 *  Function Name   : HWI_Uart_tx
 *  Description     :
 * 						envoie d une trame rs
 *******************************************************************************/
void  HWI_Uart_tx(U16 data_length, U8* data,U32 timeout){
    HAL_UART_Transmit(&RS232UartHandle,data,data_length, timeout);
}


/*******************************************************************************
 *
 *  Function Name   : HWI_Configuration_Rs232_High_Prio
 *  Description     : passe le wifi en high prio
 *
 *******************************************************************************/
void HWI_Configuration_Rs232_High_Prio()
{
    HAL_NVIC_SetPriority(USART1_IRQn, HAL_NVIC_PREEMPT_PRIORITY_UART_RS232_HIGHT, HAL_NVIC_SUB_PRIORITY_UART_RS232_HIGHT);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Configuration_Rs232_Normal_Prio
 *  Description     : passe le wifi en normal prio
 *
 *******************************************************************************/
void HWI_Configuration_Rs232_Normal_Prio()
{
    HAL_NVIC_SetPriority(USART1_IRQn, HAL_NVIC_PREEMPT_PRIORITY_UART_RS232_NORMAL, HAL_NVIC_SUB_PRIORITY_UART_RS232_NORMAL);
}
