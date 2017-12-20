#include "CItemStop.h"

CItemStop::CItemStop(EMTrainItem item)
{
    setItem(item);
    initdata();
//    qDebug() << "CItemStop !!";
}

bool CItemStop::isAlarmType()
{
//    qDebug() << "CItemStop" << getItem();
    return true;
}

void CItemStop::initdata()
{
    initvariable();
    qDebug() << "CItemStop initdata";
}

void CItemStop::initvariable()
{
    qDebug() << "CItemStop initvariable";
}
