#include "QTcpTransport.h"
#include "mydebug.h"
#include "SystemSetting/QSystemSetting.h"
QTcpTransport::QTcpTransport(QObject *parent) :
    QObject(parent)
{
    m_upload = new QUploadClient("192.168.1.138", 9876, 1123);
    //关联成功接收连接的信号
    connect(&m_server,SIGNAL(sigNewCon(int)),this,SLOT(slotNewCom(int)));
    m_server.startListen(20080);
//    m_timeridwho = new QTimer();
//    connect(m_timer,SIGNAL(timeout()), this, SLOT(slotTimerOut()));
//    m_timer->start(3000);
}

bool QTcpTransport::send(quint16 flag, QString data)
{
#if(1)
    m_upload->connectServer();
    //return m_upload->sendData(flag, data);
#else
    char *buffer = NULL;
    int malloc_size = 8;
    int module_id = 0;

    buffer = (char*)malloc(malloc_size);
    if (buffer == NULL)
    {
        ERR("malloc error \n");
        return false;
    }
    memset(buffer, 0 , malloc_size);
    buffer[0] = 2;
    buffer[1] = module_id;

    quint32 deviceNum = QSystemSetting::getInstance()->getDeviceNumbet().toInt();
    TRACE("flag:0x%x, deviceNum:%u\n", flag, deviceNum);
    buffer[2] = (deviceNum >> 24)&0xff;
    buffer[3] = (deviceNum >> 16)&0xff;
    buffer[4] = (deviceNum >> 8)&0xff;
    buffer[5] = (deviceNum)&0xff;
    return m_upload->sendBytes_read_test(buffer, malloc_size);
#endif
}

//
//FUCTION:有新的TCP连接,创建一个客户端，处理连接
//PARAMETER：socketDescriptor,TCP连接的描述符
//RETURN：No
void QTcpTransport::slotNewCom(int socketDescriptor)
{
    INFO("==================================== new  connect ========================== \n");
    //构建客户端对象
    QListenClient *pClientJob = new QListenClient(socketDescriptor);
    //获取远端IP和端口
    QString strIPAndPort = pClientJob->getIPAndPort();
    INFO("%s\n", strIPAndPort.toStdString().c_str());
    //更新连接列表
    connect(pClientJob, SIGNAL(sigCallManageDeleteClient(QString)), this, SLOT(slotDeleteOneClient(QString)));
    //接收到数据，通过信号转发出去
    connect(pClientJob, SIGNAL(signalReadData(quint16, QByteArray)), this, SLOT(slotDataNotify(quint16, QByteArray)));
}

void QTcpTransport::slotDataNotify(quint16 flag, QByteArray data)
{
    FUNC_IN;
    QDataStream _in(&data,QIODevice::ReadOnly);

    quint16 temp;
    QString videoName;
    _in >> temp;
    _in >> videoName;
    INFO("slotDataNotify::flag:0x%x, temp:0x%x, %s\n", flag, temp, videoName.toStdString().c_str());
}

void QTcpTransport::slotTimerOut()
{
    m_timeridwho->stop();
    static int i = 0;
    if (i++ < 4)
    {
        m_timeridwho->start(3000);
    }
    QByteArray data = QByteArray("test");
    send(0xff01, data);
}

//
//FUCTION: 如果失去与订阅服务器连接，删除客户端，释放资源
//PARAMETER：strIPAndPort,当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
//RETURN：No
void QTcpTransport::slotDeleteOneClient(QString strIPAndPort)
{
    QListenClient *pClientJob = qobject_cast<QListenClient *>(sender());

    ERR("no this connection:%s\n", strIPAndPort.toStdString().c_str());

    //删除对应客户端
    pClientJob->deleteLater();
}
