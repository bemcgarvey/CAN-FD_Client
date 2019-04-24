#include "rxthread.h"
#include <QDebug>

RxThread::RxThread(USBDevice *pUsb) : QThread()
{
    running = false;
    usb = pUsb;
    state = WaitingForHeader;
}

void RxThread::Stop() {
    running = false;
}

void RxThread::run() {
    //uint8_t buffer[76];
    uint8_t buffer[128];
    unsigned int len = 0;
    running = true;
    bool result;
    while (running) {
        if (state == WaitingForHeader) {
            result = usb->ReadPipe(USBDevice::RXFIFO, buffer, 64);
            if (result) {
                len = buffer[0];
                if (len > 64) {
                    state = WaitingForBody;
                } else {
                    state = Processing;
                }
            }
        }
        if (state == WaitingForBody){
            result = usb->ReadPipe(USBDevice::RXFIFO, buffer + 64, 64);
            if (result) {
                state = Processing;
            }
        }
        if (state == Processing) {
            RxMessage *m = new RxMessage(buffer + 4);
            emit MessageReady(m);
            state = WaitingForHeader;
        }
//        if (state == WaitingForHeader) {
//            result = usb->ReadPipe(USBDevice::RXFIFO, buffer, 12);
//            if (result) {
//                len = RxMessage::dlcToLen(buffer[4] & 0x0f);
//                state = WaitingForBody;
//            }
//        } else if (state == WaitingForBody){
//            if (len == 0) {
//                state = Processing;
//            } else {
//                result = usb->ReadPipe(USBDevice::RXFIFO, buffer + 12, len);
//                if (result) {
//                    state = Processing;
//                }
//            }
//        } else if (state == Processing) {
//            RxMessage *m = new RxMessage(buffer);
//            emit MessageReady(m);
//            state = WaitingForHeader;
//        }
    }
}
