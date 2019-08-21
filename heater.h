/*********************************************************************
 *
 *                 Dew heater PWM controm
 *
 *********************************************************************
 * FileName:        heater.c
 * Dependencies:
 * Processor:       PIC18F2550
 * Complier:        XC8 v1.45
 *
 * Software License Agreement
 *
 * Copyright © 2011-2019 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	05/04/2012  Creation
 ********************************************************************/

#ifndef _HEATER_H
#define _HEATER_H

void cmd_fhr(void);
void cmd_fhl(void);

#endif // _HEATER_H