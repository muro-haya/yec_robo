/* アーム制御 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "arm_ctl.h"

#include "../D_DEVICE/arm_mtr.h"

/* 外部公開変数 */
uint16_t g_u16_arm_ctl_way;             /* アーム制御方法(0:DUTY 1:回転速度) */
uint16_t g_s16_arm_ctl_spd;             /* アーム制御回転速度 */
uint16_t g_s16_arm_ctl_duty;            /* アーム制御duty */

/* アーム制御初期化 */
void ini_arm_ctl( void ){
    /* 変数初期化 */
    g_u16_arm_ctl_way  = 0;
    g_s16_arm_ctl_spd  = 0;
    g_s16_arm_ctl_duty = 0;
}

/* アーム制御周期処理 */
void cyc_arm_ctl( void ){
    if( 0 == g_u16_arm_ctl_way ){       /* DUTY制御 */
        set_arm_mtr_duty(g_s16_arm_ctl_duty);
    }
    else{                               /* 回転速度制御 */
        set_arm_mtr_spd(g_s16_arm_ctl_spd);
    }
}