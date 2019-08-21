/*********************************************************************
 *
 *                 ZeFocuser software - main
 *
 *********************************************************************
 * FileName:        messages.h
 * Dependencies:
 * Processor:       PIC18F2550
 * Complier:        XC8 v1.45
 *
 * Software License Agreement
 *
 * Copyright © 2012-2019 Sylvain Girard
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sylvain Girard   	09/06/2011  Creation
 *                      26/02/2012  Adaptation for PIC15f2550 USB
 *                                  @48MHz
 ********************************************************************/

#ifndef _MESSAGES_H
#define _MESSAGES_H

//******* D E F I N I T I O N S ************************************************/
#define MSG_QUEUE_SIZE  16
// timer periode : 0xFA23 = 0xFFFF - 625*2.4 = 1ms
#define MSG_TIMER_H     0xFA
#define MSG_TIMER_L     0x23

#define BUTTONS_MASK    0x30
#define TEMPO_BUTTONS   50

/*typedef enum
{
    NONE,
    CHK_SENSOR,
    CHK_CMD,
    FOCUS_TEMP,
    LED,
    BUTTONS_UP,
    BUTTONS_DOWN
}
e_msg;*/

#define NONE            0
#define CHK_SENSOR      1
#define CHK_CMD         2
#define FOCUS_TEMP      3
#define CHK_POWER       4
#define BUTTONS_UP      5
#define BUTTONS_DOWN    6
#define TIMER           7
#define LED             8

#define e_msg unsigned char

typedef union
{
    unsigned char byte;
    struct
    {
        unsigned is_moving :1;
        unsigned last_direction :1;
        unsigned manual_move :1;
        unsigned park :1;
    };
} u_stepper_flag;


//******* P R O T O T Y P E S ***********************************************************/
void timer1_isr(void);
void msg_queue_init(void);
void msg_post(e_msg msg);
e_msg msg_get(void);

void start_timer(unsigned char num_timer, unsigned long elapse);
void stop_timer(unsigned char num_timer);

void process_messages(void);

#ifndef _MESSAGES_C
extern unsigned char nb_flash_red;
extern unsigned char nb_flash_green;
#endif

#endif // _MESSAGES_H
