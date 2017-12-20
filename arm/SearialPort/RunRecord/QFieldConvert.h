#ifndef QFIELDCONVERT_H
#define QFIELDCONVERT_H

#include <QObject>
#include "Entity/LKJRuntimeTypes.h"
#include <QMap>

class QFieldConvert : public QObject
{
    Q_OBJECT
public:
    explicit QFieldConvert(QObject *parent = 0);
    //事件描述集
    QMap<QString, int> m_EventDisp;

private:
    //{功能：初始化事件描述}
    void InitEventDisp();
public:
    //{功能：获取事件代码}
    int GetnEvent(QString Disp);
    //{功能：获取公里标信息}
    int GetnCoord(QString strCoord);
    QString ConvertCoordToStr(int nCoor);
    //{功能：获取零位/非零手柄状态}
    EM_WorkZero ConvertWorkZero(uchar bHandle);
    //{功能：获取牵引/制动手柄状态}
    EM_WorkDrag ConvertWorkDrag(uchar bHandle);
    //{功能：获取前/后手柄状态}
    EM_HandPos ConvertHandPos(uchar bHandle);
    //{功能：获取信号状态}
    EM_LampSign ConvertSignal(uchar nSignal);
    //{功能：获取信号机类型状态}
    EM_LKJSignType ConvertSignType(uchar nxhj_type);
    //{功能：获取车次信息}
    void GetCheCiInfo(QString FieldOther, int CheCi, QString TrainHead);
    //{功能：获取客货、本补信息}
    void GetKeHuoBenBu(QString FieldOther, EM_LKJTrainType TrainType, EM_LKJBenBu BenBu);
    //{功能：获取监控厂家信息}
    EM_LKJFactory  GetJkFactoryInfo(QString FieldOther);
    //{功能：获取乘务员工号}
    int GetDriverNo(QString FieldOther);
    //{功能：获取机车号}
    int GetLocalID(QString FieldOther);

signals:
    
public slots:
    
};

#endif // QFIELDCONVERT_H
