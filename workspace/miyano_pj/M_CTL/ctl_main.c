/* 機体制御管理*/
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include "spike/hub/speaker.h"  /* debug */
#include <miyano_pj.h>
#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "ctl_main.h"
#include "linetrace_run.h"
#include "const_run.h"
#include "arm_ctl.h"
#include "color_chase.h"

#include "../D_DEVICE/color_snc.h"
#include "../D_DEVICE/comm.h"

/* 外部公開変数 */
uint16_t g_u16_ctl_main_mode;       /* 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ) */

/* 機体制御初期化 */
void ini_ctl_main( void ){
    ini_linetrace_run();            /* ライントレース走行初期化 */
    ini_const_run();                /* 一定出力走行初期化 */
    ini_arm_ctl();                  /* アーム制御初期化 */
    ini_color_chase();              /* カラーチェイス 初期化処理 */

    g_u16_ctl_main_mode = STOP;     /* 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ) */
}

/* 機体制御周期処理 */
void cyc_ctl_main( void ){
    get_color_rgb(&g_s16_linetrace_run_r,&g_s16_linetrace_run_g,&g_s16_linetrace_run_b);
    switch (g_u16_ctl_main_mode)
    {
    case LINETRACE_RUN:             /* ライントレース走行 */
        cyc_linetrace_run();
        break;
    case CONST_RUN:                 /* 一定出力走行 */
        cyc_const_run();
        break;
    case CONST_TURN:
        cyc_const_turn();           /* 一定旋回 */
        break;
    case ARM_CTL:                   /* アーム動作 */
        cyc_arm_ctl();
        break;
    case COLOR_CHASE:               /* カラーチェイス */
        cyc_color_chase();
        break;
    case STOP:                      /* 停止 */
        cyc_const_stop();
        break;
    default:
        break;
    }
}