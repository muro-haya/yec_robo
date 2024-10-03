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

/* 外部非公開変数 */
uint16_t u16_r_valu;
uint16_t u16_g_valu;
uint16_t u16_b_valu;


uint16_t g16_snc_r;
uint16_t g16_snc_g;
uint16_t g16_snc_b;
// PUP カラーセンサポンタ
pup_device_t *colorS;

/* カラーセンサ初期化 */
void ini_color_snc( void ){
  colorS = pup_color_sensor_get_device(PBIO_PORT_ID_C);

  u16_r_valu = 0;
  u16_g_valu = 0;
  u16_b_valu = 0;
}

/* RGBセンサ値取得 */
void cyc_get_color_rgb( void ){
  pup_color_rgb_t rgbcolor;
  
  rgbcolor   = pup_color_sensor_rgb(colorS);
  u16_r_valu = rgbcolor.r;
  u16_g_valu = rgbcolor.g;
  u16_b_valu = rgbcolor.b;

}

/* RGB値取得 */
void get_color_rgb( uint16_t *r_valu, uint16_t *g_valu, uint16_t *b_valu ){

  *r_valu = u16_r_valu;
  *g_valu = u16_g_valu;
  *b_valu = u16_b_valu;

}

/* 反射光取得 */
void get_color_ref( uint16_t* ref_valu ){
  *ref_valu = pup_color_sensor_reflection(colorS);
}