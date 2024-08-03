#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <test1.h>

#include "spike/pup/motor.h"
#include "spike/pup/colorsensor.h"
#include "spike/pup/forcesensor.h"
#include "spike/pup/ultrasonicsensor.h"

#include "spike/hub/battery.h"
#include "spike/hub/button.h"
#include "spike/hub/display.h"
#include "spike/hub/imu.h"
#include "spike/hub/light.h"
#include "spike/hub/speaker.h"

#include <pbio/color.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

void Main(intptr_t exinf)
{
  // ここからプログラムを書く
  int count;
  int* rx_data;
  int* tx_data;
  int ercd;
  hub_button_t pressed;

  count = 0;
  ercd = serial_opn_por(SIO_USB_PORTID);
  tx_data = &count;

  while (1){
    // ercd = serial_rea_dat(SIO_USB_PORTID, rx_data, 1);

    hub_button_is_pressed(&pressed);

    // if( sizeof(rx_data) == ercd ){
    if( (pressed & HUB_BUTTON_CENTER) == HUB_BUTTON_CENTER ){
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
      hub_display_text("C", 1000, 0);
      // ercd = serial_wri_dat(SIO_USB_PORTID,tx_data , 1);
      ercd = serial_wri_dat(SIO_USB_PORTID,count , 8);
    }
    hub_display_text("-", 1, 0);
  }
  exit(0);
}
