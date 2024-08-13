#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/pup/ultrasonicsensor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "sonic_snc.h"

pup_device_t *sonicS;

/* 超音波センサ初期化 */
void ini_sonic_snc( void ){
  sonicS = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_D);
}

/* 超音波センサ値取得 */
void get_sonic_snc( uint16_t* distance ){
  *distance = pup_ultrasonic_sensor_distance(sonicS);
}
