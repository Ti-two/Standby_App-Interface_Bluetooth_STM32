/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef __HWI_CAN_H
#define __HWI_CAN_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"


/*******************************************************************************
*  Application Definitions
*******************************************************************************/
#define CAN_USE				CAN1

#define MAIL_BOX1       ((U8)0x00)
#define MAIL_BOX2       ((U8)0x01)
#define MAIL_BOX3       ((U8)0x02)

#define MAX_MAIL_BOX       MAIL_BOX3+1


typedef enum
{
  HWI_NO_SPEED = 0,
  HWI_CBR1000000 = 1000000,
  HWI_CBR500000 = 500000,
  HWI_CBR250000 = 250000,
  HWI_CBR125000 = 125000
} HWI_CAN_BAURATE_TYPE;


#define CAN_TX_TIMEOUT_VALUE   (U32)1//(U32)1000 //ben

#define P_L_CAN_LENGHT_DATA     8

typedef enum
{
  HWI_STD,
  HWI_EXT
} HWI_CAN_ID_TYPE;

typedef struct
{
	uint32_t HWI_CAN_MSG_ID;
	uint8_t HWI_CAN_LENGTH;
  HWI_CAN_ID_TYPE HWI_ID_FORMAT;
}HWI_CAN_OBJECT_TYPE;



/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void Hwi_Can_Send_Message(CAN_TypeDef* CANx,HWI_CAN_OBJECT_TYPE * hwi_can_object_ptr, U8 * hwi_can_buffer_send,U32 TransmitMailbox);
extern void HAL_CAN_Write_EN(BOOL_TYPE Pin_State);
extern void HAL_CAN_Write_NSTB(BOOL_TYPE Pin_State);
extern BOOL_TYPE HAL_CAN_Read_ERR(void);
extern void HWI_Can_FilterConfig(void);
extern U8 HWI_Hal_Can_Init(HWI_CAN_BAURATE_TYPE CANSpeed);


#endif
