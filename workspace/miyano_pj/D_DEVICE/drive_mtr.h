/* 外部参照関数 */
extern void ini_drive_mtr( void );                                              /* 駆動モータ初期化 */
extern void set_drive_mtr_duty( int16_t dutyL, int16_t dutyR );                 /* 駆動モータDUTY設定 */
extern void set_drive_mtr_spd( int16_t spdL, int16_t spdR );                    /* 駆動モータ回転速度設定 */
extern void rst_drive_mtr_cnt( void );                                          /* 駆動モータエンコーダカウントリセット */
<<<<<<< Updated upstream
extern void get_drive_mtr_cnt( int16_t* drive_cntL, int16_t* drive_cntR );      /* 駆動モータエンコーダカウント取得 */
=======
extern void get_drive_mtr_cnt( int16_t* drive_cntL, int16_t* drive_cntR );      /* 駆動モータエンコーダカウント取得 */

/* 外部公開変数 */
extern int16_t g_s16_drive_mtr_spdL;
extern int16_t g_s16_drive_mtr_spdR;

/*  適合値*/
extern uint16_t x_u16_drive_mtr_spdL_namashi;
extern uint16_t x_u16_drive_mtr_spdR_namashi;
extern uint16_t x_u16_drive_mtr_rpmL_kp;
extern uint16_t x_u16_drive_mtr_rpmL_ki;
extern uint16_t x_u16_drive_mtr_rpmL_kd;
extern uint16_t x_u16_drive_mtr_rpmR_kp;
extern uint16_t x_u16_drive_mtr_rpmR_ki;
extern uint16_t x_u16_drive_mtr_rpmR_kd;

/*ロギング用にglobalで定義*/
extern int16_t s16_drive_cntL;
extern int16_t s16_drive_cntR;
extern int16_t s16_drive_rpmL;
extern int16_t s16_drive_rpmR;
extern int16_t s16_drive_rpmL_dlt;
extern int16_t s16_drive_rpmR_dlt;
extern int16_t s16_drive_rpmL_p;
extern int16_t s16_drive_rpmL_i;
extern int16_t s16_drive_rpmL_d;
extern int16_t s16_drive_rpmR_p;
extern int16_t s16_drive_rpmL_i;
extern int16_t s16_drive_rpmR_d;
extern int16_t s16_drive_dltL;
extern int16_t s16_drive_dltR;
extern int16_t s16_drive_dltbufL;
extern uint16_t u16_drive_cntL_buf_index;
>>>>>>> Stashed changes
