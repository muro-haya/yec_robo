/* スマートキャリー攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../M_CTL/ctl_main.h"
#include "../M_MEASURE/cal_distance.h"
#include "../M_MEASURE/cal_movement.h"
#include "../M_MEASURE/rec_color.h"
#include "../M_CTL/linetrace_run.h"
#include "../M_CTL/const_run.h"
#include "../M_CTL/arm_ctl.h"

#include "smart_carry.h"

/* 適合値 */
uint16_t x_u16_smart_carry_deg = 100;       /* スマートキャリー指定角[deg] */

/* 外部公開変数 */
uint16_t g_u16_smart_carry_phase;           /* スマートキャリーフェイズカウント */

/* 外部非公開変数 */
uint16_t reset_flg;                         /* リセットフラグ */

/* 外部非公開関数 */
void turn_jdg_color( void );                /* 色認識まで旋回するフェイズ */
void turn_jdg_deg( void );                  /* 指定角まで旋回するフェイズ */
void rd_jdg_color( void );                  /* 色認識までR・D走行するフェイズ */
void rd_jdg_movement( void );               /* 指定距離までR・D走行するフェイズ */
void line_jdg_obj( void );                  /* 物体付近までライントレースするフェイズ */
void line_jdg_movement( void );             /* 指定距離までライントレースするフェイズ */

/* スマートキャリー初期化処理 */
void ini_smart_carry( void ){
    g_u16_smart_carry_phase = 0;
    reset_flg = 0;
}

/* スマートキャリー周期処理 */
bool_t cyc_smart_carry( void ){
    bool_t bdat;

    bdat = 0;

    switch (g_u16_smart_carry_phase)
    {
    case TURN_JDG_COLOR:
        turn_jdg_color();
        break;
    case TURN_JDG_DEG:
        turn_jdg_deg();
        break;
    case RD_JDG_COLOR:
        rd_jdg_color();
        break;
    case RD_JDG_MOVEMENT:
        rd_jdg_movement();
        break;
    case LINE_JDG_OBJ:
        line_jdg_obj();
        break;
    case LINE_JDG_MOVEMENT:
        line_jdg_movement();
        break;
    case SMART_CARRY_END:
        bdat = 1;
        break;
    default:
        break;
    }

    return bdat;
}

/* 色認識まで旋回するフェイズ */
void turn_jdg_color( void ){
    uint16_t color_result;

    g_u16_ctl_main_mode = CONST_TURN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = 100;

    /* カメラで色認識 */
    if( 1 ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定角まで旋回するフェイズ */
void turn_jdg_deg( void ){
    uint16_t deg_result;

    if( 0 == reset_flg ){
        reset_cal_movement();
        reset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = CONST_TURN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 100;

        deg_result = get_cal_movement_body_deg();
        if( x_u16_smart_carry_deg < deg_result ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}
/* 色認識までR・D走行するフェイズ */
void rd_jdg_color( void ){
    uint16_t color_result;

    g_u16_ctl_main_mode = CONST_RUN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = 200;

    color_result = get_rec_color();
    if( 1 ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定距離までR・D走行するフェイズ */
void rd_jdg_movement( void ){
    uint16_t movement_result;

    if( 0 == reset_flg ){
        reset_cal_movement();
        reset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 200;
        
        movement_result = get_cal_movement();
        if( 1 ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}
/* 物体付近までライントレースするフェイズ */
void line_jdg_obj( void ){
    uint16_t distance_result;

    g_u16_ctl_main_mode = LINETRACE_RUN;
    g_u16_linetrace_run_way = 1;
    g_u16_linetrace_run_bsV = 100;

    distance_result = get_cal_distance();
    if( 1 ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定距離までライントレースするフェイズ */
void line_jdg_movement( void ){
    uint16_t movement_result;

    if( 0 == reset_flg ){
        reset_cal_movement();
        reset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = LINETRACE_RUN;
        g_u16_linetrace_run_way = 1;
        g_u16_linetrace_run_bsV = 100;
        
        movement_result = get_cal_movement();
        if( 1 ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}