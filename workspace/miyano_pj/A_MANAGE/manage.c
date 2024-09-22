/* 競技攻略 */
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "manage.h"

#include "double_loop.h"
#include "debri_remove.h"
#include "smart_carry.h"

/* 外部公開変数 */
uint16_t g_u16_manage_cnt;          /* 難所カウント */

/* 外部非公開変数 */

/* 外部非公開関数 */

/* 競技攻略初期化処理 */
void ini_manage( void ){
    ini_debri_remove();
    ini_smart_carry();

    g_u16_manage_cnt = 2;
}

/* 競技攻略周期処理 */
void cyc_manage( void ){
    bool_t bdat;

    switch (g_u16_manage_cnt)
    {
    case DOUBLE_LOOP:
        /* code */
        break;
    case DEBRI_REMOVE:
        bdat = cyc_debri_remove();
        if( 1 == bdat ){
            //g_u16_manage_cnt += 1;
        }
        break;
    case SMART_CARRY:
        bdat = cyc_smart_carry();
        if( 1 == bdat ){
            g_u16_manage_cnt += 1;
        }
        break;
    default:
        break;
    }
}