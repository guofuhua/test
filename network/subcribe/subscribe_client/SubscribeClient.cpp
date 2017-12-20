#include "SubscribeClient.h"
#include "../SubscribeCommon/PublicFun.h"
#include "/project/ProjectCommon/Include/mydebug.h"
#include <QThread>
#define TIME_INTERVAL_MANAGE_SCAN   (3000)  //3000 ms

namespace SubScribe{

SubscribeClient::SubscribeClient(QObject *parent) :
    QObject(parent)
{
}

//
//FUCTION:初始化订阅客户端
//PARAMETER：port,订阅客户端的端口
//RETURN：No
bool SubscribeClient::init(int port)
{
    INFO("========================================SubscribeClient init  =========================================================== \n");

    //构建TCP服务器
    m_server = new QTcpServerExt();
    //关联成功接收连接的信号
    connect(m_server,SIGNAL(sigNewCon(int)),this,SLOT(slotNewCom(int)));
    //创建定时器，定期检查TCP连接
    m_managerTM = new QTimer();
    connect(m_managerTM,SIGNAL(timeout()), this, SLOT(slotManagerTM()));
    connect(m_managerTM,SIGNAL(timeout()), this, SLOT(slotCheckSubscibeState()));

    //监听端口
    m_port = port;
    bool ret = m_server->startListen(m_port);
    if (ret)
    {
        INFO("listen port:%d\n", m_port);
    }
    else
    {
        ERR("listern port:%d failed!!\n", m_port);
    }
    return ret;
}

//
//FUCTION:有新的TCP连接,创建一个客户端，处理连接
//PARAMETER：socketDescriptor,TCP连接的描述符
//RETURN：No
void SubscribeClient::slotNewCom(int socketDescriptor)
{
    INFO("==================================== new  connect ========================== \n");
    //构建客户端对象
    QConnectClient *pClientJob = new QConnectClient(socketDescriptor);
    //获取远端IP和端口
    QString strIPAndPort = pClientJob->getIPAndPort();
    //IP-port列表
    m_listIPAndPorts.append(strIPAndPort);
    //客户端列表
    m_listClients.append(pClientJob);
    //更新连接列表
    connect(pClientJob, SIGNAL(sigCallManageDeleteClient(QString)), this, SLOT(slotDeleteOneClient(QString)));
    //接收到数据，通过信号转发出去
    connect(pClientJob, SIGNAL(signalReadData(QByteArray,quint16,QString)), this, SIGNAL(signalPushNotify(QByteArray,quint16,QString)));
    connect(pClientJob, SIGNAL(signalReadData(QByteArray,quint16,QString)), this, SLOT(slotReceiveData(QByteArray,quint16,QString)));
}

//
//FUCTION: 如果失去与订阅服务器连接，删除客户端，释放资源
//PARAMETER：strIPAndPort,当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
//RETURN：No
void SubscribeClient::slotDeleteOneClient(QString strIPAndPort)
{
    QConnectClient *pClientJob = qobject_cast<QConnectClient *>(sender());
    int nIndex = m_listIPAndPorts.indexOf(strIPAndPort);
    if(nIndex < 0)
    {
        //序号出错
        ERR("no this connection:%s\n", strIPAndPort.toStdString().c_str());
        return;
    }
    //删除对应客户端
    pClientJob->deleteLater();
    m_listIPAndPorts.removeAt(nIndex); //删除IP字符串列表对应条目
    m_listClients.removeAt(nIndex);//删除客户端
}

//
//FUCTION: 检查TCP连接,如果没有订阅上，发送订阅请求
//PARAMETER：No
//RETURN：No
void SubscribeClient::slotManagerTM()
{
    bool isAllRegister = true;
    m_managerTM->stop();
    for (int i = 0; i < m_subscribeList.size(); ++i) {
        const subcribeInfo *info = &(m_subscribeList.at(i));
        if(info->registerDone){
            qDebug() << info->serverIP << ":" <<info->serverPort << " register done";
            continue;
        }
        isAllRegister = false;//有未成功注册的订阅服务器
        QTcpRequestClient *pCon = info->registerTcp;
        if(pCon == NULL)
        {
            INFO("=================== pCon NULL create con query ======================\n");
            pCon = m_subscribeList[i].registerTcp = new QTcpRequestClient(info->serverIP, info->serverPort);
            connect(info->registerTcp, SIGNAL(sigSubscribeDone(QTcpRequestClient*)), this, SLOT(slotSubscribeDone(QTcpRequestClient*)), Qt::DirectConnection);
        }

        int curTm = PublicFun::getSystemUsedTime();

        if(pCon->m_bSocketConnected && (curTm - pCon->m_lastReceiveTm > 8000)){

            qDebug() << info->serverIP << ":" <<info->serverPort << " receive time internal " << curTm <<" - " << pCon->m_lastReceiveTm;
            pCon->disconnectServer();
        }
        if(pCon->m_bSocketConnected == false)
        {
            if(pCon->connectServer() == false)
            {
                continue;
            }
        }
        sendRegisterQuery(info);
    }
    //未订阅成功，间隔一段时间后重新订阅
    if (!isAllRegister)
        m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
}

void SubscribeClient::slotCheckSubscibeState()
{
    bool isAllRegister = true;
    m_managerTM->stop();
    for (int i = 0; i < m_clientInfoList.size(); ++i) {
        const QClientInfo *info = m_clientInfoList.at(i);
        if(info->registerDone){
            qDebug() << info->serverIP << ":" <<info->serverPort << " register done";
            continue;
        }
        isAllRegister = false;//有未成功注册的订阅服务器
        QTcpRequestClient *pCon = info->registerTcp;
        if(pCon == NULL)
        {
            INFO("=================== pCon NULL create con query ======================\n");
            pCon = m_clientInfoList[i]->registerTcp = new QTcpRequestClient(info->serverIP, info->serverPort);
            connect(info->registerTcp, SIGNAL(sigSubscribeDone(QTcpRequestClient*)), this, SLOT(slotSubscribeDone_N(QTcpRequestClient*)), Qt::DirectConnection);
        }

        int curTm = PublicFun::getSystemUsedTime();

        if(pCon->m_bSocketConnected && (curTm - pCon->m_lastReceiveTm > 8000)){

            qDebug() << info->serverIP << ":" <<info->serverPort << " receive time internal " << curTm <<" - " << pCon->m_lastReceiveTm;
            pCon->disconnectServer();
        }
        if(pCon->m_bSocketConnected == false)
        {
            if(pCon->connectServer() == false)
            {
                continue;
            }
        }
        info->sendRegisterQuery(m_port);
    }
    //未订阅成功，间隔一段时间后重新订阅
    if (!isAllRegister)
        m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
}

//
//FUCTION: 收到订阅请求回复,删除请求订阅的对象，设置订阅成功标记
//PARAMETER：regTcp,指向请求订阅的对象
//RETURN：No
void SubscribeClient::slotSubscribeDone(QTcpRequestClient *regTcp)
{
    INFO(" ========================slot query done ========================\n");
    for (int i = 0; i < m_subscribeList.size(); ++i) {
        const subcribeInfo info = m_subscribeList.at(i);
        QTcpRequestClient *pCon = info.registerTcp;
        if(pCon == regTcp)
        {
            INFO("=============== subscribe success ================\n");
            m_subscribeList[i].registerDone = true;
            regTcp->disconnectServer();
            m_subscribeList[i].registerTcp->deleteLater();
            m_subscribeList[i].registerTcp = NULL;
            return;
        }
    }
}

void SubscribeClient::slotReceiveData(QByteArray data, quint16 port, QString ip)
{
    DEB("ip:%s, port:%d\ndata:%s\n",ip.toStdString().c_str(), port, QString(data.toHex()).toStdString().c_str());
    for (int i = 0; i < m_clientInfoList.size(); ++i)
    {
        QClientInfo *info = m_clientInfoList.at(i);
        DEB("serverIP:%s, port:%d\n",info->serverIP.toStdString().c_str(),info->serverPort);
        if(info->serverIP == ip && info->serverPort == port)
        {
            info->receiveReadData(data, port, ip);
        }
    }
}

void SubscribeClient::slotSubscribeDone_N(QTcpRequestClient *regTcp)
{
    INFO(" ========================slot query done ========================\n");
    for (int i = 0; i < m_clientInfoList.size(); ++i) {
        const QClientInfo *info  = m_clientInfoList.at(i);
        QTcpRequestClient *pCon = info->registerTcp;
        if(pCon == regTcp)
        {
            INFO("=============== subscribe success ================\n");
            m_clientInfoList[i]->registerDone = true;
            regTcp->disconnectServer();
            m_clientInfoList[i]->registerTcp->deleteLater();
            m_clientInfoList[i]->registerTcp = NULL;
            return;
        }
    }
}

//
//FUCTION: 向服务端发送订阅请求
//PARAMETER：info,连接类型，是否保持连接，是否是临时连接
//RETURN：No
void SubscribeClient::sendRegisterQuery(const subcribeInfo *info)
{
    info->registerTcp->sendScribeRequest(m_port, info->keepAlive, info->type);
}

//
//FUCTION: 添加订阅服务器
//PARAMETER：subscribeServerIP,订阅服务器IP，subscribeServerPort,订阅服务器端口，
//           keepAlive是否保持连接, subscribeType是否是临时连接
//RETURN：成功返回true,如果已订阅返回false
bool SubscribeClient::AddSubscribe(QString subscribeServerIP, int subscribeServerPort, bool keepAlive, qint8 subscribeType)
{
    QList<subcribeInfo>::const_iterator itr;
    for (itr = m_subscribeList.constBegin(); itr != m_subscribeList.constEnd(); ++itr)
    {
        if(itr->serverIP == subscribeServerIP && itr->serverPort == subscribeServerPort )
        {
            INFO("You have already subscribed to this server!\n");
            return false;
        }
    }
    subcribeInfo subInfo;
    subInfo.serverIP = subscribeServerIP;
    subInfo.serverPort = subscribeServerPort;
    subInfo.keepAlive = keepAlive;
    subInfo.type=subscribeType;
    subInfo.registerTcp = NULL;
    subInfo.registerDone = false;
    m_subscribeList.append(subInfo);
    //添加新订阅，重启检查订阅定时器
    m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
    return true;
}

bool SubscribeClient::AddSubscribe(QClientInfo &clientInfo)
{
    for (int i = 0; i < m_clientInfoList.size(); ++i)
    {
        const QClientInfo *info = m_clientInfoList.at(i);
        if(info->serverIP == clientInfo.serverIP && info->serverPort == clientInfo.serverPort )
        {
            INFO("You have already subscribed to this server!\n");
            return false;
        }
    }
    m_clientInfoList.append(&clientInfo);
    //添加新订阅，重启检查订阅定时器
    m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
    return true;
}

}//namespace SubScribe
