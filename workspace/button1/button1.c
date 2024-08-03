// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include "kernel_cfg.h"
#include "button1.h"
#include "spike/pup/motor.h"
#include "spike/hub/button.h"
#include "spike/hub/display.h"
#include "pbio/light_matrix.h"

//#include <pbsys/user_program.h>

#define RASPIKE_PORTNO  4

/*
 * Definitions of Port ID for Serial Adapter
 */
#define SIO_USART_F_PORTID    1
#define SIO_USB_PORTID        2
#define SIO_BLUETOOTH_PORTID  3
#define SIO_TEST_PORTID       4

/*
 * Main Task
 */
#include <stdio.h>

void main_task(intptr_t exinf)
{
  syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", 0);

  // pbsys_user_program_prepare(NULL);
  // wup_pybricks();

  int count;
  int* rx_data;
  int ercd;

  count = 0;
  
  // ercd = serial_opn_por(SIO_USB_PORTID);

  while (1)
  {
    hub_button_t pressed;
    static int pre_command;
    int command;
    
#if 0
    ercd = serial_rea_dat(SIO_USB_PORTID, rx_data, 1);

    if( sizeof(rx_data) == ercd ){
      count += 1;
      switch ((int)&rx_data)
      {
      case 0:
        hub_display_text("0", 1000, 0);
        break;
      
      case 1:
        hub_display_text("1", 1000, 0);
        break;
      
      case 2:
        hub_display_text("2", 1000, 0);
        break;
      
      default:
        break;
      }
      ercd = serial_wri_dat(SIO_USB_PORTID, count, sizeof(count));
    }

# else if
    hub_button_is_pressed(&pressed);

    if((pressed & HUB_BUTTON_RIGHT) == HUB_BUTTON_RIGHT)    command = command + 1;
    if((pressed & HUB_BUTTON_LEFT) == HUB_BUTTON_LEFT)      command = command + 2;
    if((pressed & HUB_BUTTON_CENTER) == HUB_BUTTON_CENTER)  command = command + 4;
    if((pressed & HUB_BUTTON_BT) == HUB_BUTTON_BT)          command = command + 8;

    if (command != pre_command) {
      switch(command){
        case 1:
          syslog(LOG_NOTICE, "R", 0);
          hub_display_text("R", 1000, 0);
          break;
        case 2:
          syslog(LOG_NOTICE, "L", 0);
          hub_display_text("L", 1000, 0);
          break;
        case 3:
          syslog(LOG_NOTICE, "R+L", 0);
          hub_display_text_scroll(" R+L", 70);
          break;
        case 4:
          syslog(LOG_NOTICE, "C", 0);
          hub_display_text("C", 1000, 0);
          break;
        case 5:
          syslog(LOG_NOTICE, "R+C", 0);
          hub_display_text_scroll(" R+C",70);
          break;
        case 6:
          syslog(LOG_NOTICE, "L+C", 0);
          hub_display_text_scroll(" L+C", 70);
          break;
        case 7:
          syslog(LOG_NOTICE, "R+L+C", 0);
          hub_display_text_scroll(" R+L+C", 70);
          break;
        case 8:
          syslog(LOG_NOTICE, "B", 0);
          hub_display_text("B", 1000, 0);
          break;
        case 9:
          syslog(LOG_NOTICE, "R+B", 0);
          hub_display_text_scroll(" R+B",70);
          break;
        case 10:
          syslog(LOG_NOTICE, "L+B", 0);
          hub_display_text_scroll(" L+B", 70);
          break;
        case 11:
          syslog(LOG_NOTICE, "R+L+B", 0);
          hub_display_text_scroll(" R+L+B", 70);
          break;
        case 12:
          syslog(LOG_NOTICE, "C+B", 0);
          hub_display_text_scroll(" C+B", 70);
          break;
        case 13:
          syslog(LOG_NOTICE, "R+C+B", 0);
          hub_display_text_scroll(" R+C+B", 70);
          break;
        case 14:
          syslog(LOG_NOTICE, "L+C+B", 0);
          hub_display_text_scroll(" L+C+B", 70);
          break;
        case 15:
          syslog(LOG_NOTICE, "ALL", 0);
          hub_display_text_scroll(" ALL", 70);
          break;
        default:
          break;
      }
    }
# endif
    hub_display_text("-", 1, 0);

    pre_command = command;
    command = 0;
  }

  // pbsys_user_program_unprepare();
  // wup_pybricks();
}
