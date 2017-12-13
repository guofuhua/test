#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QtNetwork>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
protected:
    QTcpServer tcpServer;
    QTcpSocket *tcpSocket;
    QByteArray receiveBuf;
private slots:
    void on_listenButton_clicked();
    void acceptConnection();
    void receiveData();
};

#endif // DIALOG_H
