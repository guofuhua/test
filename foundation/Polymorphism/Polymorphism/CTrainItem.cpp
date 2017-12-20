#include "CTrainItem.h"

CTrainItem::CTrainItem()
{
    m_item = None;
    qDebug() << "CTrainItem !!";
}

CTrainItem::CTrainItem(EMTrainItem item)
{
    m_item = item;
    qDebug() << "CTrainItem item !!";
}

bool CTrainItem::isAlarmType()
{
    qDebug() << "CTrainItem" << getItem();
    return false;
}

//
//FUCTION: 获取当前项点
//PARAMETER：
//RETURN：NO
EMTrainItem CTrainItem::getItem()
{
    return m_item;
}

//
//FUCTION: 设置当前项点
//PARAMETER：
//RETURN：NO
void CTrainItem::setItem(EMTrainItem item)
{
    m_item = item;
    qDebug() << "item " << m_item;
}

//
//FUCTION: 项点编号的枚举类型转换成int类型
//PARAMETER：
//RETURN：NO
int CTrainItem::EMTrainItemToInt(EMTrainItem item)
{
    int value = 0;
    switch(item)
    {
    case ARRIVING:
        value = 1;
        break;

    case LEAVING:
        value = 2;
        break;

    case WAYSIDE_STOP:
        value = 3;
        break;

    case STATION_STOP:
        value = 4;
        break;

    case PIPE_PRESSURE_ZERO:
        value = 5;
        break;

    case SIGNAL_DEGRADE:
        value = 6;
        break;

    case SHUNTING_STARTING:
        value = 7;
        break;

    case TRAIN_STARTING:
        value = 8;
        break;

    case SIGNAL_HULT:
        value = 9;
        break;

    case ABNORMAL_STATION_STOP:
        value = 10;
        break;

    default:
        value = 0;
        break;
    }

    return value;
}
