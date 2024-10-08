#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>
#include "spike/hub/speaker.h"

#include <pbio/color.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"
#include "drive_mtr.h"

#include "../D_DEVICE/color_snc.h"
#include "../D_DEVICE/button.h"
#include "../M_CTL/linetrace_run.h"
#include "../M_CTL/const_run.h"
#include "../M_CTL/color_chase.h"
#include "../M_CTL/ctl_main.h"
#include "../A_MANAGE/double_loop.h"
#include "../A_MANAGE/smart_carry.h"


#include "../A_MANAGE/debri_remove.h"

#include "comm.h"

#define COM_PACKET_SIZE (sizeof("@000:000000\n"))
static char tx_buf[COM_PACKET_SIZE] = "@000:000000\n";
static char rx_buf[COM_PACKET_SIZE] = "@000:000000\n";
static uint16_t ercd;

uint16_t vlume;

/* 外部公開変数 */
uint16_t g_u16_comm_rx_flg;                             /* 通信中フラグ(1:通信中) */
uint16_t g_u16_comm_rx_jdg_red;                         /* 指定座標の赤判定フラグ(0:ある 1:ない) */
uint16_t g_u16_comm_rx_pet_xpos_red;                    /* カラーチェイス用赤ペットボトルx軸位置 */
uint16_t g_u16_comm_rx_pet_xpos_bl;                     /* カラーチェイス用青ペットボトルx軸位置 */
uint16_t g_u16_comm_rx_jdg_pet;                         /* ペットボトル色判定(1:赤 2:青 0:無) */
uint16_t g_u16_comm_rx_pet_srt;                         /* ペットボトル判定開始(1:開始) */
uint16_t comm_reset_flg;                                /* 通信リセットフラグ(0:正常 1:リセット中) */


/* 外部非公開変数 */
static uint16_t comm_tx_cnt;                            /* 送信確認カウンタ */
static uint16_t comm_rx_cnt;                            /* 受信確認カウンタ */
static uint16_t comm_watch_cnt;                         /* 通信監視カウンタ */

struct comm_data{
    uint16_t comm_cnt;                                  /* 周期カウンタ(受信では使用しない) */
    uint16_t comm_cyc;                                  /* 周期(受信では使用しない 一応記載) */
    uint16_t comm_cmd;                                  /* コマンド */
    int32_t* comm_data;                                 /* 通信データ */
};

#define TX_DATA_NUM 14                                   /* 送信データ数 */
/* 送信情報 */
struct comm_data tx_datas[] = {
    {  0, 1000, 500, (uint16_t*)&comm_tx_cnt              },       /* 送信確認カウンタ */
    {  1, 1000, 501, (uint16_t*)&comm_rx_cnt              },       /* 受信確認返信カウンタ */
    {  2, 1000, 502, (uint16_t*)&vlume                    },       /* 受信確認返信カウンタ */
    {  3,  100, 503, (uint16_t*)&g_u16_comm_rx_pet_srt    },       /* ペットボトル判定開始(1:開始) */

    {  0, 100, 600, (uint16_t*)&g_u16_r_valu              },       /* 計測値0 */
    {  1, 100, 601, (uint16_t*)&g_u16_g_valu              },       /* 計測値1 */
    {  2, 100, 602, (uint16_t*)&g_u16_b_valu              },       /* 計測値2 */
    {  3, 100, 603, (uint16_t*)&g_u16_comm_rx_pet_srt     },       /* 計測値3 */
    {  4, 100, 604, (uint16_t*)&g_u16_ctl_main_mode       },       /* 計測値4 */
    {  5, 100, 605, (uint16_t*)&g_u16_comm_rx_pet_xpos_red},       /* 計測値5 */
    {  6, 100, 606, (uint16_t*)&g_u16_comm_rx_jdg_pet     },       /* 計測値6 */
    {  7, 100, 607, (uint16_t*)&g_s16_color_chase_debug   },       /* 計測値7 */
    {  8, 100, 608, (uint16_t*)&g_16_DLdeg                },       /* 計測値8 */
    {  9, 100, 609, (uint16_t*)&vlume                     },       /* 計測値9 */
};
#define RX_DATA_NUM 15                                   /* 受信データ数 */
/* 受信情報 */
struct comm_data rx_datas[] = {
    {  0,  100, 000, (uint16_t*)&comm_rx_cnt               },      /* 受信確認カウンタ */
    {  0,   10, 001, (uint16_t*)&g_u16_comm_rx_jdg_red     },      /* 指定座標の赤判定フラグ(0:ある 1:ない) */
    {  0,   10, 002, (uint16_t*)&g_u16_comm_rx_pet_xpos_red},      /* カラーチェイス用赤ペットボトルx軸位置 */
    {  0,   10, 003, (uint16_t*)&g_u16_comm_rx_pet_xpos_bl },      /* カラーチェイス用青ペットボトルx軸位置 */
    {  0,   10, 004, (uint16_t*)&g_u16_comm_rx_jdg_pet     },      /* ペットボトル色判定(1:赤 2:青 0:無) */
    
    {  0,   10, 100, (uint16_t*)&g_u16_comm_rx_pet_srt     },      /* 適合値0 */
    {  0,   10, 101, (uint16_t*)&vlume                     },      /* 適合値1 */
    {  0,   10, 102, (uint16_t*)&vlume                     },      /* 適合値2 */
    {  0,   10, 103, (uint16_t*)&vlume                     },      /* 適合値3 */
    {  0,   10, 104, (uint16_t*)&vlume                     },      /* 適合値4 */
    {  0,   10, 105, (uint16_t*)&vlume                     },      /* 適合値5 */
    {  0,   10, 106, (uint16_t*)&vlume                     },      /* 適合値6 */
    {  0,   10, 107, (uint16_t*)&vlume                     },      /* 適合値7 */
    {  0,   10, 108, (uint16_t*)&vlume                     },      /* 適合値8 */
    {  0,   10, 109, (uint16_t*)&vlume                     },      /* 適合値9 */
};

void ini_comm( void ){
  ercd = serial_opn_por(SIO_USB_PORTID);

  g_u16_comm_rx_jdg_red      = 0;                                   /* 指定座標の赤判定フラグ(0:ある 1:ない) */
  g_u16_comm_rx_pet_xpos_red = 50;                                  /* カラーチェイス用赤ペットボトルx軸位置 */
  g_u16_comm_rx_pet_xpos_bl  = 50;                                  /* カラーチェイス用青ペットボトルx軸位置 */
  g_u16_comm_rx_jdg_pet      = 0;                                   /* ペットボトル色判定(1:赤 2:青 0:無) */
  g_u16_comm_rx_pet_srt      = 0;                                   /* ペットボトル判定開始(1:開始) */
  g_u16_comm_rx_flg          = 0;

  comm_tx_cnt                = 0;                                   /* 送信確認カウンタ */
  comm_rx_cnt                = 0;                                   /* 受信確認カウンタ */
  comm_watch_cnt             = 0;                                   /* 通信監視カウンタ */
  comm_reset_flg             = 0;                                   /* 通信リセットフラグ(0:正常 1:リセット中) */

  vlume = 10;
}

void cyc_watch_comm( void ){
    uint16_t btn;

    btn = get_button(BUTTON_BT);

    if( 1 == btn ){
        comm_reset_flg = 1;
    }

    // if( 1 == comm_reset_flg ){
    //     if( 10 < comm_watch_cnt ){
    //         serial_cls_por(SIO_USB_PORTID);                         /* ポートクローズ */
    //         comm_watch_cnt = 0;                                     /* カウンタリセット */
    //         comm_reset_flg = 0;                                     /* 通信リセットフラグ(0:リセット中 1:正常) */
    //     }
    // }
    // else if( 0 == comm_reset_flg ){
    //     if( 5 < comm_watch_cnt ){                                   /* 時間経過 */
    //         serial_opn_por(SIO_USB_PORTID);                          /* ポートオープン */
    //         comm_watch_cnt = 0;                                      /* カウンタリセット */
    //         comm_reset_flg = 4;                                      /* 通信リセットフラグ(0:リセット中 1:正常) */
    //     }
    // }
    // else{
    //     comm_reset_flg -= 1;
    // }
    // comm_watch_cnt += 1;
}

void cyc_tx( void ){
    uint16_t idat;

    if( ( 1 != comm_reset_flg )
    ){
        return;
        /* リセット中のため以下の処理は実施しない */
    }

    for ( idat = 0; idat < TX_DATA_NUM; idat++){
        if( tx_datas[idat].comm_cyc == tx_datas[idat].comm_cnt ){
            send_data( tx_datas[idat].comm_cmd, *tx_datas[idat].comm_data );
            tx_datas[idat].comm_cnt = 0;
        }
        tx_datas[idat].comm_cnt += 1;
    }

    comm_tx_cnt += 1;

    g_u16_comm_rx_flg = 1;
    // hub_speaker_set_volume(20);
    // hub_speaker_play_tone(2000, 2);

    comm_watch_cnt = 0;                         /* カウンタクリア */
}

void cyc_rx( void ){
    uint16_t idat;
    uint16_t cmd;
    uint16_t data;

    if( 1 != comm_reset_flg ){
        return;
        /* リセット中のため以下の処理は実施しない */
    }
    
    received_data(&cmd, &data);
    for ( idat = 0; idat < RX_DATA_NUM; idat++){
        if( cmd == rx_datas[idat].comm_cmd ){
            *rx_datas[idat].comm_data = data;
            break;
        }
    }
}

void send_data(uint16_t tx_cmd, uint16_t tx_val){
    int16_t ercode;

    tx_buf[3] = '0' + tx_cmd % 10; tx_cmd /= 10;
    tx_buf[2] = '0' + tx_cmd % 10; tx_cmd /= 10;
    tx_buf[1] = '0' + tx_cmd % 10; tx_cmd /= 10;

    tx_buf[10] = '0' + tx_val % 10; tx_val /= 10;
    tx_buf[9]  = '0' + tx_val % 10; tx_val /= 10;
    tx_buf[8]  = '0' + tx_val % 10; tx_val /= 10;
    tx_buf[7]  = '0' + tx_val % 10; tx_val /= 10;
    tx_buf[6]  = '0' + tx_val % 10; tx_val /= 10;
    tx_buf[5]  = '0' + tx_val % 10; tx_val /= 10;
    
    ercode = serial_wri_dat(SIO_USB_PORTID, tx_buf, COM_PACKET_SIZE-1);
}


void received_data(uint16_t *rx_cmd,uint16_t *rx_val){
    uint16_t cmd[3];
    uint16_t cmd_id;
    uint16_t val[6];
    int32_t  val_sum;

    while (1){
        serial_rea_dat(SIO_USB_PORTID, &rx_buf[0], 1);
        if( '@' == rx_buf[0] ){
            break;
        }
    }

    for(int i=1;i<COM_PACKET_SIZE-1;i++){
        serial_rea_dat(SIO_USB_PORTID, &rx_buf[i], 1);
    }

    cmd[0] = rx_buf[1] - '0';                       /* コマンド:百の桁 */
    cmd[1] = rx_buf[2] - '0';                       /* コマンド:十の桁 */
    cmd[2] = rx_buf[3] - '0';                       /* コマンド:一の桁 */
    cmd_id = cmd[0] * 100 + cmd[1] * 10 + cmd[2];
    *rx_cmd = cmd_id;

    val[0] = rx_buf[6]  - '0';                       /* 受信値:万の桁 */
    val[1] = rx_buf[7]  - '0';                       /* 受信値:千の桁 */
    val[2] = rx_buf[8]  - '0';                       /* 受信値:百の桁 */
    val[3] = rx_buf[9]  - '0';                       /* 受信値:十の桁 */
    val[4] = rx_buf[10] - '0';                       /* 受信値:一の桁 */
    val_sum = val[0] * 10000 + val[1] * 1000 + val[2] * 100 + val[3] * 10 + val[4];

    if( '-' == rx_buf[5] ){                          /* －がある場合 */
        val_sum = -val_sum;
    }

    *rx_val = val_sum;                              /* 受信データ格納 */

}