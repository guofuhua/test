#ifndef CITEMSTOP_H
#define CITEMSTOP_H
#include "CTrainItem.h"

class CItemStop : public CTrainItem
{
public:
    CItemStop(EMTrainItem item = None);
    bool isAlarmType();
    void initdata();
    virtual void initvariable(void);
};

#endif // CITEMSTOP_H
