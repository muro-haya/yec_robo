#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <pbio/color.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "comm.h"

#define COM_PACKET_SIZE (sizeof("@000:000000"))
static char buf[COM_PACKET_SIZE] = "@000:000000";
static uint16_t ercd;

void ini_comm( void ){
  ercd = serial_opn_por(SIO_USB_PORTID);
}

void send_data(uint16_t tx_cmd, uint16_t tx_val){

    buf[3] = '0' + tx_cmd % 10; tx_cmd /= 10;
    buf[2] = '0' + tx_cmd % 10; tx_cmd /= 10;
    buf[1] = '0' + tx_cmd % 10; tx_cmd /= 10;

    if (tx_val >= 0) {
        buf[10] = '0' + tx_val % 10; tx_val /= 10;
        buf[9]  = '0' + tx_val % 10; tx_val /= 10;
        buf[8]  = '0' + tx_val % 10; tx_val /= 10;
        buf[7]  = '0' + tx_val % 10; tx_val /= 10;
        buf[6]  = '0' + tx_val % 10; tx_val /= 10;
        buf[5]  = '0' + tx_val % 10; tx_val /= 10;
    }
    else {
        tx_val = -tx_val;
        buf[10] = '0' + tx_val % 10; tx_val /= 10;
        buf[9]  = '0' + tx_val % 10; tx_val /= 10;
        buf[8]  = '0' + tx_val % 10; tx_val /= 10;
        buf[7]  = '0' + tx_val % 10; tx_val /= 10;
        buf[6]  = '0' + tx_val % 10; tx_val /= 10;
        buf[5]  = '-';
    }

    serial_wri_dat(SIO_USB_PORTID, buf, COM_PACKET_SIZE-1);
}


bool_t received_data(uint16_t rx_cmd,int32_t *rx_val){
    uint16_t  cmd[3];
    uint16_t  cmd_id;
    uint16_t  val[6];
    int32_t val_sum;

    serial_rea_dat(SIO_USB_PORTID, buf, COM_PACKET_SIZE-1);

    cmd[0] = buf[1] - '0';                       /* コマンド:百の桁 */
    cmd[1] = buf[2] - '0';                       /* コマンド:十の桁 */
    cmd[2] = buf[3] - '0';                       /* コマンド:一の桁 */

    cmd_id = cmd[0] * 100 + cmd[1] * 10 + cmd[2];
    if( cmd_id != rx_cmd ){
        return 0;
    }

    val[0] = buf[6]  - '0';                    /* 受信値:万の桁 */
    val[1] = buf[7]  - '0';                    /* 受信値:千の桁 */
    val[2] = buf[8]  - '0';                    /* 受信値:百の桁 */
    val[3] = buf[9]  - '0';                    /* 受信値:十の桁 */
    val[4] = buf[10] - '0';                    /* 受信値:一の桁 */

    val_sum = val[0] * 10000 + val[1] * 1000 + val[2] * 100 + val[3] * 10 + val[4];

    if( '-' == buf[5] ){                        /* －がある場合 */
        val_sum = -val_sum;
    }

    *rx_val = val_sum;                          /* 受信データ格納 */

    return 1;

}