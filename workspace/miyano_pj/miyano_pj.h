#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>

/*
 *  タスクの優先度の定義
 */
#define MAIN_PRIORITY	5		/* メインタスクの優先度 */

#define HIGH_PRIORITY	9		/* 並行実行されるタスクの優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void Main(intptr_t exinf);
extern void Main_2m( intptr_t unused );
extern void Main_10m( intptr_t unused );
extern void Main_100m( intptr_t unused );
extern void Tx_1m( intptr_t unused );
extern void Rx_1m( intptr_t unused );

#endif /* TOPPERS_MACRO_ONLY */
