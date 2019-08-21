/*********************************************************************
 *
 *                 1-Wire Communication Protocol
 *
 *********************************************************************
 * FileName:        1wire.c
 * Dependencies:
 * Processor:       PIC18
 * Complier:        XC18 v1.45
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright © 2004-2007 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, copy and distribute Software
 * only when embedded on a Microchip microcontroller or digital signal
 * controller and used with a Microchip radio frequency transceiver, which
 * are integrated into your product or third party product (pursuant to the
 * sublicense terms in the accompanying license agreement).  You may NOT
 * modify or create derivative works of the Software.
 *
 *
 * You should refer to the license agreement accompanying this Software for
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED
 * UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF
 * WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR
 * EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT,
 * PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY
 * THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER
 * SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sasha     			12/20/07    Original
 * Sylvain Girard   	03/27/11    Added OW_calc_CRC (not tested)
 ********************************************************************/

/****** I N C L U D E S **********************************************************/
#include "system.h"
#include "1wire.h"

#define DI  { INTCONbits.GIEL = 0; INTCONbits.GIEH = 0; }
#define EI  { INTCONbits.GIEL = 1; INTCONbits.GIEH = 1; }

//****** V A R I A B L E S ********************************************************/
unsigned char macro_delay;

/**********************************************************************
 * Function:        void drive_OW_low (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.
 ***********************************************************************/
void drive_OW_low(void)
{
    OW_PIN_DIRECTION = OUTPUT;
    OW_WRITE_PIN = LOW;
}

/**********************************************************************
 * Function:        void drive_OW_high (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   None
 * Overview:		   Configure the OW_PIN as Output and drive the OW_PIN HIGH.
 ***********************************************************************/
void drive_OW_high(void)
{
    OW_PIN_DIRECTION = OUTPUT;
    OW_WRITE_PIN = HIGH;
}

/**********************************************************************
 * Function:        unsigned char read_OW (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the status of OW pin.
 * Overview:		   Configure as Input pin and Read the status of OW_PIN
 ***********************************************************************/
unsigned char read_OW(void)
{
    unsigned char read_data = 0;

    OW_WRITE_PIN = INPUT;

    if (HIGH == OW_READ_PIN)
        read_data = SET;
    else
        read_data = CLEAR;

    return read_data;
}

/**********************************************************************
 * Function:        unsigned char OW_reset_pulse(void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the Presense Pulse from the slave.
 * Overview:		   Initialization sequence start with reset pulse.
 *				   This code generates reset sequence as per the protocol
 ***********************************************************************/
unsigned char OW_reset_pulse(void)
{
    unsigned char presence_detect;

    macro_delay = 0;    // access to macro_delay will set BSR to be sure to access it in wait() asm macro
    
    DI;

    drive_OW_low(); // Drive the bus low

    wait(DELAY_240Us); // delay 480 microsecond (us)
    wait(DELAY_240Us);

    drive_OW_high(); // Release the bus

    wait(DELAY_70Us); // delay 70 microsecond (us)

    presence_detect = read_OW(); //Sample for presence pulse from slave

    wait(DELAY_205Us); // delay 410 microsecond (us)
    wait(DELAY_205Us);

    drive_OW_high(); // Release the bus

    EI;

    return presence_detect;
}

/**********************************************************************
 * Function:        void OW_write_bit (unsigned char write_data)
 * PreCondition:    None
 * Input:		   Write a bit to 1-wire slave device.
 * Output:		   None
 * Overview:		   This function used to transmit a single bit to slave device.
 *
 ***********************************************************************/

void OW_write_bit(unsigned char write_bit)
{
    macro_delay = 0;    // access to macro_delay will set BSR to be sure to access it in wait() asm macro
    DI;

    if (write_bit)
    {
        //writing a bit '1'
        drive_OW_low(); // Drive the bus low
        wait(DELAY_6Us); // delay 6 microsecond (us)
        drive_OW_high(); // Release the bus
        wait(DELAY_64Us); // delay 64 microsecond (us)
    }
    else
    {
        //writing a bit '0'
        drive_OW_low(); // Drive the bus low
        wait(DELAY_60Us); // delay 60 microsecond (us)
        drive_OW_high(); // Release the bus
        wait(DELAY_10Us); // delay 10 microsecond for recovery (us)
    }

    EI;
}

/**********************************************************************
 * Function:        unsigned char OW_read_bit (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the status of the OW PIN
 * Overview:		   This function used to read a single bit from the slave device.
 *
 ***********************************************************************/

unsigned char OW_read_bit(void)
{
    unsigned char read_data;

    macro_delay = 0;    // access to macro_delay will set BSR to be sure to access it in wait() asm macro

    DI;
    //reading a bit
    drive_OW_low(); // Drive the bus low
    wait(DELAY_6Us); // delay 6 microsecond (us)
    drive_OW_high(); // Release the bus
    wait(DELAY_9Us); // delay 9 microsecond (us)

    read_data = read_OW(); //Read the status of OW_PIN

    wait(DELAY_55Us); // delay 55 microsecond (us)

    EI;
    return read_data;
}

/**********************************************************************
 * Function:        void OW_write_byte (unsigned char write_data)
 * PreCondition:    None
 * Input:		   Send byte to 1-wire slave device
 * Output:		   None
 * Overview:		   This function used to transmit a complete byte to slave device.
 *
 ***********************************************************************/
void OW_write_byte(unsigned char write_data)
{
    unsigned char loop;

    for (loop = 0; loop < 8; loop++)
    {
        OW_write_bit(write_data & 0x01); //Sending LS-bit first
        write_data >>= 1; // shift the data byte for the next bit to send
    }
}

/**********************************************************************
 * Function:        unsigned char OW_read_byte (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the read byte from slave device
 * Overview:		   This function used to read a complete byte from the slave device.
 *
 ***********************************************************************/

unsigned char OW_read_byte(void)
{
    unsigned char loop, result = 0;

    for (loop = 0; loop < 8; loop++)
    {

        result >>= 1; // shift the result to get it ready for the next bit to receive
        if (OW_read_bit())
            result |= 0x80; // if result is one, then set MS-bit
    }

    return result;
}

/********************************************************************************************
                  E N D     O F     1 W I R E . C
 *********************************************************************************************/

static unsigned char crc88540_table[256] =
{
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

unsigned char OW_calc_CRC(unsigned char *data)
{
    unsigned char count = 8;
    unsigned char result = 0;

    while (count--)
    {
      result = crc88540_table[result ^ *data++];
    }

    return result;
}
