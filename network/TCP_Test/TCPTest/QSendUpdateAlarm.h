#ifndef QSENDUPDATEALARM_H
#define QSENDUPDATEALARM_H

#include <QThread>

class QSendUpdateAlarm : public QThread
{
    Q_OBJECT
public:
    explicit QSendUpdateAlarm(QObject *parent = 0);
    void run();
    
signals:
    
public slots:
    
};

#endif // QSENDUPDATEALARM_H
