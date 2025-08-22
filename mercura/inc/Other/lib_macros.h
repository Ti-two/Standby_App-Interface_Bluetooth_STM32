/*******************************************************************************
*                    Synergie Ing�nierie Industrielle
*
*                    This document is the property of
*                    Synergie Ing�nierie Industrielle
*                    It must not be copied (in whole or in part)
*                    or disclosed without prior written consent
*                    of the company. Any copies by any method
*                    must also include a copy of this legend.
*
*******************************************************************************/

#ifndef _lib_macro_h
#define _lib_macro_h

/*******************************************************************************
*  Library Include Files
*******************************************************************************/
#include "types.h"

/*******************************************************************************
*  Library Type Definitions
*******************************************************************************/

/*NONE*/

/*******************************************************************************
*  Library Macros Definition
*******************************************************************************/
/*******************************************************************************
*  Macro name:   
*
*  The following macro will Return the absolute on its input.and lower limit
*
*  If input is greater than 0 then
*   Return input.
*  Else
*   Return -input.
*
*  input needs to be a signed variable.
*******************************************************************************/
#define ABS(input) (((input)<0) ? (-(input)) : (input))


/*******************************************************************************
*  Macro name:   SETB
*
*  The following macro is used to set a bit in octet
*
*  Where:
*   octet = octet qui contient le bit
*   position = numero du bit de (0...7)
*
*******************************************************************************/

#define SETB(octet, position) \
   (octet) = ( (octet) | ((U8)(0x01 << (position) )) )


/*******************************************************************************
*  Macro name:   CLRB
*
*  The following macro is used to clr a bit in octet
*
*  Where:
*   octet = octet qui contient le bit
*   position = numero du bit de (0...7)
*
*******************************************************************************/

#define CLRB(octet, position) \
 	(octet) = ( (octet) & (~(U8)(0x01 << (position))))

/*******************************************************************************
*  Macro name:   TESTB
*
*  The following macro is used to set a bit in octet
*
*  Where:
*   octet = octet qui contient le bit
*   position = numero du bit de (0...7)
*
*******************************************************************************/

#define TESTB(octet,position) \
	 ((0x00 == ((octet) & (U8)(0x01 << (position)))) ?FALSE:TRUE)  \
 
/*******************************************************************************
*  Macro name:  BT_TIMER
*
*  The following macro is used to perform a timer function.
*
*  If the current_time is greater than start_time + timer_duration
*   The output will be TRUE .
*  Else
*   The output will be FALSE.
*
*  output must be implemented as a BOOL_TYPE.
*  start_time, current_time and timer_duration must all be
*  of the same type and BIN Point.
*
*  Only works with U16 and U32 timers
*******************************************************************************/
#define BT_TIMER(start_time, current_time, timer_duration) \
    ((((current_time)-(start_time))<(timer_duration))?FALSE:TRUE)


/*******************************************************************************/
#define BT_TIMER_LOCK(start_time,current_time,timer_duration,lock,return_fc) \
    	if ((((current_time)-(start_time))>(timer_duration))||((lock)==(TRUE))) \
    	{   \
    		(lock)=(TRUE); \
    		(return_fc)=(TRUE);	\
    	}	\
    	else \
    	{	\
    		(return_fc)=(FALSE);	\
    	}  
    	 
 
/*******************************************************************************
*  Macro name:  BT_TIMER_INIT
*
*  The following macro is used to reset a timer function.
*
*  It resets the flag and update the start time ti the current time
*
*  output must be implemented as a BOOL_TYPE.
*  start_time and current_time must all be of the same type and BIN Point.
*
*  Only works with U16 and U32 timers
*******************************************************************************/
#define BT_TIMER_INIT(start_time, current_time) \
    (start_time)=(current_time);

/*******************************************************************************/
#define BT_TIMER_INIT_LOCK(start_time, current_time, lock) \
    (start_time)=(current_time); \
    (lock)=(FALSE);

/*******************************************************************************
*  Macro name:  MIN
*
*  The following macro will return the minimum of two variables passed to it.
*
*  input_a and input_b must be
*  of the same type and BIN Point.
*******************************************************************************/
#define MIN(input_a, input_b)	(((input_a)<(input_b))?(input_a):(input_b))


/*******************************************************************************
*  Macro name:   MAX
*
*  The following macro will return the maximum of two variables
*  passed to it.
*
*  input_a and input_b must be
*  of the same type and BIN Point.
*******************************************************************************/
#define MAX(input_a, input_b)	(((input_a)>(input_b))?(input_a):(input_b))


/*******************************************************************************
*  Macro name:   LIMIT
*
*  The following macro will limit a variable between an upper
*  and lower limit.
*
*  If input is greater than upper_limit then
*   Return upper_limit.
*  Else If input is less than lower_limit then
*   Return lower_limit
*  Else
*   Return input
*
*  input, lower_limit and upper_limit must be
*  of the same type and BIN Point.
*******************************************************************************/
#define LIMIT(input, lower_limit, upper_limit) \
    (((input)>(upper_limit))?(upper_limit)\
    :(((input)<(lower_limit))?(lower_limit)\
    :(input)))


/*******************************************************************************
*  Macro name:  SLEW
*
*  The following macro will slew a variable using an up_rate and down_rate
*
*  If input is greater than previous_input + up_rate then
*   Return previous_input + up_rate
*  Else If input is less than previous_input - down_rate then
*   Return previous_input - down_rate
*  Else
*   Return input.
*
*  input, previous_input, up_rate and down_rate must be
*  of the same type and BIN Point.
*******************************************************************************/
#define SLEW(input, previous_input, up_rate, down_rate) \
    (((input)>((previous_input) + (up_rate)))?((previous_input)+(up_rate))\
    :(((input)<((previous_input) - (down_rate)))?((previous_input)-(down_rate))\
    :(input)))


/*******************************************************************************
*  Macro name:   ABS
*
*  The following macro will Return the absolute on its input.and lower limit
*
*  If input is greater than 0 then
*   Return input.
*  Else
*   Return -input.
*
*  input needs to be a signed variable.
*******************************************************************************/
#define ABS(input) (((input)<0) ? (-(input)) : (input))


/*******************************************************************************
*  Macro name:   SHIFT_RIGHT
*
*  The following macro will Return the shifted input
*
*  If input is greater than 0 then
*   Return shifted (input)
*  Else
*   Return -(shifted(-(input)))

*  no_of_shifts must be a positive value
*******************************************************************************/
#define SHIFT_RIGHT(input,no_of_shifts) \
    (((input) < 0) ? -((-(input)) >> (no_of_shifts)) \
	:((input) >> (no_of_shifts)))


/*******************************************************************************
*  Macro name:   SETF
*
*  The following macro is used to set a bit flag within a flag carrier
*
*  flag_struct must be the name of a BOOL type structure
*
*  Where:
*   flag_pos is the bit number of the flag ( 0 ... N)
*       N = (number of bits in flag_carrier) - 1
*   flag_carrier is the address of the array that contains the flag.
*
*******************************************************************************/
#define SETF(flag_struct) \
    ( \
    (*(flag_struct).flag_carrier) = \
    (U16)((U32)(*(flag_struct).flag_carrier) | ((U32)1<<(flag_struct).flag_pos))  \
    )

/*******************************************************************************
*  Macro name:   CLRF
*
*  The following macro is used to clear a bit flag within a flag carrier
*
*  flag_struct must be the name of a BOOL type structure
*
*  Where:
*   flag_pos is the bit number of the flag ( 0 ... N)
*       N = (number of bits in flag_carrier) - 1
*   flag_carrier is the address of the array that contains the flag
*
*******************************************************************************/
#define CLRF(flag_struct) \
    ( \
    (*(flag_struct).flag_carrier) = \
    (U16)((U32)(*(flag_struct).flag_carrier) & (~((U32)1<<(flag_struct).flag_pos))) \
    )

/*******************************************************************************
*  Macro name:   TSTF
*
*  The following macro is used to test a bit flag
*  within a flag carrier.
*
*   TRUE is returned if the bit is set
*   FALSE is returned if the bit is clear
*
*  flag_struct must the name of a BOOL type structure
*
*  Where:
*   flag_pos is the bit number of the flag ( 0 ... N).
*       N = (number of bits in flag_carrier) - 1.
*   flag_carrier is the address of the array that contains the flag
*******************************************************************************/

#define TSTF(flag_struct) \
    ( \
    ((( (U32) *((flag_struct).flag_carrier)) & ((U32)1<<(flag_struct).flag_pos))  > 0 ) \
    ?TRUE:FALSE \
    )

/*******************************************************************************
*  Macro name:  INV_BOOL_TYPE
*
*  The following macro will return the inverse value of input_bt passed to it.
*
*  input_bt must be	of type BOOL_TYPE
*
*******************************************************************************/
#define INV_BOOL_TYPE(input_bt)	(((input_bt)==(FALSE))?(TRUE):(FALSE))


/*******************************************************************************
*  Macro name:  LINEAR_RAMP
*
*  ramp_duration must be different of zero.
*  elapsed_time and ramp_duration will have the same bin point and the same type.
*  input and target will have the same bin point and the same type.
*
*******************************************************************************/
#define LINEAR_RAMP(input,target,ramp_duration,elapsed_time)\
   (((elapsed_time)<(ramp_duration))?\
   ((input)+(((target)-(input)) * ((S32)(elapsed_time))/(ramp_duration))) : (target))



/*******************************************************************************
*  Library Initialisation Function Prototypes
*******************************************************************************/

/* NONE */
#endif

