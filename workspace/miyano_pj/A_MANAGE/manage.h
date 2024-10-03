/* 競技攻略 */
#define START_SET       0
#define DOUBLE_LOOP     1
#define DEBRI_REMOVE    2
#define SMART_CARRY     3

/* 外部公開変数 */
extern uint16_t g_u16_manage_cnt;           /* 難所カウント */

/* 外部公開関数 */
extern void ini_manage( void );             /* 競技攻略初期化処理 */
extern void cyc_manage( void );             /* 競技攻略周期処理 */
