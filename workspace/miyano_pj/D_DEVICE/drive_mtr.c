#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/pup/motor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../M_CTL/const_run.h"

#include "drive_mtr.h"

#define MA_WINDOW_SIZE 50 //移動平均の窓サイズ

// PUPモータデバイスポインタ
pup_motor_t *motorL;
pup_motor_t *motorR;

/*  適合値*/
// uint16_t x_u16_drive_mtr_rpmL_kp = 12;      /* 1号機 */
uint16_t x_u16_drive_mtr_rpmL_kp = 14;      /* 2号機 */
uint16_t x_u16_drive_mtr_rpmL_kd = 0;
uint16_t x_u16_drive_mtr_rpmL_ki = 0;

uint16_t x_u16_drive_mtr_rpmR_kp = 11;//46  /* 1号機・2号機 */
uint16_t x_u16_drive_mtr_rpmR_kd = 0;//52
uint16_t x_u16_drive_mtr_rpmR_ki = 0;//52

/* 外部公開変数 */
int16_t g_s16_drive_mtr_spdL;
int16_t g_s16_drive_mtr_spdR;

/* 外部非公開変数 */
int16_t s16_drive_rpmL_p;
int16_t s16_drive_rpmL_i;
int16_t s16_drive_rpmL_d;
int16_t s16_drive_rpmL_err_old;
int16_t s16_drive_rpmL_err_cumsum;

int16_t s16_drive_rpmR_p;
int16_t s16_drive_rpmR_i;
int16_t s16_drive_rpmR_d;
int16_t s16_drive_rpmR_err_old;
int16_t s16_drive_rpmR_err_cumsum;

int16_t MovingAve_SizeL;
int16_t MovingAve_indexL;
int16_t MovingAve_sumL;
int16_t MovingAve_countL;
int16_t err_windowL[MA_WINDOW_SIZE];

int16_t MovingAve_SizeR;
int16_t MovingAve_indexR;
int16_t MovingAve_sumR;
int16_t MovingAve_countR;
int16_t err_windowR[MA_WINDOW_SIZE];

/*ロギング用にglobalで定義*/
int16_t s16_drive_cntL;
int16_t s16_drive_cntR;
int16_t s16_drive_rpmL;
int16_t s16_drive_rpmR;
int16_t s16_drive_rpmL_err;
int16_t s16_drive_rpmR_err;


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

  g_s16_drive_mtr_spdL = 0;
  g_s16_drive_mtr_spdR = 0;

  s16_drive_rpmL_p = 0;
  s16_drive_rpmL_i = 0;
  s16_drive_rpmL_d = 0;
  s16_drive_rpmL_err_old = 0;

  s16_drive_rpmR_p = 0;
  s16_drive_rpmR_i = 0;
  s16_drive_rpmR_d = 0;
  s16_drive_rpmR_err_old = 0;

  //移動平均計算用変数
    for (int i = 0; i < MovingAve_SizeL; i++)
    {
    err_windowR[i] = 0; // 配列をゼロで初期化
    err_windowR[i] = 0; // 配列をゼロで初期化
    }
    MovingAve_indexL = 0; // インデックスをリセット
    MovingAve_indexR = 0; // インデックスをリセット
    MovingAve_sumL = 0; // 合計をリセット
    MovingAve_sumR = 0; // 合計をリセット
    MovingAve_countL = 0; // 入力された誤差の数をリセット
    MovingAve_countR = 0; // 入力された誤差の数をリセット

}

/* 駆動モータDUty設定 */
void set_drive_mtr_duty( int16_t dutyL, int16_t dutyR ){
  pup_motor_set_power(motorL, dutyL);             // パワーを設定
  pup_motor_set_power(motorR, dutyR);             // パワーを設定
}

/* 駆動モータ回転速度設定 */
void set_drive_mtr_spd( int16_t spdL, int16_t spdR )
{  
  int16_t g_s16_drive_mtr_spdL_FF;
  int16_t g_s16_drive_mtr_spdR_FF;
  int16_t g_s16_drive_mtr_powerL;
  int16_t g_s16_drive_mtr_powerR;


  //FF直達項　
  if(spdL < 0){
    g_s16_drive_mtr_spdL_FF = spdL*5/72-28;//25
  }else if(spdL == 0){
    g_s16_drive_mtr_spdL_FF = 0;
  }else{
    g_s16_drive_mtr_spdL_FF = spdL*5/72+28;//25
  }

  if(spdR < 0){
    g_s16_drive_mtr_spdR_FF = spdR*5/72-25;
  }else if(spdR == 0){
    g_s16_drive_mtr_spdR_FF = 0;
  }else{
    g_s16_drive_mtr_spdR_FF = spdR*5/72 + 25;
  }

  //FB項
  s16_drive_rpmL = pup_motor_get_speed(motorL);
  s16_drive_rpmL_err = (spdL - s16_drive_rpmL);
  if(s16_drive_rpmL_err != s16_drive_rpmL_err_old)
  {
    s16_drive_rpmL_p = ( s16_drive_rpmL_err * x_u16_drive_mtr_rpmL_kp ) / 100;
    s16_drive_rpmL_d = (( s16_drive_rpmL_err - s16_drive_rpmL_err_old ) * x_u16_drive_mtr_rpmL_kd ) / 100;
    if(s16_drive_rpmL_i <= 100)
    {
      s16_drive_rpmL_err_cumsum += s16_drive_rpmL_err;  
    }
    s16_drive_rpmL_i = s16_drive_rpmL_err_cumsum*0.002 * x_u16_drive_mtr_rpmL_ki/100;
    g_s16_drive_mtr_spdL = s16_drive_rpmL_p + s16_drive_rpmL_i - s16_drive_rpmL_d;
  }
  if( 100 < g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = 100;
    s16_drive_rpmL_err_cumsum = 0;
  }
  else if( -100 > g_s16_drive_mtr_spdL ){
    g_s16_drive_mtr_spdL = -100;
    s16_drive_rpmL_err_cumsum = 0;
  }

  s16_drive_rpmR = pup_motor_get_speed(motorR);
  s16_drive_rpmR_err = (spdR - s16_drive_rpmR);
  s16_drive_rpmR_p = ( s16_drive_rpmR_err * x_u16_drive_mtr_rpmR_kp ) / 100;
  s16_drive_rpmR_d = ((s16_drive_rpmR_err - s16_drive_rpmR_err_old) * x_u16_drive_mtr_rpmR_kd) / 100;
  if(s16_drive_rpmR_i <= 100)
  {
    s16_drive_rpmR_err_cumsum += s16_drive_rpmR_err;  
  }
  s16_drive_rpmR_i = s16_drive_rpmR_err_cumsum*0.002 * x_u16_drive_mtr_rpmR_ki/100;
  g_s16_drive_mtr_spdR = s16_drive_rpmR_p +s16_drive_rpmR_i - s16_drive_rpmR_d;
  if( 100 < g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = 100;
    s16_drive_rpmR_err_cumsum = 0;
  }
  else if( -100 > g_s16_drive_mtr_spdR ){
    g_s16_drive_mtr_spdR = -100;
    s16_drive_rpmR_err_cumsum = 0;
  }
  
/* 移動平均
  MovingAve_sumR -= err_windowR[MovingAve_indexR];// 古い誤差を引いて合計を更新
  err_windowR[MovingAve_indexR] = g_s16_drive_mtr_spdR;  // 新しい誤差を配列に追加
  MovingAve_sumR += g_s16_drive_mtr_spdR;    // 合計を更新
  MovingAve_indexR = (MovingAve_indexR + 1) % MA_WINDOW_SIZE;     // インデックスを更新
  if (MovingAve_countR < MA_WINDOW_SIZE) {    // ウィンドウが満たされていない場合、カウントを増やす
      MovingAve_countR++;
  }
  g_s16_drive_mtr_spdR = MovingAve_sumR / MovingAve_countR;
*/
  g_s16_drive_mtr_powerL = g_s16_drive_mtr_spdL_FF + g_s16_drive_mtr_spdL;
  g_s16_drive_mtr_powerR = g_s16_drive_mtr_spdR_FF + g_s16_drive_mtr_spdR;

  pup_motor_set_power(motorL, g_s16_drive_mtr_powerL);
  pup_motor_set_power(motorR, g_s16_drive_mtr_powerR);  // パワーを設定
  //pup_motor_set_speed(motorL, spdL);                  // スピードを設定
  //pup_motor_set_speed(motorR, spdR);                  // スピードを設定
  
  s16_drive_rpmL_err_old = s16_drive_rpmL_err;
  s16_drive_rpmR_err_old = s16_drive_rpmR_err;
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