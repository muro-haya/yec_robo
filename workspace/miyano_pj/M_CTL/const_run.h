/* 一定出力走行 */
#ifndef CONST_H
#define CONST_H

/* 外部参照関数 */
extern void ini_const_run( void );      /* 一定出力走行初期化 */
extern void cyc_const_run( void );      /* 一定出力走行周期処理 */

/* 外部参照変数 */
uint16_t g_u16_const_run_way;    /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
uint16_t g_u16_const_run_spd;    /* 設定速度 */
uint16_t g_u16_const_run_duty;   /* 設定DUTY */

#endif