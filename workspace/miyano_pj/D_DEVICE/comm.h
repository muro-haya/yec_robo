/*
 * Definitions of Port ID for Serial Adapter
 */
#define SIO_USART_F_PORTID    1
#define SIO_USB_PORTID        2
#define SIO_BLUETOOTH_PORTID  3
#define SIO_TEST_PORTID       4

/* 外部参照関数 */
extern void ini_comm( void );                                  /* 通信初期化処理 */
extern void send_data(uint16_t tx_cmd, uint16_t tx_val);       /* シリアル通信送信処理 */
extern bool_t received_data(uint16_t rx_cmd,int32_t *rx_val);  /* シリアル通信受信処理 */