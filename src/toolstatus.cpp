#include "toolstatus.h"
#include "mcp2517fd.h"
#include <QDebug>

ToolStatus::ToolStatus()
{
    errorFlags = busState = txErrors = rxErrors = 0;
    mode = CONFIG_MODE;
    systemErrors = 0;
}

void ToolStatus::Update(USBDevice *usb) {
    if (!usb->IsConnected()) {
        return;
    }
    uint8_t data[8];
    usb->ReadControl(GET_STATUS, data, 8);
    errorFlags = data[0];
    busState = data[1];
    rxErrors = data[2];
    txErrors = data[3];
    mode = data[4];
    systemErrors = data[5];
}

HardwareInfo::HardwareInfo() {
    nominalBaud = N_125K;
    dataBaud = D_1M;
    txQueueLength = 0;
    txFIFOLength = 1;
    rxFIFOLength = 0;
}

void HardwareInfo::Update(USBDevice *usb) {
    if (!usb->IsConnected()) {
        return;
    }
    uint8_t data[8];
    usb->ReadControl(GET_HARDWARE_INFO, data, 8);
    nominalBaud = data[0];
    dataBaud = data[1];
    txQueueLength = data[2];
    txFIFOLength = data[3];
    rxFIFOLength = data[4];
}
