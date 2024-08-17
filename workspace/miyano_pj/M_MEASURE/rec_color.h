/* 色認識 */

#define RESULT_RED      0b1                 /* 赤:1 */
#define RESULT_GREEN    0b10                /* 緑:2 */
#define RESULT_BLUE     0b100               /* 青:4 */
#define RESULT_YELLOW   0b1000              /* 黄:8 */
#define RESULT_WHITE    0b10000             /* 白:16 */
#define RESULT_BLACK    0b100000            /* 黒:32 */
#define RESULT_NONE     0b0                 /* なし:0 */

#define RED_RVALUE      700                 /* 赤色R値 */
#define RED_GVALUE      100                 /* 赤色G値 */
#define RED_BVALUE      100                 /* 赤色B値 */
#define GREEN_RVALUE    100                 /* 緑色R値 */
#define GREEN_GVALUE    700                 /* 緑色G値 */
#define GREEN_BVALUE    100                 /* 緑色B値 */
#define BLUE_RVALUE     100                 /* 青色R値 */
#define BLUE_GVALUE     100                 /* 青色G値 */
#define BLUE_BVALUE     700                 /* 青色B値 */
#define YELLOW_RVALUE   100                 /* 黄色R値 */
#define YELLOW_GVALUE   100                 /* 黄色G値 */
#define YELLOW_BVALUE   700                 /* 黄色B値 */
#define WHITE_RVALUE    900                 /* 白色R値 */
#define WHITE_GVALUE    900                 /* 白色G値 */
#define WHITE_BVALUE    900                 /* 白色B値 */
#define BLACK_RVALUE    100                 /* 黒色R値 */
#define BLACK_GVALUE    100                 /* 黒色G値 */
#define BLACK_BVALUE    100                 /* 黒色B値 */

#define COLOR_WIDTH     50                  /* 色幅 */

/* 外部公開関数 */
extern void ini_rec_color( void );         /* 色認識初期化処理 */
extern uint16_t get_rec_color( void );     /* 色認識 */

/* 外部公開関数 */
extern uint16_t g_u16_rec_color_define_color[36];   /* 色定義 */