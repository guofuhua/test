#include "QRuntimeFileSync.h"

QRuntimeFileSync::QRuntimeFileSync()
{
    connect(this, SIGNAL(signalinit()), this, SLOT(slotinit()));
}

void QRuntimeFileSync::slotDoSync()
{
    FUNC_IN;
    strerr = QString("slotDoSync. interval:%1\n").arg(m_timer->interval());
    DEB("%s", strerr.toStdString().c_str());
    //dealTask();
}


void QRuntimeFileSync::dealTask()
{
    DEB("status:%d, MCU status:%d\n", 3, 4);
}


void QRuntimeFileSync::slotQuick()
{
    FUNC_IN;
    strerr = QString("slotQuick. interval:%1\n").arg(m_timer_quick->interval());
    DEB("%s", strerr.toStdString().c_str());
    //dealQuickTask();
    m_timer_quick->start();
    FUNC_OUT;
}

void QRuntimeFileSync::slotinit()
{
    FUNC_IN;
    m_timer = new QTimer(this);
    m_timer2 = new QTimer(this);
    m_timer_quick = new QTimer(this);


    m_timer2->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotDoSync()));
    connect(m_timer_quick, SIGNAL(timeout()), this, SLOT(slotQuick()));
    connect(m_timer2, SIGNAL(timeout()), this, SLOT(slotCheckCmdReply()));


    m_timer_quick->setSingleShot(true);
    m_timer_quick->setInterval(5000);
    m_timer_quick->start();
//    QTimer::singleShot(5000, this, SLOT(slotQuick()));
//    QTimer::singleShot(5000, this, SLOT(slotDoSync()));
    m_timer->start(10000);
}

void QRuntimeFileSync::slotCheckCmdReply()
{
    FUNC_IN;
    strerr = QString("slotCheckCmdReply. interval:%1\n").arg(m_timer2->interval());
    DEB("%s", strerr.toStdString().c_str());
}

void QRuntimeFileSync::dealQuickTask()
{
    FUNC_IN;
    DEB("status:%d, MCU status:%d\n", 3, 4);
}

void QRuntimeFileSync::doSync()
{
    FUNC_IN;
    emit signalinit();
}
