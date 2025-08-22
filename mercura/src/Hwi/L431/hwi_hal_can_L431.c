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
#include "hwi_hal_can.h"

/********************************************************************/
/*  Feature Level Include Files                                     */
/********************************************************************/
#include "hwi_nvic_priority.h"
#include "S_S_Scheduler.h"
#include "p_l_rx_can1.h"
#include "p_l_tx_can1.h"
#include "app_manage_wifi.h"
#include "app_manage_mapping.h"
#include "S_S_Scheduler.h"

#ifdef _PROGRAMATION
#include "p_l_can_flashage_rx.h"
#include "p_l_can_flashage_tx.h"
#endif


/********************************************************************/
/*  Configuration Variable Definition                               */
/********************************************************************/
/* NONE*/

/********************************************************************/
/* External Variable Definition                                     */
/********************************************************************/
/*NONE*/

/********************************************************************/
/*  Variable Definition                                             */
/********************************************************************/
CAN_HandleTypeDef Can1Handle;

/********************************************************************/
/*  Static Function Definition                                      */
/********************************************************************/
//indique quel gestion wifi blt faire
extern APP_MODE_GES_WIFI_BLT_TYPE          APP_Mode_gestion_wifi_blt;

//flag indiquant qu'une carte SD autorisant la reprogramation de module
extern BOOL_TYPE                            P_L_CarteSd_Programation_vue;
//flag indiquant qu'une carte SD autorise aucun mode particulier
extern BOOL_TYPE                            P_L_CarteSd_No_Mode_vue;

extern BOOL_TYPE                           APP_Demande_Arret_SendCan;

#ifdef _PROGRAMATION
//step for programming module
extern APP_FLASHAGE_STEP                     APP_Flashage_state;

#ifdef _TEST_ETHERNET
//flag indiquant qu'il faut diagnostique un module
extern BOOL_TYPE                             P_L_Ethernet_Diag_Module;
#endif
#endif

/********************************************************************/
/*  Function Definition                                             */
/********************************************************************/
static void Hwi_CanHandle_Send_Message(CAN_HandleTypeDef* hcan,HWI_CAN_OBJECT_TYPE * hwi_can_object_ptr, U8 * hwi_can_buffer_send,U32 TransmitMailbox);
static void Hwi_Can_add_filter(CAN_FilterTypeDef  *FilterConfig,U8 *numero_filtre,U32 ID);
static void HWI_CanFilter_accept_all(CAN_FilterTypeDef  *FilterConfig);
static void HWI_CanFilter_accept_facade_PMV_PC(CAN_FilterTypeDef  *FilterConfig);
static void HWI_CanFilter_accept_PMV_PC(CAN_FilterTypeDef  *FilterConfig);



/*******************************************************************************
 *
 *  Function Name   : HWI_Can_Init
 *  Description     : initialisation du can
 *
 *
 *******************************************************************************/
U8 HWI_Hal_Can_Init(HWI_CAN_BAURATE_TYPE CANSpeed) {

    __HAL_RCC_CAN1_FORCE_RESET();
    __HAL_RCC_CAN1_RELEASE_RESET();

    Can1Handle.Instance = CAN1;
    Can1Handle.Init.Mode = CAN_MODE_NORMAL;

    Can1Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    Can1Handle.Init.TimeSeg1 = CAN_BS1_6TQ;
    Can1Handle.Init.TimeSeg2 = CAN_BS2_3TQ;

    if (HWI_CBR1000000 == CANSpeed)
	{
    	Can1Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    	Can1Handle.Init.TimeSeg1 = CAN_BS1_4TQ;
    	Can1Handle.Init.TimeSeg2 = CAN_BS2_2TQ;
    	Can1Handle.Init.Prescaler = 5;
	}
	else if (HWI_CBR500000 == CANSpeed)
	{
		Can1Handle.Init.Prescaler = 7;
	}
	else if (HWI_CBR250000 == CANSpeed)
	{
		Can1Handle.Init.Prescaler = 14;
	}
	else if (HWI_CBR125000 == CANSpeed)
	{
		Can1Handle.Init.Prescaler = 28;
	}
	else
	{
		Can1Handle.Init.Prescaler = 7;
	}

    Can1Handle.Init.TimeTriggeredMode = DISABLE;
    Can1Handle.Init.AutoBusOff = ENABLE;
    Can1Handle.Init.AutoWakeUp = DISABLE;
    Can1Handle.Init.AutoRetransmission = ENABLE;
    Can1Handle.Init.ReceiveFifoLocked = DISABLE;
    Can1Handle.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&Can1Handle) != HAL_OK)
    {
        return(HAL_ERROR);
    }

	//##-2- Configure the CAN Filter ###########################################
	HWI_Can_FilterConfig();


	//##-3- Start the Reception process and enable reception interrupt #########
	if (HAL_CAN_Start(&Can1Handle) != HAL_OK) {
		// Reception Error
		return(HAL_ERROR);
	}

    HAL_CAN_ActivateNotification(&Can1Handle, CAN_IT_RX_FIFO0_MSG_PENDING); // Active le mode interruption
    HAL_CAN_ActivateNotification(&Can1Handle, CAN_IT_TX_MAILBOX_EMPTY);

	return(HAL_OK);
}

/*******************************************************************************
 *
 *  Function Name   : HWI_Can_FilterConfig
 *  Description     : initialisation des filtres can
 *
 *
 *******************************************************************************/
void HWI_Can_FilterConfig(void)
{
CAN_FilterTypeDef  FilterConfig;

    HWI_CanFilter_accept_all(&FilterConfig);

    if (TRUE == P_L_CarteSd_No_Mode_vue)
   	{
   		if (APP_Mode_gestion_wifi_blt == APP_MODE_BTN_ET_MESSAGE_PMV)
   		{
   			HWI_CanFilter_accept_facade_PMV_PC(&FilterConfig);
   		}
   		else if (APP_Mode_gestion_wifi_blt == APP_MODE_MESSAGE_PMV)
   		{
   			HWI_CanFilter_accept_PMV_PC(&FilterConfig);
   		}
   	}

}


/*******************************************************************************
 *
 *  Function Name   : HWI_Can_Init_accept_all
 *  Description     : initialisation du fltre can
 *
 *
 *******************************************************************************/
static void HWI_CanFilter_accept_all(CAN_FilterTypeDef *FilterConfig)
{
    FilterConfig->SlaveStartFilterBank = 14;

    FilterConfig->FilterBank = 0;
    FilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
    FilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;
    FilterConfig->FilterIdHigh = 0x0000;
    FilterConfig->FilterIdLow = 0x0000;
    FilterConfig->FilterMaskIdHigh = 0x0000;
    FilterConfig->FilterMaskIdLow = 0x0000;
    FilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    FilterConfig->FilterActivation = ENABLE;

    if (HAL_CAN_ConfigFilter(&Can1Handle, FilterConfig) != HAL_OK) {
		// Filter configuration Error
		_Error_Handler(__FILE__, __LINE__);
	}

    FilterConfig->FilterBank = 14;
    FilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
    FilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;
    FilterConfig->FilterIdHigh = 0x0000;
    FilterConfig->FilterIdLow = 0x0000;
    FilterConfig->FilterMaskIdHigh = 0x0000;
    FilterConfig->FilterMaskIdLow = 0x0000;
    FilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    FilterConfig->FilterActivation = ENABLE;

    if (HAL_CAN_ConfigFilter(&Can1Handle, FilterConfig) != HAL_OK) {
		// Filter configuration Error
		_Error_Handler(__FILE__, __LINE__);
	}

}

/*******************************************************************************
*
*  Function Name   : HWI_CanFilter_accept_PMV_PC
*  Model Reference :
*  Description     :
*******************************************************************************/
static void HWI_CanFilter_accept_PMV_PC(CAN_FilterTypeDef  *FilterConfig)
{
U8 numero_filtre;

    numero_filtre = 0;
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,0x01);
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,0x02);
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,APP_ADR_NOEUD_CAN_BLOC_PMV+1);

    // initialisation en cas de monbre impaire de filtre configure
    if ((numero_filtre % 2))
    {
        FilterConfig->FilterMaskIdHigh=0xFFFF;
        FilterConfig->FilterMaskIdLow=0xFFFF;
        FilterConfig->FilterFIFOAssignment=CAN_FILTER_FIFO0;
        FilterConfig->FilterActivation=ENABLE;

        if (HAL_CAN_ConfigFilter(&Can1Handle, FilterConfig) != HAL_OK) {
            // Filter configuration Error
            _Error_Handler(__FILE__, __LINE__);
        }
    }
}

/*******************************************************************************
*
*  Function Name   : HWI_CanFilter_accept_facade_PMV_PC
*  Model Reference :
*  Description     :
*******************************************************************************/
static void HWI_CanFilter_accept_facade_PMV_PC(CAN_FilterTypeDef  *FilterConfig)
{
const U32   *ptr_configFacadesU32;
const U32   *ptr_configBlocsU32;
U8 numero_filtre;
U32 adresse;

    ptr_configFacadesU32 = (const U32 *) APP_LISTE_FACADE_CFG_FACADE;
    ptr_configBlocsU32 = (const U32 *) APP_LISTE_BLOC_CFG_FACADE;

    numero_filtre = 0;
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,0x01);
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,0x02);
    Hwi_Can_add_filter(FilterConfig,&numero_filtre,APP_ADR_NOEUD_CAN_BLOC_PMV+1);

    adresse = *ptr_configFacadesU32;
    if (0xFFFFFFFF != adresse)
    {
        Hwi_Can_add_filter(FilterConfig,&numero_filtre,adresse);
    }

    adresse = *(ptr_configFacadesU32+1);
    if (0xFFFFFFFF != adresse)
    {
        Hwi_Can_add_filter(FilterConfig,&numero_filtre,adresse);
    }

    Hwi_Can_add_filter(FilterConfig,&numero_filtre,*ptr_configBlocsU32);

    // initialisation en cas de monbre impaire de filtre configure
    if ((numero_filtre % 2))
    {
        FilterConfig->FilterMaskIdHigh=0xFFFF;
        FilterConfig->FilterMaskIdLow=0xFFFF;
        FilterConfig->FilterFIFOAssignment=CAN_FILTER_FIFO0;
        FilterConfig->FilterActivation=ENABLE;

        if (HAL_CAN_ConfigFilter(&Can1Handle, FilterConfig) != HAL_OK) {
            // Filter configuration Error
            _Error_Handler(__FILE__, __LINE__);
        }
    }
}

/*******************************************************************************
*
*  Function Name   : Hwi_Can_add_filter
*  Model Reference : [
*  Description     :
*******************************************************************************/
static void Hwi_Can_add_filter(CAN_FilterTypeDef  *FilterConfig,U8 *numero_filtre,U32 ID)
{
U32 ID_cal;

   ID_cal = ID;

   if ( ID_cal > 0x7FFF)
   {
    ID_cal = ID << 3;
    ID_cal = ID_cal| CAN_ID_EXT;
   }
   else
   {
    ID_cal = ID << 21;
    ID_cal = ID_cal| CAN_ID_STD;
   }

     if (!((*numero_filtre) % 2))
    {
        FilterConfig->FilterBank =  ((*numero_filtre)/2);
        FilterConfig->FilterMode=CAN_FILTERMODE_IDLIST;
        FilterConfig->FilterScale=CAN_FILTERSCALE_32BIT;
        FilterConfig->FilterIdHigh=(uint16_t)(ID_cal >> 16);
        FilterConfig->FilterIdLow=(uint16_t)( ID_cal& 0x0000FFFF);
    }
    else
    {
        FilterConfig->FilterMaskIdHigh=(uint16_t)(ID_cal >> 16);
        FilterConfig->FilterMaskIdLow=(uint16_t)( ID_cal & 0x0000FFFF);
        FilterConfig->FilterFIFOAssignment=CAN_FILTER_FIFO0;
        FilterConfig->FilterActivation=ENABLE;

        if (HAL_CAN_ConfigFilter(&Can1Handle, FilterConfig) != HAL_OK) {
            // Filter configuration Error
            _Error_Handler(__FILE__, __LINE__);
        }
    }
    *numero_filtre = (*numero_filtre) + 1;
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_MspInit
 *  Description     : init by hal for F105
 *
 *
 *******************************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

	if (hcan->Instance == CAN1)
    {
		//##-1- Enable peripherals and GPIO Clocks #################################
		// CAN1 Periph clock enable
		__HAL_RCC_CAN1_CLK_ENABLE();

		// Enable GPIO clock ***************************************
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		//##-2- Configure peripheral GPIO ##########################################
        //CAN1 GPIO Configuration
		// PB8     ------> CAN1_RX
		// PB9     ------> CAN1_TX

		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// initialisation des signaux du driver can
		//EN
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//NSTB
		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		//ERR
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		// Peripheral interrupt init
		// Sets the priority grouping field

		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, HAL_NVIC_PREEMPT_PRIORITY_CAN_RX,HAL_NVIC_SUB_PRIORITY_CAN_RX);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

//		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, HAL_NVIC_PREEMPT_PRIORITY_CAN_RX,HAL_NVIC_SUB_PRIORITY_CAN_RX);
//		HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);


		HAL_NVIC_SetPriority(CAN1_TX_IRQn, HAL_NVIC_PREEMPT_PRIORITY_CAN_TX,HAL_NVIC_SUB_PRIORITY_CAN_TX);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);

		HAL_CAN_Write_EN(TRUE);
		HAL_CAN_Write_NSTB(TRUE);


	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_Write_EN
 *  Description     :
 *
 *
 *******************************************************************************/
void HAL_CAN_Write_EN(BOOL_TYPE Pin_State) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (GPIO_PinState) Pin_State);
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_Write_NSTB
 *  Description     :
 *
 *
 *******************************************************************************/
void HAL_CAN_Write_NSTB(BOOL_TYPE Pin_State) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (GPIO_PinState) Pin_State);
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_Read_ERR
 *  Description     :
 *
 *
 *******************************************************************************/
BOOL_TYPE HAL_CAN_Read_ERR(void) {
	return ((BOOL_TYPE) HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3));
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_MspDeInit
 *  Description     : initialisation par la hal
 *
 *
 *******************************************************************************/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{

	if (hcan->Instance == CAN1)
    {
        // USER CODE BEGIN CAN1_MspDeInit 0
        // USER CODE END CAN1_MspDeInit 0
        // Peripheral clock disable
		__HAL_RCC_CAN1_CLK_DISABLE();

        //CAN1 GPIO Configuration
		//PB8     ------> CAN1_RX
		//PB9     ------> CAN1_TX

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

        // CAN1 interrupt DeInit
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
//        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
	}
}

/*******************************************************************************
*
*  Function Name   : AUDIO_IN_TransferComplete_CallBack
*  Description     : Audio Transfer completed callbacks
*
*
*******************************************************************************/
__weak void P_L_Can_1_rx_frame(U32 p_l_can_msg_id, U8* p_l_can_data_ptr, U8 p_l_can_lenght)
{

  // This function should be implemented by the user application..
}

/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_RxCpltCallbacwk
 *  Description     : call back reception data can
 *
 *
 *******************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
CAN_RxHeaderTypeDef RxMessage;
U8 hwi_can_data[8];
U32 hwi_can_id;

	if (hcan->Instance == CAN1)
	{
		if (HAL_CAN_GetRxMessage(&Can1Handle,CAN_RX_FIFO0,&RxMessage, hwi_can_data) == HAL_OK)
		{

		    if (RxMessage.IDE == 0)
		    {
		        hwi_can_id= RxMessage.StdId;
		    }
		    else
		    {
		        hwi_can_id= RxMessage.ExtId;
		    }

#ifdef _PROGRAMATION
#ifdef _TEST_ETHERNET
            if (TRUE == P_L_Ethernet_Diag_Module)
            {
               APP_SendTrame_CAN_ethernet(hwi_can_id, hwi_can_data,RxMessage.DLC);
            }
#endif
            if (FLASHAGE_DIAGNOSTIQUE == APP_Flashage_state)
            {
                APP_SendTrame_CAN_wifi(hwi_can_id, hwi_can_data,RxMessage.DLC);
            }

            if (TRUE == P_L_CarteSd_Programation_vue)
            {
                P_L_Can_flashage_rx_frame(hwi_can_id, hwi_can_data,RxMessage.DLC);
            }
            else
            {
                P_L_Can_1_rx_frame(hwi_can_id, hwi_can_data,RxMessage.DLC);
            }
#else
            P_L_Can_1_rx_frame(hwi_can_id, hwi_can_data,RxMessage.DLC);
#endif
		}
	}
}



/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_RxCpltCallbacwk
 *  Description     : call back reception data can
 *
 *
 *******************************************************************************/
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan)
{
    if (hcan->Instance == CAN1)
	{
#ifdef _PROGRAMATION
        if (TRUE == P_L_CarteSd_Programation_vue)
        {
            P_L_Can_flashage_Tx_IT();
        }
        else
#endif
        {
            P_L_Can_Send_Data_Can_IT(0x00);
        }

	}
}


/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_RxCpltCallbacwk
 *  Description     : call back reception data can
 *
 *
 *******************************************************************************/
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef* hcan)
{
   if (hcan->Instance == CAN1)
	{
        P_L_Can_Send_Data_Can_IT(0x01);
	}
}


/*******************************************************************************
 *
 *  Function Name   : HAL_CAN_RxCpltCallbacwk
 *  Description     : call back reception data can
 *
 *
 *******************************************************************************/
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef* hcan)
{

}

/*******************************************************************************
*
*  Function Name   : Hwi_Can_Send_Message
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : fonction qui gere l'envoie du message sur le can via la mail box TransmitMailbox
*
*******************************************************************************/
void Hwi_Can_Send_Message(CAN_TypeDef* CANx,HWI_CAN_OBJECT_TYPE * hwi_can_object_ptr, U8 * hwi_can_buffer_send,U32 TransmitMailbox)
{
	if (APP_Demande_Arret_SendCan == FALSE)
	{
		if(CANx == CAN1)
		{
			Hwi_CanHandle_Send_Message(&Can1Handle,hwi_can_object_ptr,hwi_can_buffer_send,TransmitMailbox);
		}
	}
}

/*******************************************************************************
*
*  Function Name   : Hwi_Can_Send_Message
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : fonction qui gere l'envoie du message sur le can via la mail box TransmitMailbox
*
*******************************************************************************/
static void Hwi_CanHandle_Send_Message(CAN_HandleTypeDef* hcan,HWI_CAN_OBJECT_TYPE * hwi_can_object_ptr, U8 * hwi_can_buffer_send,U32 TransmitMailbox)
{
CAN_TxHeaderTypeDef TxMessage;
U8 i;
U8 TransmitResult;
U32 mailBox = CAN_TX_MAILBOX0;

i = 0x00;

    if (MAIL_BOX1 == TransmitMailbox)
    {
        mailBox = CAN_TX_MAILBOX0;
    }
    else if (MAIL_BOX2 == TransmitMailbox)
    {
        mailBox = CAN_TX_MAILBOX1;
    }
    else if (MAIL_BOX3 == TransmitMailbox)
    {
        mailBox = CAN_TX_MAILBOX2;
    }
    TransmitResult = HAL_CAN_IsTxMessagePending(hcan,mailBox);

    if(0  !=  TransmitResult)
    {

        HAL_CAN_AbortTxRequest(hcan,mailBox);

        if (MAIL_BOX1 == TransmitMailbox)
        {
            while((hcan->Instance->TSR&CAN_TSR_TME0) != CAN_TSR_TME0)
            {
                i++;
            }
        }
        else if (MAIL_BOX2 == TransmitMailbox)
        {
            while((hcan->Instance->TSR&CAN_TSR_TME1) != CAN_TSR_TME1)
            {
                i++;
            }
        }
        else if (MAIL_BOX3 == TransmitMailbox)
        {
            while((hcan->Instance->TSR&CAN_TSR_TME2) != CAN_TSR_TME2)
            {
                i++;
            }
        }
    }


 // Set up the Id

    if(HWI_EXT == hwi_can_object_ptr->HWI_ID_FORMAT)
    {
        TxMessage.IDE = CAN_ID_EXT;
        TxMessage.ExtId = hwi_can_object_ptr->HWI_CAN_MSG_ID;
    }
    else
    {
        TxMessage.IDE = CAN_ID_STD;
        TxMessage.StdId = hwi_can_object_ptr->HWI_CAN_MSG_ID;
    }

    TxMessage.RTR = CAN_RTR_DATA;

    TxMessage.DLC = hwi_can_object_ptr->HWI_CAN_LENGTH;
    TxMessage.TransmitGlobalTime = DISABLE;

    /* Check the parameters */
    assert_param(IS_CAN_IDTYPE(TxMessage.IDE));
    assert_param(IS_CAN_RTR(TxMessage.RTR));
    assert_param(IS_CAN_DLC(TxMessage.DLC));

    if (TxMessage.IDE == CAN_ID_STD)
    {
        assert_param(IS_CAN_STDID(TxMessage.StdId));
    }
    else
    {
        assert_param(IS_CAN_EXTID(TxMessage.ExtId));
    }

    assert_param(IS_FUNCTIONAL_STATE(pHeader->TransmitGlobalTime));

    if (TxMessage.IDE == CAN_ID_STD)
    {
        hcan->Instance->sTxMailBox[TransmitMailbox].TIR = ((TxMessage.StdId << CAN_TI0R_STID_Pos) | TxMessage.RTR);
    }
    else
    {
        hcan->Instance->sTxMailBox[TransmitMailbox].TIR = ((TxMessage.ExtId << CAN_TI0R_EXID_Pos) | TxMessage.IDE | TxMessage.RTR);
    }

    /* Set up the DLC */
    hcan->Instance->sTxMailBox[TransmitMailbox].TDTR = (TxMessage.DLC);

    /* Set up the Transmit Global Time mode */
    if (TxMessage.TransmitGlobalTime == ENABLE)
    {
        SET_BIT(hcan->Instance->sTxMailBox[TransmitMailbox].TDTR, CAN_TDT0R_TGT);
    }

    /* Set up the data field */
    WRITE_REG(hcan->Instance->sTxMailBox[TransmitMailbox].TDHR,
            ((uint32_t)hwi_can_buffer_send[7] << CAN_TDH0R_DATA7_Pos) |
            ((uint32_t)hwi_can_buffer_send[6] << CAN_TDH0R_DATA6_Pos) |
            ((uint32_t)hwi_can_buffer_send[5] << CAN_TDH0R_DATA5_Pos) |
            ((uint32_t)hwi_can_buffer_send[4] << CAN_TDH0R_DATA4_Pos));
    WRITE_REG(hcan->Instance->sTxMailBox[TransmitMailbox].TDLR,
            ((uint32_t)hwi_can_buffer_send[3] << CAN_TDL0R_DATA3_Pos) |
            ((uint32_t)hwi_can_buffer_send[2] << CAN_TDL0R_DATA2_Pos) |
            ((uint32_t)hwi_can_buffer_send[1] << CAN_TDL0R_DATA1_Pos) |
            ((uint32_t)hwi_can_buffer_send[0] << CAN_TDL0R_DATA0_Pos));

    /* Request transmission */
    SET_BIT(hcan->Instance->sTxMailBox[TransmitMailbox].TIR, CAN_TI0R_TXRQ);
}

/*******************************************************************************
*
*  Function Name   :  P_L_Can_Mail_Box_Vide
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     :
*******************************************************************************/
BOOL_TYPE Hwi_Can_Mail_Box_Vide(CAN_TypeDef* CANx)
{
BOOL_TYPE result;

    result = TRUE;

 //Select one empty transmit mailbox
  if ((CANx->TSR&CAN_TSR_TME0) != CAN_TSR_TME0)
  {
    result = FALSE;
  }

  if ((CANx->TSR&CAN_TSR_TME1) != CAN_TSR_TME1)
  {
    result = FALSE;
  }

  if ((CANx->TSR&CAN_TSR_TME2) != CAN_TSR_TME2)
  {
    result = FALSE;
  }

return(result);
}


/*******************************************************************************
 *
 *  Function Name   : Hwi_Can_Send_Message
 *  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
 *  Description     : fonction qui gere l'envoie du message sur le can via la mail box TransmitMailbox
 *
 *******************************************************************************/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* hcan)
{

//	P_L_Can_1_Error_frame(hcan->ErrorCode);

}



