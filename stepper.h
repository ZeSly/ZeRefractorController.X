/*********************************************************************
 *
 *                 Stepper motor control
 *
 *********************************************************************
 * FileName:        stepper.h
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

#ifndef _STEPPER_H
#define _STEPPER_H

#include <p18cxxx.h>

//******* D E F I N I T I O N S ************************************************/
#define MOTOR_TRIS  TRISB
#define MOTOR_PORT  PORTB
#define MOTOR_1A   PORTBbits.RB0
#define MOTOR_2A   PORTBbits.RB1
#define MOTOR_3A   PORTBbits.RB4
#define MOTOR_4A   PORTBbits.RB5

#define PWM        PORTBbits.RB3

#define DIRECTION_IN  0
#define DIRECTION_OUT 1

#define HALF_STEP 1
#define FULL_STEP 2
#define FULL_STEP_LOWCONSO 3

#define TIMER_BASE_PERIOD   150

//******* D A T A   S T R U C T U R E S ******************************************************/
typedef union
{
  unsigned int lt;
  char bt[2];
} un_timers;

typedef struct
{
    unsigned char motor_1a;
    unsigned char motor_2a;
    unsigned char motor_3a;
    unsigned char motor_4a;
} st_motor_seq;

typedef union
{
    unsigned char byte;
    struct
    {
        unsigned is_moving :1;
        unsigned last_direction :1;
        unsigned manual_move :1;
        unsigned park :1;
    };
} u_stepper_flag;


//******* P R O T O T Y P E S ***********************************************************/
void timer0_isr (void);

void stepper_init(void);
void stepper_stop(unsigned char force);
void stepper_start_in(void);
void stepper_start_out(void);
void stepper_high_speed(void);

void stepper_relative_move(unsigned int nb, unsigned char dir);
void stepper_absolute_move(unsigned int position);

unsigned char stepper_mode(unsigned char mode);

//****** V A R I A B L E S ********************************************************/
#ifndef _STEPPER_C
extern unsigned int stepper_delay_manual;
extern unsigned int stepper_delay_auto;
extern unsigned int stepper_pos_max;
extern unsigned int stepper_pos_min;
extern unsigned int stepper_position;
extern unsigned int stepper_backlash;
extern u_stepper_flag stepper_flag;
//extern unsigned char stepper_is_moving;
//extern unsigned char stepper_last_direction;
extern void (*p_step_sequencer)(void);
#endif

#endif //_STEPPER_H
