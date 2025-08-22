/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_GPIO_H
#define __HWI_GPIO_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "lib_filter.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
/*
#define HWI_GPIO_READ_BIT(HWI_Cmd_port_bit) \
	HAL_GPIO_ReadPin(HWI_Cmd_port_bit.GPIOx, HWI_Cmd_port_bit.GPIO_Pin)\

#define HWI_GPIO_SET_BIT(HWI_Cmd_port_bit)  \
      HAL_GPIO_WritePin(HWI_Cmd_port_bit.GPIOx,HWI_Cmd_port_bit.GPIO_Pin,(GPIO_PinState) GPIO_PIN_SET)\

#define HWI_GPIO_RESET_BIT(HWI_Cmd_port_bit)  \
      HAL_GPIO_WritePin(HWI_Cmd_port_bit.GPIOx,HWI_Cmd_port_bit.GPIO_Pin,(GPIO_PinState) GPIO_PIN_RESET)\


typedef struct
{
    GPIO_TypeDef* GPIOx;
    U16 GPIO_Pin;
}HWI_GPIO_CMD_TYPE;


typedef struct
{
	GPIO_TypeDef* GPIOx;
    U16 GPIO_Pin;
}HWI_GPIO_INPUT_TYPE;

*/

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_Rst_Latch_WU_Cmd(BOOL_TYPE cmd);
extern BOOL_TYPE HWI_Info_Reveil_read(void);
extern void HWI_Reveil_Cmd(BOOL_TYPE cmd);
extern BOOL_TYPE HWI_Button_read(void);
extern void HWI_Led_1_Rouge_Cmd(BOOL_TYPE cmd);
extern void HWI_Led_1_Verte_Cmd(BOOL_TYPE cmd);
extern void HWI_Led_2_Rouge_Cmd(BOOL_TYPE cmd);
extern void HWI_Led_2_Verte_Cmd(BOOL_TYPE cmd);
extern BOOL_TYPE HWI_Presence_SD_read(void);
extern void HWI_Hal_Gpio_Init(void);
#endif
