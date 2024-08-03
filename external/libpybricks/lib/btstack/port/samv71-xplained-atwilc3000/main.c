#define __BTSTACK_FILE__ "main.c"

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

// BTstack
#include "btstack_chipset_atwilc3000.h"
#include "btstack_debug.h"
#include "btstack_memory.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_embedded.h"
#include "hal_uart_dma.h"
#include "hal_cpu.h"
#include "hal_tick.h"
#include "hci.h"
#include "hci_dump.h"
#include "hci_dump_embedded_stdout.h"
#include "wilc3000_ble_firmware.h"

// #define USE_XDMAC_FOR_USART
#define XDMA_CH_UART_TX  0
#define XDMA_CH_UART_RX  1

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK   0xffffffff
  
#ifdef __cplusplus
extern "C" {
#endif

extern int btstack_main(int argc, const char * argv[]);

static void dummy_handler(void){}
static void (*tick_handler)(void) = &dummy_handler;

static btstack_uart_config_t uart_config;

static hci_transport_config_uart_t transport_config = {
	HCI_TRANSPORT_CONFIG_UART,
	2000000,  // directly use high baud rate after config
	0, 		  // use 0 to skip baud rate change from 115200 to X for debugging purposes
	1,        // flow control
	NULL,
};

/**
 *  \brief Handler for System Tick interrupt.
 */
void SysTick_Handler(void)
{
	tick_handler();
}

// Debug console Output

/**
 *  Configure UART console.
 */
// [main_console_configure]
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

// Debug console Input

#include "btstack_stdin.h"

static void (*stdin_handler)(char c);
static btstack_data_source_t stdin_data_source;

static void btstack_stdin_process(struct btstack_data_source *ds, btstack_data_source_callback_type_t callback_type){
	// try to read from console
	uint32_t stdin_character;
	uint32_t res = usart_read(CONF_UART, &stdin_character);
	if (res) return;

	if (stdin_handler){
		(*stdin_handler)(stdin_character & 0xff);
	}
}

void btstack_stdin_setup(void (*handler)(char c)){
	// set handler
	stdin_handler = handler;

	// set up polling data_source
	btstack_run_loop_set_data_source_handler(&stdin_data_source, &btstack_stdin_process);
	btstack_run_loop_enable_data_source_callbacks(&stdin_data_source, DATA_SOURCE_CALLBACK_POLL);
	btstack_run_loop_add_data_source(&stdin_data_source);
}

// [main_console_configure]

/**
 * \brief Wait for the given number of milliseconds (ticks
 * generated by the SAM's microcontrollers's system tick).
 *
 * \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
// [main_ms_delay]
static void mdelay(uint32_t delay_in_ms)
{
	// delay_ms(delay_in_ms);
	uint32_t time_to_wait = btstack_run_loop_get_time_ms() + delay_in_ms;
	while (btstack_run_loop_get_time_ms() < time_to_wait);
}
// [main_ms_delay]

////////////////////////////////////////////////////////////////////////////////
// hal_cpu.h implementation
////////////////////////////////////////////////////////////////////////////////
// hal_led.h implementation
#include "hal_led.h"
void hal_led_off(void);
void hal_led_on(void);

void hal_led_off(void){
	// gpio_set_pin_low(GPIOA, GPIO_LED2);
}
void hal_led_on(void){
	// gpio_set_pin_high(GPIOA, GPIO_LED2);
}
void hal_led_toggle(void){
	// gpio_toggle_pin(GPIOA, GPIO_LED2);
}

// hal_cpu.h implementation
#include "hal_cpu.h"

void hal_cpu_disable_irqs(void){
	//__disable_irq();
}

void hal_cpu_enable_irqs(void){
	// __enable_irq();
}

void hal_cpu_enable_irqs_and_sleep(void){
	hal_led_off();
	// __enable_irq();
	// __asm__("wfe");	// go to sleep if event flag isn't set. if set, just clear it. IRQs set event flag

	// note: hal_uart_needed_during_sleep can be used to disable peripheral clock if it's not needed for a timer
	hal_led_on();
}


#ifndef USE_XDMAC_FOR_USART
// RX state
static volatile uint16_t  bytes_to_read = 0;
static volatile uint8_t * rx_buffer_ptr = 0;

// TX state
static volatile uint16_t  bytes_to_write = 0;
static volatile uint8_t * tx_buffer_ptr = 0;
#endif

static volatile int       rx_notify;
static volatile int       tx_notify;

static int simulate_flowcontrol;

// handlers
static void (*rx_done_handler)(void) = dummy_handler;
static void (*tx_done_handler)(void) = dummy_handler;
static void (*cts_irq_handler)(void) = dummy_handler;

// @note While the Atmel SAM S7x data sheet states
// "The hardware handshaking feature enables an out-of-band flow control by automatic management 
//  of the pins RTS and CTS.",
// I didn't see RTS going up automatically up, ever. So, at least for RTS, the automatic management
// is just a glorified GPIO pin control feature, which provides no benefit, but irritates a lot

// J505:6
#define DEBUG_PIN_1 PIO_PD16_IDX
// J505:5
#define DEBUG_PIN_2 PIO_PD15_IDX

static inline void hal_uart_rts_high(void){
	if (!simulate_flowcontrol) return;
	ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_HIGH);
	BOARD_USART->US_CR = US_CR_RTSEN;
}
static inline void hal_uart_rts_low(void){
	if (!simulate_flowcontrol) return;
	ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_LOW);
	BOARD_USART->US_CR = US_CR_RTSDIS;
}

/**
 */
static int hal_uart_dma_initialized = 0;
void hal_uart_dma_init(void)
{
	if (hal_uart_dma_initialized){
		log_info("hal_uart_dma_init already initialized");
		return;
	}
	hal_uart_dma_initialized = 1;

	// debug
#ifdef DEBUG_PIN_1
	ioport_set_pin_dir(DEBUG_PIN_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DEBUG_PIN_1, IOPORT_PIN_LEVEL_LOW);
#endif
#ifdef DEBUG_PIN_2
	ioport_set_pin_dir(DEBUG_PIN_2, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_LOW);
#endif
	// power on
	ioport_set_pin_dir(BLUETOOTH_CHP_EN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BLUETOOTH_CHP_EN, IOPORT_PIN_LEVEL_HIGH);

	// reset
	ioport_set_pin_dir(BLUETOOTH_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BLUETOOTH_RESET, IOPORT_PIN_LEVEL_LOW);
	mdelay(250);
	ioport_set_pin_level(BLUETOOTH_RESET, IOPORT_PIN_LEVEL_HIGH);
	mdelay(250);

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

	// configure Bluetooth USART
	const sam_usart_opt_t bluetooth_settings = {
		115200,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Configure USART mode. */
	simulate_flowcontrol = 0;
	usart_init_rs232(BOARD_USART, &bluetooth_settings, sysclk_get_peripheral_hz());
	// Set RTS = 0 (normal mode)
	BOARD_USART->US_CR = US_CR_RTSEN;

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);

	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
	
#ifdef USE_XDMAC_FOR_USART

	// setup XDMAC
	
	/* Initialize and enable DMA controller */
	pmc_enable_periph_clk(ID_XDMAC);

	/* Enable XDMA interrupt */
	NVIC_ClearPendingIRQ(XDMAC_IRQn);
	NVIC_SetPriority( XDMAC_IRQn ,1);
	NVIC_EnableIRQ(XDMAC_IRQn);
	
	// Setup XDMA Channel for USART TX
	xdmac_channel_set_destination_addr(XDMAC, XDMA_CH_UART_TX, (uint32_t)&BOARD_USART->US_THR);
	xdmac_channel_set_config(XDMAC, XDMA_CH_UART_TX,
		XDMAC_CC_TYPE_PER_TRAN |
		XDMAC_CC_DSYNC_MEM2PER |
		XDMAC_CC_MEMSET_NORMAL_MODE |
		XDMAC_CC_MBSIZE_SINGLE |
		XDMAC_CC_DWIDTH_BYTE |
		XDMAC_CC_SIF_AHB_IF0 |
		XDMAC_CC_DIF_AHB_IF1 |
		XDMAC_CC_SAM_INCREMENTED_AM |
		XDMAC_CC_DAM_FIXED_AM |
		XDMAC_CC_CSIZE_CHK_1 |
		XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_USART0_TX)
	);
	xdmac_channel_set_descriptor_control(XDMAC, XDMA_CH_UART_TX, 0);
	xdmac_channel_set_source_microblock_stride(XDMAC, XDMA_CH_UART_TX, 0);
	xdmac_channel_set_destination_microblock_stride(XDMAC, XDMA_CH_UART_TX, 0);
	xdmac_channel_set_datastride_mempattern(XDMAC, XDMA_CH_UART_TX, 0);
	xdmac_channel_set_block_control(XDMAC, XDMA_CH_UART_TX, 0);
	xdmac_enable_interrupt(XDMAC, XDMA_CH_UART_TX);
	xdmac_channel_enable_interrupt(XDMAC, XDMA_CH_UART_TX, XDMAC_CIE_BIE);
	
	// Setup XDMA Channel for USART RX
	xdmac_channel_set_source_addr(XDMAC, XDMA_CH_UART_RX, (uint32_t)&BOARD_USART->US_RHR);
	xdmac_channel_set_config(XDMAC, XDMA_CH_UART_RX,
		XDMAC_CC_TYPE_PER_TRAN |
		XDMAC_CC_DSYNC_PER2MEM |
		XDMAC_CC_MEMSET_NORMAL_MODE |
		XDMAC_CC_MBSIZE_SINGLE |
		XDMAC_CC_DWIDTH_BYTE |
		XDMAC_CC_SIF_AHB_IF1 |
		XDMAC_CC_DIF_AHB_IF0 |
		XDMAC_CC_SAM_FIXED_AM |
		XDMAC_CC_DAM_INCREMENTED_AM |
		XDMAC_CC_CSIZE_CHK_1 |
		XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_USART0_RX)
	);
	xdmac_channel_set_descriptor_control(XDMAC, XDMA_CH_UART_RX, 0);
	xdmac_channel_set_source_microblock_stride(XDMAC, XDMA_CH_UART_RX, 0);
	xdmac_channel_set_destination_microblock_stride(XDMAC, XDMA_CH_UART_RX, 0);
	xdmac_channel_set_datastride_mempattern(XDMAC, XDMA_CH_UART_RX, 0);
	xdmac_channel_set_block_control(XDMAC, XDMA_CH_UART_RX, 0);
	xdmac_enable_interrupt(XDMAC, XDMA_CH_UART_RX);
	xdmac_channel_enable_interrupt(XDMAC, XDMA_CH_UART_RX, XDMAC_CIE_BIE);
#endif
}

void hal_uart_dma_set_sleep(uint8_t sleep){
}

void hal_uart_dma_set_block_received( void (*the_block_handler)(void)){
	rx_done_handler = the_block_handler;
}

void hal_uart_dma_set_block_sent( void (*the_block_handler)(void)){
	tx_done_handler = the_block_handler;
}

void hal_uart_dma_set_csr_irq_handler( void (*the_irq_handler)(void)){
	cts_irq_handler = the_irq_handler;
}

int  hal_uart_dma_set_baud(uint32_t baud){
	/* Disable TX & RX function. */
	usart_disable_tx(BOARD_USART);
	usart_disable_rx(BOARD_USART);
	uint32_t res = usart_set_async_baudrate(BOARD_USART, baud, sysclk_get_peripheral_hz());
	if (res){
		log_error("hal_uart_dma_set_baud library call failed");
	}
	
	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	log_info("set baud rate %u", (int) baud);
	return 0;
}

int  hal_uart_dma_set_flowcontrol(int flowcontrol){
	log_info("hal_uart_dma_set_flowcontrol %u", flowcontrol);
	simulate_flowcontrol = flowcontrol;
	if (flowcontrol){
		/* Set hardware handshaking mode. */
		BOARD_USART->US_MR = (BOARD_USART->US_MR & ~US_MR_USART_MODE_Msk) | US_MR_USART_MODE_HW_HANDSHAKING;
		hal_uart_rts_high();
	} else {
		/* Set nomal mode. */
		BOARD_USART->US_MR = (BOARD_USART->US_MR & ~US_MR_USART_MODE_Msk) | US_MR_USART_MODE_NORMAL;
		// Set RTS = 0 (normal mode)
		BOARD_USART->US_CR = US_CR_RTSEN;
	}
	return 0;
}

void hal_uart_dma_send_block(const uint8_t *data, uint16_t size){

	tx_notify = 1;

#ifdef USE_XDMAC_FOR_USART
	xdmac_channel_get_interrupt_status( XDMAC, XDMA_CH_UART_TX);
	xdmac_channel_set_source_addr(XDMAC, XDMA_CH_UART_TX, (uint32_t)data);
	xdmac_channel_set_microblock_control(XDMAC, XDMA_CH_UART_TX, size);
	xdmac_channel_enable(XDMAC, XDMA_CH_UART_TX);
#else
	if (bytes_to_write){
		log_error("send block, bytes to write %u", bytes_to_write);
		return;
	}
    tx_buffer_ptr = (uint8_t *) data;
    bytes_to_write = size;
	usart_enable_interrupt(BOARD_USART, US_IER_TXRDY);
#endif
}

void hal_uart_dma_receive_block(uint8_t *data, uint16_t size){

#ifdef DEBUG_PIN_1
	ioport_set_pin_level(DEBUG_PIN_1, IOPORT_PIN_LEVEL_HIGH);
#endif

	hal_uart_rts_low();

	rx_notify = 1;

#ifdef USE_XDMAC_FOR_USART
	xdmac_channel_get_interrupt_status( XDMAC, XDMA_CH_UART_RX);
	xdmac_channel_set_destination_addr(XDMAC, XDMA_CH_UART_RX, (uint32_t)data);
	xdmac_channel_set_microblock_control(XDMAC, XDMA_CH_UART_RX, size);
	xdmac_channel_enable(XDMAC, XDMA_CH_UART_RX);
#else
    rx_buffer_ptr = data;
    bytes_to_read = size;
    usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
#endif
}

#ifdef USE_XDMAC_FOR_USART
void XDMAC_Handler(void)
{
	uint32_t dma_status;
	dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_UART_TX);
	if (dma_status & XDMAC_CIS_BIS) {
		if (tx_notify){
			tx_notify = 0;
			tx_done_handler();
		}
	}
	dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_UART_RX);
	if (dma_status & XDMAC_CIS_BIS) {
		hal_uart_rts_high();
		if (rx_notify){
			rx_notify = 0;
			rx_done_handler();
		}
	}
}
#else
void USART_Handler(void)
{

#ifdef DEBUG_PIN_2
	// ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_HIGH);
#endif

	/* Read USART status. */
	uint32_t ul_status = usart_get_status(BOARD_USART);

	// handle ready to send
	if(ul_status & US_IER_TXRDY) {
		if (bytes_to_write){
			// send next byte
			usart_write(BOARD_USART, *tx_buffer_ptr);
			tx_buffer_ptr++;
			bytes_to_write--;
		} else {

			// done. disable tx ready interrupt to avoid starvation here
			usart_disable_interrupt(BOARD_USART, US_IER_TXRDY);
			if (tx_notify){
				tx_notify = 0;
				tx_done_handler();
			}
		}
	}

	// handle byte available for read
	if (ul_status & US_IER_RXRDY) {
		if (bytes_to_read){
			uint32_t ch;
			usart_read(BOARD_USART, (uint32_t *)&ch);
			*rx_buffer_ptr++ = ch;
			bytes_to_read--;
			if (bytes_to_read == 0){

#ifdef DEBUG_PIN_1
			ioport_set_pin_level(DEBUG_PIN_1, IOPORT_PIN_LEVEL_LOW);
#endif

				// done. disable rx ready interrupt, raise RTS
				hal_uart_rts_high();
				usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
				if (rx_notify){
					rx_notify = 0;
					rx_done_handler();
				}
			}
		} else {
			// shoult not happen, disable irq anyway
			usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
		}
	}
#ifdef DEBUG_PIN_2
	// ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_LOW);
#endif

}
#endif

void hal_tick_init()
{
	/* Configure systick for 1 ms */
	puts("Configure system tick to get 1ms tick period.\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
		while (1);
	}
}

void hal_tick_set_handler(void (*handler)(void)){
	if (handler == NULL){
		tick_handler = &dummy_handler;
		return;
	}
	tick_handler = handler;
}

int  hal_tick_get_tick_period_in_ms(void){
	return 1;
}

static const btstack_uart_block_t * uart_driver;

static void phase2(int status){

    if (status){
        printf("Download firmware failed\n");
        return;
    }

    printf("Phase 2: Main app\n");

    // init HCI
    const hci_transport_t * transport = hci_transport_h4_instance(uart_driver);
    hci_init(transport, (void*) &transport_config);
    hci_set_chipset(btstack_chipset_atwilc3000_instance());
    
    // setup app
    btstack_main(0, NULL);
}

/**
 *  \brief getting-started Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
// [main]
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();

	/* Output boot info */
	printf("BTstack on SAMV71 Xplained Ultra with ATWILC3000\n");
	printf("CPU %lu hz, peripheral clock %lu hz\n", sysclk_get_cpu_hz(), sysclk_get_peripheral_hz());
#ifdef USE_XDMAC_FOR_USART
	printf("Using XDMA for Bluetooth UART\n");
#else
	printf("Using IRQ driver for Bluetooth UART\n");
#endif
	printf("--\n");

	// start with BTstack init - especially configure HCI Transport
	btstack_memory_init();
	btstack_run_loop_init(btstack_run_loop_embedded_get_instance());

	// enable full log output while porting
    // hci_dump_init(hci_dump_embedded_stdout_get_instance());

	// setup UART HAL + Run Loop integration
	uart_driver = btstack_uart_block_embedded_instance();

    // extract UART config from transport config, but disable flow control and use default baudrate
    uart_config.baudrate    = HCI_DEFAULT_BAUDRATE;
    uart_config.flowcontrol = 0;
    uart_config.device_name = transport_config.device_name;
    uart_driver->init(&uart_config);

    // phase #1 download firmware
    printf("Phase 1: Download firmware\n");

    // phase #2 start main app
    btstack_chipset_atwilc3000_download_firmware(uart_driver, transport_config.baudrate_init, transport_config.flowcontrol,  (const uint8_t *) firmware_ble, sizeof(firmware_ble), &phase2);

	// go
	btstack_run_loop_execute();

	// compiler happy
	while(1);
}
#ifdef __cplusplus
}
#endif
