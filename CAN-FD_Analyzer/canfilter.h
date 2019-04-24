#ifndef CANFILTER_H
#define CANFILTER_H

#include <stdint.h>

class CANFilter
{
public:
    CANFilter();
    uint32_t mask;
    uint32_t filter;
    bool EXIDE;
    bool MIDE;
    bool active;
};

#endif // CANFILTER_H
