#include "SubscribeServer.h"
#include <QFile>
#include "../SubscribeCommon/PublicFun.h"
#include "../../../../ThirdPart/JSON/include/json.h"
#include "/project/ProjectCommon/Include/mydebug.h"
#define TIME_INTERVAL_MANAGE_SCAN   (3000)  //3000 ms
#define TCP_SOCKET_RCV_TIMEOUT      (8000)  //8000 ms
#define TCP_SOCKET_HEARTBEAT_TIME   (3000)  //3000 ms
#define TEMPVALUDE  (2)

namespace SubScribe{

SubscribeServer::SubscribeServer(QObject *parent) :
    QObject(parent)
{
}

//
//FUCTION:监听TCP,初始化订阅列表及定时器
//PARAMETER：port,要监听的端口,proName,订阅服务器模块名
//RETURN：No
void SubscribeServer::init(int port, const char* proName)
{
    INFO("========================================SubscribeClient init  =========================================================== \n");

    //初始化订阅客户端列表
    m_config_file = QString("./config/%1.sublist").arg(proName);
    initSubscribeList();

    //构建TCP服务器
    m_server = new QTcpServerExt();
    //关联成功接收连接的信号
    connect(m_server,SIGNAL(sigNewCon(int)),this,SLOT(slotNewCom(int)));
    //监听订阅服务器端口
    m_port = port;
    if(m_server->startListen(m_port))
    {
        INFO("listen port:%d\n", m_port);
    }
    else
    {
        ERR("listern port:%d failed!!\n", m_port);
    }

    //创建定时器，定期检查TCP连接
    m_managerTM = new QTimer();
    connect(m_managerTM,SIGNAL(timeout()), this, SLOT(slotManagerTM()));
    m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
}

//
//FUCTION:初始化订阅列表
//PARAMETER：No
//RETURN：No
void SubscribeServer::initSubscribeList()
{
    QFile runRecordFile(m_config_file);

    if(!runRecordFile.open(QIODevice::ReadOnly))
    {
        ERR("open sublist file error \n");
        return ;
    }

    QTextStream FileStream(&runRecordFile);
    QString line;
    bool err = false;
    while(FileStream.atEnd() == false) {
        line = FileStream.readLine();
        if(line.isEmpty()) continue;
        Json::Value _itemMsg;
        Json::Reader reader;
        if (reader.parse(line.toStdString(), _itemMsg))
        {
            if(_itemMsg.isMember("ip") && _itemMsg.isMember("port") && _itemMsg.isMember("keep"))
            {
                subscribeItem item;
                item.ip = QString::fromStdString(_itemMsg["ip"].asString());
                item.port = _itemMsg["port"].asInt();
                item.keepAlive = _itemMsg["keep"].asBool();
                item.conSocket = new QTcpPushClient(item.ip, item.port, item.keepAlive);
                m_subscribeList.append(item);
                INFO("===============%s:%d, keepAlive=%d, socket:%p\n",item.ip.toStdString().c_str(),item.port,item.keepAlive,item.conSocket);
            }
        } else {
            ERR("json parse error data:%s\n", line.toStdString().c_str());
            err = true;
            break;
        }
    }

    runRecordFile.close();
    if(err)
    {
        QFile::remove(m_config_file);
    }
    return;
}

//
//FUCTION:保存订阅客户端
//PARAMETER：ip,客户端IP; port,客户端端口;keepCon,true保持持续连接
//RETURN：No
void SubscribeServer::saveSubscribe(QString ip, int port, bool keepCon)
{
    //日志转成json格式数据
    Json::Value _itemRoot;

    Json::FastWriter writer;

    _itemRoot["ip"] = ip.toStdString();
    _itemRoot["port"] = port;
    _itemRoot["keep"] = keepCon;

    QFile runRecordFile(m_config_file);

    if(!runRecordFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        printf("open config file error \n");
        return ;
    }

    QTextStream FileStream(&runRecordFile);
    try
    {
        FileStream << writer.write(_itemRoot).c_str();
    }
    catch(...)
    {
        printf("confile file text stream error \n");
    }
    runRecordFile.close();
    return;
}

//
//FUCTION:添加到列表中
//PARAMETER：ip,客户端IP; port,客户端端口;keepCon,true保持持续连接
//RETURN：No
void SubscribeServer::addSubscribeItem(QString ip, int port, bool keepAlive, QTcpPushClient *conSocket)
{
    subscribeItem item;
    item.ip = ip;
    item.port = port;
    item.keepAlive = keepAlive;
    item.conSocket = conSocket;
    m_subscribeList.append(item);
}

//
//FUCTION:有新的TCP连接,创建一个客户端，处理连接
//PARAMETER：socketDescriptor,TCP连接的描述符
//RETURN：No
void SubscribeServer::slotNewCom(int socketDescriptor)
{
    INFO("==================================== new  connect ========================== \n");
    //构建客户端对象
    QTcpReplyClient *pClientJob = new QTcpReplyClient(socketDescriptor);
    INFO("pclientjob:%p\n", pClientJob);
    //获取远端IP和端口
    QString strIPAndPort = pClientJob->getIPAndPort();
    //IP-port列表
    m_listIPAndPorts.append(strIPAndPort);
    //客户端列表
    m_listClients.append(pClientJob);
    //更新连接列表
    connect(pClientJob, SIGNAL(sigCallManageDeleteClient(QString)), this, SLOT(slotDeleteOneClient(QString)));
    //接收到客户端的订阅注册请求
    connect(pClientJob, SIGNAL(sigSubscribeQuery(QString, int, bool, qint8, QTcpReplyClient*)),this,SLOT(slotSubscribeQuery(QString , int, bool,qint8, QTcpReplyClient*)));
}

//
//FUCTION: 如果失去与订阅客户端连接，删除连接，释放资源
//PARAMETER：strIPAndPort,当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
//RETURN：No
void SubscribeServer::slotDeleteOneClient(QString strIPAndPort)
{
    QTcpReplyClient *pClientJob = qobject_cast<QTcpReplyClient *>(sender());
    int nIndex = m_listIPAndPorts.indexOf(strIPAndPort);
    INFO("strip:%s, index:%d, :%p\n", strIPAndPort.toStdString().c_str(), nIndex, pClientJob);
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
//FUCTION: 如果失去与订阅客户端连接，删除连接，释放资源
//PARAMETER：ip,客户端IP; port,客户端端口;keepCon,保持持续连接;type,订阅类型,2为临时,其他为永久;pClientJob,指向连接客户端
//RETURN：No
void SubscribeServer::slotSubscribeQuery(QString ip, int port, bool keepCon, qint8 type, QTcpReplyClient *pClientJob)
{
    QString logMes = QString("new subscribe query %1:%2, keepalive:%3, type:%4\n").arg(ip).arg(port).arg(keepCon).arg(type);
    INFO("%s", logMes.toStdString().c_str());
    QList<subscribeItem>::iterator itr;
    for (itr = m_subscribeList.begin(); itr != m_subscribeList.end(); ++itr)
    {
        if(ip == itr->ip && port == itr->port)
        {
            printf("================== this subsribe already exist ==========================\n");
            pClientJob->sendSubscribeSuccess();
            return;
        }
    }
    QTcpPushClient* condata = new QTcpPushClient(ip, port, keepCon);
    addSubscribeItem(ip, port, keepCon, condata);
    pClientJob->sendSubscribeSuccess();
    if(type==TEMPVALUDE)return;
    saveSubscribe(ip, port, keepCon);
}

//
//FUCTION: 保持TCP持续连接
//PARAMETER：No
//RETURN：No
void SubscribeServer::slotManagerTM()
{
    m_managerTM->stop();
    for (int i = 0; i < m_subscribeList.size(); i++)
    {
        QTcpPushClient *pCon = m_subscribeList.at(i).conSocket;
        int curTm = PublicFun::getSystemUsedTime();
        if(pCon->m_bKeepAlive )
        {
            if(pCon->m_bSocketConnected && (curTm - pCon->m_lastReceiveTm > TCP_SOCKET_RCV_TIMEOUT)){
                WARN("receive timer long disconnect \n ");
                pCon->disconnectServer();
            }
            if(pCon->m_bSocketConnected == false)
            {
                pCon->connectServer();
            } else if(curTm - pCon->m_lastReceiveTm > TCP_SOCKET_HEARTBEAT_TIME)
            {
                pCon->sendBeatHeat();
            }
        } else {
            if(pCon->m_bSocketConnected)
            {
                if((curTm - pCon->m_lastReceiveTm) > TCP_SOCKET_RCV_TIMEOUT)
                {
                    WARN("connect abnormal disconnect\n");
                    pCon->disconnectServer();
                }
            }
        }
    }
    m_managerTM->start(TIME_INTERVAL_MANAGE_SCAN);
}

//
//FUCTION: 发送订阅推送
//PARAMETER：data,推送的数据
//RETURN：No
void SubscribeServer::slotBroadCast(QByteArray data)
{
    bool ret = BroadCastData(data);
    INFO("broadCastData return:%d\n", ret);
}

void SubscribeServer::slotInit(int port, QString proName)
{
    init(port, proName.toStdString().c_str());
}

//
//FUCTION: 发送订阅推送
//PARAMETER：data,推送的数据
//RETURN：No
bool SubscribeServer::BroadCastData(QByteArray data)
{
    bool rs = true;
    QByteArray SendBlock;
    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
    sendStream<<(uchar)0xAC;
    sendStream<<(quint32)m_port;
    sendStream.writeRawData(data.data(), data.size());

    QList<subscribeItem>::iterator itr;
    for (itr = m_subscribeList.begin(); itr != m_subscribeList.end(); ++itr) {
        QTcpPushClient *pCon = itr->conSocket;
        if(pCon->m_bSocketConnected == false)
        {
            if(pCon->connectServer() == false)
            {
                rs = false;
                continue;
            }
        }
        if(pCon->sendData(SendBlock) == false)
        {
            //            printf(" ================ server send data error ====================== \n");
            rs = false;
        } else {
            //            printf("send over \n");
        }
        if(pCon->m_bKeepAlive == false)
        {
            pCon->disconnectServer();
        }
    }
    return rs;
}

}//namespace SubScribe
