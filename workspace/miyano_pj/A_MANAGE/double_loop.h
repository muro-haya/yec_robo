/* ダブルループ攻略 */

/* 適合値 */
extern uint16_t x_u16_DoubleLoop_1_movement;         /* 分岐1での移動量[mm] */
extern uint16_t x_u16_DoubleLoop_2_movement;         /* 分岐2での移動量[mm] */
extern uint16_t x_u16_DoubleLoop_3_movement;         /* 分岐3での移動量[mm] */
extern uint16_t x_u16_DoubleLoop_4_movement;         /* 分岐4での移動量[mm] */
extern uint16_t x_u16_DoubleLoop_spd;                /* 各分岐での通過速度 */
extern uint16_t x_u16_DoubleLoop_deg;                /* 指定旋回角[deg] */

/* 外部公開変数 */
extern uint16_t g_u16_DoubleLoop_phase;        /* ダブルループフェイズカウント */

extern uint16_t g_u16_DLmove;
extern uint16_t g_u16_movefin;
extern uint16_t g_16_DLdeg;
extern uint16_t g_u16_degfin;



/* 外部公開関数 */
extern void ini_DoubleLoop( void );            /* ダブルループ初期化処理 */
extern bool_t cyc_DoubleLoop( void );          /* ダブルループ周期処理 */