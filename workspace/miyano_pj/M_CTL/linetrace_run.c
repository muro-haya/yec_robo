/* ライントレース走行 */

#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "linetrace_run.h"

#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/color_snc.h"
#include "../D_DEVICE/button.h"

#define FB_WAY 1                                /* FB制御指令方法(0:DUTY 1:回転速度) */
#define BSSPD  50                              /* 基本指令値 */

/* 適合値 */
int16_t  x_u16_linetrace_run_kp = 3;            /* P項ゲイン値[0.1]*/
int16_t  x_u16_linetrace_run_ki = 0;            /* I項ゲイン値[0.01]*/
int16_t  x_u16_linetrace_run_kd = 0;            /* D項ゲイン値[0.1]*/

/* 外部公開変数 */
uint16_t g_u16_linetrace_run_way;               /* ライントレース制御指令方法(0:DUTY 1:回転速度) */
uint16_t g_u16_linetrace_run_bsV;               /* FB制御基本値[-] */
uint16_t g_u16_linetrace_run_fbTgt;             /* FB制御目標値[-] */
uint16_t g_u16_linetrace_run_fbPv;              /* FB制御現在値[-] */
int16_t  g_u16_linetrace_run_p;                 /* P項計算結果[0.1]*/
int16_t  g_u16_linetrace_run_i;                 /* I項計算結果[0.1]*/
int16_t  g_u16_linetrace_run_d;                 /* D項計算結果[0.1]*/
int16_t  g_u16_linetrace_run_fbCmdv;            /* FB制御指令値[-] */
uint16_t g_u16_linetrace_run_lpos;              /* 左色値 */
uint16_t g_u16_linetrace_run_rpos;              /* 右色値 */

/* 外部非公開変数 */
static int16_t s16_posdlt_old;                  /* 位置偏差前回値 */
static int16_t s16_spddlt_old;                  /* 速度偏差前回値 */
static uint16_t u16_dlt_sum;                    /* 位置偏差積算値 */

/* ライントレース走行初期化 */
void ini_linetrace_run( void ){

    g_u16_linetrace_run_way    = FB_WAY;        /* FB制御指令方法(0:DUTY 1:回転速度) */
    g_u16_linetrace_run_bsV    = BSSPD;         /* FB制御基本値[-] */
    g_u16_linetrace_run_fbTgt  = 0;             /* FB制御目標値[-] */
    g_u16_linetrace_run_fbPv   = 0;             /* FB制御現在値[-] */
    g_u16_linetrace_run_p      = 0;             /* P項計算結果[0.1]*/
    g_u16_linetrace_run_i      = 0;             /* I項計算結果[0.1]*/
    g_u16_linetrace_run_d      = 0;             /* D項計算結果[0.1]*/
    g_u16_linetrace_run_fbCmdv = 0;             /* FB制御指令値[-] */
    g_u16_linetrace_run_lpos   = 0;             /* 左色値 */
    g_u16_linetrace_run_rpos   = 0;             /* 右色値 */

    s16_posdlt_old             = 0;             /* 位置偏差前回値 */
    s16_spddlt_old             = 0;             /* 速度偏差前回値 */
    u16_dlt_sum                = 0;             /* 位置偏差積算値 */
}
/* ライントレース目標値設定 */
void set_tgt_linetrace_run( void ){
    uint16_t button;

    button = get_button( BUTTON_LEFT );
    if( 1 == button ){
        // get_color_rgb(&u16_wpos,0, 0);
        get_color_ref(&g_u16_linetrace_run_lpos);                /* 現在値取得 */
    }
    button = get_button( BUTTON_RIGHT );
    if( 1 == button ){
        // get_color_rgb(&u16_bpos,0, 0);
        get_color_ref(&g_u16_linetrace_run_rpos);                /* 現在値取得 */
    }
    button = get_button( BUTTON_CENTER );
    if( 1 == button ){
        g_u16_linetrace_run_fbTgt = ( g_u16_linetrace_run_lpos + g_u16_linetrace_run_rpos ) / 2;
    }
}

/* ライントレース走行周期処理 */
void cyc_linetrace_run( void ){
    int16_t s16_posdlt;                         /* 位置偏差 */
    int16_t s16_spddlt;                         /* 速度偏差 */
    int16_t s16_LVulue;                         /* 左モータ指示値 */
    int16_t s16_RVulue;                         /* 右モータ指示値 */
    
    // get_color_rgb(&g_u16_linetrace_run_fbPv,0, 0);
    get_color_ref(&g_u16_linetrace_run_fbPv);                           /* 現在値取得 */
    /* P項計算 */
    s16_posdlt = g_u16_linetrace_run_fbTgt - g_u16_linetrace_run_fbPv;  /* 位置偏差計算 */
    g_u16_linetrace_run_p = s16_posdlt * x_u16_linetrace_run_kp;        /* P項計算 */
    /* I項計算 */
    u16_dlt_sum += s16_posdlt;                                          /* 位置偏差積算 */
    g_u16_linetrace_run_i = u16_dlt_sum * x_u16_linetrace_run_ki;       /* I項計算 */
    /* D項計算 */
    s16_spddlt = s16_posdlt - s16_posdlt_old;                           /* 速度偏差取得 */
    g_u16_linetrace_run_d = s16_spddlt * x_u16_linetrace_run_kd;        /* D項計算 */

    /* 指示値算出 */
    g_u16_linetrace_run_fbCmdv = g_u16_linetrace_run_p
                                + g_u16_linetrace_run_i
                                + g_u16_linetrace_run_d;

    s16_LVulue = g_u16_linetrace_run_bsV - g_u16_linetrace_run_fbCmdv;  /* 左モータ指示値計算 */
    s16_RVulue = g_u16_linetrace_run_bsV + g_u16_linetrace_run_fbCmdv;  /* 右モータ指示値計算 */

    /* モータ駆動指示 */
    if( 0 == g_u16_linetrace_run_way ){       /* DUTY指示 */
        set_drive_mtr_duty(s16_LVulue, s16_RVulue);
    }
    else{                                       /* 回転速度指示 */
        set_drive_mtr_spd(s16_LVulue, s16_RVulue);
    }

    s16_posdlt_old = s16_posdlt;                /* 前回値保存 */
    s16_spddlt_old = s16_spddlt;                /* 前回値保存 */
    
}