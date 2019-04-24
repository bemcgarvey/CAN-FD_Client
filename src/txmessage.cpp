#include "txmessage.h"

TxMessage::TxMessage()
{
    id = 0;
    ide = false;
    fdf = false;
    brs = false;
    rtr = false;
    dlc = 0;
    for (int i = 0; i < 64; ++i) {
        data[i] = 0;
    }
}

uint8_t TxMessage::ToBytes(uint8_t *buffer) const {
    uint32_t *pid = reinterpret_cast<uint32_t *>(buffer);
    if (ide) {
        *pid = (id >> 18) & 0x7ff;
        *pid |= (id & 0x3ffff) << 11;
    } else {
        *pid = id & 0x7ff;
    }
    buffer[4] = dlc & 0x0f;
    if (ide) {
        buffer[4] |= 0x10;
    }
    if (rtr) {
        buffer[4] |= 0x20;
    }
    if (brs) {
        buffer[4] |= 0x40;
    }
    if (fdf) {
        buffer[4] |= 0x80;
    }
    for (unsigned int i = 0; i < dlcToLen(dlc); ++i) {
        buffer[8 + i] = data[i];
    }
    return static_cast<uint8_t>(8 + dlcToLen(dlc));
}

void TxMessage::ToDataStream(QDataStream &out)
{
    out << static_cast<quint32>(id);
    out << ide;
    out << rtr;
    out << fdf;
    out << brs;
    out << dlc;
    for (int i = 0; i < 64; ++i) {
        out << static_cast<quint8>(data[i]);
    }
}

void TxMessage::FromDataStream(QDataStream &in)
{
    in >> id;
    in >> ide;
    in >> rtr;
    in >> fdf;
    in >> brs;
    in >> dlc;
    for (int i = 0; i < 64; ++i) {
        in >> data[i];
    }
}

unsigned int TxMessage::dlcToLen(int dlc) {
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
