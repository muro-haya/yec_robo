/* スマートキャリー攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "spike/hub/display.h"
#include "pbio/light_matrix.h"

#include "../M_MEASURE/cal_distance.h"
#include "../M_CTL/ctl_main.h"
#include "../M_CTL/linetrace_run.h"
#include "../M_CTL/const_run.h"
#include "../D_DEVICE/button.h"
#include "../D_DEVICE/comm.h"
#include "../D_DEVICE/color_snc.h"
#include "../D_DEVICE/sonic_snc.h"

#include "start_set.h"

#include "../D_DEVICE/comm.h"

/* 適合値 */

/* 外部公開変数 */
uint16_t g_u16_start_set_distance;

/* 外部非公開変数 */

/* 外部非公開関数 */
static uint16_t start_set_distance_flg;

/* 開始時設定初期化処理 */
void ini_start_set( void ){
    g_u16_start_set_distance = 0;
    start_set_distance_flg = 0;
}

/* 開始時設定周期処理 */
bool_t cyc_start_set( void ){
    uint16_t btn;
    uint16_t distance;

    btn = get_button(BUTTON_BT);
    if( 1 == btn ){
        if( 0 == comm_reset_flg ){
            comm_reset_flg = 0;
        }
        else{
            comm_reset_flg = 1;
        }
    }

    btn = get_button( BUTTON_LEFT );
    if( 1 == btn ){
        get_color_rgb(0, &g_u16_linetrace_run_lpos, 0);
    }
    btn = get_button( BUTTON_RIGHT );
    if( 1 == btn ){
        get_color_rgb(0, &g_u16_linetrace_run_rpos, 0);
    }
    //g_u16_linetrace_run_fbTgt = ( g_u16_linetrace_run_lpos + g_u16_linetrace_run_rpos ) / 2;
    g_u16_linetrace_run_fbTgt = ( g_u16_linetrace_run_lpos*3 + g_u16_linetrace_run_rpos*1 ) / 5;
    
    btn = get_button( BUTTON_CENTER );
    if( 1 == btn ){
        g_u16_ctl_main_mode = CONST_RUN;
        g_u16_const_run_way = 1;
        g_s16_const_run_spd = 300;
        g_s16_const_curve_rate = 100;
    }

    btn = 0;
    get_sonic_snc(&distance);
    if( 200 > distance ){
        start_set_distance_flg += 1;
    }
    // hub_display_text_scroll("YEC", 100);
    if( 200 <= start_set_distance_flg ){
        hub_display_text_scroll("OK", 40);
        if( 200 < distance ){
            start_set_distance_flg = 0;
            btn = 1;
        }
    }
    g_u16_start_set_distance = distance;

    return btn;
}