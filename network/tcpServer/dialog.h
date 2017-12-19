#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QtNetwork>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void parseReceiveBytes(QByteArray *receiveBytes);
    void getFile(QByteArray databuf);
protected:
    QTcpServer tcpServer;
    QTcpSocket *tcpSocket;
    QByteArray receiveBuf;
    quint32 m_blockSize;
private slots:
    void slotReadData();
    void on_listenButton_clicked();
    void acceptConnection();
    void receiveData(QByteArray tmpbuf);
};

#endif // DIALOG_H
