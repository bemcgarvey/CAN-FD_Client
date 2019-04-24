#include "canfilter.h"

CANFilter::CANFilter()
{
    mask = filter = 0;
    EXIDE = MIDE = active = false;
}
