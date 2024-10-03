#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include "spike/hub/speaker.h"  /* debug */
#include "spike/hub/button.h"
#include "spike/hub/display.h"
#include "pbio/light_matrix.h"

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "A_MANAGE/manage.h"
#include "M_CTL/song.h"
#include "M_CTL/ctl_main.h"
#include "M_CTL/linetrace_run.h"
#include "M_CTL/const_run.h"
#include "M_CTL/arm_ctl.h"
#include "M_MEASURE/cal_movement.h"
#include "M_MEASURE/cal_distance.h"
#include "M_MEASURE/rec_color.h"
#include "D_DEVICE/drive_mtr.h"
#include "D_DEVICE/arm_mtr.h"
#include "D_DEVICE/color_snc.h"
#include "D_DEVICE/sonic_snc.h"
#include "D_DEVICE/button.h"
#include "D_DEVICE/comm.h"

uint8_t cnt;
hub_button_t pressed_main;

/* 初回処理 */
void Main(intptr_t exinf){

  /* デバイス層 */
  ini_drive_mtr();
  ini_arm_mtr();
  ini_sonic_snc();
  ini_color_snc();
  ini_button();
  ini_comm();
  /* ミドル層 */
  ini_ctl_main();
  ini_cal_distance();
  ini_cal_movement();
  ini_rec_color();
  ini_song();
  /* アプリ層 */
  ini_manage();

  /* 変数初期化 */
  cnt = 0;

  /* 2msecタスクの起動 */
  sta_cyc(MAIN_2M_CYC);
  /* 10msecタスクの起動 */
  sta_cyc(MAIN_10M_CYC);
  /* 100msecタスクの起動 */
  sta_cyc(MAIN_100M_CYC);
  /* 通信用タスク起動 */
  sta_cyc(TX_1M_CYC);
  sta_cyc(RX_1M_CYC);
  
  /* タスク終了 */
  ext_tsk();
}

/* 2msec周期処理 */
void Main_2m( intptr_t unused ){
  cyc_get_color_rgb();            /* RGBセンサ値取得 */
  cyc_ctl_main();                 /* 機体制御周期処理 */
  /* タスク終了 */
  ext_tsk();
}

/* 10msec周期処理 */
void Main_10m( intptr_t unused ){
  cyc_manage();                   /* 競技攻略周期処理 */
  /* タスク終了 */
  ext_tsk();
}

/* 100msec周期処理 */
void Main_100m( intptr_t unused ){
  cyc_watch_comm();

  if(1 < cnt){
    cyc_song();
    cnt = 0;
  }
  cnt += 1;

  /* タスク終了 */
  ext_tsk();
}

/* 1msec送信処理 */
void Tx_1m( intptr_t unused ){
  cyc_tx();
  /* タスク終了 */
  ext_tsk();
}

/* 1msec受信処理 */
void Rx_1m( intptr_t unused ){
  cyc_rx();
  /* タスク終了 */
  ext_tsk();
}