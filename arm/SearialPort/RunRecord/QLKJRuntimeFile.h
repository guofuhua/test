#ifndef QLKJRUNTIMEFILE_H
#define QLKJRUNTIMEFILE_H

#include <QObject>
#include "Entity/LKJRuntimeTypes.h"
#include <QDebug>
#include <QVector>

////////////////////////////////////////////////////////////////////////////////
//TLKJRuntimeFileRec LKJ运行记录文件中记录基类，所有类型的记录都从此类派生}
////////////////////////////////////////////////////////////////////////////////
class TLKJRuntimeFileRec
{
public:
    //{功能：将类中的内容转化为字符串}
    virtual QString ToString()=0;
    //{功能：根据字符串中的内容设置类中各个成员的值}
    virtual void FromString(QString strText)=0;
};

///////////////////////////////////////////////////////////////////////////////
//TLKJCommonRec 全程记录通用记录信息
///////////////////////////////////////////////////////////////////////////////
class QLKJCommonRec : public TLKJRuntimeFileRec
{
public:
    QLKJCommonRec()
    {
        this->CommonRec.nRow = 0;               //全程记录行号
        this->CommonRec.nEvent = 0;             //事件数字代码
        this->CommonRec.nCoord = 0;             //公里标
        this->CommonRec.nDistance = 0;          //距信号机距离
        this->CommonRec.nLampNo = 0;            //信号机编号6244
        this->CommonRec.nSpeed = 0;             //运行速度
        this->CommonRec.nLimitSpeed = 0;        //限制速度
        this->CommonRec.nShoub = 0;           //手柄状态
        this->CommonRec.nLieGuanPressure = 0;   //列管压力 - Gya
        this->CommonRec.nGangPressure = 0;      //缸压力
        this->CommonRec.nRotate = 0;            //转速
        this->CommonRec.nJG1Pressure = 0;       //均缸1压力
        this->CommonRec.nJG2Pressure = 0;       //均缸2压力
        this->CommonRec.nJKLineID = 0;          //当前交路号
        this->CommonRec.nDataLineID = 0;        //当前数据交路号
        this->CommonRec.nStation = 0;           //已过车站号
        this->CommonRec.nToJKLineID = 0;        //上一个站的交路号
        this->CommonRec.nToDataLineID = 0;      //上一个站的数据交路号
        this->CommonRec.nToStation = 0;         //上一个站编号
        this->CommonRec.nStationIndex = 0;      //从始发站开始战间编号
        this->CommonRec.JKZT = 0;               //监控状态  监控＝1，平调＝2，调监＝3
        this->CommonRec.nValidJG = 0;           //有效均缸号
        this->CommonRec.nStreamPostion = 0;     //流位置

        this->CommonRec.strDisp.clear();        //事件描述
        this->CommonRec.strSignal.clear();      //色灯
        this->CommonRec.strXhj.clear();         //信号机
        this->CommonRec.strGK.clear();          //工况状态
        this->CommonRec.strOther.clear();       //其他
        this->CommonRec.ShuoMing.clear();       //说明
        this->CommonRec.strCaption.clear();     //标题
        this->CommonRec.DTEvent = 0;      //事件发生时间
        this->CommonRec.LampSign = lsPDNone;   //灯信号
        this->CommonRec.SignType = stNone;//信号机类型
        this->CommonRec.WorkZero = wAtZero;   //零位[零, 非]
        this->CommonRec.HandPos = hpInvalid;     //前后[前, 后]
        this->CommonRec.WorkDrag = wdInvalid;   //牵制[牵, 制]
    }

    QString ToString()
    {
        QString string = QString::number(this->CommonRec.nRow)+"  "+
                QString::number(this->CommonRec.DTEvent)+"  "+
                QString::number(this->CommonRec.nEvent)+"  "+
                QString::number(this->CommonRec.nCoord)+"  "+
                QString::number(this->CommonRec.nDistance)+"  "+
                QString::number(this->CommonRec.nLampNo)+"  "+
                QString::number(this->CommonRec.nSpeed)+"  "+
                QString::number(this->CommonRec.nLimitSpeed)+"  "+
                QString::number(this->CommonRec.nLieGuanPressure)+"  "+
                ((wAtZero == this->CommonRec.WorkZero)?"true":"false")+"  "+
                QString::number(this->CommonRec.nRotate)+"  "+
                QString::number(this->CommonRec.nJG1Pressure)+"  "+
                QString::number(this->CommonRec.nJG2Pressure);
        return string;
    }

    void FromString(QString strText)
    {
        qDebug() << strText;
    }
public:
    RCommonRec CommonRec; //全程记录通用记录信息
};

typedef QList<QLKJCommonRec> TLkjCommonRecLst;

////////////////////////////////////////////////////////////////////////////////
// 公共类
// 功能：存贮解格式化文件信息
////////////////////////////////////////////////////////////////////////////////
class QLKJRuntimeFile : public QObject
{
    Q_OBJECT
public:
    explicit QLKJRuntimeFile(QObject *parent = 0);
    RLKJRTFileHeadInfo HeadInfo; //文件基础信息
    QVector<RLKJJieShiInfo> JieShiInfoArr;  //存储的揭示信息
    bool bLoaded;
    //{功能：获取文件头信息}
    void GetHeadInfoFromOrg(QString orgFileName, RLKJRTFileHeadInfo &HeadInfo);
    //{功能：从原始文件加载运行记录}
    void LoadFromOrgFile(QString FileName);
//    void LoadFromOrgFileList(QStringList FileNames);
    void Clear();
    void DebugHeadInfo(RLKJRTFileHeadInfo info);
    void SyncFile();
//protected:
    TLkjCommonRecLst m_Records; //全程记录列表
    void DebugRecords();
    void DebugOneRecord(RCommonRec info);
    QString TranslateLamp(EM_LampSign LampSign);
    QString TranslateSignType(EM_LKJSignType Sign);
    QString TranslateWorkDrag(EM_WorkDrag WorkDrag);
    QString TranslateHandPos(EM_HandPos HandPos);
    QString TranslateWorkZero(EM_WorkZero WorkZero);
    QString CheseFill(QString info, int W, bool left = true);
    void DebugJieShiInfo();
    void DebugOneJieShiInfo(RLKJJieShiInfo info);
    QString GetInfo_GK(uchar nType);
    void toTXTData(unsigned char *_data, RCommonRec &info);
    uchar LampToTax(EM_LampSign LampSign);
    QByteArray getBroadcastData(const RCommonRec &src);
signals:
    
public slots:
    
};

#endif // QLKJRUNTIMEFILE_H
