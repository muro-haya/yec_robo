#include <stdlib.h>
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

  while(1){
    hub_light_on_color(PBIO_COLOR_RED);
    dly_tsk(1*1000*1000);
    hub_light_on_color(PBIO_COLOR_GREEN);
    dly_tsk(1*1000*1000);
    hub_light_on_color(PBIO_COLOR_BLUE);
    dly_tsk(1*1000*1000);
  }

  exit(0);
}