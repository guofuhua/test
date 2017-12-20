#ifndef QTCPSENDRUNRECORD_H
#define QTCPSENDRUNRECORD_H

#include <QObject>
#include <QThread>
#include "Subscribe/SubscribeServer/SubscribeServer.h"

class QTcpSendRecThread : public QThread
{
    Q_OBJECT
public:
    explicit QTcpSendRecThread(QObject *parent = 0);
    void SendTrainItem();
    void setSendOver(bool isOver);
    bool getSendOver();
    SubServer::SubscribeServer* m_pSubAlarmSvr;//订阅服务器
    bool isSendOver;
    QMutex m_mutex;
    void run();

signals:
    
public slots:
    void slotNewData();
};

#endif // QTCPSENDRUNRECORD_H
