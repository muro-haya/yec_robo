/* 機体制御管理*/
#ifndef CTL_H
#define CTL_H

#define LINETRACE_RUN   0               /* ライントレース走行 */
#define CONST_RUN       1               /* 一定出力走行 */
#define ARM_CTL         2               /* アーム動作 */

/* 外部参照関数 */
extern void ini_ctl_main( void );      /* 機体制御初期化 */
extern void cyc_ctl_main( void );      /* 機体制御周期処理 */

#endif
