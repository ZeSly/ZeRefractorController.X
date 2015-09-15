/*********************************************************************
 *
 *                 Dew heater PWM controm
 *
 *********************************************************************
 * FileName:        heater.c
 * Dependencies:
 * Processor:       PIC18F2550
 * Complier:        MCC18 v3.40
 *
 * Software License Agreement
 *
 * Copyright © 2011 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	05/04/2012  Creation
 ********************************************************************/

#define _HEATER_C

#include <xc.h>
#include "system.h"
#include "hid_focus.h"

unsigned int heat = 0;

void warm_up(unsigned int pwm)
{
    if (pwm == 0)
    {
        T2CONbits.TMR2ON = 0;
        CCP1CON = 0;
        LATCbits.LATC2 = 0;
    }
    else
    {
        if (pwm > 1023)
        {
            T2CONbits.TMR2ON = 0;
            CCP1CON = 0;
            LATCbits.LATC2 = 1;
        }
        else
        {
            PR2 = 255;
            CCPR1L = (pwm >> 2) & 0xFF;
            CCP1CON = (pwm << 4) & 0x30;
            TRISCbits.TRISC2 = 0;
            T2CON = 3;     // prescaler 1:16
            T2CONbits.TMR2ON = 1;
            CCP1CON |= 0x0F;         // PWM Mode
        }
    }
}

void cmd_fhr(void)
{
    answer( heat);
}

void cmd_fhl(void)
{
    unsigned int h;

    if (read_number(&h, 2) == TRUE)
    {
        heat = h;
        warm_up(h);
        answer(heat);
    }
}