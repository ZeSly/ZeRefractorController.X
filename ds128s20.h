/*********************************************************************
 *
 *                 DS128S20 sensor temperature definitions
 *
 *********************************************************************
 * FileName:        ds128s20.h
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
 * Sylvain Girard   	12/04/2011  Creation
 ********************************************************************/

#ifndef __DS128S20_H
#define __DS128S20_H

#define COUNT_REMAIN    6
#define COUNT_PER_C     7

//******* P R O T O T Y P E S ***********************************************************/
unsigned char init_one_ds18s20(void);
unsigned char init_ds18s20(void);
void read_temperature(unsigned char n);
void convert_t(unsigned char n);
void start_measure(unsigned char n);

void print_sn(void);

#ifndef __DS128S20_C
extern int temperature[2];
extern unsigned char is_measuring;
#endif

#endif // __DS128S20_H
