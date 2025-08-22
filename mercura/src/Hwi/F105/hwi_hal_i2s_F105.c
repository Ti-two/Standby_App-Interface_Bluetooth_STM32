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
I2S_HandleTypeDef   		I2S2Handle;
U8                  		HWI_I2S_Value;
BOOL_TYPE           		HWI_I2S_OnOff;
BOOL_TYPE           		HWI_I2S_flipflop;

GPIO_InitTypeDef GPIO_InitStructCK = {0};

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
    I2S2Handle.Instance = SPI2;
    I2S2Handle.Init.Mode = I2S_MODE_SLAVE_RX;
#ifdef _PCM
    I2S2Handle.Init.Standard = I2S_STANDARD_PCM_LONG;
#else
    I2S2Handle.Init.Standard = I2S_STANDARD_MSB;
#endif
    I2S2Handle.Init.DataFormat = I2S_DATAFORMAT_16B;
    I2S2Handle.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    I2S2Handle.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    I2S2Handle.Init.CPOL = I2S_CPOL_HIGH;

    if (HAL_I2S_Init(&I2S2Handle) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }


#ifdef _PCM
    // Enable TXE and ERR interrupt
    __HAL_I2S_ENABLE_IT(&I2S2Handle,I2S_IT_RXNE|I2S_IT_ERR);
    HWI_I2S_Stop();
#else
    // Enable TXE and ERR interrupt
    __HAL_I2S_ENABLE_IT(&I2S2Handle,I2S_IT_RXNE|I2S_IT_ERR);
    HWI_I2S_Stop();
#endif

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
#ifdef _PCM
    HWI_I2S_OnOff = TRUE;
    while (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {};
    while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {};
    __HAL_I2S_ENABLE(&I2S2Handle);
#else
    HWI_I2S_OnOff = TRUE;
    while (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {};
    while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {};
    while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {};
    __HAL_I2S_ENABLE(&I2S2Handle);
#endif
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
    HWI_I2S_OnOff = FALSE;
    __HAL_I2S_DISABLE(&I2S2Handle);
}

/*******************************************************************************
 *
 *  Function Name   : HAL_I2S_MspInit
 *  Description     : This function configures the hardware resources used
 *
 *
 *******************************************************************************/
void HAL_I2S_MspInit(I2S_HandleTypeDef* hi2s)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(hi2s->Instance==SPI2)
    {
        // Peripheral clock enable
        __HAL_RCC_SPI2_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        //I2S2 GPIO Configuration
        //PB12     ------> I2S2_WS
        //PB13     ------> I2S2_CK
        //PB15     ------> I2S2_SD


#ifdef _PCM
        GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT | GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT | GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStructCK.Pin = GPIO_PIN_13;
        GPIO_InitStructCK.Mode = GPIO_MODE_AF_INPUT;
        GPIO_InitStructCK.Pull = GPIO_NOPULL;
        GPIO_InitStructCK.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructCK);

        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT | GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

        HAL_NVIC_SetPriority(SPI2_IRQn, 2, 2);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);

        HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    }
}

/*******************************************************************************
 *
 *  Function Name   : HAL_I2S_MspDeInit
 *  Description     : This function freeze the hardware resources used
 *
 *
 *******************************************************************************/
void HAL_I2S_MspDeInit(I2S_HandleTypeDef* hi2s)
{
    if(hi2s->Instance==SPI2)
    {
        // Peripheral clock disable
        __HAL_RCC_SPI2_CLK_DISABLE();

        //I2S2 GPIO Configuration
        //PB12     ------> I2S2_WS
        //PB13     ------> I2S2_CK
        //PB15     ------> I2S2_SD
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15);

        HAL_NVIC_DisableIRQ(SPI2_IRQn);
    }
}
U16 I2S_data_tab[2];
U8 I2S_data_ptr;
U32 data_ok;
U32 data_pb;
void My_I2S_IRQHandler(I2S_HandleTypeDef *hi2s)
{
#ifdef _PCM
U16 I2S_data;
U16 Audio;

         if (__HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_RXNE) != RESET)
        {
        I2S_data = hi2s->Instance->DR;
/*
        if ( I2S_data  < 0x0000)
        {
          Audio  =  (I2S_data  - 0x8000);
        }
        else
        {
          Audio  = ( I2S_data  + 0x8000);
        }
*/
        Audio  = ( I2S_data  + 0x8000);

        HWI_I2S_Value = ((Audio >> 8 ) & 0x00FF);
        data_ok++;
        }

        if (__HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR) != RESET)
        {
            __HAL_I2S_DISABLE(hi2s);
            // Clear Overrun flag
            __HAL_I2S_CLEAR_OVRFLAG(hi2s);

            __HAL_I2S_ENABLE(&I2S2Handle);
            data_pb++;
        }
#else
U16 I2S_data;
U16 Audio;

    if (__HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_RXNE) != RESET)
    {
        I2S_data = hi2s->Instance->DR;

/*
        if ( I2S_data  < 0x0000)
        {
          Audio  =  (I2S_data  - 0x8000);
        }
        else
        {
          Audio  = ( I2S_data  + 0x8000);
        }
*/

        Audio  = ( I2S_data  + 0x8000);

        HWI_I2S_Value = ((Audio >> 8 ) & 0x00FF);
    }


    if (__HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR) != RESET)
    {
        __HAL_I2S_DISABLE(hi2s);
        // Clear Overrun flag
        __HAL_I2S_CLEAR_OVRFLAG(hi2s);
        while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {};
        __HAL_I2S_ENABLE(&I2S2Handle);
    }

#endif
}
BOOL_TYPE flipflop;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#ifdef _PCM
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
    {
        __HAL_I2S_ENABLE(&I2S2Handle);
    }
    else
    {
        __HAL_I2S_DISABLE(&I2S2Handle);
    }

/*
    if (TRUE == HWI_I2S_OnOff)
    {
        if (TRUE == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
        {
            __HAL_I2S_ENABLE(&I2S2Handle);
        }
        else
        {
            __HAL_I2S_DISABLE(&I2S2Handle);
        }
    }
*/
#else
    if (TRUE == HWI_I2S_OnOff)
    {
      //  if (TRUE == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
        {
     //       while (FALSE == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {};
             __HAL_I2S_ENABLE(&I2S2Handle);
        }
     //   else
        {
         //   __HAL_I2S_ENABLE(&I2S2Handle);
        }
    }
#endif
}
