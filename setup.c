/*********************************************************************
 *
 *                 Setup commands
 *
 *********************************************************************
 * FileName:        setup.c
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
 * Sylvain Girard   	08/05/2011  Creation
 ********************************************************************/

#include <stdio.h>
#include "hid_focus.h"
#include "positionning.h"
#include "system.h"
#include "stepper.h"
#include "eep.h"

//const char str_fsa[] = "fsa=";
//const char str_fsm[] = "fsm=";
//const char str_fss[] = "fss=";
//const char str_fsi[] = "fsi=";
//const char str_fso[] = "fso=";
//const char str_fsp[] = "fsp=";
//const char str_fsb[] = "fsb=";
//const char str_fsc[] = "fsc=";

unsigned int n;

/**********************************************************************
* Function:        void cmd_fsal(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAL RS232 Command : setup delay between step, auto
***********************************************************************/
void cmd_fsal(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        stepper_delay_auto = n;
        eep_write(eep_adr_stepper_delay_auto, (unsigned char *)&stepper_delay_auto, sizeof(stepper_delay_auto));
        answer(stepper_delay_auto);
    }
}

/**********************************************************************
* Function:        void cmd_fsar(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAR RS232 Command : read delay between step, auto
***********************************************************************/
void cmd_fsar(void)
{
    answer(stepper_delay_auto);
}

/**********************************************************************
* Function:        void cmd_fsal(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAL RS232 Command : setup delay between step, manual
***********************************************************************/
void cmd_fsml(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        stepper_delay_manual = n;
        eep_write(eep_adr_stepper_delay_manual, (unsigned char *)&stepper_delay_manual, sizeof(stepper_delay_manual));
        answer(stepper_delay_manual);
    }
}

/**********************************************************************
* Function:        void cmd_fsar(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAR RS232 Command : read delay between step, manual
***********************************************************************/
void cmd_fsmr(void)
{
    answer(stepper_delay_manual);
}

/**********************************************************************
* Function:        void cmd_fssl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSSL RS232 Command : setup stepper mode
***********************************************************************/
void cmd_fssl(void)
{
    unsigned int mode;

    if (read_number(&mode, 1) == TRUE)
    {
        stepper_mode(mode);
        put_char(mode);
    }
}

/**********************************************************************
* Function:        void cmd_fssr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAR RS232 Command : read stepper mode
***********************************************************************/
void cmd_fssr(void)
{
    unsigned char mode;

    mode = stepper_mode(0);
    put_char(mode);
}

/**********************************************************************
* Function:        void cmd_fsil(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAL RS232 Command : setup min position
***********************************************************************/
void cmd_fsil(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        stepper_pos_min = n;
        eep_write(eep_adr_stepper_pos_min, (unsigned char *)&stepper_pos_min, sizeof(stepper_pos_min));
        answer(stepper_pos_min);
    }
}

/**********************************************************************
* Function:        void cmd_fsir(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAR RS232 Command : read min position
***********************************************************************/
void cmd_fsir(void)
{
    answer(stepper_pos_min);
}

/**********************************************************************
* Function:        void cmd_fsol(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAL RS232 Command : setup max position
***********************************************************************/
void cmd_fsol(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        stepper_pos_max = n;
        eep_write(eep_adr_stepper_pos_max, (unsigned char *)&stepper_pos_max, sizeof(stepper_pos_max));
        answer(stepper_pos_max);
    }
}

/**********************************************************************
* Function:        void cmd_fsor(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSAR RS232 Command : read max position
***********************************************************************/
void cmd_fsor(void)
{
    answer(stepper_pos_max);
}

/**********************************************************************
* Function:        void cmd_fspl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSPL RS232 Command : set initial position
***********************************************************************/
void cmd_fspl(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        eep_write(eep_adr_stepper_position, (unsigned char *)&n, sizeof(n));
        answer(n);
    }
}

/**********************************************************************
* Function:        void cmd_fspc(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSPC RS232 Command : set current position as initial
*                                       position
***********************************************************************/
void cmd_fspc(void)
{
    eep_write(eep_adr_stepper_position, (unsigned char *)&stepper_position, sizeof(n));
    answer(stepper_position);
}

/**********************************************************************
* Function:        void cmd_fsir(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSPR RS232 Command : read initial position
***********************************************************************/
void cmd_fspr(void)
{
    eep_read(eep_adr_stepper_position, (unsigned char *)&n, sizeof(n));
    answer(n);
}

/**********************************************************************
* Function:        void cmd_fsbl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSBL RS232 Command : setup backlash
***********************************************************************/
void cmd_fsbl(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        stepper_backlash = n;
        eep_write(eep_adr_stepper_backlash, (unsigned char *)&stepper_backlash, sizeof(stepper_backlash));
        answer(stepper_backlash);
    }
}

/**********************************************************************
* Function:        void cmd_fsbr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSBR RS232 Command : read backlash
***********************************************************************/
void cmd_fsbr(void)
{
    answer(stepper_backlash);
}

/**********************************************************************
* Function:        void cmd_fsbl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSCL RS232 Command : set config_byte
***********************************************************************/
void cmd_fscl(void)
{
    if (read_number(&n, 1) == TRUE)
    {
        config_byte.byte = n;
        eep_write(eep_adr_config_byte, (unsigned char *)&config_byte.byte, sizeof(config_byte.byte));
        put_char(config_byte.byte);
    }
}

/**********************************************************************
* Function:        void cmd_fsbr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FSCR RS232 Command : read config_byte
***********************************************************************/
void cmd_fscr(void)
{
    put_char(config_byte.byte);
}

