/* 移動量計測 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "cal_movement.h"

#include "D_DEVICE/drive_mtr.h"
#include "D_DEVICE/arm_mtr.h"

#define ONE_LAP_PLS     360                       // タイヤ一周のパルス数
#define TIRE_DIAMETER   100                       // タイヤ直径[mm]
#define PI              3                         // 円周率[-]
#define TREAD           126                      // 左右輪の幅 [mm]

/* 外部非公開変数 */

/* 移動量計測初期化 */
void ini_cal_movement( void ){

}

/* 走行距離計測 */
int16_t get_cal_movement( void ){
    int16_t s16_cnt_l;
    int16_t s16_cnt_r;
    int16_t s16_distanceL;
    int16_t s16_distanceR;
    int16_t s16_distance;

    get_drive_mtr_cnt(&s16_cnt_l, &s16_cnt_r);
    s16_distanceL = (int16_t)((int32_t)TIRE_DIAMETER * PI * s16_cnt_l / ONE_LAP_PLS );    // 左車輪移動距離[mm]
    s16_distanceR = (int16_t)((int32_t)TIRE_DIAMETER * PI * s16_cnt_r / ONE_LAP_PLS );    // 右車輪移動距離[mm]
    s16_distance  = (s16_distanceL + s16_distanceR)/2;                                      // 左右輪の平均を算出

    return s16_distance;
}

/* 機体旋回角計測 */
/* 時計周りが正 */
int16_t get_cal_movement_body_deg( void ){
    int16_t s16_cnt_l;
    int16_t s16_cnt_r;
    int16_t s16_distanceL;
    int16_t s16_distanceR;
    int32_t s32_diff;
    int16_t s16_body_deg;

    get_drive_mtr_cnt(&s16_cnt_l, &s16_cnt_r);
    s16_distanceL = (int16_t)((int32_t)TIRE_DIAMETER * PI * s16_cnt_l / ONE_LAP_PLS );      // 左車輪移動距離[mm]
    s16_distanceR = (int16_t)((int32_t)TIRE_DIAMETER * PI * s16_cnt_r / ONE_LAP_PLS );      // 右車輪移動距離[mm]
    s32_diff      = (int32_t)s16_distanceL - s16_distanceR;                                 // 左右車輪差分
    s16_body_deg  = (int16_t)(s32_diff*360/(2*PI*TREAD));                                   // 旋回角計算

    return s16_body_deg;
}

/* アーム回転角取得 */
uint16_t get_cal_movement_arm_deg( void ){
    uint16_t degree;
    uint16_t cnt_arm;

    get_arm_mtr_cnt(&cnt_arm);                              // アームモータエンコーダカウント取得
    degree = 360 * cnt_arm / ONE_LAP_PLS;                   // アーム回転角算出

    return degree;
}

/* 計測リセット */
void reset_cal_movement( void ){
    rst_drive_mtr_cnt();            /* 駆動モータエンコーダカウントリセット */
    rst_arm_mtr_cnt();              /* アームモータエンコーダカウントリセット */
}