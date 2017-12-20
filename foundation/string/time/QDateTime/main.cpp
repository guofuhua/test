#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

typedef struct RCommonRec
{
    int nRow;               //全程记录行号
    QString strDisp;        //事件描述
    int nEvent;             //事件数字代码
    uint DTEvent;           //事件发生时间
    int nCoord;             //公里标
    int nDistance;          //距信号机距离
    int LampSign;   //灯信号
    QString strSignal;      //色灯
    int nLampNo;            //信号机编号6244
    int SignType;//信号机类型
    QString strXhj;         //信号机
    int nSpeed;             //运行速度
    int nLimitSpeed;        //限制速度
    uchar nShoub;           //手柄状态
    QString strGK;          //工况状态
    int WorkZero;   //零位[零, 非]
    int HandPos;     //前后[前, 后]
    int WorkDrag;   //牵制[牵, 制]
    int nLieGuanPressure;   //列管压力 - Gya
    int nGangPressure;      //缸压力
    int nRotate;            //转速
    int nJG1Pressure;       //均缸1压力
    int nJG2Pressure;       //均缸2压力
    QString strOther;       //其他
    int nJKLineID;          //当前交路号
    int nDataLineID;        //当前数据交路号
    int nStation;           //已过车站号
    int nToJKLineID;        //上一个站的交路号
    int nToDataLineID;      //上一个站的数据交路号
    int nToStation;         //上一个站编号
    int nStationIndex;      //从始发站开始战间编号
    QString ShuoMing;       //说明
    int JKZT;               //监控状态  监控＝1，平调＝2，调监＝3
    int nValidJG;           //有效均缸号
    int nStreamPostion;     //流位置
    QString strCaption;     //标题
}RCommonRec;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QByteArray data;
//    data.resize(1*16);
    char buf[] = {0x00, 0x12, 0x42, 0xbf, 0x93, 0x02, 0x12, 0x44, 0xbf, 0x93, 0x00, 0x12, 0x42, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d};
//    QDataStream stream(&data, QIODevice::ReadWrite);
    data = QByteArray(buf, sizeof(buf));
    qDebug() << data.toHex();
    qDebug() << data.c().toHex();
    int nTrainNo = 0x99001;
    int nDriverNo = 0x88888;
    int size = 0x880000;
//    stream.setVersion(QDataStream::Qt_4_6);
//    for (int i = 0; i < m_fileInfo.size(); i++) {
        QDateTime dateTime = QDateTime::fromTime_t(0);
        QString condition = QString(" and RecordDateTime >= '%1' and RecordDateTime <= '%2'").arg(dateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
        qDebug() << condition;
        quint64 data1 = 0x14d73c72ab0;
        uint data2 = data1;
        qDebug() << data2 << data1 << (uint)data1;

        QString test("   3");
        qDebug() << test << test.simplified();

    return a.exec();
}
