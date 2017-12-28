#ifndef QPREPUBLICPCTL_H
#define QPREPUBLICPCTL_H

#include <QObject>

//
//QRunRecordInfo 公共报文
//
//处理任务：
//

typedef struct{
  //========================原始数据
   uchar   m_uMainFrameByte[64];//0x03帧数据
}TPublicByte;
//（4）	机车号报文
typedef struct{
    uchar m_uTrainNumberByte[32];

}TTrainNumberByte;
//（1）	以太网精确时间报文
typedef struct{
    uchar m_uLocalTimeByte[22];
}TTimeByte;
class QPrePublicPctl : public QObject
{
    Q_OBJECT
public:

    void pdtPublicPtl();
    void pdtTrainNumberPlt();
    void pdtTimePlt();
    void sendUdpData(QByteArray &_bArrayData);
    static QPrePublicPctl* getInstance()
      {
          static QPrePublicPctl instance;
          return &instance;
      }
private:
    explicit QPrePublicPctl(QObject *parent = 0);
signals:
    void signalSendUdpData(QByteArray &_bArrayData);
public slots:
};

#endif // QPREPUBLICPCTL_H
