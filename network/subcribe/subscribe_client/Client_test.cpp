#include "Client_test.h"
#include <QDateTime>
#include <QDebug>
#include "/project/ProjectCommon/Include/mydebug.h"
void receiveTrainItem(QByteArray _data);

Client_test::Client_test(QObject *parent) :
    QObject(parent)
{
    //订阅机车项点报警
    m_pTrainItemClnt = new SubScribe::SubscribeClient();
    m_pTrainItemClnt->init(20886);
    SubScribe::QClientInfo *client = new SubScribe::QClientInfo("192.168.1.231", 10885,true,0,this);
    connect(client, SIGNAL(signalReadData(QByteArray,quint16,QString)), this, SLOT(slotReadData(QByteArray,quint16,QString)));
    m_pTrainItemClnt->AddSubscribe("127.0.0.1",10883,true);
    m_pTrainItemClnt->AddSubscribe(*client);
    connect(m_pTrainItemClnt, SIGNAL(signalPushNotify(QByteArray,quint16,QString)), this, SLOT(slotReadData(QByteArray,quint16,QString)));
    m_pTrainItemClnt->moveToThread(&m_thread);
    m_thread.start();
}

void Client_test::slotReadData(QByteArray data, quint16 port, QString ip)
{
    FUNC_IN;
    TRACE("ip:%s", ip.toStdString().c_str());
    if (ip == "127.0.0.1")
    {
    }
    TRACE("port :%d", port);
    if (port == 10883)
    {
    }
    receiveTrainItem(data);
}

void Client_test::receiveTrainItem(QByteArray _data)
{
    printf("[%s][%s][%d]\n",__FILE__,__FUNCTION__,__LINE__);
    QDataStream _in(&_data,QIODevice::ReadOnly);

    int itemNum = 0;

    QString videoName;
    QDateTime time;
    uchar  checkCharacter = 0;
    _in >> checkCharacter;
    _in >> itemNum;
    _in >> time;
    _in >> videoName;
    printf("receiveTrainItem verify failed!! receive check byte:%x, receive data:0x%x\n", checkCharacter, itemNum);
    qDebug() << time.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << videoName;
}
