/* kernel_cfg.h */
#ifndef TOPPERS_KERNEL_CFG_H
#define TOPPERS_KERNEL_CFG_H

#define TNUM_TSKID	8
#define PYBRICKS_TASK	1
#define TSKID_tTask_LogTask_Task	2
#define MAIN_TASK	3
#define MAIN_2M_TASK	4
#define MAIN_10M_TASK	5
#define MAIN_100M_TASK	6
#define TX_1M_TASK	7
#define RX_1M_TASK	8

#define TNUM_SEMID	9
#define APP_HEAP_SEM	1
#define SEMID_tSemaphore_SerialPort1_ReceiveSemaphore	2
#define SEMID_tSemaphore_SerialPort1_SendSemaphore	3
#define SEMID_tSemaphore_SerialPortUSB1_ReceiveSemaphore	4
#define SEMID_tSemaphore_SerialPortUSB1_SendSemaphore	5
#define SEMID_tSemaphore_SerialPortBluetooth1_ReceiveSemaphore	6
#define SEMID_tSemaphore_SerialPortBluetooth1_SendSemaphore	7
#define SEMID_tSemaphore_SerialPortTest1_ReceiveSemaphore	8
#define SEMID_tSemaphore_SerialPortTest1_SendSemaphore	9

#define TNUM_FLGID	0

#define TNUM_DTQID	0

#define TNUM_PDQID	0

#define TNUM_MTXID	0

#define TNUM_MPFID	0

#define TNUM_CYCID	6
#define PB_CYCHDR	1
#define MAIN_2M_CYC	2
#define MAIN_10M_CYC	3
#define MAIN_100M_CYC	4
#define TX_1M_CYC	5
#define RX_1M_CYC	6

#define TNUM_ALMID	0

#define TNUM_ISRID	1
#define ISRID_tISR_SIOPortTarget1_ISRInstance	1

#define TNUM_INIRTN	3

#define TNUM_TERRTN	2

#endif /* TOPPERS_KERNEL_CFG_H */
