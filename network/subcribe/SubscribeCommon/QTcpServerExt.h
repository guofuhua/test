#ifndef QTCPSERVEREXT_H
#define QTCPSERVEREXT_H

#include <QObject>
#include <QTcpServer>

namespace SubScribe{

class QTcpServerExt : public QTcpServer
{
    Q_OBJECT
public:
    explicit QTcpServerExt(QObject *parent = 0);
    ~QTcpServerExt();
    bool startListen(int port, QHostAddress host = QHostAddress::Any);

protected:
    void incomingConnection(int socketDescriptor);
    
signals:
    void sigNewCon(int socketDescriptor);

public slots:
    
};

}
#endif // QTCPSERVEREXT_H
