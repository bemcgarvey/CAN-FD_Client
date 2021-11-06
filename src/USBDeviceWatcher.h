#ifndef QHIDWATCHER_H
#define QHIDWATCHER_H

#include <QWidget>
#include <stdint.h>


#define MY_VID             0x4d63

class USBDeviceWatcher : public QWidget
{
    Q_OBJECT
public:
    explicit USBDeviceWatcher(QWidget *parent, uint16_t pid, uint16_t vid = MY_VID);
    ~USBDeviceWatcher(void);
signals:
    void connected(void);
    void removed(void);
private:
    QString pid;
    QString vid;
    void *DeviceNotificationHandle;
protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
};

#endif // QHIDWATCHER_H
