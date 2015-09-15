/*********************************************************************
 *
 *                 Data EEPROM memory access
 *
 *********************************************************************
 * FileName:        eep.h
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
 * Sylvain Girard   	15/05/2011  Creation
 ********************************************************************/

#ifndef _EEP_H
#define _EEP_H

#define EEP_SIGNATURE 1831

//******* P R O T O T Y P E S *********************************************************/
void eep_read(unsigned char adr, unsigned char *buf, unsigned int lg);
void eep_write(unsigned char adr, unsigned char *buf, unsigned int lg);
void load_param(void);

//******* V A R I A B L E S ***********************************************************/
#ifndef _EEP_C
extern unsigned char eep_adr_config_byte;
extern unsigned char eep_adr_stepper_delay_manual;
extern unsigned char eep_adr_stepper_delay_auto;
extern unsigned char eep_adr_stepper_pos_max;
extern unsigned char eep_adr_stepper_pos_min;
extern unsigned char eep_adr_stepper_position;
extern unsigned char eep_adr_stepper_backlash;
extern unsigned char eep_adr_temperature_coef;
extern unsigned char eep_adr_p_step_sequencer;
extern unsigned char eep_adr_pwr_threshold;
#endif

#endif // _EEP_H
