/*********************************************************************
 *
 *                 Temperature commands
 *
 *********************************************************************
 * FileName:        temperature.h
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
 * Sylvain Girard   	07/05/2011  Creation
 ********************************************************************/

#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

//******* P R O T O T Y P E S ***********************************************************/
void put_str_ftr(unsigned char sensor);
void send_temperature(int temperature);
void cmd_ftn();
void cmd_ftm(void);
void cmd_ft2m(void);
void cmd_ftr(void);
void cmd_ft2r(void);
// void cmd_ftd(void);
void cmd_ftzr(void);
void cmd_ftzl(void);
void cmd_ftcr(void);
void cmd_ftcl(void);
void focus_temperature(void);
void focus_compensate(void);

//****** V A R I A B L E S ********************************************************/
#ifndef _TEMPERATUE_C
extern unsigned int temperature_coef;
extern unsigned char is_cmd;
extern unsigned char is_compensate;
#endif

#endif // _TEMPERATURE_H
