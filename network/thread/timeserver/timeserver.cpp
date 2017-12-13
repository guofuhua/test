
#include "timeserver.h"
#include "timethread.h"
#include "dialog.h"
TimeServer::TimeServer(QObject *parent)
    : QTcpServer(parent)
{
    dlg = (Dialog*)parent;//设置私有指针
}

void TimeServer::incomingConnection(int socketDescriptor)
{
    TimeThread *thread = new TimeThread(socketDescriptor,this);
   // connect(thread, SIGNAL(finished()), dlg, SLOT(showResult()),Qt::QueuedConnection);//关联现在的结束信号到对话框的槽
  //  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
    //线程的结束消息finished(),
}
