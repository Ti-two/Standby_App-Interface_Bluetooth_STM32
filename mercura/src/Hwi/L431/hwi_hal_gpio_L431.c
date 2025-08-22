/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"
#include "hwi_hal_gpio.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
/*NONE*/

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
/* NONE*/

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
static void HWI_Rst_Latch_WU_int(void);
static void HWI_Info_Reveil_int(void);
static void HWI_Reveil_init(void);
static void HWI_Button_int(void);
static void HWI_Led_2_Rouge_init(void);
static void HWI_Led_2_Verte_init(void);
static void HWI_Version_int(void);
static void HWI_Presence_SD_int(void);
static void HWI_BF_1_init(void);
static void HWI_BF_2_init(void);
/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE RESET LATCH
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_BF_1_init
 *  Description     : init de la BF 1
 *
 *******************************************************************************/
static void HWI_BF_1_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	// Peripheral clock enable
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,(GPIO_PinState) GPIO_PIN_RESET);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_BF_2_init
 *  Description     : init de la BF 2
 *
 *******************************************************************************/
static void HWI_BF_2_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	// Peripheral clock enable
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(GPIO_PinState) GPIO_PIN_RESET);
}
/*******************************************************************************
 *
 *  Function Name   : HWI_Rst_Latch_WU_int
 *  Description     : init du reveil exterieur
 *
 *******************************************************************************/
static void HWI_Rst_Latch_WU_int(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	// Peripheral clock enable
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Reveil_Ext_Cmd
 *  Description     : commande du reveil exterieur
 *
 *******************************************************************************/
void HWI_Rst_Latch_WU_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,(GPIO_PinState) cmd);
}

//////////////////////////////////////////////////////////////
// DEFINITION LECTURE INFO REVEIL
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Info_Reveil_int
 *  Description     : init de la lecture info reveil
 *
 *******************************************************************************/
static void HWI_Info_Reveil_int(void) {

	 GPIO_InitTypeDef GPIO_InitStruct;

	 __HAL_RCC_GPIOC_CLK_ENABLE();
	//--------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Info_Reveil_read
 *  Description     : lecture info reveil
 *
 *******************************************************************************/
BOOL_TYPE HWI_Info_Reveil_read(void) {
    return ((BOOL_TYPE) HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2));
}

//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE  REVEIL
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Reveil_init
 *  Description     : init de la commande de reveil
 *
 *******************************************************************************/
static void HWI_Reveil_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Reveil_Cmd
 *  Description     : commande du reveil
 *
 *******************************************************************************/
void HWI_Reveil_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,(GPIO_PinState) cmd);
}


//////////////////////////////////////////////////////////////
// DEFINITION ENTREE  BOUTTON
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Button_int
 *  Description     : init de la lecture du boutton
 *
 *******************************************************************************/
static void HWI_Button_int(void) {

	 GPIO_InitTypeDef GPIO_InitStruct;

	 __HAL_RCC_GPIOC_CLK_ENABLE();
	//--------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Button_read
 *  Description     : lecture du boutton
 *
 *******************************************************************************/
BOOL_TYPE HWI_Button_read(void) {
    return ((BOOL_TYPE) !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9));
}


//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE  LED 2 ROUGE
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Led_2_Rouge_init
 *  Description     : init de la commande de la led 2 rouge
 *
 *******************************************************************************/
static void HWI_Led_2_Rouge_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Led_2_Rouge_Cmd
 *  Description     : commande de la led 2 rouge
 *
 *******************************************************************************/
void HWI_Led_2_Rouge_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,(GPIO_PinState) cmd);
}

//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE  LED 2 VERTE
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Led_2_Verte_init
 *  Description     : init de la commande de la led 2 verte
 *
 *******************************************************************************/
static void HWI_Led_2_Verte_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Led_2_Verte_Cmd
 *  Description     : commande de la led 2 verte
 *
 *******************************************************************************/
void HWI_Led_2_Verte_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,(GPIO_PinState) cmd);
}

//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE  LED 1 ROUGE
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Led_1_Rouge_init
 *  Description     : init de la commande de la led 1 rouge
 *
 *******************************************************************************/
static void HWI_Led_1_Rouge_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Led_1_Rouge_Cmd
 *  Description     : commande de la led 1 rouge
 *
 *******************************************************************************/
void HWI_Led_1_Rouge_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,(GPIO_PinState) cmd);
}

//////////////////////////////////////////////////////////////
// DEFINITION COMMANDE  LED 1 VERTE
//////////////////////////////////////////////////////////////
/*******************************************************************************
 *
 *  Function Name   : HWI_Led_1_Verte_init
 *  Description     : init de la commande de la led 1 verte
 *
 *******************************************************************************/
static void HWI_Led_1_Verte_init(void) {
GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Led_1_Verte_Cmd
 *  Description     : commande de la led 1 verte
 *
 *******************************************************************************/
void HWI_Led_1_Verte_Cmd(BOOL_TYPE cmd)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,(GPIO_PinState) cmd);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Version_int
 *  Description     : init de la lecture de version
 *
 *******************************************************************************/
static void HWI_Version_int(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

	 __HAL_RCC_GPIOD_CLK_ENABLE();
	//--------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Version_read
 *  Description     : lecture de la version
 *
 *******************************************************************************/
BOOL_TYPE HWI_Version_read(void) {
    return ((BOOL_TYPE) !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2));
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Version_int
 *  Description     : init de la lecture de version
 *
 *******************************************************************************/
static void HWI_Presence_SD_int(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

	 __HAL_RCC_GPIOA_CLK_ENABLE();

	//--------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Version_read
 *  Description     : lecture de la version
 *
 *******************************************************************************/
BOOL_TYPE HWI_Presence_SD_read(void) {
    return ((BOOL_TYPE) !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4));
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Gpio_int
 *  Description     : init de la gestion des gpio
 *
 *******************************************************************************/
void HWI_Hal_Gpio_Init(void) {

    HWI_Version_int();
    HWI_Rst_Latch_WU_int();
    HWI_Info_Reveil_int();
    HWI_Reveil_init();
    HWI_Button_int();
    HWI_Led_2_Rouge_init();
    HWI_Led_2_Verte_init();
    HWI_Led_1_Rouge_init();
    HWI_Led_1_Verte_init();
    HWI_Presence_SD_int();
    HWI_BF_1_init();
    HWI_BF_2_init();
    HWI_Rst_Latch_WU_Cmd(FALSE);

    HWI_Led_2_Verte_Cmd(FALSE);
    HWI_Led_1_Verte_Cmd(FALSE);
    HWI_Led_2_Rouge_Cmd(FALSE);
    HWI_Led_1_Rouge_Cmd(FALSE);

    HWI_Reveil_Cmd(FALSE);
}
