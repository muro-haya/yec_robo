/*
 * This file was automatically generated by tecsgen.
 * This file is not intended to be edited.
 */
#include "tSerialAdapter_tecsgen.h"
#include "tSerialAdapter_factory.h"


/* entry port descriptor referenced by call port (differ from actual definition) #_CPEPD_# */
extern struct tag_sSerialPort_VDES SerialPort1_SerialPortMain_eSerialPort_des;
extern struct tag_sSerialPort_VDES SerialPortUSB1_SerialPortMain_eSerialPort_des;
extern struct tag_sSerialPort_VDES SerialPortBluetooth1_SerialPortMain_eSerialPort_des;
extern struct tag_sSerialPort_VDES SerialPortTest1_SerialPortMain_eSerialPort_des;

/* call port array #_CPA_# */
struct tag_sSerialPort_VDES * const SerialAdapter_cSerialPort[] = {
    &SerialPort1_SerialPortMain_eSerialPort_des,
    &SerialPortUSB1_SerialPortMain_eSerialPort_des,
    &SerialPortBluetooth1_SerialPortMain_eSerialPort_des,
    &SerialPortTest1_SerialPortMain_eSerialPort_des,
};

/* array of attr/var #_AVAI_# */
/* cell INIB #_INIB_# */
tSerialAdapter_INIB tSerialAdapter_SINGLE_CELL_INIB = 
{
    /* call port (INIB) #_CP_# */ 
    SerialAdapter_cSerialPort,               /* #_CCP3B_# */
    4,                                       /* length of cSerialPort (n_cSerialPort) #_CCP4_# */
};

/* entry port descriptor #_EPD_# */
/* CB initialize code #_CIC_# */
void
tSerialAdapter_CB_initialize()
{
    SET_CB_INIB_POINTER(i,p_cb)
    INITIALIZE_CB()
}
