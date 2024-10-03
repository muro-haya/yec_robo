/* 色認識 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "rec_color.h"

#include "D_DEVICE/color_snc.h"
#include "D_DEVICE/comm.h"

/* 外部非公開関数 */
uint16_t g16_colorRe;

/* 外部公開関数 */
uint16_t g_u16_rec_color_define_color[36] =   /* 色定義 */
    // { BLUE_RVALUE,   COLOR_WIDTH, BLUE_GVALUE,   COLOR_WIDTH, BLUE_BVALUE,   COLOR_WIDTH,
    //   RED_RVALUE,    COLOR_WIDTH, RED_GVALUE,    COLOR_WIDTH, RED_BVALUE,    COLOR_WIDTH,
    //   GREEN_RVALUE,  COLOR_WIDTH, GREEN_GVALUE,  COLOR_WIDTH, GREEN_BVALUE,  COLOR_WIDTH,
    { RED_RVALUE,    COLOR_WIDTH, RED_GVALUE,    COLOR_WIDTH, RED_BVALUE,    COLOR_WIDTH,
      GREEN_RVALUE,  COLOR_WIDTH, GREEN_GVALUE,  COLOR_WIDTH, GREEN_BVALUE,  COLOR_WIDTH,
      BLUE_RVALUE,   COLOR_WIDTH, BLUE_GVALUE,   COLOR_WIDTH, BLUE_BVALUE,   COLOR_WIDTH,
      YELLOW_RVALUE, COLOR_WIDTH, YELLOW_GVALUE, COLOR_WIDTH, YELLOW_BVALUE, COLOR_WIDTH,
      WHITE_RVALUE,  COLOR_WIDTH, WHITE_GVALUE,  COLOR_WIDTH, WHITE_BVALUE,  COLOR_WIDTH,
      BLACK_RVALUE,  COLOR_WIDTH, BLACK_GVALUE,  COLOR_WIDTH, BLACK_BVALUE,  COLOR_WIDTH };
    /* 赤{ R値,      R値幅,        G値,            G値幅,        B値,           B値幅,  */
    /* 緑  R値,      R値幅,        G値,            G値幅,        B値,           B値幅,  */
    /* 青  R値,      R値幅,        G値,            G値幅,        B値,           B値幅,  */
    /* 黄  R値,      R値幅,        G値,            G値幅,        B値,           B値幅,  */
    /* 白  R値,      R値幅,        G値,            G値幅,        B値,           B値幅,  */
    /* 黒  R値,      R値幅,        G値,            G値幅,        B値,           B値幅 } */

/* 色認識初期化処理 */
void ini_rec_color( void ){

}

/* 色認識 */
uint16_t get_rec_color( void ){
    uint16_t u16_dat1;
    uint16_t u16_dat2;
    uint16_t u16_get_color[6];
    uint16_t u16_compare_data_max;
    uint16_t u16_compare_data_min;
    uint16_t u16_cnt;
    uint16_t u16_color;

    u16_cnt   = 0;
    u16_color = 0;

    get_color_rgb( &u16_get_color[0], &u16_get_color[2], &u16_get_color[4] );

    for( u16_dat1=0; u16_dat1<6; u16_dat1++ ){
    // for( u16_dat1=0; u16_dat1<1; u16_dat1++ ){
        for( u16_dat2=0; u16_dat2<5; u16_dat2+=2 ){
            u16_compare_data_max = g_u16_rec_color_define_color[6*u16_dat1+u16_dat2] + g_u16_rec_color_define_color[6*u16_dat1+u16_dat2+1];
            u16_compare_data_min = g_u16_rec_color_define_color[6*u16_dat1+u16_dat2] - g_u16_rec_color_define_color[6*u16_dat1+u16_dat2+1];

            if( u16_compare_data_max > u16_get_color[u16_dat2] ){
                if( u16_compare_data_min < u16_get_color[u16_dat2] ){
                    u16_cnt += 1;
                }
            }
        }
        if( 3 == u16_cnt ){                 /* R,G,B値が範囲内なら */
            u16_color += (1 << u16_dat1);   /* ビットシフトして足す */
        }
        u16_cnt = 0;
    }
    g16_colorRe = u16_color;
    return u16_color;
}

