#ifndef QTESTSLOT_H
#define QTESTSLOT_H

#include <QObject>
#include <QTimer>

class QTestSlot : public QObject
{
    Q_OBJECT
public:
    explicit QTestSlot(QObject *parent = 0);
    QTimer m_timer;
    bool sendStart(int seconds);
    char buffer[64];
    
signals:
    void sigSendData(quint16, char*, int);
    
public slots:
    void slotReadData(quint16 flag, char*data, int size, bool &isSuccess);
    void slotTimeout();
};

#endif // QTESTSLOT_H
