#include "usbdevice.h"
#include <setupapi.h>
#include <cfgmgr32.h>
#include <QDebug>

USBDevice::USBDevice()
{
    deviceHandle = INVALID_HANDLE_VALUE;
    winUSBHandle = INVALID_HANDLE_VALUE;
}

USBDevice::~USBDevice() {
    Close();
}

bool USBDevice::Open() {
    QString path = getDevicePath();
    if (path.length() == 0) {
        return false;
    }
    BOOL    bResult;
    WCHAR wpath[250];
    path.toWCharArray(wpath);
    wpath[path.length()] = '\0';  //Append '\0' since toWCharArray does not.
    deviceHandle = CreateFile(wpath,
                              GENERIC_WRITE | GENERIC_READ,
                              FILE_SHARE_WRITE | FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                              NULL);
    if (INVALID_HANDLE_VALUE == deviceHandle) {
        return false;
    }
    bResult = WinUsb_Initialize(deviceHandle, &winUSBHandle);
    if (FALSE == bResult) {
        CloseHandle(deviceHandle);
        return false;
    }
    int timeout = 100;
    BOOL value = TRUE;
    BOOL result;
    result = WinUsb_SetPipePolicy(winUSBHandle, RXFIFO, RAW_IO, sizeof(value), &value);
    result = WinUsb_SetPipePolicy(winUSBHandle, RXFIFO, PIPE_TRANSFER_TIMEOUT, sizeof(int), &timeout);
    return true;
}

void USBDevice::Close() {
    if (deviceHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(deviceHandle);
        deviceHandle = INVALID_HANDLE_VALUE;
    }
    if (winUSBHandle != INVALID_HANDLE_VALUE) {
        WinUsb_Free(winUSBHandle);
        winUSBHandle = INVALID_HANDLE_VALUE;
    }
}

QString USBDevice::getDevicePath() {
        CONFIGRET cr = CR_SUCCESS;
        HRESULT   hr = S_OK;
        PTSTR     DeviceInterfaceList = NULL;
        ULONG     DeviceInterfaceListLength = 0;
        GUID GUID_DEVINTERFACE_CAN_FD_ANALYZER = {0xf68fb972, 0xc07f, 0x4e61, {0x87, 0xa1, 0xf5, 0xa5, 0xfe, 0x93, 0xfd, 0x0c}};

        // Enumerate all devices exposing the interface. Do this in a loop
        // in case a new interface is discovered while this code is executing,
        // causing CM_Get_Device_Interface_List to return CR_BUFFER_SMALL.
        //
        do {
            cr = CM_Get_Device_Interface_List_Size(&DeviceInterfaceListLength,
                                                   (LPGUID)&GUID_DEVINTERFACE_CAN_FD_ANALYZER,
                                                   NULL,
                                                   CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

            if (cr != CR_SUCCESS) {
                hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                break;
            }

            DeviceInterfaceList = (PTSTR)HeapAlloc(GetProcessHeap(),
                                                   HEAP_ZERO_MEMORY,
                                                   DeviceInterfaceListLength * sizeof(TCHAR));

            if (DeviceInterfaceList == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            cr = CM_Get_Device_Interface_List((LPGUID)&GUID_DEVINTERFACE_CAN_FD_ANALYZER,
                                              NULL,
                                              DeviceInterfaceList,
                                              DeviceInterfaceListLength,
                                              CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

            if (cr != CR_SUCCESS) {
                HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

                if (cr != CR_BUFFER_SMALL) {
                    hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                }
            }
        } while (cr == CR_BUFFER_SMALL);

        if (FAILED(hr)) {
            return "";
        }

        //
        // If the interface list is empty, no devices were found.
        //
        if (*DeviceInterfaceList == TEXT('\0')) {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);
            return "";
        }

        //
        // Give path of the first found device interface instance to the caller. CM_Get_Device_Interface_List ensured
        // the instance is NULL-terminated.
        //
        QString path = QString::fromWCharArray(DeviceInterfaceList);

        HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

        return path;
}

bool USBDevice::WritePipe(PipeID id, uint8_t *buffer, unsigned int len) {
    BOOL result;
    ULONG lenTransferred;
    result = WinUsb_WritePipe(winUSBHandle, id, buffer, len, &lenTransferred, NULL);
    return result == TRUE;
}

bool USBDevice::ReadPipe(PipeID id, uint8_t *buffer, unsigned int len) {
    BOOL result;
    ULONG lenTransferred;
    result = WinUsb_ReadPipe(winUSBHandle, id, buffer, len, &lenTransferred, NULL);
    if (result == TRUE && len == lenTransferred)
        return true;
    else
        return false;
}

bool USBDevice::SendControl(uint8_t request, uint8_t *data, uint16_t len, uint16_t wValue, uint16_t wIndex) {
    WINUSB_SETUP_PACKET setupPacket;
    setupPacket.RequestType = 0x41;
    setupPacket.Request = request;
    setupPacket.Length = len;
    setupPacket.Value = wValue;
    setupPacket.Index = wIndex;
    BOOL bResult = TRUE;
    ULONG actual;
    bResult = WinUsb_ControlTransfer(winUSBHandle, setupPacket, data, len, &actual, 0);
    if (bResult && actual == len) {
        return true;
    } else {
        return false;
    }
}

bool USBDevice::ReadControl(uint8_t request, uint8_t *data, uint16_t len, uint16_t wValue, uint16_t wIndex) {
    WINUSB_SETUP_PACKET setupPacket;
    setupPacket.RequestType = 0xc1;
    setupPacket.Request = request;
    setupPacket.Length = len;
    setupPacket.Value = wValue;
    setupPacket.Index = wIndex;
    BOOL bResult = TRUE;
    ULONG actual;
    bResult = WinUsb_ControlTransfer(winUSBHandle, setupPacket, data, len, &actual, 0);
    if (bResult && actual == len) {
        return true;
    } else {
        return false;
    }
}
