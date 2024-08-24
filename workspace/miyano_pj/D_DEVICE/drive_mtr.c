#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/pup/motor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "drive_mtr.h"

// PUPモータデバイスポインタ
pup_motor_t *motorL;
pup_motor_t *motorR;

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
}

/* 駆動モータDUty設定 */
void set_drive_mtr_duty( int16_t dutyL, int16_t dutyR ){
  pup_motor_set_power(motorL, dutyL);             // パワーを設定
  pup_motor_set_power(motorR, dutyR);             // パワーを設定
}

/* 駆動モータ回転速度設定 */
void set_drive_mtr_spd( int16_t spdL, int16_t spdR ){
  pup_motor_set_speed(motorL, spdL);              // 速度を設定
  pup_motor_set_speed(motorR, spdR);              // 速度を設定
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