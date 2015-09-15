/*********************************************************************
 *
 *                 Stepper motor control
 *
 *********************************************************************
 * FileName:        stepper.c
 * Dependencies:
 * Processor:       PIC18F2525
 * Complier:        MCC18 v3.37
 *
 * Software License Agreement
 *
 * Copyright © 2011 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	28/03/2011  Creation
 ********************************************************************/

#define _STEPPER_C
#include "system.h"
#include "stepper.h"
#include "eep.h"
#include "messages.h"

//****** V A R I A B L E S ********************************************************/
unsigned int stepper_delay_manual = 50;       // delay betwen 2 step, 0.1ms
unsigned int stepper_delay_manual2 = 10;       // delay betwen 2 step, 0.1ms
unsigned int stepper_delay_auto = 10;
unsigned int stepper_position = 0;
unsigned int stepper_pos_max = 10000;
unsigned int stepper_pos_min = 0;
unsigned int stepper_backlash = 0;
//unsigned char stepper_is_moving = FALSE;
//unsigned char stepper_last_direction = 0;
u_stepper_flag stepper_flag;

static unsigned int timer_period;
static unsigned char nb_step_seq;
static unsigned char motor_step_seq = 0;

//static unsigned char manual_move;
static unsigned int dest_position;
static unsigned int backlash = 0;

static void (*p_timer_isr)(void);
void (*p_step_sequencer)(void);

/**********************************************************************
* Function:        void timer_isr (void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Timer0 interrupt subroutines for step generation
*
***********************************************************************/
void timer0_isr(void)
{
    un_timers timer;

    p_timer_isr();
    timer.lt = timer_period;
    TMR0H = timer.bt[1];        // set Timer0 to timer_period
    TMR0L = timer.bt[0];
}

/**********************************************************************
* Function:        void half_step_sequencer(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:
*
***********************************************************************/
st_motor_seq half_step_motor_seq[] =
{
    { 0,1, 0,0 },
    { 0,1, 0,1 },
    { 0,0, 0,1 },
    { 1,0, 0,1 },
    { 1,0, 0,0 },
    { 1,0, 1,0 },
    { 0,0, 1,0 },
    { 0,1, 1,0 }
};

void half_step_sequencer(void)
{
    MOTOR_1A = half_step_motor_seq[motor_step_seq].motor_1a;
    MOTOR_2A = half_step_motor_seq[motor_step_seq].motor_2a;
    MOTOR_3A = half_step_motor_seq[motor_step_seq].motor_3a;
    MOTOR_4A = half_step_motor_seq[motor_step_seq].motor_4a;
}

/**********************************************************************
* Function:        void full_step_sequencer(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:
*
***********************************************************************/
st_motor_seq full_step_motor_seq[] =
{
    { 0,1, 0,1 },
    { 0,1, 1,0 },
    { 1,0, 1,0 },
    { 1,0, 0,1 },
};

void full_step_sequencer(void)
{
    MOTOR_1A = full_step_motor_seq[motor_step_seq].motor_1a;
    MOTOR_2A = full_step_motor_seq[motor_step_seq].motor_2a;
    MOTOR_3A = full_step_motor_seq[motor_step_seq].motor_3a;
    MOTOR_4A = full_step_motor_seq[motor_step_seq].motor_4a;
}

/**********************************************************************
* Function:        void full_step_sequencer(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:
*
***********************************************************************/
st_motor_seq full_step_lowconso_motor_seq[] =
{
    { 0,1, 0,0 },
    { 0,0, 1,0 },
    { 1,0, 0,0 },
    { 0,0, 0,1 },
};

void full_step_lowconso_sequencer(void)
{
    MOTOR_1A = full_step_lowconso_motor_seq[motor_step_seq].motor_1a;
    MOTOR_2A = full_step_lowconso_motor_seq[motor_step_seq].motor_2a;
    MOTOR_3A = full_step_lowconso_motor_seq[motor_step_seq].motor_3a;
    MOTOR_4A = full_step_lowconso_motor_seq[motor_step_seq].motor_4a;
}

/**********************************************************************
* Function:        void inc_step_seq(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Increment the step sequence index
***********************************************************************/
void inc_step_seq(void)
{
    motor_step_seq++;
    if (motor_step_seq == nb_step_seq) motor_step_seq = 0;
}

/**********************************************************************
* Function:        void inc_step_seq(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Decrement the step sequence index
***********************************************************************/
void dec_step_seq(void)
{
    if (motor_step_seq == 0) motor_step_seq = nb_step_seq;
    motor_step_seq--;
}

/**********************************************************************
* Function:        void step_in(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Move the motor one step inside
***********************************************************************/
void step_in(void)
{
    if (!config_byte.invert_motor)
    {
        inc_step_seq();
    }
    else
    {
        dec_step_seq();
    }

    p_step_sequencer();
    if ((!stepper_flag.manual_move && stepper_position == dest_position) || stepper_position == stepper_pos_min)
    {
        stepper_stop(0);
        stepper_flag.park = 0;
    }
    else
    {
        if (backlash) backlash--;
        else stepper_position--;
    }
}

/**********************************************************************
* Function:        void step_out(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Move the motor one step outside
***********************************************************************/
void step_out(void)
{
    if (!config_byte.invert_motor)
    {
        dec_step_seq();
    }
    else
    {
        inc_step_seq();
    }

    p_step_sequencer();
    if ((!stepper_flag.manual_move && stepper_position == dest_position) || stepper_position == stepper_pos_max)
    {
        stepper_stop(0);
        stepper_flag.park = 0;
    }
    else
    {
        if (backlash) backlash--;
        else stepper_position++;
    }
}

/**********************************************************************
* Function:        void stepper_init(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Configure the PWM port and MOTOR as Output,
*                  disable motor drivers bu setting PWM to 0
*                  and initialize Timer0
***********************************************************************/
void stepper_init(void)
{
    un_timers timer;

    MOTOR_TRIS &= 0b11000100;
    PWM = 0;

    stepper_flag.byte = 0;
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_auto);
    p_step_sequencer = full_step_sequencer;
    nb_step_seq = 4;

    INTCON2bits.TMR0IP = 0;     // set timer0 interrupr low priority
    T0CON = 0x04;               // 16 bit Timer0, 1:32 prescale
    timer.lt = timer_period;
    TMR0H = timer.bt[1];        // set Timer0 to timer_period
    TMR0L = timer.bt[0];
    INTCONbits.TMR0IF = 0;      // Clear Timer0 overflow flag
    INTCONbits.TMR0IE = 1;      // Enable Timer0 overflow interrupt

    T0CONbits.TMR0ON = 0;       // do not start Timer0 now
}

/**********************************************************************
* Function:        void stepper_stop(void)
* PreCondition:    None
* Input:		   unsigned char force
* Output:		   None
* Overview:		   Stop stepper motor
***********************************************************************/
void stepper_stop(unsigned char force)
{
    if (stepper_flag.manual_move == TRUE && !force)
    {
        while (backlash != 0)
            ;
    }

    if (!config_byte.keep_power_motor_stop)
{
        PWM = 0;                  // disable motor drivers
    }
    T0CONbits.TMR0ON = 0;       // stop Timer0
    stepper_flag.is_moving = FALSE;
    start_timer(1, 60000);
}

/**********************************************************************
* Function:        void stepper_start_in(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Start stepper motor inside direction
***********************************************************************/
void start_in(void)
{
    stop_timer(1);
    if (stepper_flag.last_direction != DIRECTION_IN && backlash == 0)
    {
        backlash = stepper_backlash;
    }
    stepper_flag.last_direction = DIRECTION_IN;
    p_timer_isr = step_in;
    PWM = 1;
    T0CONbits.TMR0ON = 1;       // start Timer0
    stepper_flag.is_moving = TRUE;
}

void stepper_start_in(void)
{
    stepper_flag.manual_move = TRUE;
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_manual);
    start_in();
}

/**********************************************************************
* Function:        void stepper_start_out(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Start stepper motor outside direction
***********************************************************************/
void start_out(void)
{
    stop_timer(1);
    if (stepper_flag.last_direction != DIRECTION_OUT && backlash == 0)
    {
        backlash = stepper_backlash;
    }
    stepper_flag.last_direction = DIRECTION_OUT;
    p_timer_isr = step_out;
    PWM = 1;
    T0CONbits.TMR0ON = 1;       // start Timer0
    stepper_flag.is_moving = TRUE;
}

void stepper_start_out(void)
{
    stepper_flag.manual_move = TRUE;
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_manual);
    start_out();
}

/**********************************************************************
* Function:        void stepper_relative_move(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Move to a relative position
***********************************************************************/
void stepper_high_speed(void)
{
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_manual2);
}

/**********************************************************************
* Function:        void stepper_relative_move(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Move to a relative position
***********************************************************************/
void stepper_relative_move(unsigned int nb_steps, unsigned char dir)
{
    stepper_flag.manual_move = FALSE;
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_auto);

    if (dir == DIRECTION_IN)
    {
        dest_position = stepper_position - nb_steps;
        start_in();
    }
    else
    {
        dest_position = stepper_position + nb_steps;
        start_out();
    }
}

/**********************************************************************
* Function:        void stepper_absolute_move(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Move to a absolute position
***********************************************************************/
void stepper_absolute_move(unsigned int position)
{
    stepper_flag.manual_move = FALSE;
    timer_period = 0xFFFF - (TIMER_BASE_PERIOD * stepper_delay_auto);

    dest_position = position;
    if (position < stepper_position)
    {
        start_in();
    }
    if (position > stepper_position)
    {
        start_out();
    }
}

/**********************************************************************
* Function:        void stepper_mode(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   set p_step_sequencer function pointer
***********************************************************************/
unsigned char stepper_mode(unsigned char mode)
{
    switch (mode)
    {
    case HALF_STEP :
        p_step_sequencer = half_step_sequencer;
        nb_step_seq = 8;
        break;

    case FULL_STEP :
        p_step_sequencer = full_step_sequencer;
        nb_step_seq = 4;
        break;

    case FULL_STEP_LOWCONSO :
        p_step_sequencer = full_step_lowconso_sequencer;
        nb_step_seq = 4;
        break;

    default:
        if (p_step_sequencer == half_step_sequencer) return HALF_STEP;
        if (p_step_sequencer == full_step_sequencer) return FULL_STEP;
        if (p_step_sequencer == full_step_lowconso_sequencer) return FULL_STEP_LOWCONSO;
        break;

    }
    eep_write(eep_adr_p_step_sequencer, (unsigned char *)&p_step_sequencer, sizeof(p_step_sequencer));
    return mode;
}
