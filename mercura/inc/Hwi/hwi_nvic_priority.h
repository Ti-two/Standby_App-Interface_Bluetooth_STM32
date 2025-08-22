
#ifndef _NVIC_PRIORITY
#define _NVIC_PRIORITY

#if defined(STM32F105xC)
  #include "stm32f1xx_hal.h"
#elif defined(STM32L431xx)
  #include "stm32l4xx_hal.h"
#else
erreur choisir un processeaur
#endif


/**
  * @brief  Sets the priority grouping field (pre-emption priority and subpriority)
  *         using the required unlock sequence.
  * @param  PriorityGroup: The priority grouping bits length.
  *         This parameter can be one of the following values:
  *         @arg NVIC_PRIORITYGROUP_0: 0 bits for pre-emption priority
  *                                    4 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_1: 1 bits for pre-emption priority
  *                                    3 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_2: 2 bits for pre-emption priority
  *                                    2 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_3: 3 bits for pre-emption priority
  *                                    1 bits for subpriority
  *         @arg NVIC_PRIORITYGROUP_4: 4 bits for pre-emption priority
  *                                    0 bits for subpriority
  * @note   When the NVIC_PriorityGroup_0 is selected, IRQ pre-emption is no more possible.
  *         The pending IRQ priority will be managed only by the subpriority.
  * @retval None
  */
#define NVIC_PRIORITY_GROUPE 							NVIC_PRIORITYGROUP_4
/*!< 4 bits for pre-emption priority		 0 bits for subpriority */

/**
  * @brief  Sets the priority of an interrupt.
  * @param  IRQn: External interrupt number.
  *         This parameter can be an enumerator of IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to stm32f4xx.h file)
  * @param  PreemptPriority: The pre-emption priority for the IRQn channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority
  * @param  SubPriority: the subpriority level for the IRQ channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority.
  * @retval None
  */

// A lower priority value indicates a higher priority.

#define 	HAL_NVIC_PREEMPT_PRIORITY_T_1_MS			    8
#define 	HAL_NVIC_SUB_PRIORITY_T_1_MS				    0

#define     HAL_NVIC_PREEMPT_PRIORITY_CAN_RX                7
#define     HAL_NVIC_SUB_PRIORITY_CAN_RX                    0

#define     HAL_NVIC_PREEMPT_PRIORITY_T_1_MS_REPROG         6
#define     HAL_NVIC_SUB_PRIORITY_T_1_MS_REPROG             0

#define     HAL_NVIC_PREEMPT_PRIORITY_UART_RS232_NORMAL     5
#define     HAL_NVIC_SUB_PRIORITY_UART_RS232_NORMAL         0

#define     HAL_NVIC_PREEMPT_PRIORITY_CAN_TX            	4
#define     HAL_NVIC_SUB_PRIORITY_CAN_TX                	0

#define 	HAL_NVIC_PREEMPT_PRIORITY_I2S					3
#define 	HAL_NVIC_SUB_PRIORITY_I2S						0

#define 	HAL_NVIC_PREEMPT_PRIORITY_AUDIO					2
#define 	HAL_NVIC_SUB_PRIORITY_AUDIO						0

#define     HAL_NVIC_PREEMPT_PRIORITY_UART_RS232_HIGHT      1
#define     HAL_NVIC_SUB_PRIORITY_UART_RS232_HIGHT          0

#define 	HAL_NVIC_PREEMPT_PRIORITY_STICK				    0
#define 	HAL_NVIC_SUB_PRIORITY_STICK					    0

#define     TICK_INT_PRIORITY							HAL_NVIC_PREEMPT_PRIORITY_STICK


#endif







