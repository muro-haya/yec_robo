/* ダブルループ攻略 */
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

#include "double_loop.h"

/* 適合値 */
uint16_t x_u16_DoubleLoop_1_movement = 100;         /* 分岐1での移動量[mm] */
uint16_t x_u16_DoubleLoop_2_movement = 100;         /* 分岐2での移動量[mm] */
uint16_t x_u16_DoubleLoop_3_movement = 100;         /* 分岐3での移動量[mm] */
uint16_t x_u16_DoubleLoop_4_movement = 100;         /* 分岐4での移動量[mm] */
uint16_t x_u16_DoubleLoop_spd        = 100;         /* 各分岐での通過速度 */
uint16_t x_u16_DoubleLoop_deg        = 30;          /* 指定旋回角[deg] */

/* 外部公開変数 */
uint16_t g_u16_DoubleLoop_phase;                 /* ダブルループフェイズカウント */

/* 外部非公開変数 */
uint16_t DLreset_flg;                            /* リセットフラグ */

/* 外部非公開関数 */
void line_jdg_color( uint16_t fin_color );                            /* 色認識までライントレース走行するフェイズ */
void DLrd_jdg_movement( uint16_t fin_movement, int16_t run_spd );     /* 指定距離までR・D走行するフェイズ */
void DLturn_jdg_deg( uint16_t fin_deg );                                /* 指定角まで旋回するフェイズ */


/* ダブルループ周期処理 */
bool_t cyc_DoubleLoop( void ){
    bool_t bdat;

    bdat = 0;

    switch (g_u16_DoubleLoop_phase)
    {
    case 1:
    case 4:
    case 6:
    case 8:
        line_jdg_color( RESULT_BLUE );
        break;
    case 2:
        DLturn_jdg_deg( x_u16_DoubleLoop_deg );
        break;
    case 3:
        DLrd_jdg_movement( x_u16_DoubleLoop_1_movement, x_u16_DoubleLoop_spd );
        break;
    case 5:
        DLrd_jdg_movement( x_u16_DoubleLoop_2_movement, x_u16_DoubleLoop_spd );
        break;
    case 7:
        DLrd_jdg_movement( x_u16_DoubleLoop_3_movement, x_u16_DoubleLoop_spd );
        break;
    case 9:
        DLrd_jdg_movement( x_u16_DoubleLoop_4_movement, x_u16_DoubleLoop_spd );
        break;
    case 10:
        bdat = 1;
        break;
    default:
        break;
    }

    return bdat;
}

/* 色認識までライントレースするフェイズ */
void line_jdg_color( uint16_t fin_color ){
    uint16_t color_result;

    g_u16_ctl_main_mode = LINETRACE_RUN;
    g_u16_linetrace_run_way = 1;
    g_u16_linetrace_run_bsV = 100;

    color_result = get_rec_color();
    if( fin_color == color_result ){
        g_u16_DoubleLoop_phase += 1;

         /* ライントレースの左右を反転*/
        uint16_t A_side = g_u16_linetrace_run_lpos;
        uint16_t B_side = g_u16_linetrace_run_rpos;
        g_u16_linetrace_run_lpos = B_side;
        g_u16_linetrace_run_rpos = A_side;
    } 

}

/* 指定距離までR・D走行するフェイズ */
void DLrd_jdg_movement( uint16_t fin_movement, int16_t run_spd ){
    uint16_t movement_result;

    if( 0 == DLreset_flg ){
        reset_cal_movement();
        DLreset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 200;
        
        movement_result = get_cal_movement();
        if( fin_movement < movement_result ){
            g_u16_DoubleLoop_phase += 1;
            DLreset_flg = 0;
        }
    }
}

/* 指定角まで旋回するフェイズ */
void DLturn_jdg_deg( uint16_t fin_deg ){
    uint16_t deg_result;

    if( 0 == DLreset_flg ){
        reset_cal_movement();
        DLreset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = CONST_TURN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 100;

        deg_result = get_cal_movement_body_deg();
        if( fin_deg < deg_result ){
            g_u16_DoubleLoop_phase += 1;
            DLreset_flg = 0;
        }
    }
}