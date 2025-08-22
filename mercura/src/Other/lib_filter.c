/*******************************************************************************
*  Synergie Ing�nierie Industirelle Co.
*
*  This document is the property of Synergie Ing�nierie Industrielle
*  It must not be copied (in whole or in part) or disclosed without 
*  prior written consent of the company. Any copies by any method
*  must also include a copy of this legend.
*******************************************************************************/

/*******************************************************************************
*   Library Include Files
*******************************************************************************/
#include "hwi.h"
#include "lib_filter.h"


/******************************************************************************
*  Library Local Function Prototype
*******************************************************************************/



/*******************************************************************************
*   Library Main Functions
*******************************************************************************/
#define DECALAGE_MASK(posByte,posBit)   (uint8_t)((((uint16_t)0x0001) << ((posByte + 1) * 8 - (posBit)))-1)


/*******************************************************************************
*  Function name:  M_DEBOUNCE
*
*  Description :
*  The following fucntion apply a debouynce filter to the input
*
*
*  Where:
*   input is the input state
*   high_thr is the number of input set to ON before switching ON the output
*   low_thr is the number of input set to OFF before switching OFF the output
*   debounce_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   user is in charge reset the filter before first use by using macro
*	M_DEBOUCE_INIT
*   Low_thr and high_thr must be of type U8 BIN0 and mustn't exceed the value 254
*
*******************************************************************************/
BOOL_TYPE M_DEBOUNCE(BOOL_TYPE input,
                     U8 low_thr,
                     U8 high_thr,
                     M_DEBOUNCE_TYPE *debounce_struct_ptr)
{
    if(input != debounce_struct_ptr->previous_output)
    {
        if(U8_MAX > *(debounce_struct_ptr->dbnc_cntr_ptr))
        {
            (*(debounce_struct_ptr->dbnc_cntr_ptr))++;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        if(U8_MIN < *(debounce_struct_ptr->dbnc_cntr_ptr))
        {
            (*(debounce_struct_ptr->dbnc_cntr_ptr))--;
        }
        else
        {
            /* Do nothing */
        }
    }

    if(ON == debounce_struct_ptr->previous_output)
    {
        if(low_thr < *(debounce_struct_ptr->dbnc_cntr_ptr))
        {   /* Switch in OFF position */
            *(debounce_struct_ptr->dbnc_cntr_ptr)   = U8_MIN;
            debounce_struct_ptr->previous_output    = OFF;
        }
        else
        {
            /* Do nothing */
        }
    }
    else if(OFF == debounce_struct_ptr->previous_output)
    {
        if(high_thr < *(debounce_struct_ptr->dbnc_cntr_ptr))
        {   /* Switch in ON position */
            *(debounce_struct_ptr->dbnc_cntr_ptr)   = U8_MIN;
            debounce_struct_ptr->previous_output    = ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        *(debounce_struct_ptr->dbnc_cntr_ptr)   = U8_MIN;
        debounce_struct_ptr->previous_output    = OFF;
    }

    return(debounce_struct_ptr->previous_output);
}

/*******************************************************************************
*  Macro name:  M_DEBOUNCE_INIT
*
*  Description :
*  The following macro manage initialization before starting
*  new filter with function M_DEBOUNCE .
*
*  Where:
*   init_value is the init value
*	dbnc_cntr_ptr is address of the counter which will be used by M_DEBOUNCE to
*               manage debouce
*   debounce_struct_ptr is the structure associated to macro
*
*  REMARKS:
*	This macro should be used each time a new filter order is launched with
*   M_DEBOUNCE.
*******************************************************************************/
BOOL_TYPE  M_DEBOUNCE_INIT(BOOL_TYPE init_value,
                     U8 *dbnc_cntr_ptr,
                     M_DEBOUNCE_TYPE *debounce_struct_ptr)
{
    debounce_struct_ptr->previous_output  = init_value;
    debounce_struct_ptr->dbnc_cntr_ptr    = dbnc_cntr_ptr;
    *(debounce_struct_ptr->dbnc_cntr_ptr) = 0;

	return(FALSE);
}


/*******************************************************************************
*  Function name:  M_LINEAR_RAMP_S16
*
*  Description :
*  The following macro is used to apply a LINEAR_RAMP from source to target value
*  during ramp_duration
*
*  x = (current_time - start_time) / ramp_duration
*  Output = target * x + source * (1 - x)
*
*
*  Where:
*   source is the input source value
*   target is the input target value
*   current_time is the curren,t timer
*   ramp_duration is the ramp duration
*   linear_ramp_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   current_time and ramp_duration must be of the same type and BIN Point
*   source and Target must be of the same type and BIN Point
*
*******************************************************************************/
S16 M_LINEAR_RAMP_S16(S16 source,
                    S16 target,
                    U32 current_time,
                    U16 ramp_duration,
                    M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr)
{
    S16 local_output;
    U32 delta_time;

    if(TRUE == linear_ramp_struct_ptr->ramp_finished)
	{
		local_output = target;
	}
	else /* input = FALSE */
	{
	    delta_time = current_time - linear_ramp_struct_ptr->start_time;

        if(delta_time >= ramp_duration)
        {
            linear_ramp_struct_ptr->ramp_finished = TRUE;
            local_output = target;
        }
        else
        {
            local_output = (S16)(( (target * (S32)delta_time)
                            + (source * (S32)(ramp_duration - delta_time)))
                           / (S32)ramp_duration);
        }
    }
	return(local_output);
}

/*******************************************************************************
*  Macro name:  M_LINEAR_RAMP_INIT_S16
*
*  Description :
*  The following macro manage init of M_LINAER_RAMP_S function
*
*  ramp_finished  = FALSE
*  start_time = current_time
*
*  Where:
*   current_time is the current time
*   linear_ramp_struct_ptr is the structure associated to macro
*
*******************************************************************************/
void M_LINEAR_RAMP_INIT_S16(U32 current_time,
                        M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr)
{
    linear_ramp_struct_ptr->ramp_finished   = FALSE;
    linear_ramp_struct_ptr->start_time      = current_time;
}


/*******************************************************************************
*  Function name:  M_LINEAR_RAMP_U16
*
*  Description :
*  The following macro is used to apply a LINEAR_RAMP from source to target value
*  during ramp_duration
*
*  x = (current_time - start_time) / ramp_duration
*  Output = target * x + source * (1 - x)
*
*
*  Where:
*   source is the input source value
*   target is the input target value
*   current_time is the curren,t timer
*   ramp_duration is the ramp duration
*   linear_ramp_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   current_time and ramp_duration must be of the same type and BIN Point
*   source and Target must be of the same type and BIN Point
*
*******************************************************************************/
U16 M_LINEAR_RAMP_U16(U16 source,
                    U16 target,
                    U32 current_time,
                    U16 ramp_duration,
                    M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr)
{
    U16 local_output;
    U32 delta_time;

    if(TRUE == linear_ramp_struct_ptr->ramp_finished)
	{
		local_output = target;
	}
	else /* input = FALSE */
	{
	    delta_time = current_time - linear_ramp_struct_ptr->start_time;

        if(delta_time >= ramp_duration)
        {
            linear_ramp_struct_ptr->ramp_finished = TRUE;
            local_output = target;
        }
        else
        {
            local_output = (U16)(( (target * delta_time)
                            + (source * (ramp_duration - delta_time)))
                           / ramp_duration);
        }
    }
	return(local_output);
}

/*******************************************************************************
*  Macro name:  M_LINEAR_RAMP_INIT_U16
*
*  Description :
*  The following macro manage init of M_LINAER_RAMP_S function
*
*  ramp_finished  = FALSE
*  start_time = current_time
*
*  Where:
*   current_time is the current time
*   linear_ramp_struct_ptr is the structure associated to macro
*
*******************************************************************************/
void M_LINEAR_RAMP_INIT_U16(U32 current_time,
                        M_LINEAR_RAMP_TYPE *linear_ramp_struct_ptr)
{
    linear_ramp_struct_ptr->ramp_finished   = FALSE;
    linear_ramp_struct_ptr->start_time      = current_time;
}

/*******************************************************************************
*  Function name:  M_SLEW_S32
*
*  Description :
*  The following macro will slew a variable using an up_rate and down_rate
*
*  If input is greater than previous_output + up_rate then
*   Return previous_output + up_rate
*  Else If input is less than previous_output - down_rate then
*   Return previous_output - down_rate
*  Else
*   Return input.
*
*
*  Where:
*   input is the target value
*   up_rate is the increment of slew
*   down_rate is the decrement of slew
*   slew_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   input, up_rate and down_rate must all be of the same BIN Point
*
*******************************************************************************/
S32 M_SLEW_S32(S32 input,
               U32 up_rate,
               U32 down_rate,
               M_SLEW_TYPE *slew_struct_ptr)
{
    U32 delta;

    /* Check delta overflow */
    if(input >= slew_struct_ptr->prev_output.prev_S32)
    {
        delta = (U32)input - (U32)(slew_struct_ptr->prev_output.prev_S32);
        if( delta > up_rate)
        {
            slew_struct_ptr->prev_output.prev_S32 += (S32)up_rate;
        }
        else
        {
            slew_struct_ptr->prev_output.prev_S32 = input;
        }
    }
    else
    {
        delta = (U32)(slew_struct_ptr->prev_output.prev_S32) - (U32)input;
        if(delta > down_rate)
        {
            slew_struct_ptr->prev_output.prev_S32 -= (S32)down_rate;
        }
        else
        {
            slew_struct_ptr->prev_output.prev_S32 = input;
        }
    }


    return (slew_struct_ptr->prev_output.prev_S32);
}



/*******************************************************************************
*  Macro name:  M_SLEW_INIT_S32
*
*  Description :
*  The following macro manage init of M_SLEW_S macro
*
*  previous_output = init_value
*
*  Where:
*   init_value is the int value of hysteresis
*   slew_struct_ptr is the structure associated to macro
*
*******************************************************************************/
S32 M_SLEW_INIT_S32(S32 init_value,
                   M_SLEW_TYPE *slew_struct_ptr)
{
	slew_struct_ptr->prev_output.prev_S32   = init_value;

	return(init_value);
}


/*******************************************************************************
*  Function name:  M_SLEW_U32
*
*  Description :
*  The following macro will slew a variable using an up_rate and down_rate
*
*  If input is greater than previous_output + up_rate then
*   Return previous_output + up_rate
*  Else If input is less than previous_output - down_rate then
*   Return previous_output - down_rate
*  Else
*   Return input.
*
*
*  Where:
*   input is the target value
*   up_rate is the increment of slew
*   down_rate is the decrement of slew
*   slew_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   input, up_rate and down_rate must all be of the same type and BIN Point
*
*******************************************************************************/
U32 M_SLEW_U32(U32 input,
               U32 up_rate,
               U32 down_rate,
               M_SLEW_TYPE *slew_struct_ptr)
{
    U32 delta;

    /* Check delta overflow */
    if(input >= slew_struct_ptr->prev_output.prev_U32)
    {
        delta = input - slew_struct_ptr->prev_output.prev_U32;
        if( delta > up_rate)
        {
            slew_struct_ptr->prev_output.prev_U32 += up_rate;
        }
        else
        {
            slew_struct_ptr->prev_output.prev_U32 = input;
        }
    }
    else
    {
        delta = slew_struct_ptr->prev_output.prev_U32 - input;
        if(delta > down_rate)
        {
            slew_struct_ptr->prev_output.prev_U32 -= down_rate;
        }
        else
        {
            slew_struct_ptr->prev_output.prev_U32 = input;
        }
    }
    return (slew_struct_ptr->prev_output.prev_U32);
}



/*******************************************************************************
*  Macro name:  M_SLEW_INIT_U32
*
*  Description :
*  The following macro manage init of M_SLEW_U32 macro
*
*  previous_output = init_value
*
*  Where:
*   init_value is the int value of hysteresis
*   slew_struct_ptr is the structure associated to macro
*
*******************************************************************************/
U32 M_SLEW_INIT_U32(U32 init_value,
                   M_SLEW_TYPE *slew_struct_ptr)
{
	slew_struct_ptr->prev_output.prev_U32   = init_value;

	return(init_value);
}

/*******************************************************************************
*  Function name:  M_TIMER
*
*  Description :
*  The following function is used to perform a timer function.
*
*  If the current_time is greater than start_time + timer_duration
*   The output will be TRUE .
*  Else
*   The output will be FALSE.
*
*  Where:
*   current_time is the current time
*   timer_duration is the timer duration
*   timer_struct_ptr is the structure associated to macro
*
*  REMARKS:
*   current_time and timer_duration must be of same BIN Point.
*
*******************************************************************************/
BOOL_TYPE M_TIMER(U32 current_time,
                  U32 timer_duration,
                  M_TIMER_TYPE *timer_struct_ptr)
{
    U32 time_passed_tmp;

    /* Time passed calculation */
    time_passed_tmp = current_time - timer_struct_ptr->start_time;

    if(time_passed_tmp < timer_struct_ptr->time_passed)
    {
        timer_struct_ptr->time_passed = U32_MAX;
    }
    else
    {
        timer_struct_ptr->time_passed = time_passed_tmp;
    }


    if(FALSE == timer_struct_ptr->previous_output)
    {
        if(timer_struct_ptr->time_passed < timer_duration)
        {
            /* Do nothing */
        }
        else
        {
            timer_struct_ptr->previous_output = TRUE;
        }
    }
    else
    {
        /* Do nothing */
    }

    return(timer_struct_ptr->previous_output);
}


/*******************************************************************************
*  Macro name:  M_TIMER_INIT
*
*  Description :
*  The following macro manage reset of timer managed by function M_TIMER.
*
*  previous_output = FALSE
*  start_time = current_time
*
*  Where:
*   current_time is the current time
*   timer_struct_ptr is the structure associated to macro
*
*******************************************************************************/
BOOL_TYPE M_TIMER_INIT(U32 current_time,
                  M_TIMER_TYPE *timer_struct_ptr)
{
	timer_struct_ptr->previous_output   = FALSE;
    timer_struct_ptr->start_time        = current_time;
    timer_struct_ptr->time_passed       = 0;

    return(FALSE);
}

 /*******************************************************************************
*
*  Function Name   :  ExtractData
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : extrait les donn�es d'une trame
*******************************************************************************/
U32 ExtractData(U8* p_l_can_data_ptr, U8 BitDeDepart, U8 TailleDonnee)
{
/*
    assert_param(IS_CORRECT_START(BitDeDepart));
    assert_param(IS_CORRECT_SIZE(TailleDonnee));
    assert_param(IS_CORRECT_INFO(BitDeDepart + TailleDonnee));
*/

    uint8_t BitDeFin = BitDeDepart + TailleDonnee;
    uint8_t posByte;
    uint8_t Mask;
    uint8_t Decalage;
    uint32_t Result = 0; 

    for (posByte = (BitDeDepart/8) ; posByte <= ((BitDeFin-1)/8) ; posByte++)
    {//boucle sur tous les octets contenant la donn�e
        Mask = 0xFF;
        Decalage = 0;
        // cr�ation d'un masque pour filtrer les informations de la donn�e 
        if (posByte * 8 <  BitDeDepart)
        {
            Mask = DECALAGE_MASK(posByte , BitDeDepart);
        }      
        
        if (((posByte + 1) * 8 - 1) >=  BitDeFin)
        {
            Mask &= ~DECALAGE_MASK(posByte , BitDeFin);
            Decalage = ((((BitDeFin-1)/8) + 1)*8 - BitDeFin);
        }
        // ajoute au r�sultat la partie extraite du buffer CAN
        Result = (Result << (8 - Decalage)) + ((p_l_can_data_ptr[posByte] & Mask) >> Decalage);
    }
    return Result;
}
/*******************************************************************************
*
*  Function Name   :  InsertData
*  Model Reference : [DFD] [STD] [PAT] [DT] [P-Spec]
*  Description     : insert les donn�es d'une trame
*******************************************************************************/
void InsertData(U8* p_l_can_data_ptr, U32 Value, U8 BitDeDepart, U8 TailleDonnee)
{
/*
    assert_param(IS_CORRECT_START(BitDeDepart));
    assert_param(IS_CORRECT_SIZE(TailleDonnee));
    assert_param(IS_CORRECT_INFO(BitDeDepart + TailleDonnee));
*/
    uint8_t BitDeFin = BitDeDepart + TailleDonnee;
    uint8_t posByte;
    uint8_t Mask;
    uint8_t Result = 0; 
 
        
    for (posByte = (BitDeDepart/8) ; posByte <= ((BitDeFin-1)/8) ; posByte++)
    {
        Mask = 0xFF;
        Result = (uint8_t)Value;
        if (posByte * 8 <  BitDeDepart)
        {
            Mask = DECALAGE_MASK(posByte , BitDeDepart);
            Result = (uint8_t)(Value >> (BitDeFin - (posByte + 1) * 8));
        }      
        
        if (((posByte + 1) * 8 - 1) >=  BitDeFin)
        {
            Mask &= ~DECALAGE_MASK(posByte , BitDeFin);
            Result = (uint8_t)(Value << ((((BitDeFin-1)/8) + 1)*8 - BitDeFin));
        }
    
        p_l_can_data_ptr[posByte] &= (uint8_t)~Mask;
        p_l_can_data_ptr[posByte] = p_l_can_data_ptr[posByte] + (Result & Mask);
    }
}


/*******************************************************************************
*   Library Local Function Definitions
*******************************************************************************/

/* NONE */

/*******************************************************************************
*	Library Initialisation Function Definition
*******************************************************************************/

/* NONE */
