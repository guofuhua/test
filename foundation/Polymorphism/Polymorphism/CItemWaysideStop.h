#ifndef CITEMWAYSIDESTOP_H
#define CITEMWAYSIDESTOP_H
#include "CItemStop.h"

class CItemWaysideStop : public CItemStop
{
public:
    CItemWaysideStop(EMTrainItem item);
    bool isAlarmType();
    void initvariable();
};

#endif // CITEMWAYSIDESTOP_H
