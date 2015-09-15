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
 * Sylvain Girard   	09/05/2011  Creation
 ********************************************************************/

#include <p18cxxx.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "hid_focus.h"
#include "eep.h"

#define AN0         0
#define AN1         4
#define VDD_VALUE   5033L
#define PWR_FACTOR  1538L    //(14700/47*VDD_VALUE/1023)

//const char str_pwr[] = "pwr=";
//const char str_fva[] = "fva=";
//
//const char str_fpwr[] = "fpwr=";
//const char str_fhmd[] = "fhmd=";

unsigned int pwr_threshold;

/**********************************************************************
* Function:        unsigned long read_power_suply(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   read power suplly voltage from AN0
***********************************************************************/
unsigned long read_adc(char channel)
{
    unsigned long adc;

    ADCON0 = channel;     // Select channel 0 (AN0)
    ADCON2 = 0b10100010;  // right justified, Fosc/32
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO == 1)
        ;
    adc = ADRESH;
    adc = adc << 8;
    adc |= ADRESL;
    ADCON0bits.ADON = 0;
    return adc;
}

/**********************************************************************
* Function:        unsigned long read_power_suply(void)
* PreCondition:    None
* Input:	   None
* Output:	   power supply voltage
* Side Effects:
* Overview:
***********************************************************************/
unsigned long read_power_suply(void)
{
    unsigned long voltage;

    voltage = read_adc(AN0);
    voltage *= PWR_FACTOR;
    return voltage;
}

/**********************************************************************
* Function:        void cmd_power(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPWR RS232 Command :read power suplly voltage
***********************************************************************/
void cmd_power(void)
{
    unsigned long voltage;
    unsigned int volt;

    voltage = read_power_suply();
    volt = voltage / 100;
    answer(volt);
}

/**********************************************************************
* Function:        void cmd_fpar(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPWR RS232 Command :read power suplly alarm threshold
***********************************************************************/
void cmd_fvar(void)
{
    answer(pwr_threshold);
}

/**********************************************************************
* Function:        void cmd_fpar(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPWR RS232 Command :set power suplly alarm threshold
***********************************************************************/
extern unsigned int n;
void cmd_fval(void)
{
    if (read_number(&n, 2) == TRUE)
    {
        pwr_threshold = n;
        eep_write(eep_adr_pwr_threshold, (unsigned char *)&pwr_threshold, sizeof(pwr_threshold));
        answer(pwr_threshold);
    }
}

/**********************************************************************
* Function:        unsigned long read_power_suply(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   read humidity sensor voltage from AN1
***********************************************************************/
unsigned int read_humidity(void)
{
    unsigned short sensor_value;
    unsigned int humidity;
    unsigned long vout;

    sensor_value = read_adc(AN1);
    vout = (unsigned long)sensor_value * VDD_VALUE / 1023L;
    humidity = (vout - 838) / 30;

    return humidity;
}

/**********************************************************************
* Function:        void cmd_power(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   FPWR RS232 Command :read power suplly voltage
***********************************************************************/
void cmd_humidity(void)
{
    unsigned int humidity;

    humidity = read_humidity();
    answer(humidity);
}

