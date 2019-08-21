/*********************************************************************
 *
 *                 Positionning commands
 *
 *********************************************************************
 * FileName:        positionning.h
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

 #ifndef _POSITIONNING_H
 #define _POSITIONNING_H

//******* P R O T O T Y P E S ***********************************************************/
void end_of_move(void);
void cmd_fpr(void);
void cmd_fpl(void);
void cmd_fpd(void);
void cmd_fpi(void);
void cmd_fpo(void);
void cmd_fpa(void);
void cmd_fph(void);

#endif // _POSITIONNING_H
