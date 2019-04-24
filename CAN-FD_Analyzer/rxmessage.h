#ifndef RXMESSAGE_H
#define RXMESSAGE_H

#include <stdint.h>

class RxMessage
{
public:
    RxMessage();
    RxMessage(uint8_t *buffer);
    uint32_t id;
    bool ide;
    bool rtr;
    bool fdf;
    bool brs;
    int dlc;
    uint32_t timeStamp;
    uint8_t data[64];
    static unsigned int dlcToLen(int dlc);
};

#endif // RXMESSAGE_H
