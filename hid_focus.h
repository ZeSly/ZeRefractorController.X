/*********************************************************************
 *
 *                  HID USB inteface
 *
 *********************************************************************
 * FileName:        hid_focus.h
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
 * Sylvain Girard   	01/04/2011  Creation
 ********************************************************************/

 #ifndef _HID_POWER_H
 #define _HID_POWER_H

//******* D A T A   S T R U C T U R E S ******************************************************/
typedef void (*hid_command)(void) ;

//******* P R O T O T Y P E S ***********************************************************/
unsigned char check_command(void);

void put_string(char *mess);
#define put_rom_string(s) put_string((char *)s)
//void put_rom_string(const char *mess);
void put_char (char send_data);
void hex_ascii(unsigned char display_data);
void answer(unsigned int n);

unsigned char read_number(unsigned int *val, unsigned char lg);


#endif // _HID_POWER_H

