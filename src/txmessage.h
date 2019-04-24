#ifndef TXMESSAGE_H
#define TXMESSAGE_H

#include <stdint.h>
#include <QDataStream>

class TxMessage
{
public:
    TxMessage();
    uint32_t id;
    bool ide;
    bool rtr;
    bool fdf;
    bool brs;
    int dlc;
    uint8_t data[64];
    uint8_t ToBytes(uint8_t *buffer) const;
    void ToDataStream(QDataStream &out);
    void FromDataStream(QDataStream &in);
    static unsigned int dlcToLen(int dlc);
};

#endif // TXMESSAGE_H
