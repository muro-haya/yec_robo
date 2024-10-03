/* スマートキャリー攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../M_MEASURE/cal_distance.h"
#include "../M_CTL/linetrace_run.h"
#include "../D_DEVICE/button.h"
#include "../D_DEVICE/comm.h"
#include "../D_DEVICE/color_snc.h"

#include "start_set.h"

#include "../D_DEVICE/comm.h"

/* 適合値 */

/* 外部公開変数 */

/* 外部非公開変数 */

/* 外部非公開関数 */

/* 開始時設定初期化処理 */
void ini_start_set( void ){

}

/* 開始時設定周期処理 */
bool_t cyc_start_set( void ){
    uint16_t btn;

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
    g_u16_linetrace_run_fbTgt = ( g_u16_linetrace_run_lpos + g_u16_linetrace_run_rpos ) / 2;
    
    btn = get_button( BUTTON_CENTER );

    return btn;
}