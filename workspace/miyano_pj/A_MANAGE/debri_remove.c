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
#include "../M_CTL/const_run.h"
#include "../M_CTL/arm_ctl.h"

#include "debri_remove.h"

#include "../D_DEVICE/comm.h"

//テスト用ボタン処理
#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/button.h"

/* 適合値 */

//左右切り替えの適合値も必要か　左右切り替えた場合、角度とライントレースだけ反対にすればよい（*-1か）
uint16_t pattern_nom = 4;                //パターン番号（0～4）

int16_t D_straight = 200;                //直進量適合値_デブリボトル運搬[mm]
int16_t D_spd = 30;                     //直進速度適合値_デブリボトル運搬
int16_t R_straight = 200;               //後退量適合値_デブリボトル運搬後[mm]
int16_t R_spd = -30;                    //後退速度適合値_デブリボトル運搬後

int16_t turn_pattern[5][5] = {           //回転角適合値[deg]
        {45, -45, 90, -90, 45},          //パターン0
        {90, -90, 45, -45, 90},          //パターン1
        {-45, 45, -90, 90, 45},          //パターン2
        {-90, 90, -45, 45, 90},          //パターン3
        {180, -180, 90, 90, 180},           //パターン4
        };

int16_t chase_straight_pattern[5][5] = { //直進量適合値_ボトル判定迄[mm]
        {100, 50, 150, 200, 300},        //パターン0
        {50, 100, 200, 150, 1000},       //パターン1
        {200, 150, 50, 100, 400},        //パターン2
        {150, 200, 100, 50, 300},        //パターン3
        {300, 300, 400, 500, 50},        //パターン4
        };

int16_t chase_straight_spd = 50;        //直進速度適合値_ボトル判定迄

int16_t last_turn_pattern[5][5] = {      //回転角適合値_最終回転[deg]（攻略終了地点に向けた回転）
        {0, 0, 45, 90, 135},                 //パターン0
        {0, 0, 90, 135, 45},                //パターン1
        {0, 0, 135, 45, 90},              //パターン2
        {0, 0, -45, -90, -135},              //パターン3
        {0, 0, -90, -135, 45},              //パターン4
        };

int16_t last_straight_pattern[5][5] = {  //直進量適合値_最終直進[mm]（攻略終了地点に向けた直進）
        {0, 0, 50, 100, 150},                 //パターン0
        {0, 0, 100, 150, 200},                //パターン1
        {0, 0, 150, 200, 250},              //パターン2
        {0, 0, 200, 250, 50},              //パターン3
        {0, 0, 250, 50, 1000},              //パターン4
        };

int16_t last_straight_spd = 75;                //直進速度適合値_最終直進
int16_t last_straight_color_search_spd = 30;   //直進速度適合値_最終直進色認識迄

int16_t assist_turn_pattern[5][5] = {    //回転角適合値_ライントレース前の補正[deg]
        {0, 0, 10, 15, 20},                 //パターン0
        {0, 0, 20, 15, 10},                //パターン1
        {0, 0, 30, 45, 90},              //パターン2
        {0, 0, 90, 45, 30},              //パターン3
        {0, 0, -30, -45, -90},              //パターン4
        };

int16_t dr_edge_side = 0;                //ライントレースのエッジ（左右判定）

int16_t color_karioki[4] = {0, 1, 0, 1}; //仮置き色判定用適合値（後に削除）

/* 外部公開変数 */
uint16_t g_u16_debri_remove_phase;       /* デブリリムーバルフェイズカウント */
uint16_t g_u16_debri_count;              /* デブリボトルカウント */
uint16_t g_u16_danger_count;             /* デンジャーボトルカウント */
uint16_t g_u16_bottle_count;             /* 総ボトルカウント */
//テスト用ボタン処理
uint16_t dr_button;
uint16_t dr_start_button;

/* 外部非公開変数 */
uint16_t dr_reset_flg;                      /* リセットフラグ */

/* 外部非公開関数 */
void dr_turn_jdg_deg( int16_t target_deg );                              /* 指定角まで旋回するフェイズ */
void dr_chase_jdg_movement( int16_t target_movement, int16_t run_spd );  /* ボトルに向かって直進するフェイズ */
void dr_jdg_color( void );                                               /* ボトルカウント＆ボトルの色判定フェイズ */
void dr_rd_jdg_movement( int16_t target_movement, int16_t run_spd );     /* 指定距離までR・D走行するフェイズ */
void dr_jdg_bottle_count( void );                                        /* 判定フェイズ */
void dr_rd_jdg_color( uint16_t target_color , int16_t run_spd );         /* 色認識までR・D走行するフェイズ */
void line_jdg_color( uint16_t target_color, uint16_t edge_side );        /* 色認識までライントレースするフェイズ */

/* デブリリムーバル初期化処理 */
void ini_debri_remove( void ){
    g_u16_debri_remove_phase = 99;  //要変更
    g_u16_debri_count = 0;
    g_u16_danger_count = 0;
    g_u16_bottle_count = 0;
    dr_reset_flg = 0;
    //テスト用ボタン処理
    dr_button = 0;
    dr_start_button = 0;
}

/* デブリリムーバル周期処理 */
bool_t cyc_debri_remove( void ){
    bool_t flg_dr_end;

    flg_dr_end = 0;
    g_u16_bottle_count = g_u16_debri_count + g_u16_danger_count;
    
    dr_button = get_button( BUTTON_BT );
    if( 1 == dr_button ){
        g_u16_debri_remove_phase = 99;
        set_drive_mtr_spd(0, 0);
    }

    switch (g_u16_debri_remove_phase)
    {
    case 0:  //ボトルの方に回転
        dr_turn_jdg_deg( turn_pattern[pattern_nom][g_u16_bottle_count] );
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
        dr_turn_jdg_deg( last_turn_pattern[pattern_nom][g_u16_bottle_count] );
        break;
    case 7:  //攻略終了地点に向けて直進
        dr_rd_jdg_movement( last_straight_pattern[pattern_nom][g_u16_bottle_count], last_straight_spd );
        break;
    case 8:  //黒ライン迄直進
        dr_rd_jdg_color( RESULT_BLACK, last_straight_color_search_spd );
        break;
    case 9:  //ライントレースのために回転
        dr_turn_jdg_deg( assist_turn_pattern[pattern_nom][g_u16_bottle_count] );
        break;
    case 10: //黄色検知迄ライントレース
        line_jdg_color( RESULT_YELLOW, dr_edge_side );
        break;
    case 11:
        flg_dr_end = 1;
        break;
    case 99: //テスト用ボタンフェイズ
        set_drive_mtr_spd(0, 0);
        g_u16_debri_count = 0;
        g_u16_danger_count = 0;
        g_u16_bottle_count = 0;
        dr_reset_flg = 0;
        dr_start_button = get_button( BUTTON_LEFT );
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
    else{
        g_u16_ctl_main_mode = CONST_TURN;
        g_u16_const_run_way = 1;
        /* 指定角度が正なら時計回り */
        if( target_deg >= 0 ){
            g_s16_const_run_spd = 100;
        }
        /* 指定角度が負なら反時計回り */
        else{
            g_s16_const_run_spd = -100;
        }

        deg_result = get_cal_movement_body_deg();
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
        g_u16_ctl_main_mode = CONST_RUN;  //変更要
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = run_spd;
        
        movement_result = get_cal_movement();
        if( target_movement < movement_result ){
            g_u16_debri_remove_phase += 1;
            dr_reset_flg = 0;
        }
    }
}

/* ボトルカウント＆ボトルの色判定フェイズ */
void dr_jdg_color( void ){
    uint16_t dr_color_result;
    uint16_t RED = 0;
    uint16_t BLUE = 1;

    dr_color_result = color_karioki[g_u16_bottle_count]; //☆要変更　get_front_color();  //正面の色を取得する機能欲しい 赤なら0, 青なら1を返したい
    if( RED == dr_color_result ){  //赤ならデンジャボトルのカウントを+1, フェイズを0に戻す
        g_u16_danger_count += 1;
        g_u16_debri_remove_phase = 0;
    }
    else if( BLUE == dr_color_result ){  //青ならデブリボトルのカウントを+1, フェイズを+1
        g_u16_debri_count += 1;
        g_u16_debri_remove_phase += 1;
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

    set_drive_mtr_spd(0, 0);
    g_u16_debri_remove_phase += 1;
/*

    g_u16_ctl_main_mode = CONST_RUN;
    g_u16_const_run_way = 1;
    g_s16_const_run_spd = run_spd;

    color_result = get_rec_color();
    if( target_color == color_result ){
        g_u16_debri_remove_phase += 1;
    }
*/
}

/* 色認識までライントレースするフェイズ */ //左右どうするか
void line_jdg_color( uint16_t target_color, uint16_t edge_side ){
    uint16_t dr_end_color_result;

    g_u16_ctl_main_mode = LINETRACE_RUN;
    g_u16_linetrace_run_way = 1;
    g_u16_linetrace_run_bsV = 100;
        
    dr_end_color_result = get_rec_color();
    if( target_color == dr_end_color_result ){
        g_u16_debri_remove_phase += 1;
    }
}
