#ifndef RXTHREAD_H
#define RXTHREAD_H

#include <QThread>
#include "usbdevice.h"
#include "rxmessage.h"

class RxThread : public QThread
{
    Q_OBJECT
public:
    RxThread(USBDevice *pUsb);
    void Stop();
    void run();
signals:
    void MessageReady(RxMessage *m);
private:
    USBDevice *usb;
    bool running;
    enum {WaitingForHeader, WaitingForBody, Processing} state;
};

#endif // RXTHREAD_H
