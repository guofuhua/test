#include "QTest1.h"
#include <QDebug>

QTest1::QTest1(QObject *parent) :
    QObject(parent)
{
}

QTest1::~QTest1()
{
    qDebug() << "QTest1::~QTest1()";
}

void QTest1::Test()
{
    qDebug() << "test-------6";
    m_thread = new QTestThread(this);
}

void QTest1::TestEnd()
{
    qDebug() << "test-------7";
    m_thread->Work();
//    delete m_thread;
}
