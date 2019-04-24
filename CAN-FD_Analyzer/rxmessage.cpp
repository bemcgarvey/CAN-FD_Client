#include "rxmessage.h"

RxMessage::RxMessage() {
    ide = false;
    brs = false;
    fdf = false;
    rtr = false;
    dlc = 0;
    timeStamp = 0;
    id = 0;
}

RxMessage::RxMessage(uint8_t *buffer)
{
    ide = false;
    brs = false;
    fdf = false;
    rtr = false;
    dlc = buffer[4] & 0x0f;
    uint32_t *pid = reinterpret_cast<uint32_t *>(buffer);
    id = *pid & 0x7ff;
    if (buffer[4] & 0x10) {
        ide = true;
        id <<= 18;
        id |= (*pid >> 11);
    }
    if (buffer[4] & 0x20) {
        rtr = true;
    }
    if (buffer[4] & 0x40) {
        brs = true;
    }
    if (buffer[4] & 0x80) {
        fdf = true;
    }
    timeStamp = *reinterpret_cast<uint32_t *>(&buffer[8]);
    unsigned int dataLen = dlcToLen(dlc);
    for (unsigned int i = 0; i < dataLen; ++i) {
        data[i] = buffer[12 + i];
    }
}

unsigned int RxMessage::dlcToLen(int dlc) {
    if (dlc == 0) {
        return 0;
    } else if (dlc <= 4) {
        return 4;
    } else if (dlc <= 8) {
        return 8;
    } else {
        switch (dlc) {
        case 9: return 12;
        case 10: return 16;
        case 11: return 20;
        case 12: return 24;
        case 13: return 32;
        case 14: return 48;
        case 15: return 64;
        default: return 0;
        }
    }
}
