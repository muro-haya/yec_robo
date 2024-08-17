/* アーム制御 */

/* 外部公開関数 */
extern void cyc_arm_ctl( void );                /* アーム制御周期処理 */
extern void ini_arm_ctl( void );                /* アーム制御初期化 */

/* 外部公開変数 */
extern uint16_t g_u16_arm_ctl_way;              /* アーム制御方法(0:DUTY 1:回転速度) */
extern uint16_t g_s16_arm_ctl_spd;              /* アーム制御回転速度 */
extern uint16_t g_s16_arm_ctl_duty;             /* アーム制御duty */