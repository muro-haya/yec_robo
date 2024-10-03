/* 一定出力走行 */

#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "spike/hub/speaker.h"  /* debug */

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "const_run.h"

#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/button.h"

/* 外部公開変数 */
uint16_t g_u16_const_run_way;           /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
int16_t  g_s16_const_run_spd;           /* 設定速度 */
int16_t  g_s16_const_run_duty;          /* 設定DUTY */
int16_t  g_s16_const_curve_rate;        /* 一定出力 直進率 */


/* 一定出力走行初期化 */
void ini_const_run( void ){
    g_u16_const_run_way  = 1;           /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
    g_s16_const_run_spd  = 720;         /* 設定速度 */
    g_s16_const_run_duty = 32;          /* 設定DUTY */
    g_s16_const_curve_rate = 100;       /* 一定出力旋回 直進率 */
}

/* R・D走行周期処理 */
void cyc_const_run( void ){
    int16_t const_spd;

    const_spd = (int16_t)((int32_t)g_s16_const_run_spd * g_s16_const_curve_rate / 100);

    /* モータ駆動指示 */
    if( 0 == g_u16_const_run_way ){     /* DUTY指示 */
        set_drive_mtr_duty(g_s16_const_run_duty, g_s16_const_run_duty);
    }
    else{                               /* 回転速度指示 */
        set_drive_mtr_spd(g_s16_const_run_spd, const_spd);
    }
}

/* 旋回処理 */
void cyc_const_turn( void ){

    /* モータ駆動指示 */
    if( 0 == g_u16_const_run_way ){     /* DUTY指示 */
        set_drive_mtr_duty(g_s16_const_run_duty, -g_s16_const_run_duty);
    }
    else{                               /* 回転速度指示 */
        set_drive_mtr_spd(g_s16_const_run_spd, -g_s16_const_run_spd);
    }
}

/* 停止周期処理 */
void cyc_const_stop( void ){
    set_drive_mtr_duty(0, 0);
}