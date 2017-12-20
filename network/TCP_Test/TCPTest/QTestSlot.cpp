#include "QTestSlot.h"
#include <stdio.h>
#include "../../../../ProjectCommon/Include/mydebug.h"

QTestSlot::QTestSlot(QObject *parent) :
    QObject(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

bool QTestSlot::sendStart(int seconds)
{
    m_timer.start(seconds*1000);
    return true;
}

void QTestSlot::slotReadData(quint16 flag, char *data, int size, bool &isSuccess)
{
    DEB("param flag:0x%x, data:%p, size:%d\n",flag, data, size);
    for (int i = 0; i < size; i++)
    {
        printf("0x%x\t", data[i] & 0xff);
    }
    printf("\n");
    DEB("slot read over");
    if (0xf1ff == flag)
    {
        isSuccess = true;
    }
    else
    {
        isSuccess = false;
    }
}

void QTestSlot::slotTimeout()
{
    for (int i = 0; i < 8; i ++)
    {
        buffer[i] = i;
    }
    emit sigSendData(0xabcd, buffer, 8);
}
