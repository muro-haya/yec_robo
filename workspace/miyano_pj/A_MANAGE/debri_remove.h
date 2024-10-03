/* デブリリムーバル攻略 */

/*適合値*/
extern uint16_t pattern_nom;                      // パターン番号（0～4）
extern int16_t D_straight;                        // 直進量適合値_デブリボトル運搬[mm]
extern int16_t D_spd;                             // 直進速度適合値_デブリボトル運搬
extern int16_t R_straight;                        // 後退量適合値_デブリボトル運搬後[mm]
extern int16_t R_spd;                             // 後退速度適合値_デブリボトル運搬後
extern int16_t turn_pattern[5][5];                // 回転角適合値[deg]
extern int16_t chase_straight_pattern[5][5];      // 直進量適合値_ボトル判定迄[mm]
extern int16_t chase_straight_spd;                // 直進速度適合値_ボトル判定迄
extern int16_t last_turn_pattern[5][5];           // 回転角適合値_最終回転[deg]（攻略終了地点に向けた回転）
extern int16_t last_straight_pattern[5][5];       // 直進量適合値_最終直進[mm]（攻略終了地点に向けた直進）
extern int16_t last_straight_spd;                 // 直進速度適合値_最終直進
extern int16_t last_straight_color_search_spd;    // 直進速度適合値_最終直進色認識迄
extern int16_t assist_turn_pattern[5][5];         // 回転角適合値_ライントレース前の補正[deg]
extern int16_t dr_edge_side;                      // ライントレースのエッジ（左右判定）

/* 外部公開変数 */
extern uint16_t g_u16_debri_remove_phase;         /* デブリリムーバルフェイズカウント */
extern uint16_t g_u16_debri_count;                /* デブリボトルカウント */
extern uint16_t g_u16_danger_count;               /* デンジャーボトルカウント */
extern uint16_t g_u16_bottle_count;               /* 総ボトルカウント */
extern uint16_t deg;              

/* 外部公開関数 */
extern void ini_debri_remove( void );             /* デブリリムーバル初期化処理 */
extern bool_t cyc_debri_remove( void );           /* デブリリムーバル周期処理 */