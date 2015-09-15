/*********************************************************************
 *
 *                 Positionning commands
 *
 *********************************************************************
 * FileName:        positionning.c
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
 * Sylvain Girard   	07/05/2011  Creation
 ********************************************************************/

#define _POSITIONNING_C

#include <stdio.h>
#include <stdlib.h>
#include "system.h"
#include "hid_focus.h"
#include "stepper.h"
#include "positionning.h"

//const char str_fpr[] = "fpr=";
//const char str_fpl[] = "fpl=";
//const char str_fpi[] = "fpi=";
//const char str_fpo[] = "fpo=";

/**********************************************************************
* Function:        void wait_moving(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   Wait the end of stepper motor movement and send its
*                  position to the RS232
**********************************************************************
void wait_moving(void)
{
    while (stepper_flag.is_moving)
        ;
    cmd_fpr();
}*/

/**********************************************************************
* Function:        void end_of_move(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   Test if the motor finished movinf and send its
*                  position
***********************************************************************/
void end_of_move(void)
{
    static unsigned char was_moving = FALSE;

    if  (!stepper_flag.is_moving && was_moving)
    {
        cmd_fpr();
    }
    was_moving = stepper_flag.is_moving;
}

/**********************************************************************
* Function:        void cmd_fpr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPR RS232 Command : read motor position
***********************************************************************/
void cmd_fpr(void)
{
    answer(stepper_position);
}

unsigned int move;

/**********************************************************************
* Function:        void cmd_fpd(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPD RS232 Command : return the direction of the last
*                  move
***********************************************************************/
void cmd_fpd(void)
{
    put_char(stepper_flag.last_direction == DIRECTION_IN ? 'I' : 'O');
}

/**********************************************************************
* Function:        void cmd_fpl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPL RS232 Command : set motor new position
***********************************************************************/
void cmd_fpl(void)
{
    if (read_number(&move, 2) == TRUE)
    {
        stepper_position = move;
        answer(stepper_position);
    }
}

/**********************************************************************
* Function:        void cmd_fpi(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPI RS232 Command : move motor inside direction
***********************************************************************/
void cmd_fpi(void)
{

    if (read_number(&move, 2) == TRUE)
    {
        answer(move);
        stepper_relative_move(move, DIRECTION_IN);
        // wait_moving();
    }
}

/**********************************************************************
* Function:        void cmd_fpo(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPI RS232 Command : move motor outside direction
***********************************************************************/
void cmd_fpo(void)
{
    if (read_number(&move, 2) == TRUE)
    {
        answer(move);
        stepper_relative_move(move, DIRECTION_OUT);
        // wait_moving();
    }
}

/**********************************************************************
* Function:        void cmd_fpa(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPA RS232 Command : move motor absolute position
***********************************************************************/
void cmd_fpa(void)
{
    if (read_number(&move, 2) == TRUE)
    {
        answer(move);
        stepper_absolute_move(move);
        // wait_moving();
    }
}


/**********************************************************************
* Function:        void cmd_fph(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPH RS232 Command : halt motor
***********************************************************************/
void cmd_fph(void)
{
    stepper_stop(0);
}
