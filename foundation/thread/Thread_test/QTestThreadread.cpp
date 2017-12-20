#include "QTestThread.h"
#include <QDebug>

QTestThread::QTestThread(QObject *parent) :
    QObject(parent)
{
    m_test = new QTest2();
    m_test1 = new QTest2();
//    connect(this, SIGNAL(doWorkTest()), m_test, SLOT(doWork()));
//    connect(this, SIGNAL(doWorkTest()), m_test1, SLOT(doWork()));
    connect(&m_thread, SIGNAL(finished()), this, SLOT(PrintThread()));
    m_test->m_print = "hello";
    m_test1->m_print = "world";
    m_test->moveToThread(&m_thread);
    m_test1->moveToThread(&m_thread);

    m_thread.start();
//    qDebug() << "test-------2";
//    emit doWorkTest();
//    qDebug() << "test-------3";
}

QTestThread::~QTestThread()
{
    qDebug() << "QTestThread::~QTestThread()";
    m_test->deleteLater();
    m_test1->deleteLater();
    m_thread.quit();
    m_thread.wait();
}

void QTestThread::Work()
{
//    emit doWorkTest();

    qDebug() << " QTestThread::work() thread id " << QThread::currentThreadId();
    m_test->workTest();
}

void QTestThread::PrintThread()
{
    qDebug() << "void QTestThread::PrintThread()";
}
