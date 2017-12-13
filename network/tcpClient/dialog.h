#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QTcpSocket>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
protected:
     QTcpSocket tcpSocket;
     int bytessend;

private slots:
    void on_inputTextEdit_textChanged();
    void on_connectionButton_clicked();
    void connectRemote();
    void finshSend(qint64);
};

#endif // DIALOG_H
