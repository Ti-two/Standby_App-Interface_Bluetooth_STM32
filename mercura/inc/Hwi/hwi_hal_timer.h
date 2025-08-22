/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/
#ifndef _TIMER_HARDWARE_H
#define _TIMER_HARDWARE_H
/********************************************************************/
/*  Library Include Files                                           */
/********************************************************************/
#include "hwi.h"

/*******************************************************************************
*  Application Definitions
*******************************************************************************/
// TIMER 1ms definition
#define PERIOD_VALUE_1MS 				1000

#define PERIOD_VALUE_PWM				0xFF
#define PRESCALER_VALUE_PWM_20KHz		12
#define PRESCALER_VALUE_PWM_40KHz		6

typedef enum
{
    FREQUENCE_20kHz = 0,
    FREQUENCE_40kHz
}HWI_AUDIO_FREQUENCE;

#define MIDLE_VALUE_AUDIO                0x7F
#define REPOS_VALUE_AUDIO_CCR1           0x00
#define REPOS_VALUE_AUDIO_CCR2           0xFF

/*******************************************************************************
*  Application Function Prototype
*******************************************************************************/
extern void HWI_Timer_1ms_Init(void);
extern void HWI_Timer_1ms_Start(void);
extern void HWI_Timer_1ms_Stop(void);

#ifdef _BLUETOOTH
#ifdef _GESTION_AUDIO_BT
extern void HWI_Timer_PWM_Audio_Init(void);
extern void HWI_Pwm_IT_Pont_H(HWI_AUDIO_FREQUENCE audio_frequence);
extern void HWI_Timer_PWM_Audio_Start(void);
extern void HWI_Timer_PWM_Audio_Stop(void);
extern void HWI_Pwm_IT_Son(FunctionalState etat);
#endif
#endif

#ifdef _PROGRAMATION
extern void HWI_Timer_1MS_Reprog_init(void);
extern void HWI_Timer_1MS_Reprog_start(void);
extern void HWI_Timer_1MS_Reprog_stop(void);
#endif

extern void HWI_Timer_Sd_Delay_Init(void);
extern void HWI_Timer_Sd_Delay_us(U16 delay_time);
extern void HWI_Timer_Sd_Delay_ms(U32 delay_time);
extern void HWI_Timer_Sd_Delay_s(U32 delay_time);

#endif /* __TIME_HARDWARE_H */
