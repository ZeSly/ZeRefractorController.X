/*********************************************************************
 *
 *                 ZeFocuser software - messages queue manager
 *
 *********************************************************************
 * FileName:        messages.c
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
 * Sylvain Girard   	09/06/2011  Creation
 ********************************************************************/

#define _MESSAGES_C

#include "system.h"
#include "messages.h"
#include "hid_focus.h"
#include "1wire.h"
#include "ds128s20.h"
#include "temperature.h"
#include "stepper.h"
#include "adc.h"
#include "eep.h"

unsigned char msg_queue[16];
e_msg msg_in = 0;
e_msg msg_out = 0;
unsigned long tick = 0;
unsigned long timer[2] = { 0, 0};
unsigned char timer_on[2] = { FALSE, FALSE};
unsigned char nb_flash_red = 0;
unsigned char nb_flash_green = 0;

unsigned char buttons = BUTTONS_MASK;
void check_buttons(void);

void timer1_isr(void)
{
    static unsigned int led_timer = 900;

    if (tick % 100 == 0)
    {
        msg_post(CHK_SENSOR);
        msg_post(CHK_POWER);
    }

    check_buttons();

//    if (tick % 2000 == 0)
//        msg_post(FOCUS_TEMP);

    if (timer_on[0] && tick == timer[0])
    {
        msg_post(TIMER);
        timer_on[0] = FALSE;
    }
    if (timer_on[1] && tick == timer[1])
    {
        msg_post(TIMER | 0x10);
        timer_on[1] = FALSE;
    }

    if (tick % led_timer == 0 && (nb_flash_red || nb_flash_green))
    {
        if (led_timer == 900)
        {
            msg_post(0x80 | LED);
            led_timer = 100;
        }
        else
        {
            msg_post(LED);
            led_timer = 900;
        }
    }

    tick++;

    TMR1H = MSG_TIMER_H; // set Timer1 to timer_period
    TMR1L = MSG_TIMER_L;
}

/**********************************************************************
 * Function:        void msg_queue_init(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:
 * Overview:        Init timer1 for the message queue
 ***********************************************************************/
void msg_queue_init(void)
{
    IPR1bits.TMR1IP = 0; // set timer1 interrupr low priority
    T1CON = 0x30; // 16 bit Timer1, 1:8 Fosc/4 prescale
    TMR1H = MSG_TIMER_H; // set Timer1 to timer_period
    TMR1L = MSG_TIMER_L;
    PIR1bits.TMR1IF = 0; // Clear Timer1 overflow flag
    PIE1bits.TMR1IE = 1; // Enable Timer1 overflow interrupt

    T1CONbits.TMR1ON = 1; // start Timer1 now
}

/**********************************************************************
 * Function:        void check_buttons(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:
 * Overview:        Init timer1 for the message queue
 ************************************************************************/
void check_buttons(void)
{
    static unsigned char bounce = 0;
    unsigned char cur_buttons;

    cur_buttons = PORTA & BUTTONS_MASK;
    switch (bounce)
    {
    case 0:
        if (cur_buttons != buttons)
        {
            buttons = cur_buttons;
            bounce = TEMPO_BUTTONS; //  si une touche vient d'etre enfoncee
        }
        break;
    case 1:
        if (cur_buttons == 0x00 && cur_buttons != buttons)
        {
            buttons = cur_buttons;
            bounce = TEMPO_BUTTONS; //  les deux touches viennent d'etre enfoncees
        }
        else if (cur_buttons != buttons)
        {
            //  une touche vient d'être relachee
            msg_post(buttons | BUTTONS_UP);
            if (buttons != 0x00)
            {
                bounce = 0;
            }
            else
            {
                bounce = 1;
            }
            buttons = cur_buttons;
        }
        break;
    case 2:
        if (cur_buttons == buttons)
        {
            bounce = 1; //  la tempo est expirée, on poste le message
            msg_post(buttons | BUTTONS_DOWN);
        }
        else
        {
            bounce = 0;
            buttons = cur_buttons;
        }
        break;

    default:
        //  KeyMove: il faut l'état 0 stable pendant TEMPO_BUTTONS
        if (cur_buttons != buttons)
        {
            //  parasite ou touche relachee
            bounce = 0;
            buttons = cur_buttons;
        }
        else bounce--;
        break;
    }
}

/**********************************************************************
 * Function:        void msg_post(unsigned char msg)
 * PreCondition:    None
 * Input:		   message to post
 * Output:		   None
 * Side Effects:
 * Overview:		   Post a new message in the queue
 ***********************************************************************/
void msg_post(e_msg msg)
{
    e_msg new_in;

    new_in = (msg_in + 1) % MSG_QUEUE_SIZE;
    if (new_in != msg_out)
    {
        msg_queue[new_in] = msg;
        msg_in = new_in;
    }
}

/**********************************************************************
 * Function:        void msg_post(unsigned char msg)
 * PreCondition:    None
 * Input:		   message to post
 * Output:		   None
 * Side Effects:
 * Overview:		   Post a new message in the queue
 ***********************************************************************/
e_msg msg_get(void)
{
    e_msg msg;

    if (msg_in != msg_out)
    {
        msg = msg_queue[msg_out];
        msg_out++;
        msg_out %= MSG_QUEUE_SIZE;
        return msg;
    }
    return NONE;
}

/**********************************************************************
 * Function:        void start_timer(unsigned long elapse)
 * PreCondition:    None
 * Input:		   elapse time
 * Output:		   None
 * Side Effects:
 * Overview:		   Start timer (yeah really !!)
 ***********************************************************************/
void start_timer(unsigned char num_timer, unsigned long elapse)
{
    timer[num_timer] = elapse + tick;
    timer_on[num_timer] = TRUE;
}

/**********************************************************************
 * Function:        void stop_timer(unsigned long elapse)
 * PreCondition:    None
 * Input:		   elapse time
 * Output:		   None
 * Side Effects:
 * Overview:		   Stop timer
 ***********************************************************************/
void stop_timer(unsigned char num_timer)
{
    timer_on[num_timer] = FALSE;
}

/**********************************************************************
 * Function:        void process_messages(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:
 * Overview:        Process messages queue
 ***********************************************************************/
void process_messages(void)
{
    e_msg new_msg;
    unsigned long voltage, threshold;
    unsigned char pwr_fail;

    new_msg = msg_get();
    switch (new_msg & 0x0F)
    {
    case CHK_SENSOR:
        if (is_measuring != 0)
        {
            if (OW_read_byte() != 0)
            {
                read_temperature(is_measuring - 1);
                if (is_cmd)
                {
                    is_cmd = FALSE;
                }
                else
                {
                    focus_compensate();
                }
                is_measuring = 0;
            }
        }
        break;

    case LED:
        if (nb_flash_red)
        {
            if (new_msg & 0x80)
            {
                LED_On(LED_RED);
            }
            else
            {
                LED_Off(LED_RED);
                if (nb_flash_red != 255) nb_flash_red--;
            }
        }
        if (nb_flash_green)
        {
            if (new_msg & 0x80)
            {
                LED_On(LED_GREEN);
            }
            else
            {
                LED_Off(LED_GREEN);
                if (nb_flash_green != 255) nb_flash_green--;
            }
        }
        break;

    case BUTTONS_DOWN:
        if (!is_compensate && !stepper_flag.park)
        {
            if (new_msg & 0x10)
            {
                // RA4 down
                stepper_start_in();
            }
            if (new_msg & 0x20)
            {
                // RA5 down
                stepper_start_out();
            }
            start_timer(0, 2000);
        }
        if ((new_msg & 0x30) == 0x00)
        {
            stepper_flag.park = 1;
            stepper_absolute_move(stepper_pos_min);
            LED_On(LED_GREEN);
        }
        break;

    case BUTTONS_UP:
        if (!is_compensate && !stepper_flag.park)
        {
            if (new_msg & 0x10)
            {
                // RA4 down
                stepper_stop(0);
            }
            if (new_msg & 0x20)
            {
                // RA5 down
                stepper_stop(0);
            }
        }
        break;

    case TIMER:
        if ((new_msg & 0xF0) == 0)
        {
            stepper_high_speed();
        }
        else if ((new_msg & 0xF0) == 0x10)
        {
            stop_timer(1);
            eep_write(eep_adr_stepper_position, (unsigned char *) &stepper_position, sizeof (stepper_position));
        }
        break;

        //            case CHK_CMD :
        //                check_command();
        //                break;

    /*case FOCUS_TEMP:
        focus_temperature();
        break;
*/
    case CHK_POWER:
        threshold = (unsigned long) pwr_threshold * 10000;
        voltage = read_power_suply();
        if (voltage <= threshold)
        {
            if (!pwr_fail)
            {
                LED_On(LED_RED);
                pwr_fail = 1;
                if (config_byte.save_position_pwr_fail)
                {
                    eep_write(eep_adr_stepper_position, (unsigned char *) &stepper_position, sizeof (stepper_position));
                }
            }
        }
        else
        {
            LED_Off(LED_RED);
            pwr_fail = 0;
        }
        break;

    }
}
 