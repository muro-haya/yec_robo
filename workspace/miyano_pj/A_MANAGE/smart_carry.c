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
uint16_t x_u16_smart_carry_3_distance = 50;         /* キャリーボトル認識距離[mm] */
uint16_t x_u16_smart_carry_4_movement = 70;         /* ティポジション通過移動量[mm] */
uint16_t x_u16_smart_carry_5_movement = 150;        /* ディセイブルゾーン通過移動量[mm] */
uint16_t x_u16_smart_carry_6_movement = 300;        /* ランディング移動量[mm] */
uint16_t x_u16_smart_carry_7_movement = 150;        /* 後進移動量[mm] */
uint16_t x_u16_smart_carry_8_deg      = 70;         /* 指定旋回角[deg] */
uint16_t x_u16_smart_carry_4_spd      = 70;         /* ティポジション通過速速 */
uint16_t x_u16_smart_carry_6_spd      = 300;        /* ランディング速度 */
uint16_t x_u16_smart_carry_7_spd      = 150;        /* 後進速度 */
uint16_t x_u16_smart_carry_deg        = 100;        /* スマートキャリー指定角[deg] */


/* 外部公開変数 */
uint16_t g_u16_smart_carry_phase;                   /* スマートキャリーフェイズカウント */

/* 外部非公開変数 */
uint16_t reset_flg;                                 /* リセットフラグ */

/* 外部非公開関数 */
void turn_jdg_color( uint16_t fin_color );                          /* 色認識まで旋回するフェイズ */
void turn_jdg_deg( uint16_t fin_deg );                              /* 指定角まで旋回するフェイズ */
void rd_jdg_color( uint16_t fin_color );                            /* 色認識までR・D走行するフェイズ */
void rd_jdg_movement( uint16_t fin_movement, int16_t run_spd );     /* 指定距離までR・D走行するフェイズ */
void line_jdg_obj( uint16_t fin_distance );                         /* 物体付近までライントレースするフェイズ */
void line_jdg_movement( uint16_t fin_movement );                    /* 指定距離までライントレースするフェイズ */

/* スマートキャリー初期化処理 */
void ini_smart_carry( void ){
    g_u16_smart_carry_phase = 1;
    reset_flg = 0;
}

/* スマートキャリー周期処理 */
bool_t cyc_smart_carry( void ){
    bool_t bdat;

    bdat = 0;

    switch (g_u16_smart_carry_phase)
    {
    case 1:
        turn_jdg_color( RESULT_RED );
        break;
    case 2:
        rd_jdg_color( RESULT_BLUE );
        break;
    case 3:
        line_jdg_obj( x_u16_smart_carry_3_distance );
        break;
    case 4:
        rd_jdg_movement( x_u16_smart_carry_4_movement, x_u16_smart_carry_4_spd );
        break;
    case 5:
        line_jdg_movement( x_u16_smart_carry_5_movement );
        break;
    case 6:
        rd_jdg_movement( x_u16_smart_carry_6_movement, x_u16_smart_carry_6_spd );
        break;
    case 7:
        rd_jdg_movement( x_u16_smart_carry_7_movement, x_u16_smart_carry_7_spd );
        break;
    case 8:
        turn_jdg_deg( x_u16_smart_carry_8_deg );
        break;
    case 9:
        rd_jdg_color(RESULT_BLACK);
        break;
    case 10:
        bdat = 1;
        break;
    default:
        break;
    }

    return bdat;
}

/* 色認識まで旋回するフェイズ */
void turn_jdg_color( uint16_t fin_color ){
    uint16_t color_result;

    g_u16_ctl_main_mode = CONST_TURN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = 100;

    /* fin_color == color_result カメラで色認識結果 */
    if( 1 ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定角まで旋回するフェイズ */
void turn_jdg_deg( uint16_t fin_deg ){
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
        if( fin_deg < deg_result ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}
/* 色認識までR・D走行するフェイズ */
void rd_jdg_color( uint16_t fin_color ){
    uint16_t color_result;

    g_u16_ctl_main_mode = CONST_RUN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = 200;

    color_result = get_rec_color();
    if( fin_color == color_result ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定距離までR・D走行するフェイズ */
void rd_jdg_movement( uint16_t fin_movement, int16_t run_spd ){
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
        if( fin_movement < movement_result ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}
/* 物体付近までライントレースするフェイズ */
void line_jdg_obj( uint16_t fin_distance ){
    uint16_t distance_result;

    g_u16_ctl_main_mode = LINETRACE_RUN;
    g_u16_linetrace_run_way = 1;
    g_u16_linetrace_run_bsV = 100;

    distance_result = get_cal_distance();
    if( fin_distance < distance_result ){
        g_u16_smart_carry_phase += 1;
    }
}
/* 指定距離までライントレースするフェイズ */
void line_jdg_movement( uint16_t fin_movement ){
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
        if( fin_movement < movement_result ){
            g_u16_smart_carry_phase += 1;
            reset_flg = 0;
        }
    }
}