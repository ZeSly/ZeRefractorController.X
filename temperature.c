/*********************************************************************
 *
 *                 Temperature commands
 *
 *********************************************************************
 * FileName:        temperature.c
 * Dependencies:
 * Processor:       PIC18F2525
 * Complier:        XC8 v1.45
 *
 * Software License Agreement
 *
 * Copyright © 2011-2019 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	07/05/2011  Creation
 ********************************************************************/

#define _TEMPERATUE_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "system.h"
#include "temperature.h"
#include "ds128s20.h"
#include "hid_focus.h"
#include "eep.h"
#include "stepper.h"

//const char Zero[] = "0.0";
//const char str_ftr[] = "ftr=";
//const char str_ft2r[] = "ft2r=";
//const char *ftr_tab[] = { str_ftr, str_ft2r };
//const char str_ftz[] = "ftz=";
//const char str_ftd[] = "ftd=";
//const char str_ftc[] = "ftc=";

int last_temperature = -30000;
unsigned int temperature_coef = 0;
unsigned char is_cmd = FALSE;
unsigned char is_compensate = FALSE;

/*void put_str_ftr(unsigned char sensor)
{
    put_rom_string(ftr_tab[sensor]);
}*/

/********************************************************************
void send_temperature(int temperature)
{
    char stemp[8];
    char dec_temp;
    size_t l;

    if (temperature != 0)
    {
        itoa(temperature, stemp);
        l = strlen(stemp);
        dec_temp = stemp[l - 1];
        stemp[l] = dec_temp;
        stemp[l - 1] = '.';
        stemp[l + 1] = '\0';
        put_string(stemp);
    }
    else
    {
        put_rom_string(Zero);
    }
}*/

/**********************************************************************
* Function:        void cmd_ftr(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:
* Overview:        FTR Command : read temperature
***********************************************************************/
void cmd_ftxr(unsigned char sensor)
{
    answer(temperature[sensor]);
}

void cmd_ftr(void)
{
    cmd_ftxr(0);
}

void cmd_ft2r(void)
{
    cmd_ftxr(1);
}

/**********************************************************************
* Function:        void cmd_ftr(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:
* Overview:        FTR Command : start measure temperature
***********************************************************************/
void cmd_ftxm(unsigned char sensor)
{
    start_measure(sensor);
    is_measuring = sensor + 1;
    is_cmd = TRUE;
}

void cmd_ftm(void)
{
    cmd_ftxm(0);
}

void cmd_ft2m(void)
{
    cmd_ftxm(1);
}

/**********************************************************************
* Function:        void cmd_ftzl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FTZ RS232 Command : set temperature compensation
***********************************************************************/
void cmd_ftzl(void)
{
    unsigned int comp;

    if (read_number(&comp,1))
    {
        is_compensate = comp;
    }
}

/**********************************************************************
* Function:        void cmd_ftzr(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:
* Overview:        FTZ RS232 Command : read temperature compensation
***********************************************************************/
void cmd_ftzr(void)
{
    if (is_compensate)
    {
        put_char(1);
    }
    else
    {
        put_char(0);
    }
}

/**********************************************************************
* Function:        void cmd_ftcr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FTCR RS232 Command : read temperature coefficient
***********************************************************************/
void cmd_ftcr(void)
{
    answer(temperature_coef);
}

/**********************************************************************
* Function:        void cmd_ftcl(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FTCR RS232 Command : set temperature coefficient
***********************************************************************/
void cmd_ftcl(void)
{
    if (read_number(&temperature_coef, 2) == TRUE)
    {
        eep_write(eep_adr_temperature_coef, (unsigned char *)&temperature_coef, sizeof(temperature_coef));
        answer( temperature_coef);
    }
}

/**********************************************************************
* Function:        void focus_temperature(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   start a measure on sensor 1 for focus conpensation
***********************************************************************/
void focus_temperature(void)
{
    if (is_compensate)
    {
        start_measure(0);
        is_measuring = 1;
    }
}

unsigned int focus_compensate_in(void)
{
    unsigned int move;

    move = (last_temperature - temperature[0]) * temperature_coef / 10;
    stepper_relative_move(move, DIRECTION_IN);
    return move;
}

unsigned int focus_compensate_out(void)
{
    unsigned int move;

    move = (temperature[0] - last_temperature) * temperature_coef / 10;
    stepper_relative_move(move, DIRECTION_OUT);
    return move;
}

/**********************************************************************
* Function:        void focus_compensate(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   move focuser according to temperature
***********************************************************************/
char stmp[8];
void focus_compensate(void)
{
    unsigned int move = 0;
    unsigned char s;

    if (temperature_coef && last_temperature != -30000)
    {
        if (last_temperature > temperature[0])
        {
            if (!config_byte.positive_delta_temp_move_in)
            {
                move = focus_compensate_in();
                s = '-';
            }
            else
            {
                move = focus_compensate_out();
                s = ' ';
            }
        }
        else if (last_temperature < temperature[0])
        {
            if (!config_byte.positive_delta_temp_move_in)
            {
                move = focus_compensate_out();
                s = ' ';
            }
            else
            {
                move = focus_compensate_in();
                s = '-';
            }
        }
        if (move != 0)
        {
//            put_rom_string((const char *)"Compensation : ");
//            itoa(temperature[0], stmp);
//            put_string(stmp);
//            put_rom_string((const char *)" last=");
//            itoa(last_temperature, stmp);
//            put_string(stmp);
//            put_rom_string((const char *)" move= ");
//            put_char(s);
//            itoa(move, stmp);
//            put_string(stmp);
//            RC;
            last_temperature = temperature[0];
        }
    }
    else
        last_temperature = temperature[0];
}
