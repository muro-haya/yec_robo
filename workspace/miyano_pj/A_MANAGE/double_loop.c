/* ダブルループ攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "spike/hub/display.h"
#include "pbio/light_matrix.h"

#include "../M_CTL/ctl_main.h"
#include "../M_MEASURE/cal_distance.h"
#include "../M_MEASURE/cal_movement.h"
#include "../M_MEASURE/rec_color.h"
#include "../M_CTL/linetrace_run.h"
#include "../M_CTL/const_run.h"

#include "double_loop.h"

/* 適合値 */
uint16_t x_u16_DoubleLoop_1_movement = 60;         /* 分岐1での移動量[mm]50 */
uint16_t x_u16_DoubleLoop_2_movement = 200;         /* 分岐2での移動量[mm] */
uint16_t x_u16_DoubleLoop_3_movement = 50;         /* 分岐3での移動量[mm] */
uint16_t x_u16_DoubleLoop_4_movement = 50;         /* 分岐4での移動量[mm] */
uint16_t x_u16_DoubleLoop_spd        = 50;         /* 各分岐での通過速度 */
int16_t  x_u16_DoubleLoop_curve_rate = 95;         /* 各分岐での曲がり具合 */
uint16_t x_u16_DoubleLoop_deg1      = 10;          /* 指定旋回角[deg] */
uint16_t x_u16_DoubleLoop_deg2      = 10;          /* 指定旋回角[deg] */
uint16_t x_u16_DoubleLoop_line_movement_0 = 300;    /* スタート後の指定距離ライントレース移動量[mm] */
uint16_t x_u16_DoubleLoop_line_movement_1 = 1000;   /* 分岐1での指定距離ライントレース移動量[mm]*/
uint16_t x_u16_DoubleLoop_line_movement_2 = 1000;
uint16_t x_u16_DoubleLoop_line_movement_3 = 500;
uint16_t x_u16_DoubleLoop_line_movement_4 = 500;

/* 外部公開変数 */
uint16_t g_u16_DoubleLoop_phase;                 /* ダブルループフェイズカウント */

uint16_t g_16_DLdeg;
uint16_t g_u16_degfin;

uint16_t g_u16_DLmove;
uint16_t g_u16_movefin;

/* 外部非公開変数 */
uint16_t DLreset_flg;                            /* リセットフラグ */
//uint16_t DLturn_duty;
int16_t DLturn_spd;

uint16_t blueCount;


/* 外部非公開関数 */
void line_jdg_color( uint16_t fin_color );                            /* 色認識までライントレース走行するフェイズ */
void DLrd_jdg_movement( uint16_t fin_movement, int16_t run_spd );     /* 指定距離までR・D走行するフェイズ */
void DLturn_jdg_deg( int16_t fin_deg );                              /* 指定角まで旋回するフェイズ */
void DLline_jdg_movement( uint16_t fin_movement );

/* ダブルループ初期化処理 */
void ini_DoubleLoop( void ){
    g_u16_DoubleLoop_phase = 1;
    DLreset_flg = 0;
}

/* ダブルループ周期処理 */
bool_t cyc_DoubleLoop( void ){
    bool_t bdat;

    bdat = 0;

    switch (g_u16_DoubleLoop_phase)
    {
    case 1:
        hub_display_text_scroll("LD", 50);
        DLline_jdg_movement( x_u16_DoubleLoop_line_movement_0 );
        break;
    case 2:
    case 6:
    case 10:
    case 13:
        hub_display_text_scroll("LC", 50);
        line_jdg_color( RESULT_BLUE );
        break;
    case 3:
        hub_display_text_scroll("RM", 50);
        DLrd_jdg_movement( x_u16_DoubleLoop_1_movement, x_u16_DoubleLoop_spd );
        break;
    case 4:
        // DLturn_jdg_deg( x_u16_DoubleLoop_deg1 );
        g_u16_DoubleLoop_phase += 1;
        break;
    case 5:
        hub_display_text_scroll("OK", 50);
        DLline_jdg_movement( x_u16_DoubleLoop_line_movement_1 );
        break;
    case 7:
        DLrd_jdg_movement( x_u16_DoubleLoop_2_movement, x_u16_DoubleLoop_spd );
        break;
    case 8:
        DLline_jdg_movement( x_u16_DoubleLoop_line_movement_2 );
        break;
    case 9:
        DLturn_jdg_deg( x_u16_DoubleLoop_deg2 );
        break;
    case 11:
        DLrd_jdg_movement( x_u16_DoubleLoop_3_movement, x_u16_DoubleLoop_spd );
        break;
    case 12:
        DLline_jdg_movement( x_u16_DoubleLoop_line_movement_3 );
        break;
    case 14:
        DLrd_jdg_movement( x_u16_DoubleLoop_4_movement, x_u16_DoubleLoop_spd );
        break;
    case 15:
        DLline_jdg_movement( x_u16_DoubleLoop_line_movement_4 );
        break;
    case 16:
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
    g_u16_linetrace_run_bsV = 200;

    color_result = get_rec_color();

    if( fin_color == color_result ){
        blueCount += 1;
        if( blueCount > 3 ){
            g_u16_DoubleLoop_phase += 1;
            DLreset_flg = 0;
            blueCount = 0;
        }
    } 
    else{
        blueCount = 0;
    }
}


/* 指定角まで旋回するフェイズ */
void DLturn_jdg_deg( int16_t fin_deg ){
    int16_t deg_result;

    if( 0 == DLreset_flg ){
        reset_cal_movement();
        DLreset_flg = 1;
        fin_deg *= g_u16_linetrace_run_edge;
        DLturn_spd = 200;
        //if( fin_deg < 0 ){
        //    DLturn_spd *= -1;
        //}
    }
    else{
        g_u16_ctl_main_mode = CONST_TURN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = DLturn_spd;

        deg_result = get_cal_movement_body_deg();        
        
        g_16_DLdeg = deg_result;
        g_u16_degfin = fin_deg;
        
        if( fin_deg > 0 ){
            //if(fin_deg <100){
                if( fin_deg < deg_result ){
                    g_u16_DoubleLoop_phase += 1;
                    DLreset_flg = 0;
                    g_u16_linetrace_run_edge *= -1;
                }
            //}
        }

        else{
            if( fin_deg > deg_result ){
                g_u16_DoubleLoop_phase += 1;
                DLreset_flg = 0;
                g_u16_linetrace_run_edge *= -1;
            }
        }

    }
}

/* 指定距離までライントレースするフェイズ */
void DLline_jdg_movement( uint16_t fin_movement ){
    uint16_t movement_result;

    if( 0 == DLreset_flg ){
        reset_cal_movement();
        DLreset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = LINETRACE_RUN;
        g_u16_linetrace_run_way = 1;
        g_u16_linetrace_run_bsV = 250;
        
        movement_result = get_cal_movement();

        g_u16_DLmove = movement_result;
        g_u16_movefin = fin_movement;

        if(movement_result < 30000 ){
            if( fin_movement < movement_result ){
                g_u16_DoubleLoop_phase += 1;
                DLreset_flg = 0;
            }
        }
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
        g_s16_const_run_spd = 250;
        g_s16_const_curve_rate = x_u16_DoubleLoop_curve_rate;
        
        movement_result = get_cal_movement();
        if( fin_movement < movement_result ){
            g_u16_DoubleLoop_phase += 1;
            DLreset_flg = 0;
            
            //if(g_u16_DoubleLoop_phase > 4){
            //    g_u16_linetrace_run_edge *= -1;     /* ライントレースの左右を反転*/
            //}
        }
    }
}
