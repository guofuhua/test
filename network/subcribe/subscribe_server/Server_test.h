#ifndef SERVER_TEST_H
#define SERVER_TEST_H

#include <QObject>
#include "SubscribeServer.h"
#include <QTimer>
#include <QThread>

class Server_test : public QObject
{
    Q_OBJECT
public:
    explicit Server_test(QObject *parent = 0);
    SubScribe::SubscribeServer* m_pSubAlmSvr;//订阅服务器
    void AlarmToSubscriber();
    QTimer m_timer;
    QThread m_thread;

signals:
    void signalBroadCast(QByteArray data);
    void initSubscribe(int, QString);
    
public slots:
    void slotSubscribe();
};

#endif // SERVER_TEST_H
