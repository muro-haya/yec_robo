#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/pup/motor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "comm.h"

#include "drive_mtr.h"

#define MOTOR_CNT_BUF 50

// PUPモータデバイスポインタ
pup_motor_t *motorL;
pup_motor_t *motorR;

/*  適合値*/
uint16_t x_u16_drive_mtr_spdL_namashi = 40;
uint16_t x_u16_drive_mtr_spdR_namashi = 3;

uint16_t x_u16_drive_mtr_rpmL_kp = 40;
uint16_t x_u16_drive_mtr_rpmL_kd = 50;

uint16_t x_u16_drive_mtr_rpmR_kp = 46;
uint16_t x_u16_drive_mtr_rpmR_kd = 52;

/* 外部公開変数 */
int16_t g_s16_drive_mtr_spdL;
int16_t g_s16_drive_mtr_spdR;

/* 外部非公開変数 */
int16_t s16_drive_cntL_buf[MOTOR_CNT_BUF];
int16_t s16_drive_cntR_buf[MOTOR_CNT_BUF];
uint16_t u16_drive_cntL_buf_index;
uint16_t u16_drive_cntR_buf_index;
uint16_t u16_drive_cntL_buf_re_flg;
uint16_t u16_drive_cntR_buf_re_flg;

int16_t s16_drive_rpml_p;
int16_t s16_drive_rpml_d;
int16_t s16_drive_rpmL_dlt_old;

int16_t s16_drive_rpmR_p;
int16_t s16_drive_rpmR_d;
int16_t s16_drive_rpmR_dlt_old;

/* 駆動モータ初期化 */
void ini_drive_mtr( void ){
  // PUPモータデバイスポインタを取得
  motorL = pup_motor_get_device(PBIO_PORT_ID_E);
  motorR = pup_motor_get_device(PBIO_PORT_ID_B);

  // モータのセットアップ
  pup_motor_setup(motorL, PUP_DIRECTION_COUNTERCLOCKWISE, true);
  pup_motor_setup(motorR, PUP_DIRECTION_CLOCKWISE, true);
  
  pup_motor_set_power(motorL, 0);
  pup_motor_set_power(motorR, 0);
  // 駆動モータの角度をリセット
  pup_motor_reset_count(motorL);
  pup_motor_reset_count(motorR);

  u16_drive_cntL_buf_index  = 0;
  u16_drive_cntR_buf_index  = 0;
  u16_drive_cntL_buf_re_flg = 0;
  u16_drive_cntR_buf_re_flg = 0;

  g_s16_drive_mtr_spdL = 0;
  g_s16_drive_mtr_spdR = 0;

  s16_drive_rpml_p = 0;
  s16_drive_rpml_d = 0;
  s16_drive_rpmL_dlt_old = 0;

  s16_drive_rpmR_p = 0;
  s16_drive_rpmR_d = 0;
  s16_drive_rpmR_dlt_old = 0;
}

/* 駆動モータDUty設定 */
void set_drive_mtr_duty( int16_t dutyL, int16_t dutyR ){
  pup_motor_set_power(motorL, dutyL);             // パワーを設定
  pup_motor_set_power(motorR, dutyR);             // パワーを設定
}

/* 駆動モータ回転速度設定 */
void set_drive_mtr_spd( int16_t spdL, int16_t spdR ){
  int16_t s16_drive_cntL;
  int16_t s16_drive_cntR;
  int16_t s16_drive_rpmL;
  int16_t s16_drive_rpmR;
  int16_t s16_drive_dlt;

  get_drive_mtr_cnt(&s16_drive_cntL, &s16_drive_cntR);
  if( MOTOR_CNT_BUF > u16_drive_cntL_buf_index ){
    u16_drive_cntL_buf_index += 1;
    if( 0 == u16_drive_cntL_buf_re_flg ){
      s16_drive_cntL_buf[u16_drive_cntL_buf_index] = s16_drive_cntL;
      s16_drive_cntR_buf[u16_drive_cntL_buf_index] = s16_drive_cntR;
      return;
    }
  }
  else{
    u16_drive_cntL_buf_re_flg = 1;
    u16_drive_cntL_buf_index = 0;
  }
  s16_drive_dlt    = s16_drive_cntL - s16_drive_cntL_buf[u16_drive_cntL_buf_index];
  s16_drive_rpmL   = (int16_t)((int32_t)s16_drive_dlt *1000*60/ (360 * MOTOR_CNT_BUF * 2));
  s16_drive_rpml_p = ( (spdL - s16_drive_rpmL) * x_u16_drive_mtr_rpmL_kp ) / 100;
  s16_drive_rpml_d = (((spdL - s16_drive_rpmL) - s16_drive_rpmL_dlt_old) * x_u16_drive_mtr_rpmL_kd) / 100;
  g_s16_drive_mtr_spdL = s16_drive_rpml_p - s16_drive_rpml_d;
  if( 50 <= g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = 50;
  }
  else if( -20 >= g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = -20;
  }
  g_s16_drive_mtr_spdL += 50;
  s16_drive_cntL_buf[u16_drive_cntL_buf_index] = s16_drive_cntL;
  s16_drive_rpmL_dlt_old = (spdL - s16_drive_rpmL);

  s16_drive_dlt  = s16_drive_cntR - s16_drive_cntR_buf[u16_drive_cntL_buf_index];
  s16_drive_rpmR = (int16_t)((int32_t)s16_drive_dlt *1000*60/ (360 * MOTOR_CNT_BUF * 2));
  s16_drive_rpmR_p = ( (spdR - s16_drive_rpmR) * x_u16_drive_mtr_rpmR_kp ) / 100;
  s16_drive_rpmR_d = (((spdR - s16_drive_rpmR) - s16_drive_rpmR_dlt_old) * x_u16_drive_mtr_rpmR_kd) / 100;
  g_s16_drive_mtr_spdR = s16_drive_rpmR_p - s16_drive_rpmR_d;
  if( 50 <= g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = 50;
  }
  else if( -20 >= g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = -20;
  }
  g_s16_drive_mtr_spdR += 50;
  s16_drive_cntR_buf[u16_drive_cntL_buf_index] = s16_drive_cntR;
  s16_drive_rpmR_dlt_old = (spdR - s16_drive_rpmR);


  pup_motor_set_power(motorL, g_s16_drive_mtr_spdL);                  // パワーを設定
  pup_motor_set_power(motorR, g_s16_drive_mtr_spdR);                  // パワーを設定
}

/* 駆動モータエンコーダカウントリセット */
void rst_drive_mtr_cnt( void ){
  pup_motor_reset_count(motorL);                  // 左駆動モータの角度をリセット
  pup_motor_reset_count(motorR);                  // 右駆動モータの角度をリセット
}

/* アームモータエンコーダカウント取得 */
void get_drive_mtr_cnt( int16_t* drive_cntL, int16_t* drive_cntR ){
  int32_t ldat;
  ldat = pup_motor_get_count(motorL);           // 右駆動モータエンコーダカウント取得
  // if( 0 > ldat ){                               // 絶対値処理
  //   ldat = -ldat;
  // }
  *drive_cntL = ldat;
  ldat = pup_motor_get_count(motorR);
  // if( 0 > ldat ){                               // 絶対値処理
  //   ldat = -ldat;
  // }
  *drive_cntR = ldat;                           // 左駆動モータエンコーダカウント取得
}