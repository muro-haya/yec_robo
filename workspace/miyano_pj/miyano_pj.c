#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "spike/hub/speaker.h"  /* debug */
#include <t_syslog.h>           /*debug */

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "M_CTL/ctl_main.h"
#include "M_CTL/linetrace_run.h"
#include "M_CTL/const_run.h"
#include "D_DEVICE/drive_mtr.h"
#include "D_DEVICE/arm_mtr.h"
#include "D_DEVICE/color_snc.h"
#include "D_DEVICE/sonic_snc.h"
#include "D_DEVICE/button.h"

/* 初回処理 */
void Main(intptr_t exinf){
  uint16_t button;

  syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", 0);   /* debug */

  /* デバイス層 */
  ini_drive_mtr();
  ini_arm_mtr();
  ini_sonic_snc();
  ini_color_snc();
  ini_button();
  /* ミドル層 */
  ini_ctl_main();
  /* アプリ層 */

  // hub_speaker_set_volume( 50 );
  // hub_speaker_play_tone( NOTE_C6, SOUND_MANUAL_STOP );

  while(1){
  //   g_u16_const_run_spd = 500;
    set_tgt_linetrace_run();
    button = get_button( BUTTON_BT );
    if( 1 == button ){
      break;
    }
  }
  while (1){
    cyc_ctl_main();               /* 機体制御周期処理 */
  }
  
  hub_speaker_stop();

  /* 2msecタスクの起動 */
  sta_cyc(MAIN_2M_CYC);
  /* 10msecタスクの起動 */
  sta_cyc(MAIN_10M_CYC);/* ここ変更 */
  
  /* タスク終了 */
  ext_tsk();
}

/* 2msec周期処理 */
void Main_2m( intptr_t unused ){
  //cyc_ctl_main();               /* 機体制御周期処理 */
  /* タスク終了 */
  ext_tsk();
}

/* 10msec周期処理 */
void Main_10m( intptr_t unused ){
  // hub_speaker_set_volume( 20 );
  // hub_speaker_play_tone( NOTE_C5, SOUND_MANUAL_STOP );
}

