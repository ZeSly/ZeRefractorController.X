/*********************************************************************
 *
 *                 DS128S20 sensor temperature function
 *
 *********************************************************************
 * FileName:        ds128s20.c
 * Dependencies:
 * Processor:       PIC18F2525
 * Complier:        XC8 v1.45
 *
 * Software License Agreement
 *
 * Copyright � 2011-2019 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	12/04/2011  Creation
 ********************************************************************/

#define __DS128S20_C

#include <stdio.h>
#include <string.h>

#include "hid_focus.h"

#include "system.h"
#include "ds128s20.h"
#include "1wire.h"

#define MAX_DS18B20 8

extern unsigned char ToSendDataBuffer[];

int temperature[MAX_DS18B20];

// Serial numbers
static unsigned char ds18s20_sn[MAX_DS18B20][8];

/**********************************************************************
 * Function:        void init_one_ds18s20 (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Read serial number of the only DS18S20 on the bus
 ***********************************************************************/
unsigned char init_one_ds18s20(void)
{
    unsigned char i;

    if (!OW_reset_pulse()) // Is slave present
    {
        OW_write_byte(0x33); // Send a command to read a serial number

        for (i = 0; i < 8; i++)
            ds18s20_sn[0][i] = OW_read_byte(); // Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device

        //nb_device = 1;
        return 1;
    }
    return 0;
}

/**********************************************************************
 * Function:        void init_ds18s20 (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Check for DS18S20 devices on the 1-wire bus 
 ***********************************************************************/
/*
 device 1: xxx10101100
device 2: xxx01010101
device 3: xxx10101111
device 4: xxx10001000
 
                    0 p0=0 c=1
device 4: xxx10001000
device 1: xxx10101100 
                  0   p2=0 c=2
device 4: xxx10001000

                    0 c!=1
device 4: xxx10001000
device 1: xxx10101100 
                  1   c==2 p2=1 c=1
device 1: xxx10101100 

                    1 c==1 p0=1 c=0
device 2: xxx01010101
device 3: xxx10101111
                   0  p1=0 c=1
device 2: xxx01010101

                    1 p0==1 c=1
device 2: xxx01010101
device 3: xxx10101111
                   1  c==1 p1=1 c=0
device 3: xxx10101111
*/
unsigned char init_ds18s20(void)
{
    unsigned char i, j;
    unsigned char p[64];
    unsigned char c = 0;
    unsigned char two_bit;
    unsigned char n;
    unsigned char nb_device = 0;

    for (n = 0; n < MAX_DS18B20; n++)
        temperature[n] = -30000;
    
    memset(p, 255, 64);
    if (!OW_reset_pulse())
    { // Is slave present
        nb_device++;
        for (n = 0; n < MAX_DS18B20; n++)
        {
            OW_write_byte(0xF0); // Send a search ROM command

            for (i = 0; i < 8; i++)
            {
                ds18s20_sn[n][i] = 0;

                for (j = 0; j < 8; j++)
                {
                    unsigned char pos = i * 8 + j;

                    two_bit = OW_read_bit() << 1;
                    two_bit |= OW_read_bit();
                    if (two_bit != 0)
                    {
                        two_bit >>= 1;
                    }
                    else if (p[pos] == 255)
                    { // unknown conflict on current bit, select bit 0
                        p[pos] = two_bit = 0;
                        c = pos;
                        two_bit = 0;
                        nb_device++;
                    }
                    else if (p[pos] == 0 && c == pos)
                    { // known conflict on current bit, select bit 1
                        p[pos] = two_bit = 1;
                        // go to next conflict
                        do
                        {
                            c--;
                        } while (p[c] != 0 && c != 0);
                        // reset all conflict at higher position
                        for (int r = pos + 1; r < 64; r++)
                            p[r] = 255;
                    }
                    else
                    { // solved conflict, select saved bit
                        two_bit = p[pos];
                    }
                    OW_write_bit(two_bit);
                    ds18s20_sn[n][i] >>= 1;
                    if (two_bit)
                        ds18s20_sn[n][i] |= 0x80;
                }
            }
            OW_reset_pulse();
        }
    }
    return nb_device;
}

/**********************************************************************
 * Function:        void print_sn(unsigned char n)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Send a ROM buffer (serial number) on the usart
 **********************************************************************/
void print_sn()
{
    unsigned char n, i;

    put_char(nb_devices + '0');
    put_char('\n');
    for (n = 0; n < nb_devices; n++)
    {
        for (i = 0; i < 8; i++)
        {
            hex_ascii(ds18s20_sn[n][i]);
        }
        put_char('\n');
    }
    put_char(0);
}

/**********************************************************************
 * Function:        void select_ds18s20(unsigned char n)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Initiate communication on the 1 wire bus and select
 *                  ds18s20 n device
 ***********************************************************************/
void select_ds18s20(unsigned char n)
{
    unsigned char i;

    OW_reset_pulse();
//    OW_write_byte(0xCC);
    OW_write_byte(0x55); // Send a match ROM command
    for (i = 0; i < 8; i++)
        OW_write_byte(ds18s20_sn[n][i]);
}

/**********************************************************************
 * Function:        int read_temperature(void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:
 ***********************************************************************/
unsigned char is_measuring = 0;

void read_temperature(unsigned char n)
{
    int count_remain, count_per_c;
    unsigned char OW_buffer[9];
    unsigned char i;

    select_ds18s20(n);
    OW_write_byte(0xBE); // Send a Read Scratchpad command
    for (i = 0; i < 9; i++)
        OW_buffer[i] = OW_read_byte(); // Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device

    i = OW_calc_CRC(OW_buffer);

    if (i == OW_buffer[8])
    {
        temperature[n] = OW_buffer[1];
        temperature[n] = temperature[n] << 8;
        temperature[n] |= OW_buffer[0];
        if (ds18s20_sn[n][0] == 0x10) // DS18S20
        {

            temperature[n] >>= 1;
            temperature[n] *= 100;

            count_per_c = (int) OW_buffer[COUNT_PER_C] * 100;
            count_remain = (int) OW_buffer[COUNT_REMAIN] * 100;
            //    IdxToSendDataBuffer = sprintf((char *)ToSendDataBuffer,(const farchar *)"temp=%d\r\ncnt_per_c=%d\r\ncnt_remain=%d\r\n", temperature[n], count_per_c, count_remain);

            temperature[n] = temperature[n] - 25 + (count_per_c - count_remain) / (int) OW_buffer[COUNT_PER_C];
            temperature[n] /= 10;
        }
        else // DS18B20
        {
            count_remain = temperature[n] & 0xF;
            count_remain *= 10;
            count_remain /= 10;
            temperature[n] >>= 4;
            temperature[n] *= 10;
            temperature[n] += count_remain;
        }
    }
}

/**********************************************************************
 * Function:        int read_temperature(void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:
 ***********************************************************************/
void start_measure(unsigned char n)
{
    select_ds18s20(n);
    OW_write_byte(0x44); // Send a Convert T command
    //    LED_VERTE = 1;
}
