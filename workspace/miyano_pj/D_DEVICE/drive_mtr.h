/* 外部参照関数 */
extern void ini_drive_mtr( void );                                              /* 駆動モータ初期化 */
extern void set_drive_mtr_duty( uint16_t dutyL, uint16_t dutyR );               /* 駆動モータDUTY設定 */
extern void set_drive_mtr_spd( uint16_t spdL, uint16_t spdR );                  /* 駆動モータ回転速度設定 */
extern void rst_drive_mtr_cnt( void );                                          /* 駆動モータエンコーダカウントリセット */
extern void get_drive_mtr_cnt( uint16_t* drive_cntL, uint16_t* drive_cntR );    /* 駆動モータエンコーダカウント取得 */