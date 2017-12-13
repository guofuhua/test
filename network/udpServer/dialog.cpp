#include "dialog.h"
#include <QDebug>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    setWindowTitle ("udpServer");
    udpSocket.bind (QHostAddress::Any,8888);
    connect (&udpSocket,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
}

void Dialog::receiveMessage ()
{
    receiveTextEdit->clear ();
    receiveBuf.resize (udpSocket.pendingDatagramSize ());
    udpSocket.readDatagram(receiveBuf.data (),receiveBuf.size ());
   receiveTextEdit->appendPlainText (QString::fromUtf8 (receiveBuf));
    receiveBuf.resize (0);

}
