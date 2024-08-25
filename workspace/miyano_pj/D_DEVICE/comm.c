#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <pbio/color.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "comm.h"

#define COM_PACKET_SIZE (sizeof("@000:000000\n"))
static char tx_buf[COM_PACKET_SIZE] = "@000:000000\n";
static char rx_buf[COM_PACKET_SIZE] = "@000:000000\n";
static uint16_t ercd;

/* 外部公開変数 */
int32_t g_s32_comm_rx_jdg_red;                         /* 指定座標の赤判定フラグ(0:ある 1:ない) */

/* 外部非公開変数 */
static int32_t comm_tx_cnt;                            /* 送信確認カウンタ */
static int32_t comm_rx_cnt;                            /* 受信確認カウンタ */

struct comm_data{
    uint16_t comm_cnt;                                  /* 周期カウンタ(受信では使用しない) */
    uint16_t comm_cyc;                                  /* 周期(受信では使用しない 一応記載) */
    uint16_t comm_cmd;                                  /* コマンド */
    int32_t* comm_data;                                 /* 通信データ */
};

#define TX_DATA_NUM 2                                   /* 送信データ数 */
/* 送信情報 */
struct comm_data tx_datas[] = {
    {  0, 100, 500, &comm_tx_cnt               },       /* 送信確認カウンタ */
    {  1, 100, 501, &comm_rx_cnt               },       /* 受信確認返信カウンタ */
};
#define RX_DATA_NUM 2                                   /* 受信データ数 */
/* 受信情報 */
struct comm_data rx_datas[] = {
    {  0, 100, 000, &comm_rx_cnt               },       /* 受信確認カウンタ */
    {  0,  10, 001, &g_s32_comm_rx_jdg_red     },       /* 指定座標の赤判定フラグ(0:ある 1:ない) */
};

void ini_comm( void ){
  ercd = serial_opn_por(SIO_USB_PORTID);

  g_s32_comm_rx_jdg_red = 0;                            /* 指定座標の赤判定フラグ(0:ある 1:ない) */

  comm_tx_cnt           = 0;                            /* 送信確認カウンタ */
  comm_rx_cnt           = 0;                            /* 受信確認カウンタ */
}

void cyc_tx( void ){
    uint16_t idat;

    for ( idat = 0; idat < TX_DATA_NUM; idat++){
        if( tx_datas[idat].comm_cyc == tx_datas[idat].comm_cnt ){
            send_data( tx_datas[idat].comm_cmd, *tx_datas[idat].comm_data );
            tx_datas[idat].comm_cnt = 0;
        }
        tx_datas[idat].comm_cnt += 1;
    }

    comm_tx_cnt += 1;
}

void cyc_rx( void ){
    uint16_t idat;
    uint16_t cmd;
    int32_t data;

    while (1){
        received_data(&cmd, &data);
        for ( idat = 0; idat < RX_DATA_NUM; idat++){
            if( cmd == rx_datas[idat].comm_cmd ){
                *rx_datas[idat].comm_data = data;
                break;
            }
        }
    }
}

void send_data(uint16_t tx_cmd, int32_t tx_val){

    tx_buf[3] = '0' + tx_cmd % 10; tx_cmd /= 10;
    tx_buf[2] = '0' + tx_cmd % 10; tx_cmd /= 10;
    tx_buf[1] = '0' + tx_cmd % 10; tx_cmd /= 10;

    if (tx_val >= 0) {
        tx_buf[10] = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[9]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[8]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[7]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[6]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[5]  = '0' + tx_val % 10; tx_val /= 10;
    }
    else {
        tx_val  = -tx_val;
        tx_buf[10] = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[9]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[8]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[7]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[6]  = '0' + tx_val % 10; tx_val /= 10;
        tx_buf[5]  = '-';
    }

    serial_wri_dat(SIO_USB_PORTID, tx_buf, COM_PACKET_SIZE-1);
}


void received_data(uint16_t *rx_cmd,int32_t *rx_val){
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

    if( '-' == rx_buf[5] ){                         /* －がある場合 */
        val_sum = -val_sum;
    }

    *rx_val = val_sum;                              /* 受信データ格納 */

}