#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H

#include <QObject>
#include "SubscribeClient.h"
#include <QThread>

class Client_test : public QObject
{
    Q_OBJECT
public:
    explicit Client_test(QObject *parent = 0);
    SubScribe::SubscribeClient* m_pTrainItemClnt;  //订阅机车项点报警订阅客户端
    QThread m_thread;
    
    void receiveTrainItem(QByteArray _data);
signals:
    
public slots:
    void slotReadData(QByteArray data, quint16 port, QString ip);
    
};

#endif // CLIENT_TEST_H
