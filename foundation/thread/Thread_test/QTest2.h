#ifndef QTEST2_H
#define QTEST2_H

#include <QObject>

class QTest2 : public QObject
{
    Q_OBJECT
public:
    explicit QTest2(QObject *parent = 0);
    ~QTest2();
    QString m_print;
    void work();
    void workTest();
    
signals:
    void doWorkTest();
    
public slots:
    void doWork();
    
};

#endif // QTEST2_H
