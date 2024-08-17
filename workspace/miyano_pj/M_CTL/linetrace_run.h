/* ライントレース走行 */
#ifndef LINETRACE_H
#define LINETRACE_H

/* 外部参照関数 */
extern void ini_linetrace_run( void );      /* ライントレース走行初期化 */
extern void set_tgt_linetrace_run( void );  /* ライントレース目標値設定 */
extern void cyc_linetrace_run( void );      /* ライントレース走行周期処理 */

/* 外部参照変数 */
extern uint16_t g_u16_linetrace_run_way;               /* ライントレース制御指令方法(0:DUTY 1:回転速度) */
extern uint16_t g_u16_linetrace_run_bsV;               /* FB制御基本値[-] */
extern uint16_t g_u16_linetrace_run_fbTgt;             /* FB制御目標値[-] */
extern uint16_t g_u16_linetrace_run_fbPv;              /* FB制御現在値[-] */
extern int16_t  g_u16_linetrace_run_p;                 /* P項計算結果[0.1]*/
extern int16_t  g_u16_linetrace_run_i;                 /* I項計算結果[0.1]*/
extern int16_t  g_u16_linetrace_run_d;                 /* D項計算結果[0.1]*/
extern int16_t  g_u16_linetrace_run_kp;                /* P項ゲイン値[0.1]*/
extern int16_t  g_u16_linetrace_run_ki;                /* I項ゲイン値[0.01]*/
extern int16_t  g_u16_linetrace_run_kd;                /* D項ゲイン値[0.1]*/
extern int16_t  g_u16_linetrace_run_fbCmdv;            /* FB制御指令値[-] */

#endif