#ifndef CTRAINITEM_H
#define CTRAINITEM_H

#include "LocalCommon/predefine.h"
#include <QDebug>

class CTrainItem
{
public:
    CTrainItem();
    CTrainItem(EMTrainItem item);
    virtual bool isAlarmType();
    EMTrainItem getItem();
    void setItem(EMTrainItem item);
    static int EMTrainItemToInt(EMTrainItem item);

private:
    EMTrainItem m_item;
};

#endif // CTRAINITEM_H
