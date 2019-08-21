/*********************************************************************
 *
 *                 Data EEPROM memory access
 *
 *********************************************************************
 * FileName:        eep.c
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
 * Sylvain Girard   	15/05/2011  Creation
 ********************************************************************/

#define _EEP_C

#include <p18cxxx.h>
#include "system.h"
#include "eep.h"
#include "stepper.h"
#include "temperature.h"
#include "adc.h"

unsigned char eep_adr_config_byte;
unsigned char eep_adr_stepper_delay_manual;
unsigned char eep_adr_stepper_delay_auto;
unsigned char eep_adr_stepper_pos_max;
unsigned char eep_adr_stepper_pos_min;
unsigned char eep_adr_stepper_position;
unsigned char eep_adr_stepper_backlash;
unsigned char eep_adr_temperature_coef;
unsigned char eep_adr_p_step_sequencer;
unsigned char eep_adr_pwr_threshold;

 /**********************************************************************
* Function:        void eep_read(unsigned int adr, unsigned char *buf, unsigned int lg)
* PreCondition:    None
* Input:		   EEPROM address
* Output:		   byte
* Side Effects:
* Overview:		   Read the data EEPROM memory
***********************************************************************/
void eep_read(unsigned char adr, unsigned char *buf, unsigned int lg)
 {
    //adr += 0x20;
    while (lg--)
    {
//        EEADRH = adr >> 8;
        EEADR = adr;
        EECON1bits.EEPGD = 0;
        EECON1bits.CFGS = 0;
        EECON1bits.RD = 1;
        *buf++ = EEDATA;
        adr++;
    }
 }

/**********************************************************************
* Function:        void eep_write(unsigned int adr, unsigned char *buf, unsigned int lg)
* PreCondition:    None
* Input:		   EEPROM address
* Output:		   byte
* Side Effects:
* Overview:		   Write in the data EEPROM memory
***********************************************************************/
void eep_write(unsigned char adr, unsigned char *buf, unsigned int lg)
 {
    //adr += 0x20;
    while (lg--)
    {
//        EEADRH = adr >> 8;
        EEADR = adr;
        EEDATA = *buf;
        EECON1bits.EEPGD = 0;
        EECON1bits.CFGS = 0;
        EECON1bits.WREN = 1;
        //INTCONbits.GIE = 0;
        EECON2 = 0x55;
        EECON2 = 0xAA;
        EECON1bits.WR = 1;
        //INTCONbits.GIE = 1;
        while (PIR2bits.EEIF == 0)
            ;
        PIR2bits.EEIF = 0;
        EECON1bits.WREN = 0;
        buf++;
        adr++;
    }
 }

/**********************************************************************
* Function:        void load_param(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Side Effects:
* Overview:		   Load parameters from EEPROM
***********************************************************************/
void load_param(void)
{
    unsigned char adr;
    unsigned int signature, nb_essai = 3;

    adr = 0;
    while (--nb_essai && signature != EEP_SIGNATURE)
    {
        eep_read(adr, (unsigned char *)&signature, sizeof(signature));
    }

    adr += sizeof(signature);
    if (signature == EEP_SIGNATURE)
    {
        eep_adr_config_byte = adr;
        eep_read(adr, (unsigned char *)&config_byte.byte, sizeof(config_byte.byte));
        adr += sizeof(config_byte.byte);

        eep_adr_stepper_delay_manual = adr;
        eep_read(adr, (unsigned char *)&stepper_delay_manual, sizeof(stepper_delay_manual));
        adr += sizeof(stepper_delay_manual);

        eep_adr_stepper_delay_auto = adr;
        eep_read(adr, (unsigned char *)&stepper_delay_auto, sizeof(stepper_delay_auto));
        adr += sizeof(stepper_delay_auto);

        eep_adr_stepper_pos_max = adr;
        eep_read(adr, (unsigned char *)&stepper_pos_max, sizeof(stepper_pos_max));
        adr += sizeof(stepper_pos_max);

        eep_adr_stepper_pos_min = adr;
        eep_read(adr, (unsigned char *)&stepper_pos_min, sizeof(stepper_pos_min));
        adr += sizeof(stepper_pos_min);

        eep_adr_stepper_position = adr;
        eep_read(adr, (unsigned char *)&stepper_position, sizeof(stepper_position));
        adr += sizeof(stepper_position);

        eep_adr_stepper_backlash = adr;
        eep_read(adr, (unsigned char *)&stepper_backlash, sizeof(stepper_backlash));
        adr += sizeof(stepper_backlash);

        eep_adr_temperature_coef = adr;
        eep_read(adr, (unsigned char *)&temperature_coef, sizeof(temperature_coef));
        adr += sizeof(temperature_coef);

        eep_adr_p_step_sequencer = adr;
        eep_read(adr, (unsigned char *)&p_step_sequencer, sizeof(p_step_sequencer));
        adr += sizeof(p_step_sequencer);

        eep_adr_pwr_threshold = adr;
        eep_read(adr, (unsigned char *)&pwr_threshold, sizeof(pwr_threshold));
        adr += sizeof(pwr_threshold);
    }
    else
    {
        // bad signature, writing default values
        eep_adr_config_byte = adr;
        eep_write(adr, (unsigned char *)&config_byte.byte, sizeof(config_byte.byte));
        adr += sizeof(config_byte.byte);

        eep_adr_stepper_delay_manual = adr;
        eep_write(adr, (unsigned char *)&stepper_delay_manual, sizeof(stepper_delay_manual));
        adr += sizeof(stepper_delay_manual);

        eep_adr_stepper_delay_auto = adr;
        eep_write(adr, (unsigned char *)&stepper_delay_auto, sizeof(stepper_delay_auto));
        adr += sizeof(stepper_delay_auto);

        eep_adr_stepper_pos_max = adr;
        eep_write(adr, (unsigned char *)&stepper_pos_max, sizeof(stepper_pos_max));
        adr += sizeof(stepper_pos_max);

        eep_adr_stepper_pos_min = adr;
        eep_write(adr, (unsigned char *)&stepper_pos_min, sizeof(stepper_pos_min));
        adr += sizeof(stepper_pos_min);

        eep_adr_stepper_position = adr;
        eep_write(adr, (unsigned char *)&stepper_position, sizeof(stepper_position));
        adr += sizeof(stepper_position);

        eep_adr_stepper_backlash = adr;
        eep_write(adr, (unsigned char *)&stepper_backlash, sizeof(stepper_backlash));
        adr += sizeof(stepper_backlash);

        eep_adr_temperature_coef = adr;
        eep_write(adr, (unsigned char *)&temperature_coef, sizeof(temperature_coef));
        adr += sizeof(temperature_coef);

        eep_adr_p_step_sequencer = adr;
        eep_write(adr, (unsigned char *)&p_step_sequencer, sizeof(p_step_sequencer));
        adr += sizeof(p_step_sequencer);

        eep_adr_pwr_threshold = adr;
        eep_write(adr, (unsigned char *)&pwr_threshold, sizeof(pwr_threshold));
        adr += sizeof(pwr_threshold);

        signature = EEP_SIGNATURE;
        eep_write(0, (unsigned char *)&signature, sizeof(signature));
    }
}
