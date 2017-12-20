#include "QTcpServerExt.h"

namespace SubScribe{

QTcpServerExt::QTcpServerExt(QObject *parent) :
    QTcpServer(parent)
{
}

QTcpServerExt::~QTcpServerExt()
{
}

bool QTcpServerExt::startListen(int port, QHostAddress host)
{
    return this->listen(host, port);
}

void QTcpServerExt::incomingConnection(int socketDescriptor)
{
    emit sigNewCon(socketDescriptor);
}

}
