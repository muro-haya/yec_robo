/* ボタン入力 */
#ifdef BUTTON_H

#endif
#define BUTTON_H

#include <spike/hub/system.h>
#include "spike/hub/button.h"

#define BUTTON_CENTER HUB_BUTTON_CENTER /* センターボタン */
#define BUTTON_RIGHT  HUB_BUTTON_RIGHT  /* 右ボタン */
#define BUTTON_LEFT   HUB_BUTTON_LEFT   /* 左ボタン */
#define BUTTON_BT     HUB_BUTTON_BT     /* ブルートゥースボタン */

/* 外部参照関数 */
void ini_button( void );                /* ボタン入力初期化処理 */
uint16_t get_button( uint16_t btn );    /* ボタン入力取得 */