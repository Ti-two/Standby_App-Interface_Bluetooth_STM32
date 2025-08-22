/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _HWI_UART_H_
#define _HWI_UART_H_
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define TAILLE_BUFFER_RX_RS	        300
#define TAILLE_BUFFER_RX_RS_DECODE	140
/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_Configuration_Rs232_High_Prio(void);
extern void HWI_Configuration_Rs232_Normal_Prio(void);
extern void Hwi_RS232_time(void);
extern void HWI_Uart_tx(U16 data_length, U8* data,U32 timeout);
extern void HWI_It_uart(void);
extern HAL_StatusTypeDef HWI_Uart_Init(U32 BaudeRate,BOOL_TYPE HWControl_RTS_CTS);
extern void HWI_EN_Rs232_Cmd(/*UART_HandleTypeDef* huart,*/BOOL_TYPE cmd);

#ifdef STM32F105xC
extern void HWI_Uart_rx(UART_HandleTypeDef* huart);
#endif
#endif
