#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <pbio/color.h>

#include "spike/pup/motor.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "arm_mtr.h"

// PUPモータデバイスポインタ
pup_motor_t *motorArm;

/* アームモータ初期化 */
void ini_arm_mtr( void ){
  // PUPモータデバイスポインタを取得
  motorArm = pup_motor_get_device(PBIO_PORT_ID_A);

  // モータのセットアップ
  pup_motor_setup(motorArm, PUP_DIRECTION_CLOCKWISE, true);
  
  // アームモータの角度をリセット
  pup_motor_reset_count(motorArm);
}

/* アームモータDUty設定 */
void set_arm_mtr_duty( int16_t duty ){
  pup_motor_set_power(motorArm, duty);              // パワーを設定
}

/* アームモータ回転速度設定 */
void set_arm_mtr_spd( int16_t spd ){
  pup_motor_set_speed(motorArm, spd);               // 速度を設定
}

/* アームモータエンコーダカウントリセット */
void rst_arm_mtr_cnt( void ){
  pup_motor_reset_count(motorArm);                  // アームモータの角度をリセット
}

/* アームモータエンコーダカウント取得 */
void get_arm_mtr_cnt( uint16_t* arm_cnt ){
  *arm_cnt = abs(pup_motor_get_count(motorArm));     // アームモータエンコーダカウント取得
}