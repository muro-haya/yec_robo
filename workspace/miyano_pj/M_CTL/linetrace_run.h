/* ライントレース走行 */
#ifndef LINETRACE_H
#define LINETRACE_H

/* 外部参照関数 */
extern void ini_linetrace_run( void );      /* ライントレース走行初期化 */
extern void set_tgt_linetrace_run( void );  /* ライントレース目標値設定 */
extern void cyc_linetrace_run( void );      /* ライントレース走行周期処理 */

#endif