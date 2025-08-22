/*******************************************************************************
*  Synergie Ingénierie Industirelle Co.
*
*  This document is the property of Synergie Ingénierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/

/*******************************************************************************/
#ifndef LIB_FILTER_H
#define LIB_FILTER_H

/*******************************************************************************
*  Library Include Files
*******************************************************************************/
#include "types.h"          /* Generic types Definitions */

/*******************************************************************************
*  Library Type Definitions
*******************************************************************************/


/* Store data structure for macro M_DEBOUNCE */
typedef struct
{
    U8 *dbnc_cntr_ptr;
    BOOL_TYPE previous_output;
} M_DEBOUNCE_TYPE;

/* Store data structure for macro M_LINEAR_RAMP */
typedef struct
{
    U32       start_time;
    BOOL_TYPE ramp_finished ;

} M_LINEAR_RAMP_TYPE;

/* Store data structure for macro M_SLEW */
typedef struct
{
    union
    {
        S32 prev_S32;
        U32 prev_U32;
    }prev_output;
} M_SLEW_TYPE;

/* Store data structure for macro M_TIMER         */
typedef struct
{
	U32 start_time ;
    U32 time_passed;
    BOOL_TYPE previous_output ;
} M_TIMER_TYPE;

/*******************************************************************************
*  Library Macros Definition
*******************************************************************************/
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
*  Library Function Prototypes
*******************************************************************************/
extern BOOL_TYPE M_DEBOUNCE(BOOL_TYPE input,
                     U8 low_thr,
                     U8 high_thr,
                     M_DEBOUNCE_TYPE *debounce_struct_ptr);

extern BOOL_TYPE M_DEBOUNCE_INIT(BOOL_TYPE init_value,
                     U8 *dbnc_cntr_ptr,
                     M_DEBOUNCE_TYPE *debounce_struct_ptr);

extern S16 M_LINEAR_RAMP_S16(S16 source,
                    S16 target,
                    U32 current_time,
                    U16 ramp_duration,
                    M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr);

extern void M_LINEAR_RAMP_INIT_S16(U32 current_time,
                        M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr);

extern U16 M_LINEAR_RAMP_U16(U16 source,
                    U16 target,
                    U32 current_time,
                    U16 ramp_duration,
                    M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr);

extern void M_LINEAR_RAMP_INIT_U16(U32 current_time,
                        M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr);

extern S32 M_SLEW_S32(S32 input,
                    U32 up_rate,
                    U32 down_rate,
                    M_SLEW_TYPE *slew_struct_ptr);


extern S32 M_SLEW_INIT_S32(S32 init_value,
                     M_SLEW_TYPE *slew_struct_ptr);

extern U32 M_SLEW_U32(U32 input,
                    U32 up_rate,
                    U32 down_rate,
                    M_SLEW_TYPE *slew_struct_ptr);

extern U32 M_SLEW_INIT_U32(U32 init_value,
                   M_SLEW_TYPE *slew_struct_ptr);

extern void InsertData(U8* p_l_can_data_ptr, U32 Value, U8 BitDeDepart, U8 TailleDonnee);
extern U32 ExtractData(U8* p_l_can_data_ptr, U8 BitDeDepart, U8 TailleDonnee);

extern BOOL_TYPE M_TIMER_INIT(U32 current_time,M_TIMER_TYPE *timer_struct_ptr);
extern BOOL_TYPE M_TIMER(U32 current_time,U32 timer_duration,M_TIMER_TYPE *timer_struct_ptr);



#endif
