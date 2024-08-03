/* 機体制御管理*/

#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "ctl_main.h"

#include "linetrace_run.h"
#include "const_run.h"
#include "arm_ctl.h"

/* 外部参照変数 */
uint16_t g_u16_ctl_main_mode;    /* 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ) */

/* 機体制御初期化 */
void ini_ctl_main( void ){
    ini_linetrace_run();            /* ライントレース走行初期化 */
    ini_const_run();                /* 一定出力走行初期化 */
    
    g_u16_ctl_main_mode = CONST_RUN;        /* 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ) */
}

/* 機体制御周期処理 */
void cyc_ctl_main( void ){
    switch (g_u16_ctl_main_mode)
    {
    case LINETRACE_RUN:             /* ライントレース走行 */
        cyc_linetrace_run();
        break;
    case CONST_RUN:                 /* 一定出力走行 */
        cyc_const_run();
        break;
    case ARM_CTL:                   /* アーム動作 */
        /* code */
        break;
    default:
        break;
    }
}