/* スマートキャリー攻略 */
#define TURN_JDG_COLOR      0               /* 色認識まで旋回するフェイズ */
#define TURN_JDG_DEG        1               /* 指定角まで旋回するフェイズ */
#define RD_JDG_COLOR        2               /* 色認識までR・D走行するフェイズ */
#define RD_JDG_MOVEMENT     3               /* 指定距離までR・D走行するフェイズ */
#define LINE_JDG_OBJ        4               /* 物体付近までライントレースするフェイズ */
#define LINE_JDG_MOVEMENT   5               /* 指定距離までライントレースするフェイズ */
#define SMART_CARRY_END     6               /* 終わり */

/* 適合値 */
extern uint16_t x_u16_smart_carry_deg;      /* スマートキャリー指定角[deg] */

/* 外部公開変数 */
extern uint16_t g_u16_smart_carry_phase;    /* スマートキャリーフェイズカウント */

/* 外部公開関数 */
extern void ini_smart_carry( void );        /* スマートキャリー初期化処理 */
extern bool_t cyc_smart_carry( void );      /* スマートキャリー周期処理 */