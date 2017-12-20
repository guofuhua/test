#ifndef QTESTTHREAD_H
#define QTESTTHREAD_H

#include <QObject>
#include <QThread>
#include "QTest2.h"

class QTestThread : public QObject
{
    Q_OBJECT
public:
    explicit QTestThread(QObject *parent = 0);
    QThread m_thread;
    ~QTestThread();
    QTest2 *m_test;
    QTest2 *m_test1;
    void Work();
    
signals:
    void doWorkTest();
    
public slots:
    void PrintThread();
    
};

#endif // QTESTTHREAD_H

