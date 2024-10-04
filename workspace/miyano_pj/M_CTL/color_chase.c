/* カラーチェイス */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <miyano_pj.h>
#include "kernel_cfg.h"
#include "syssvc/serial.h"
#include "color_chase.h"
#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/color_snc.h"
#include "../D_DEVICE/comm.h"
#include "../D_DEVICE/button.h"

#define FB_WAY 1                                /* FB制御指令方法(0:DUTY 1:回転速度) */
#define BSSPD  200                              /* 基本指令値 */

/* 適合値 */
int16_t  x_u16_color_chase_kp = 100;          /* P項ゲイン値[0.01]*/
int16_t  x_u16_color_chase_ki = 0;            /* I項ゲイン値[0.01]*/
int16_t  x_u16_color_chase_kd = 0;            /* D項ゲイン値[0.01]*/

/* 外部公開変数 */
uint16_t g_u16_color_chase_way;               /* ライントレース制御指令方法(0:DUTY 1:回転速度) */
uint16_t g_u16_color_chase_bsV;               /* FB制御基本値[-] */
uint16_t g_u16_color_chase_fbTgt;             /* FB制御目標値[-] */
uint16_t g_u16_color_chase_fbPv;              /* FB制御現在値[-] */
int16_t  g_s16_color_chase_p;                 /* P項計算結果[0.1]*/
int16_t  g_s16_color_chase_i;                 /* I項計算結果[0.1]*/
int16_t  g_s16_color_chase_d;                 /* D項計算結果[0.1]*/
int16_t  g_s16_color_chase_fbCmdv;            /* FB制御指令値[-] */

int16_t  g_s16_color_chase_debug;

/* 外部非公開変数 */
static int16_t s16_posdlt_old;                  /* 位置偏差前回値 */
static int16_t s16_spddlt_old;                  /* 速度偏差前回値 */
static uint16_t u16_dlt_sum;                    /* 位置偏差積算値 */
static uint16_t u16_wpos;                       /* 白色値 */
static uint16_t u16_bpos;                       /* 黒色値 */
/* 外部非公開関数 */
/* カラーチェイス 初期化処理 */
void ini_color_chase( void ){

    g_u16_color_chase_way    = FB_WAY;          /* FB制御指令方法(0:DUTY 1:回転速度) */
    g_u16_color_chase_bsV    = 250;              /* FB制御基本値[-] */
    g_u16_color_chase_fbTgt  = 50;              /* FB制御目標値[-] */
    g_u16_color_chase_fbPv   = 0;               /* FB制御現在値[-] */
    g_s16_color_chase_p      = 0;              /* P項計算結果[0.01]*/
    g_s16_color_chase_i      = 0;               /* I項計算結果[0.01]*/
    g_s16_color_chase_d      = 0;               /* D項計算結果[0.01]*/
    g_s16_color_chase_fbCmdv = 0;               /* FB制御指令値[-] */

    s16_posdlt_old             = 0;             /* 位置偏差前回値 */
    s16_spddlt_old             = 0;             /* 速度偏差前回値 */
    u16_dlt_sum                = 0;             /* 位置偏差積算値 */
    u16_wpos                   = 0;             /* 白色値 */
    u16_bpos                   = 0;             /* 黒色値 */
}
/* カラーチェイス 周期処理 */
void cyc_color_chase( void ){
    int16_t s16_posdlt;                         /* 位置偏差 */
    int16_t s16_spddlt;                         /* 速度偏差 */
    int16_t s16_LVulue;                         /* 左モータ指示値 */
    int16_t s16_RVulue;                         /* 右モータ指示値 */

    
    /* 偏差計算 */
    if(0 != g_u16_comm_rx_jdg_pet){             /* ペットボトル検出時 */
        s16_posdlt = g_u16_comm_rx_pet_xpos_red - g_u16_color_chase_fbTgt;  /* 位置偏差計算 */
    }
    else{                                       /* 未検出 */
        s16_posdlt     = 0;
        s16_posdlt_old = 0;
        u16_dlt_sum    = 0;
        s16_spddlt     = 0;
    }
    /* P項計算 */
    g_s16_color_chase_p = s16_posdlt * x_u16_color_chase_kp;        /* P項計算 */
    /* I項計算 */
    u16_dlt_sum += s16_posdlt;                                      /* 位置偏差積算 */
    g_s16_color_chase_i = u16_dlt_sum * x_u16_color_chase_ki;       /* I項計算 */
    /* D項計算 */
    s16_spddlt = s16_posdlt - s16_posdlt_old;                       /* 速度偏差取得 */
    g_s16_color_chase_d = s16_spddlt * x_u16_color_chase_kd;        /* D項計算 */

    /* 指示値算出 */
    g_s16_color_chase_fbCmdv =    g_s16_color_chase_p/100
                                + g_s16_color_chase_i/100
                                + g_s16_color_chase_d/100;

    s16_LVulue = (int16_t)g_u16_color_chase_bsV - g_s16_color_chase_fbCmdv;  /* 左モータ指示値計算 */
    s16_RVulue = (int16_t)g_u16_color_chase_bsV + g_s16_color_chase_fbCmdv;  /* 右モータ指示値計算 */
    g_s16_color_chase_debug = s16_LVulue;
    
    /* モータ駆動指示 */
    if( 0 == g_u16_color_chase_way ){            /* DUTY指示 */
        set_drive_mtr_duty(s16_LVulue, s16_RVulue);
    }
    else{                                       /* 回転速度指示 */
        set_drive_mtr_spd(s16_LVulue, s16_RVulue);
    }
    s16_posdlt_old = s16_posdlt;                /* 前回値保存 */
    s16_spddlt_old = s16_spddlt;                /* 前回値保存 */
}