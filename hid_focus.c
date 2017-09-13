/*********************************************************************
 *
 *                  HID USB inteface
 *
 *********************************************************************
 * FileName:        hid_focus.c
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
 * Sylvain Girard       01/04/2011  Creation
 ********************************************************************/

#define _HID_POWER_C

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"
//#include <delays.h>
#include "hid_focus.h"
#include "positionning.h"
#include "temperature.h"
#include "setup.h"
#include "eep.h"
#include "adc.h"
#include "heater.h"
#include "ds128s20.h"
#include "build_number.h"
#include <BootPIC18NonJ.h>

extern unsigned char ReceivedDataBuffer[];
extern unsigned char ToSendDataBuffer[];

unsigned char IdxReceivedDataBuffer = 0;
unsigned char IdxToSendDataBuffer = 0;

static char buf_cmd[16];
static unsigned char idx_cmd = 0;


const unsigned int VersionWord @APP_VERSION_ADDRESS =
((unsigned int) VERSION_MAJOR << 12) |
((unsigned int) VERSION_MINOR << 8) |
BUILD_NUMBER;

const char About[] = "Ze Refractor Controller\r\n(c) 2012 Sylvain Girard\r\nsylvain.girard@zesly.net\r\n";

void cmd_connection(void);
void cmd_deconnection(void);
void cmd_version(void);
void cmd_about(void);
void cmd_frd(void);
void cmd_firmware(void);

/**********************************************************************
* RS232 command function pointers array
***********************************************************************/
static hid_command hid_command_tab[] =
{
    /* Compatibilty and other useless commands */
    cmd_version,
    cmd_about,
    print_sn,

    /* Temperature commands */
    cmd_ftm,        // Measure temperature sensor 1
    cmd_ft2m,       // Measure temperature sensor 2
    cmd_ftr,        // Read temperature sensor 1
    cmd_ft2r,       // Read temperature sensor 2
    cmd_ftzr,       // set temperature compensation status
    cmd_ftzl,       // read temperature compensation status
    cmd_ftcr,       // read temperature coefficient
    cmd_ftcl,       // set temperature coefficient

    /* Motor positionning control */
    cmd_fpr,        // read motor position
    cmd_fpl,        // set motor new position
    cmd_fpd,        // return the direction of the last move
    cmd_fpi,        // move motor inside direction
    cmd_fpo,        // move motor outside direction
    cmd_fpa,        // move motor absolute direction
    cmd_fph,        // halt motor

    /* Dew heater control */
    cmd_fhr,        // read heating power
    cmd_fhl,        // set heating power

    /* Setup command */
    cmd_fsar,       // read delay between step auto
    cmd_fsal,       // setup delay between step auto
    cmd_fsmr,       // read delay between step manual
    cmd_fsml,       // setup delay between step manual
    cmd_fssr,       // read stepper mode
    cmd_fssl,       // setup stepper mode
    cmd_fsir,       // read min position
    cmd_fsil,       // setup min position
    cmd_fsor,       // read max position
    cmd_fsol,       // setup max position
    cmd_fspr,       // read initial position
    cmd_fspl,       // set initial position
    cmd_fspc,       // set current postion as initial position
    cmd_fsbr,       // read backlash value
    cmd_fsbl,       // set backlash value
    cmd_fscr,       // read config_byte
    cmd_fscl,       // set config_byte

    /* Power supply read command */
    cmd_power,      // read power suply voltage
    cmd_fvar,       // read power suply alarm threshold
    cmd_fval,       // set power suply alarm threshold

    /* Humidity and anti-dew */
    cmd_humidity,   // read humidity

    cmd_firmware,

    /* debug command, may be commented */
    cmd_frd,        // read EEPROM (debug)
    //cmd_fvb,

    NULL
};

void cmd_version(void)
{
    char Version[8];

    sprintf(Version, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, BUILD_NUMBER);
    put_string(Version);
}

void cmd_about(void)
{
    put_rom_string(About);
}

/**********************************************************************
* Function:        void check_command (void)
* PreCondition:    None
* Input:           None
* Output:          None
* Overview:        check command and launch the corresponding function
***********************************************************************/
unsigned char check_command(void)
{
    unsigned char i;

    for (i = 0 ; hid_command_tab[i] != NULL ; i++)
    {
        if (*ReceivedDataBuffer == i + 0x20)
        {
            IdxReceivedDataBuffer = 1;
            ToSendDataBuffer[IdxToSendDataBuffer++] = i + 0x20;
            hid_command_tab[i]();
//            if (IdxToSendDataBuffer > 1)
//            {
                IdxToSendDataBuffer = 0;
                return 1;
//            }
//            return 0;
        }
    }
    return 0;
}

/**********************************************************************
* Function:        void put_char (unsigned char send_data)
* PreCondition:    None
* Input:           Byte to transmit
* Output:          None
* Side Effects:
* Overview:        Transmits a byte to terminal through UART.
***********************************************************************/
void put_char (char send_data)
{
    if (IdxToSendDataBuffer < 63)
    {
        ToSendDataBuffer[IdxToSendDataBuffer++] = send_data;
    }
}


/**********************************************************************
* Function:        void put_string(unsigned char *mess)
* PreCondition:    None
* Input:		   Pointer to display message
* Output:		   None
* Side Effects:
* Overview:		   Sends the string to terminal till it finds NULL character.
***********************************************************************/
void put_string(char *mess)
{
    while (*mess != '\0')
    {
        put_char(*mess++);
    }
    put_char(0);
}

//void put_rom_string(const char *mess)
//{
//    while (*mess != '\0')
//    {
//        put_char(*mess++);
//    }
//    put_char(0);
//}

/**********************************************************************
* Function:        void answer(,unsigned int n)
* PreCondition:    None
* Input:           unsigned int n
* Output:          None
* Side Effects:
* Overview:        put an integer in the TX usb buffer
***********************************************************************/
void answer(unsigned int n)
{
    ToSendDataBuffer[IdxToSendDataBuffer++] = (n >> 8) & 0xFF;
    ToSendDataBuffer[IdxToSendDataBuffer++] = n & 0xFF;
}

/**********************************************************************
* Function:         unsigned char read_number(unsigned int *val)
* PreCondition:     None
* Input:            val : Pointer to an integer
*                   lg : number of byte to read
* Output:           TRUE or FALSE
* Side Effects:
* Overview:         Read a numerical value
***********************************************************************/
unsigned char read_number(unsigned int *val, unsigned char lg)
{
    unsigned char i, j;
    unsigned int b;

    *val = 0;
    j = lg;
    for (i = 0 ; i < lg ; i++)
    {
        j--;
        b = ReceivedDataBuffer[i + IdxReceivedDataBuffer];
        *val |= b << (j * 8);
    }
    return TRUE;
}

/**********************************************************************
* Function:         void hex_ascii(unsigned char display_data)
* PreCondition:     None
* Input:            HEX Value
* Output:           None
* Side Effects:
* Overview:         Convert Hex Value to ASCII and send to PC;
***********************************************************************/
void hex_ascii(unsigned char display_data)
{
    unsigned char temp, i;
    char str[8];

    i = 0;
    temp = ((display_data & 0xF0)>>4);
    if (temp <= 0x09)
            str[i++] = temp + '0';
    else
            str[i++] = temp + 'A' - 10;

    temp = display_data & 0x0F;
    if (temp <= 0x09)
            str[i++] = temp + '0';
    else
            str[i++] = temp + 'A' - 10;

    str[i++] = 0;
    put_string(str);
}

/**********************************************************************
 * Function:         void cmd_frd(void)
 * PreCondition:     None
 * Input:            None
 * Output:           None
 * Side Effects:
 * Overview:         frd command : read a byte in the EEPROM for debug
 ***********************************************************************/
void cmd_frd(void)
{
    unsigned int adr;
    unsigned char data;

    if (read_number(&adr, 1) == TRUE)
    {
        eep_read(adr, &data, 1);
        hex_ascii((unsigned char) adr >> 8);
        hex_ascii((unsigned char) adr & 0xFF);
        put_rom_string((char *)" = ");
        hex_ascii(data);
        put_char('\r');
        put_char('\n');
//        pt = (char *) ToSendDataBuffer;
//        pt += sprintf(pt,(const farchar *)"%03x=%02x\r\n",adr,data);
    }
}

/**********************************************************************
 * Function:         void cmd_firmware(void)
 * PreCondition:     None
 * Input:            None
 * Output:           None
 * Side Effects:
 * Overview:         firmware update command : switch to bootloader
 ***********************************************************************/
void cmd_firmware(void)
{
    #asm
        goto BOOTLOADER_ABSOLUTE_ENTRY_ADDRESS
    #endasm
}