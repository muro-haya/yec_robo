/* 機体制御管理*/
#ifndef CTL_H
#define CTL_H

#define LINETRACE_RUN   0               /* ライントレース走行 */
#define CONST_RUN       1               /* 一定出力走行 */
#define CONST_TURN      2               /* 一定旋回 */
#define ARM_CTL         3               /* アーム動作 */
#define COLOR_CHASE     4               /* カラーチェイス */

/* 外部参照関数 */
extern void ini_ctl_main( void );       /* 機体制御初期化 */
extern void cyc_ctl_main( void );       /* 機体制御周期処理 */

/* 外部公開変数 */
extern uint16_t g_u16_ctl_main_mode;    /* 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ 3:カラーチェイス) */

#endif