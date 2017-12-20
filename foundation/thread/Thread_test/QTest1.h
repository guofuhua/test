#ifndef QTEST1_H
#define QTEST1_H

#include <QObject>
#include "QTestThread.h"

class QTest1 : public QObject
{
    Q_OBJECT
public:
    explicit QTest1(QObject *parent = 0);
    ~QTest1();
    QTestThread *m_thread;
    void Test();
    void TestEnd();

signals:
    
public slots:
    
};

#endif // QTEST1_H
