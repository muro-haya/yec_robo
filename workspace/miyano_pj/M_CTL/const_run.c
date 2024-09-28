/* 一定出力走行 */

#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "spike/hub/speaker.h"  /* debug */

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "const_run.h"

#include "../D_DEVICE/drive_mtr.h"
#include "../D_DEVICE/button.h"

/* 外部公開変数 */
uint16_t g_u16_const_run_way;           /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
int16_t  g_s16_const_run_spd;           /* 設定速度 */
int16_t  g_s16_const_run_duty;          /* 設定DUTY */
int16_t  button_state;
int16_t  button_state_prev;
int16_t  button_state_flag;


/* 一定出力走行初期化 */
void ini_const_run( void ){
    g_u16_const_run_way  = 1;           /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
    g_s16_const_run_spd  = 720;           /* 設定速度 */
    g_s16_const_run_duty = 32;           /* 設定DUTY */
    button_state = 0;
    button_state_prev = 0;
    button_state_flag = 0;
}

/* R・D走行周期処理 */
void cyc_const_run( void ){
       
    button_state = get_button(BUTTON_BT);

    if(button_state != button_state_prev)
    {
        button_state_flag = 1;
    }

    if(button_state_flag == 0)
    {
        set_drive_mtr_duty(0, 0);
    }
    else{
        /* モータ駆動指示 */
        if( 0 == g_u16_const_run_way ){     /* DUTY指示 */
            set_drive_mtr_duty(g_s16_const_run_duty, g_s16_const_run_duty);
        }
        else{                               /* 回転速度指示 */
            set_drive_mtr_spd(g_s16_const_run_spd, g_s16_const_run_spd);
        }
    }
    
    button_state_prev = button_state; 

}

/* 旋回処理 */
void cyc_const_turn( void ){
    /* モータ駆動指示 */
    if( 0 == g_u16_const_run_way ){     /* DUTY指示 */
        set_drive_mtr_duty(g_s16_const_run_duty, -g_s16_const_run_duty);
    }
    else{                               /* 回転速度指示 */
        set_drive_mtr_spd(g_s16_const_run_spd, -g_s16_const_run_spd);
    }
}