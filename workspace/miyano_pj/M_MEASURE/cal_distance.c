/* 物体との距離計測 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "D_DEVICE/sonic_snc.h"

#include "cal_distance.h"

#define SNC_VALUE_2_DISTANCE 1          /* センサ値変換 */

/* 外部非公開変数 */

/* 物体との距離計測初期化 */
void ini_cal_distance( void ){

}

/* 物体との距離計測 */
uint16_t get_cal_distance( void ){
    uint16_t u16_snc_value;
    uint16_t u16_distance;

    get_sonic_snc(&u16_snc_value);

    u16_distance = u16_snc_value * SNC_VALUE_2_DISTANCE;

    return u16_distance;
}