/* スマートキャリー攻略 */

/* 適合値 */
extern uint16_t x_u16_smart_carry_3_distance;   /* キャリーボトル認識距離[mm] */
extern int16_t  x_s16_smart_carry_4_movement;   /* ティポジション通過移動量[mm] */
extern int16_t  x_s16_smart_carry_5_movement;   /* ディセイブルゾーン通過移動量[mm] */
extern int16_t  x_s16_smart_carry_6_movement;   /* ランディング移動量[mm] */
extern int16_t  x_s16_smart_carry_7_movement;   /* 後進移動量[mm] */
extern uint16_t x_u16_smart_carry_8_deg;        /* 指定旋回角[deg] */
extern int16_t  x_s16_smart_carry_4_spd;        /* ティポジション通過速速 */
extern int16_t  x_s16_smart_carry_6_spd;        /* ランディング速度 */
extern int16_t  x_s16_smart_carry_7_spd;        /* 後進速度 */
extern uint16_t x_u16_smart_carry_deg;          /* スマートキャリー指定角[deg] */

extern int16_t  x_s16_smart_carry_chase_movement;    /* カラーチェイス距離[mm] */

/* 外部公開変数 */
extern uint16_t g_u16_smart_carry_phase;        /* スマートキャリーフェイズカウント */
extern uint16_t g_u16_smart_carry_debug;        /* デバッグ */

/* 外部公開関数 */
extern void ini_smart_carry( void );            /* スマートキャリー初期化処理 */
extern bool_t cyc_smart_carry( void );          /* スマートキャリー周期処理 */