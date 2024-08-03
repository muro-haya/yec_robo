#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "button.h"

hub_button_t pressed;

/* ボタン入力初期化処理 */
void ini_button( void ){
    ;
}

/* ボタン入力取得 */
uint16_t get_button( uint16_t btn ){

    hub_button_is_pressed(&pressed);

    if((pressed & btn) == btn){
        return 1;
    }
    else{
        return 0;
    }
}