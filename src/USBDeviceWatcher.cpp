
#include "USBDeviceWatcher.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <Dbt.h>
#include <QDebug>
#include <QEvent>

USBDeviceWatcher::USBDeviceWatcher(QWidget *parent, uint16_t pid, uint16_t vid) : QWidget(parent) {
    setVisible(false);
    this->vid = QString::asprintf("vid_%04x", vid);
    this->pid = QString::asprintf("pid_%04x", pid);
    HWND hwnd = reinterpret_cast<HWND>(winId());
    GUID guid = {0xf68fb972, 0xc07f, 0x4e61, {0x87, 0xa1, 0xf5, 0xa5, 0xfe, 0x93, 0xfd, 0x0c}};
    DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;
    DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
    DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    DevBroadcastDeviceInterface.dbcc_classguid = guid;
    DeviceNotificationHandle = RegisterDeviceNotification(
        hwnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
};

USBDeviceWatcher::~USBDeviceWatcher(void) {
    UnregisterDeviceNotification(DeviceNotificationHandle);
}

bool USBDeviceWatcher::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *msg = reinterpret_cast<MSG *>(message);
    if (msg->message == WM_DEVICECHANGE) {
        DEV_BROADCAST_DEVICEINTERFACE *pdevBroadCastHdr;
        if (msg->lParam != 0) {
            pdevBroadCastHdr = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(msg->lParam);
            if (pdevBroadCastHdr->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                QString path;
                switch (msg->wParam) {
                case DBT_DEVICEARRIVAL:
                    path = path.fromWCharArray(pdevBroadCastHdr->dbcc_name).toLower();
                    if (path.contains(vid) && (path.contains(pid))) {
                        emit connected();
                    }
                    *result = 0;
                    return true;
                case DBT_DEVICEREMOVECOMPLETE:
                    path = path.fromWCharArray(pdevBroadCastHdr->dbcc_name).toLower();
                    if (path.contains(vid) && (path.contains(pid))) {
                        emit removed();
                    }
                    *result = 0;
                    return true;
                }
            }
        }
    }
    return false;
}
