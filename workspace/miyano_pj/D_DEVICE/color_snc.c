#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <pbio/color.h>

#include "spike/pup/colorsensor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "color_snc.h"

// PUP カラーセンサポンタ
pup_device_t *colorS;

/* カラーセンサ初期化 */
void ini_color_snc( void ){
  colorS = pup_color_sensor_get_device(PBIO_PORT_ID_C);
}

/* RGB値取得 */
void get_color_rgb( uint16_t* r_valu, uint16_t* g_valu, uint16_t* b_valu ){
  pup_color_rgb_t rgb;
  
  rgb = pup_color_sensor_rgb(colorS);
  *r_valu = rgb.r;
  *g_valu = rgb.g;
  *b_valu = rgb.b;
}

/* 反射光取得 */
void get_color_ref( uint16_t* ref_valu ){
  *ref_valu = pup_color_sensor_reflection(colorS);
}
