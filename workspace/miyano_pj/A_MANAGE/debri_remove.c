/* デブリリムーバル攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../M_CTL/ctl_main.h"
#include "../M_MEASURE/cal_distance.h"
#include "../M_MEASURE/cal_movement.h"
#include "../M_MEASURE/rec_color.h"
#include "../M_CTL/linetrace_run.h"
#include "../M_CTL/color_chase.h"
#include "../M_CTL/const_run.h"
#include "../M_CTL/arm_ctl.h"

#include "debri_remove.h"

#include "../D_DEVICE/comm.h"

//テスト用ボタン処理
#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/button.h"

/* 適合値 */

//左右切り替えの適合値も必要か　左右切り替えた場合、角度とライントレースだけ反対にすればよい（*-1か）
uint16_t pattern_nom = 1;               //パターン番号（0～4）
int16_t layout = 1;                     //レイアウト変更（L:1, R:-1）

int16_t D_straight = 200;               //直進量適合値_デブリボトル運搬[mm]
int16_t D_spd = 200;                    //直進速度適合値_デブリボトル運搬
int16_t R_straight = 200;               //後退量適合値_デブリボトル運搬後[mm]
int16_t R_spd = -200;                   //後退速度適合値_デブリボトル運搬後

int16_t turn_pattern[5][5] = {           //回転角適合値[deg]
        {35, 90, -135, -60, 0},          //2パターン0
        {75, -165, 150, -75, 0},         //2パターン1
        {0, 110, -175, 90, 0},           //2パターン2
        {0, 80, -140, 90, 0},            //2パターン3
        {0, 130, -90, -80, 0},           //パターン4
        };

int16_t chase_straight_pattern[5][5] = { //直進量適合値_ボトル判定迄[mm]
        {424, 424, 600, 671, 0},         //2パターン0
        {870, 750, 530, 400, 0},         //2パターン1
        {370, 970, 580, 360, 0},         //2パターン2
        {5, 950, 580, 690, 0},           //2パターン3
        {650, 400, 250, 880, 0},         //パターン4
        };

int16_t chase_straight_spd = 250;        //直進速度適合値_ボトル判定迄

int16_t last_turn_pattern[5][5] = {      //回転角適合値_最終回転[deg]（攻略終了地点に向けた回転）
        {0, 0, -120, 50, 140},           //2パターン0
        {0, 0, 135, -15, 115},           //2パターン1
        {0, 0, -115, 115, 45},           //2パターン2
        {0, 0, -80, 115, 75},            //2パターン3
        {0, 0, -105, -20, 115},          //パターン4
        };

int16_t last_straight_pattern[5][5] = {  //直進量適合値_最終直進[mm]（攻略終了地点に向けた直進）
        {0, 0, 850, 450, 900},           //2パターン0
        {0, 0, 1200, 650, 630},          //2パターン1
        {0, 0, 950, 800, 350},           //2パターン2
        {0, 0, 950, 800, 250},           //2パターン3
        {0, 0, 820, 550, 700},           //パターン4
        };

int16_t last_straight_spd = 300;                //直進速度適合値_最終直進
int16_t last_straight_color_search_spd = 250;   //直進速度適合値_最終直進色認識迄

int16_t assist_turn_pattern[5][5] = {    //回転角適合値_ライントレース前の補正[deg]
        {0, 0, -5, -10, -90},            //2パターン0
        {0, 0, 30, -20, -50},            //2パターン1
        {0, 0, 90, -30, -45},            //2パターン2
        {0, 0, 90, -25, -90},            //2パターン3
        {0, 0, 60, 75, -90},             //パターン4
        };

int16_t dr_edge_side[5][5] = {           //ライントレースのエッジ（左右判定）
        {0, 0, 1, 1, 1},                 //パターン0
        {0, 0, -1, 1, 1},                //パターン1
        {0, 0, -1, 1, 1},                //パターン2
        {0, 0, -1, 1, 1},                //パターン3
        {0, 0, -1, 1, 1},                //パターン4
        };

int16_t dr_go_circle_center = 50;        //直進量適合値_黄色円中心迄直進[mm]

int16_t dr_end_turn_pattern[5][5] = {    //回転角適合値_次の難所方向に向くための回転[deg]
        {0, 0, 90, 90, 90},              //パターン0
        {0, 0, 0, 90, 90},               //パターン1
        {0, 0, 0, 90, 90},               //パターン2
        {0, 0, 0, 90, 90},               //パターン3
        {0, 0, 0, 90, 90},               //パターン4
        };

//int16_t color_karioki[4] = {1, 2, 1, 2}; //仮置き色判定用適合値（後に削除）

/* 外部公開変数 */
uint16_t g_u16_debri_remove_phase;       /* デブリリムーバルフェイズカウント */
uint16_t g_u16_debri_count;              /* デブリボトルカウント */
uint16_t g_u16_danger_count;             /* デンジャーボトルカウント */
uint16_t g_u16_bottle_count;             /* 総ボトルカウント */
uint16_t dr_color_result;                /* 前面の色（赤:1, 青:2, 初期値:0） */
//uint16_t deg;
//テスト用ボタン処理
uint16_t dr_button;
uint16_t dr_start_button;

/* 外部非公開変数 */
uint16_t dr_reset_flg;                      /* リセットフラグ */
int16_t turndeg_0;
int16_t turndeg_6;
int16_t turndeg_9;
int16_t edge_10;
int16_t turndeg_12;

/* 外部非公開関数 */
void dr_turn_jdg_deg( int16_t target_deg );                              /* 指定角まで旋回するフェイズ */
void dr_chase_jdg_movement( int16_t target_movement, int16_t run_spd );  /* ボトルに向かって直進するフェイズ */
void dr_jdg_color( void );                                               /* ボトルカウント＆ボトルの色判定フェイズ */
void dr_rd_jdg_movement( int16_t target_movement, int16_t run_spd );     /* 指定距離迄R・D走行するフェイズ */
void dr_jdg_bottle_count( void );                                        /* 判定フェイズ */
void dr_rd_jdg_color( uint16_t target_color , int16_t run_spd );         /* 色認識迄R・D走行するフェイズ */
void dr_line_jdg_color( uint16_t target_color, uint16_t edge_side );     /* 色認識迄ライントレースするフェイズ */

/* デブリリムーバル初期化処理 */
void ini_debri_remove( void ){
    g_u16_debri_remove_phase = 99;  //要変更
    g_u16_debri_count = 0;
    g_u16_danger_count = 0;
    g_u16_bottle_count = 0;
    dr_reset_flg = 0;
    dr_color_result = 0;
    //テスト用ボタン処理
    dr_button = 0;
    dr_start_button = 0;
}

/* デブリリムーバル周期処理 */
bool_t cyc_debri_remove( void ){
    bool_t flg_dr_end;

    flg_dr_end = 0;
    g_u16_bottle_count = g_u16_debri_count + g_u16_danger_count;
    
    dr_button = get_button( BUTTON_LEFT );
    if( 1 == dr_button ){
        g_u16_debri_remove_phase = 99;
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 0;
    }

    switch (g_u16_debri_remove_phase)
    {
    case 0:  //ボトルの方に回転
        turndeg_0 = layout * turn_pattern[pattern_nom][g_u16_bottle_count];
        dr_turn_jdg_deg( turndeg_0 );
        turndeg_0 = 0;
        break;
    case 1:  //ボトル迄直進
        dr_chase_jdg_movement( chase_straight_pattern[pattern_nom][g_u16_bottle_count], chase_straight_spd );
        break;    
    case 2:  //ボトルカウント＆ボトル色判定（青ならcase+1, 赤ならcase=0）
        dr_jdg_color();
        break;
    case 3:  //デブリボトル運搬（直進）
        dr_rd_jdg_movement( D_straight, D_spd );
        break;
    case 4:  //デブリボトル運搬（後退）
        dr_rd_jdg_movement( R_straight, R_spd );
        break;
    case 5:  //判定（デブリボトル2以上ならcase+1, デブリボトル2未満ならcase=0）
        dr_jdg_bottle_count();
        break;
    case 6:  //攻略終了地点に向けて回転
        turndeg_6 = layout * last_turn_pattern[pattern_nom][g_u16_bottle_count];
        dr_turn_jdg_deg( turndeg_6 );
        turndeg_6 = 0;
        break;
    case 7:  //攻略終了地点に向けて直進
        dr_rd_jdg_movement( last_straight_pattern[pattern_nom][g_u16_bottle_count], last_straight_spd );
        break;
    case 8:  //黒ライン迄直進
        dr_rd_jdg_color( RESULT_BLACK, last_straight_color_search_spd );
        break;
    case 9:  //ライントレースのために回転
        turndeg_9 = layout * assist_turn_pattern[pattern_nom][g_u16_bottle_count];
        dr_turn_jdg_deg( assist_turn_pattern[pattern_nom][g_u16_bottle_count] );
        turndeg_9 = 0;
        break;
    case 10: //黄色検知迄ライントレース
        edge_10 = layout * dr_edge_side[pattern_nom][g_u16_bottle_count];
        dr_line_jdg_color( RESULT_YELLOW, edge_10 );
        edge_10 = 0;
        break;
    case 11: //黄色円中心迄直進
        dr_rd_jdg_movement( dr_go_circle_center, D_spd );
        break;
    case 12: //次の難所方向に向くための回転
        turndeg_12 = layout * dr_end_turn_pattern[pattern_nom][g_u16_bottle_count];
        dr_turn_jdg_deg( dr_end_turn_pattern[pattern_nom][g_u16_bottle_count] );
        turndeg_12 = 0;
        break;
    case 13:
        flg_dr_end = 1;
        break;
    case 99: //テスト用ボタンフェイズ
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 0;
        //set_drive_mtr_spd(0, 0);
        g_u16_debri_count = 0;
        g_u16_danger_count = 0;
        g_u16_bottle_count = 0;
        dr_reset_flg = 0;
        dr_start_button = get_button( BUTTON_CENTER );
        if( 1 == dr_start_button ){
            g_u16_debri_remove_phase = 0;
        }  
    
    default:
        break;
    }

    return flg_dr_end;
}

/* 指定角まで旋回するフェイズ */
void dr_turn_jdg_deg( int16_t target_deg ){
    int16_t deg_result;

    if( 0 == dr_reset_flg ){
        reset_cal_movement();
        dr_reset_flg = 1;
    }
    else if( 0 == target_deg ){
        g_u16_debri_remove_phase += 1;
        dr_reset_flg = 0;
    }
    else{
        g_u16_ctl_main_mode = CONST_TURN;
        g_u16_const_run_way = 1;
        /* 指定角度が正なら時計回り */
        if( target_deg > 0 ){
            g_s16_const_run_spd = 150;
        }
        /* 指定角度が負なら反時計回り */
        else{
            g_s16_const_run_spd = -150;
        }

        deg_result = get_cal_movement_body_deg();
        //deg = deg_result;
        if( abs(target_deg) < abs(deg_result) ){
            g_u16_debri_remove_phase += 1;
            dr_reset_flg = 0;
        }
    }
}

/* ボトルに向かって直進するフェイズ */
void dr_chase_jdg_movement( int16_t target_movement, int16_t run_spd ){
    int16_t movement_result;

    if( 0 == dr_reset_flg ){
        reset_cal_movement();
        dr_reset_flg = 1;
    }
    else{
        //g_u16_ctl_main_mode = CONST_RUN;  //変更要
        //g_u16_const_run_way = 1;
        //g_s16_const_run_spd = run_spd;

        g_u16_ctl_main_mode = COLOR_CHASE;
        g_u16_color_chase_way = 1;
        g_u16_color_chase_bsV = run_spd;
        g_u16_comm_rx_pet_srt = 1;
        
        movement_result = get_cal_movement();
        if( g_u16_comm_rx_jdg_pet != 0 ){
            dr_color_result = g_u16_comm_rx_jdg_pet; //color_karioki[g_u16_bottle_count]; //☆要変更
        }

        if( target_movement < movement_result ){
            g_u16_debri_remove_phase += 1;
            dr_reset_flg = 0;
            g_u16_comm_rx_pet_srt = 0;
            g_u16_ctl_main_mode = CONST_RUN;
            g_u16_const_run_way = 1;
            g_s16_const_run_spd = 0;
        }
    }
}

/* ボトルカウント＆ボトルの色判定フェイズ */
void dr_jdg_color( void ){
    uint16_t RED = 1;
    uint16_t BLUE = 2;

    if( RED == dr_color_result ){  //赤ならデンジャボトルのカウントを+1, フェイズを0に戻す
        g_u16_danger_count += 1;
        g_u16_debri_remove_phase = 0;
        dr_color_result = 0;
    }
    else if( BLUE == dr_color_result ){  //青ならデブリボトルのカウントを+1, フェイズを+1
        g_u16_debri_count += 1;
        g_u16_debri_remove_phase += 1;
        dr_color_result = 0;
    }
}

/* 指定距離までR・D走行するフェイズ */
void dr_rd_jdg_movement( int16_t target_movement, int16_t run_spd ){
    int16_t movement_result;

    if( 0 == dr_reset_flg ){
        reset_cal_movement();
        dr_reset_flg = 1;
    }
    else{
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = run_spd;
        
        movement_result = get_cal_movement();
        if( abs(target_movement) < abs(movement_result) ){
            g_u16_debri_remove_phase += 1;
            dr_reset_flg = 0;
        }
    }
}

/* 判定フェイズ */
void dr_jdg_bottle_count( void ){
    

    if (g_u16_debri_count >= 2){  //デブリボトルのカウントが2以上ならフェイズを+1
        g_u16_debri_remove_phase += 1;
    }
    else{                         //デブリボトルのカウントが2未満ならフェイズを0に戻す
        g_u16_debri_remove_phase = 0;
    }
}

/* 色認識までR・D走行するフェイズ */
void dr_rd_jdg_color( uint16_t target_color , int16_t run_spd ){
    uint16_t color_result;

//    set_drive_mtr_spd(0, 0);
//    g_u16_debri_remove_phase += 1;


    g_u16_ctl_main_mode = CONST_RUN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = run_spd;

    color_result = get_rec_color();
    if( target_color == color_result ){
        g_u16_debri_remove_phase += 1;
    }
}

/* 色認識までライントレースするフェイズ */ //左右どうするか
void dr_line_jdg_color( uint16_t target_color, uint16_t edge_side ){
    uint16_t dr_end_color_result;

    //g_u16_ctl_main_mode = CONST_RUN;
    //g_u16_const_run_way = 1;
    //g_s16_const_run_spd = 0;

    g_u16_ctl_main_mode = LINETRACE_RUN;
    g_u16_linetrace_run_way = 1;
    g_u16_linetrace_run_bsV = 100;
    g_u16_linetrace_run_edge = edge_side;
    
    dr_end_color_result = get_rec_color();
    if( target_color == dr_end_color_result ){
        g_u16_debri_remove_phase += 1;
    }
}
