/* スマートキャリー攻略 */

/* 適合値 */
extern uint16_t x_u16_smart_carry_deg;      /* スマートキャリー指定角[deg] */

/* 外部公開変数 */
extern uint16_t g_u16_smart_carry_phase;    /* スマートキャリーフェイズカウント */

/* 外部公開関数 */
extern void ini_smart_carry( void );        /* スマートキャリー初期化処理 */
extern bool_t cyc_smart_carry( void );      /* スマートキャリー周期処理 */