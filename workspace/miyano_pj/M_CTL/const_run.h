/* 一定出力走行 */
#ifndef _CONST_H_
#define _CONST_H_

/* 外部公開関数 */
extern void ini_const_run( void );      /* 一定出力走行初期化 */
extern void cyc_const_run( void );      /* 一定出力走行周期処理 */
extern void cyc_const_turn( void );     /* 旋回処理 */

/* 外部公開変数 */
extern uint16_t g_u16_const_run_way;    /* 一定出力制御指令方法(0:DUTY 1:回転速度) */
extern uint16_t  g_s16_const_run_spd;    /* 設定速度 */
extern int16_t  g_s16_const_run_duty;   /* 設定DUTY */

#endif