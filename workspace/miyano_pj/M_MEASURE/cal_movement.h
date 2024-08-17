/* 移動量計測 */

/* 外部公開関数 */
extern void ini_cal_movement( void );               /* 移動量計測初期化 */
extern int16_t get_cal_movement( void );            /* 計測実施 */
extern int16_t get_cal_movement_body_deg( void );   /* 機体旋回角計測 */
extern uint16_t get_cal_movement_arm_deg( void );   /* アーム回転角取得 */
extern void reset_cal_movement( void );             /* 計測リセット */