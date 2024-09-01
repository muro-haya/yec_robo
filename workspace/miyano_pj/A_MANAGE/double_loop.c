/* ダブルループ攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "double_loop.h"

/* 適合値 */
uint16_t x_u16_DoubleLoop_ = 50;
uint16_t x_u16_DoubleLoop_distance = 50;         /* キャリーボトル認識距離[mm] */

/* 外部公開変数 */
uint16_t g_u16_DoubleLoop_phase;                   /* スマートキャリーフェイズカウント */

/* 外部非公開変数 */

/* 外部非公開関数 */
void line_jdg_color( uint16_t fin_color );          /* 色認識までライントレース走行するフェイズ */