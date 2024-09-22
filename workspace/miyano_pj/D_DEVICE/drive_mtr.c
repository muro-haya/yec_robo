#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/pup/motor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "drive_mtr.h"

<<<<<<< Updated upstream
=======
#define MOTOR_CNT_BUF 20

>>>>>>> Stashed changes
// PUPモータデバイスポインタ
pup_motor_t *motorL;
pup_motor_t *motorR;

<<<<<<< Updated upstream
=======
/*  適合値*/
uint16_t x_u16_drive_mtr_rpmL_kp = 10;
uint16_t x_u16_drive_mtr_rpmL_ki = 5;
uint16_t x_u16_drive_mtr_rpmL_kd = 5;

uint16_t x_u16_drive_mtr_rpmR_kp = 10;//46
uint16_t x_u16_drive_mtr_rpmR_ki = 0;//52
uint16_t x_u16_drive_mtr_rpmR_kd = 0;//52

/* 外部公開変数 */
int16_t g_s16_drive_mtr_spdL;
int16_t g_s16_drive_mtr_spdR;

/* 外部非公開変数 */
int16_t s16_drive_cntL_buf[MOTOR_CNT_BUF];
int16_t s16_drive_cntR_buf[MOTOR_CNT_BUF];
uint16_t u16_drive_cntR_buf_index;
uint16_t u16_drive_cntL_buf_re_flg;
uint16_t u16_drive_cntR_buf_re_flg;

int16_t s16_drive_rpmL_p;
int16_t s16_drive_rpmL_i;
int16_t s16_drive_rpmL_d;
int16_t s16_drive_rpmL_dlt_old;
int16_t s16_drive_rpmL_dlt_sum;

int16_t s16_drive_rpmR_p;
int16_t s16_drive_rpmR_i;
int16_t s16_drive_rpmR_d;
int16_t s16_drive_rpmR_dlt_old;
int16_t s16_drive_rpmR_dlt_sum;

/*ロギング用にglobalで定義*/
int16_t s16_drive_cntL;
int16_t s16_drive_cntR;
int16_t s16_drive_rpmL;
int16_t s16_drive_rpmR;
int16_t s16_drive_rpmL_dlt;
int16_t s16_drive_rpmR_dlt;
int16_t s16_drive_dltL;
int16_t s16_drive_dltR;


>>>>>>> Stashed changes
/* 駆動モータ初期化 */
void ini_drive_mtr( void ){
  // PUPモータデバイスポインタを取得
  motorL = pup_motor_get_device(PBIO_PORT_ID_E);
  motorR = pup_motor_get_device(PBIO_PORT_ID_B);

  // モータのセットアップ
  pup_motor_setup(motorL, PUP_DIRECTION_COUNTERCLOCKWISE, true);
  pup_motor_setup(motorR, PUP_DIRECTION_CLOCKWISE, true);
  
  // 駆動モータの角度をリセット
  pup_motor_reset_count(motorL);
  pup_motor_reset_count(motorR);
<<<<<<< Updated upstream
=======

  u16_drive_cntL_buf_index  = 0;
  u16_drive_cntR_buf_index  = 0;
  u16_drive_cntL_buf_re_flg = 0;
  u16_drive_cntR_buf_re_flg = 0;

  g_s16_drive_mtr_spdL = 0;
  g_s16_drive_mtr_spdR = 0;

  s16_drive_rpmL_p = 0;
  s16_drive_rpmL_i = 0;
  s16_drive_rpmL_d = 0;
  s16_drive_rpmL_dlt_old = 0;

  s16_drive_rpmR_p = 0;
  s16_drive_rpmR_i = 0;
  s16_drive_rpmR_d = 0;
  s16_drive_rpmR_dlt_old = 0;
>>>>>>> Stashed changes
}

/* 駆動モータDUty設定 */
void set_drive_mtr_duty( int16_t dutyL, int16_t dutyR ){
  pup_motor_set_power(motorL, dutyL);             // パワーを設定
  pup_motor_set_power(motorR, dutyR);             // パワーを設定
}

/* 駆動モータ回転速度設定 */
void set_drive_mtr_spd( int16_t spdL, int16_t spdR ){
<<<<<<< Updated upstream
  pup_motor_set_speed(motorL, spdL);              // 速度を設定
  pup_motor_set_speed(motorR, spdR);              // 速度を設定
=======
  
  get_drive_mtr_cnt(&s16_drive_cntL,&s16_drive_cntR);
  
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
  
  s16_drive_dltbufL = s16_drive_cntL_buf[u16_drive_cntL_buf_index];
  s16_drive_dltL    = s16_drive_cntL - s16_drive_cntL_buf[u16_drive_cntL_buf_index];
  s16_drive_rpmL   = (int16_t)((int32_t)s16_drive_dltL *1000*60*6/ (360 * MOTOR_CNT_BUF * 2));
  // s16_drive_rpmL   = (int16_t)((int32_t)s16_drive_dltL *1000*60/ (360 * MOTOR_CNT_BUF * 2));
  //s16_drive_rpmL = pup_motor_get_speed(motorL);
  s16_drive_rpmL_dlt = (spdL - s16_drive_rpmL);
  s16_drive_rpmL_p = ( s16_drive_rpmL_dlt * x_u16_drive_mtr_rpmL_kp ) / 100;
  s16_drive_rpmL_d = (( s16_drive_rpmL_dlt - s16_drive_rpmL_dlt_old ) * x_u16_drive_mtr_rpmL_kd ) / 100;
  if(s16_drive_rpmL_i <= 100)
  {
    s16_drive_rpmL_dlt_sum += s16_drive_rpmL_dlt;  
  }
  s16_drive_rpmL_i = s16_drive_rpmL_dlt_sum*0.002 * x_u16_drive_mtr_rpmL_ki/100;
  g_s16_drive_mtr_spdL = s16_drive_rpmL_p + s16_drive_rpmL_i - s16_drive_rpmL_d;
  if( 100 <= g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = 100;
  }
  else if( 0 >= g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = 0;
  }
  //g_s16_drive_mtr_spdL += 50;
  
  s16_drive_cntL_buf[u16_drive_cntL_buf_index] = s16_drive_cntL;
  s16_drive_rpmL_dlt_old = s16_drive_rpmL_dlt;

  s16_drive_dltR  = s16_drive_cntR - s16_drive_cntR_buf[u16_drive_cntL_buf_index];  
  //s16_drive_rpmR = (int16_t)((int32_t)s16_drive_dltR *1000*60/ (360 * MOTOR_CNT_BUF * 2));
  s16_drive_rpmR = pup_motor_get_speed(motorR);
  s16_drive_rpmR_dlt = (spdR - s16_drive_rpmR);
  s16_drive_rpmR_p = ( s16_drive_rpmR_dlt * x_u16_drive_mtr_rpmR_kp ) / 100;
  s16_drive_rpmR_d = ((s16_drive_rpmR_dlt - s16_drive_rpmR_dlt_old) * x_u16_drive_mtr_rpmR_kd) / 100;
  g_s16_drive_mtr_spdR = s16_drive_rpmR_p - s16_drive_rpmR_d;
  if( 100 <= g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = 100;
  }
  else if( 0 >= g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = 0;
  }
  //g_s16_drive_mtr_spdR += 50;
  
  s16_drive_cntR_buf[u16_drive_cntL_buf_index] = s16_drive_cntR;
  s16_drive_rpmR_dlt_old = s16_drive_rpmR_dlt;


  //pup_motor_set_power(motorL, g_s16_drive_mtr_spdL);  // パワーを設定
  //pup_motor_set_speed(motorL, spdL);                  // スピードを設定
  //pup_motor_set_speed(motorR, spdR);                  // スピードを設定
  pup_motor_set_power(motorL, 80);                  // パワーを設定
  pup_motor_set_power(motorR, 80);                  // パワーを設定
>>>>>>> Stashed changes
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