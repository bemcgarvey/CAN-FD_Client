#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <QString>
#include <Windows.h>
#include <winusb.h>
#include <stdint.h>

class USBDevice
{
public:
    USBDevice();
    ~USBDevice();
    bool Open();
    void Close();
    bool IsConnected() {return (deviceHandle != INVALID_HANDLE_VALUE) && (winUSBHandle != INVALID_HANDLE_VALUE);}
    enum PipeID {TXQ = 0x01, TXFIFO = 0x02, RXFIFO = 0x83};
    bool WritePipe(PipeID id, uint8_t *buffer, unsigned int len);
    bool ReadPipe(PipeID id, uint8_t *buffer, unsigned int len);
    bool SendControl(uint8_t request, uint8_t *data, uint16_t len, uint16_t wValue = 0, uint16_t wIndex = 0);
    bool ReadControl(uint8_t request, uint8_t *data, uint16_t len, uint16_t wValue = 0, uint16_t wIndex = 0);
private:
    QString getDevicePath();
    WINUSB_INTERFACE_HANDLE winUSBHandle;
    HANDLE                  deviceHandle;
};

#endif // USBDEVICE_H
