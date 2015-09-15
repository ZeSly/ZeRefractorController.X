/*********************************************************************
 *
 *                 Analog Digital Converter
 *
 *********************************************************************
 * FileName:        adc.c
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
 * Sylvain Girard   	18/08/2011  Creation
 ********************************************************************/

#ifndef _ADC_H
#define _ADC_H

unsigned long read_power_suply(void);
void cmd_power(void);
void cmd_fvar(void);
void cmd_fval(void);

void cmd_humidity(void);

#ifndef _ADC_C
extern unsigned long voltage;
extern unsigned int pwr_threshold;
#endif

#endif //_ADC_H

