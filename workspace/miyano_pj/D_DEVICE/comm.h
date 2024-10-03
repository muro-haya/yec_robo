/*
 * Definitions of Port ID for Serial Adapter
 */
#define SIO_USART_F_PORTID    1
#define SIO_USB_PORTID        2
#define SIO_BLUETOOTH_PORTID  3
#define SIO_TEST_PORTID       4

/* 外部公開変数 */
extern uint16_t g_u16_comm_rx_flg;                             /* 通信中フラグ(1:通信中) */
extern uint16_t g_u16_comm_rx_jdg_red;                         /* 指定座標の赤判定フラグ(0:ある 1:ない) */
extern uint16_t g_u16_comm_rx_pet_xpos_red;                    /* カラーチェイス用赤ペットボトルx軸位置 */
extern uint16_t g_u16_comm_rx_pet_xpos_bl;                     /* カラーチェイス用青ペットボトルx軸位置 */
extern uint16_t g_u16_comm_rx_jdg_pet;                         /* ペットボトル色判定(1:赤 2:青 0:無) */
extern uint16_t g_u16_comm_rx_pet_srt;                         /* ペットボトル判定開始(1:開始) */
extern uint16_t comm_reset_flg;                                /* 通信リセットフラグ(0:正常 1:リセット中) */

/* 外部公開関数 */
extern void ini_comm( void );                                  /* 通信初期化処理 */
extern void send_data(uint16_t tx_cmd, uint16_t tx_val);       /* シリアル通信送信処理 */
extern void received_data(uint16_t *rx_cmd,uint16_t *rx_val);  /* シリアル通信受信処理 */
extern void cyc_watch_comm( void );
extern void cyc_tx( void );
extern void cyc_rx( void );