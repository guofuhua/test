#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QTcpSocket>
#include <QMutex>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    bool SendData(uchar data_type, QByteArray data);
    bool sendBytes(QByteArray &_sendData);
    void uploadFile();
protected:
     QTcpSocket tcpSocket;
     int bytessend;
     QMutex m_WriteDataMutex;

private slots:
    void on_inputTextEdit_textChanged();
    void on_connectionButton_clicked();
    void connectRemote();
    void finshSend(qint64);
    void on_uploadButton_clicked();
};

#endif // DIALOG_H
