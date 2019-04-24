#ifndef TOOLSTATUS_H
#define TOOLSTATUS_H

#include "usbdevice.h"

class ToolStatus
{
public:
    ToolStatus();
    void Update(USBDevice *usb);
    enum BusStates {RX_WARN = 0x02, RX_PASSIVE = 0x08, TX_WARN = 0x04, TX_PASSIVE = 0x10,
                    TX_BUS_OFF = 0x20, E_WARN = 0x01};
    enum ErrorFlags {ERROR_IVM = 0x80, ERROR_CER = 0x20, ERROR_SYS = 0x10, ERROR_SER = 0x10,
                     ERROR_RXOV = 0x08, ERROR_USB_RX = 0x40, ERROR_USB_TX = 0x04,
                     ERROR_MODE_CHANGE = 0x01};
    int errorFlags;
    int busState;
    int txErrors;
    int rxErrors;
    int mode;
    int systemErrors;
};

class HardwareInfo {
public:
    HardwareInfo();
    void Update(USBDevice *usb);
    int nominalBaud;
    int dataBaud;
    int txQueueLength;
    int txFIFOLength;
    int rxFIFOLength;
};

#endif
