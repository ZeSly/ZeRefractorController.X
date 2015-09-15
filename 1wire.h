
#ifndef _1WIRE_H
#define _1WIRE_H

#define DELAY_6Us	6
#define DELAY_9Us	9
#define DELAY_10Us	10
#define DELAY_55Us	55
#define DELAY_60Us	60
#define DELAY_64Us	64
#define DELAY_70Us	70
#define DELAY_205Us	205			// DELAY_410Us = DELAY_205Us + DELAY_205Us
#define DELAY_240Us	240			// DELAY_480Us = DELAY_240Us + DELAY_240Us // since the variable is declared as unsigned char

//**********************************************************************
//#define CLOCK_FREQUENCY  	48000000
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/48000000)*4 = 0.0833µs
//		Total INST Cycle in a loop = 0.0833*12 = 1µs
//**********************************************************************

#define wait(a) __delay_us(a)
//#define wait(a) #asm\
//                    movlw a\
//                    movwf macro_delay,1\
//          loop:     nop\
//                    nop\
//                    nop\
//                    nop\
//                    nop\
//                    nop\
//                    nop\
//                    nop\
//                    nop\
//                    decfsz macro_delay,1,1\
//                    bra loop\
//                #endasm

//ONE WIRE PORT PIN DEFINITION
///****************************************************
// This Configuration is required to make any PIC MicroController
// I/O pin as Open drain to drive 1-wire.
///****************************************************
#define OW_PIN_DIRECTION    LATAbits.LATA3
#define OW_WRITE_PIN        TRISAbits.TRISA3
#define OW_READ_PIN         PORTAbits.RA3

/** I N C L U D E S **********************************************************/
#include <xc.h>


/** P R O T O T Y P E S ******************************************************/
//void drive_one_wire_low (void);
//void drive_one_wire_high (void);
//unsigned char read__one_wire (void);
void OW_write_bit (unsigned char write_bit);
unsigned char OW_read_bit (void);
unsigned char OW_reset_pulse(void);
void OW_write_byte (unsigned char write_data);
unsigned char OW_read_byte (void);
unsigned char OW_calc_CRC(unsigned char *buf);

#endif
