#include "CItemWaysideStop.h"

CItemWaysideStop::CItemWaysideStop(EMTrainItem item)
{
    setItem(item);
//    qDebug() << "CItemWaysideStop item !!";
    initvariable();
}


bool CItemWaysideStop::isAlarmType()
{
    qDebug() << "CItemWaysideStop" << getItem();
    return true;
}

void CItemWaysideStop::initvariable()
{
    qDebug() << "CItemWaysideStop initvariable";
}
