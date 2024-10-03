/* 外部参照関数 */
extern void ini_color_snc( void );                                                      /* カラーセンサ初期化 */
extern void cyc_get_color_rgb( void );                                                  /* RGBセンサ値取得 */
extern void get_color_rgb( uint16_t* r_valu, uint16_t* g_valu, uint16_t* b_valu );      /* RGB値取得 */
extern void get_color_ref( uint16_t* ref_valu );                                        /* 反射光取得 */

extern uint16_t g16_snc_r;
extern uint16_t g16_snc_g;
extern uint16_t g16_snc_b;