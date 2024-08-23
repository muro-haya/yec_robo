2024年 ロボコン出場用ソフト
M_CTL
/* 以下ctl_main.hを読み込んでおけば使える */
/* 機体制御全般 */
 g_u16_ctl_main_mode;                  : 機体制御モード(0:ライントレース 1:一定出力走行 2:アームモータ)
/* ライントレース用 */
 g_u16_linetrace_run_way;              : ライントレース制御指令方法(0:DUTY 1:回転速度)
 g_u16_linetrace_run_bsV;              : FB制御基本値[-]
 /* 一定出力走行用 */
 g_u16_const_run_way;                  : 一定出力制御指令方法(0:DUTY 1:回転速度)
 g_s16_const_run_spd;                  : 設定速度
 g_s16_const_run_duty;                 : 設定DUTY
 /* アームモータ用 */
 g_u16_arm_ctl_way;                    : アーム制御方法(0:DUTY 1:回転速度)
 g_s16_arm_ctl_spd;                    : アーム制御回転速度
 g_s16_arm_ctl_duty;                   : アーム制御duty

M_MEASURE
 /* 色の認識 rec_color.h */
 get_rec_color( void );                 : 下面の色認識 　　　　　　　　　　戻り値で以下の色情報が入ってる
 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　RESULT_RED     赤:1 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　                              RESULT_GREEN   緑:2
    　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 RESULT_BLUE    青:4
    　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 RESULT_YELLOW  黄:8
    　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 RESULT_WHITE   白:16
   　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　RESULT_BLACK   黒:32
   　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　RESULT_NONE    なし:0
  /* 移動量の計測 cal_movement.h */
  get_cal_movement( void );            : 走行距離を計測する　　　　　　　　　戻り値で走行距離[mm]が返ってくる
  get_cal_movement_body_deg( void );   : 旋回角を計測する　　　　　　　　　　戻り値で旋回角[deg]が返ってくる
  get_cal_movement_arm_deg( void );    : アーム回転角取得　　　　　　　　　　戻り値でアームの角度[deg]が返ってくる
  reset_cal_movement( void );          : エンコーダカウント値をリセットする　戻り値なし
  /* 物体との距離計測 cal_distance.h */
　get_cal_distance( void );      　　　 : 物体との距離を算出する 　　　　　　　戻り値で距離[mm]が返ってくる
