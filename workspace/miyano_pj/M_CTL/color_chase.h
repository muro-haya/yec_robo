/* カラーチェイス */
/* 外部参照関数 */
extern void ini_color_chase( void );        /* カラーチェイス 初期化処理 */
extern void cyc_color_chase( void );        /* カラーチェイス 周期処理 */

/* 外部参照変数 */
/* 外部参照変数 */
extern uint16_t g_u16_color_chase_way;               /* ライントレース制御指令方法(0:DUTY 1:回転速度) */
extern uint16_t g_u16_color_chase_bsV;               /* FB制御基本値[-] */
extern uint16_t g_u16_color_chase_fbTgt;             /* FB制御目標値[-] */
extern uint16_t g_u16_color_chase_fbPv;              /* FB制御現在値[-] */
extern int16_t  g_s16_color_chase_p;                 /* P項計算結果[0.1]*/
extern int16_t  g_s16_color_chase_i;                 /* I項計算結果[0.1]*/
extern int16_t  g_s16_color_chase_d;                 /* D項計算結果[0.1]*/
extern int16_t  g_s16_color_chase_fbCmdv;            /* FB制御指令値[-] */

extern int16_t  g_s16_color_chase_debug;

/* 適合値 */
extern int16_t  x_u16_color_chase_kp;                 /* P項ゲイン値[0.01]*/
extern int16_t  x_u16_color_chase_ki;                 /* I項ゲイン値[0.01]*/
extern int16_t  x_u16_color_chase_kd;                 /* D項ゲイン値[0.01]*/