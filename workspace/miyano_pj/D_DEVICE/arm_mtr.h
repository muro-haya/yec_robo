/* 外部参照関数 */
extern void ini_arm_mtr( void );                    /* アームモータ初期化 */
extern void set_arm_mtr_duty( uint16_t duty );      /* アームモータDUty設定 */
extern void set_arm_mtr_spd( uint16_t arm_spd );    /* アームモータ回転速度設定 */
extern void rst_arm_mtr_cnt( void );                /* アームモータエンコーダカウントリセット */
extern void get_arm_mtr_cnt( uint16_t* arm_cnt );   /* アームモータエンコーダカウント取得 */