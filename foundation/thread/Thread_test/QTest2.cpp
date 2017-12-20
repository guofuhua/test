#include "QTest2.h"
#include <QDebug>
#include <QThread>

QTest2::QTest2(QObject *parent) :
    QObject(parent)
{
    connect(this, SIGNAL(doWorkTest()), this, SLOT(doWork()));
}

QTest2::~QTest2()
{
    qDebug() << "QTest2::~QTest2()";
}

void QTest2::work()
{
    qDebug() << " now QTest2::work() thread id " << QThread::currentThreadId();
}

void QTest2::workTest()
{
    emit doWorkTest();
}

void QTest2::doWork()
{
    qDebug() << "test-------5";
    work();
}
