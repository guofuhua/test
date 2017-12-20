#include <QCoreApplication>
#include "CGetProcessInfo.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <qdebug.h>
void testPrint_sys(QList<CSysMemInfoElement> &sys_info);
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CGetProcessInfo test;
    //test.testPrintPidArray();
    QList<CSysMemInfoElement> sys_info = test.getSysMemInfoList();
    test.sortByOccupy(sys_info);
    testPrint_sys(sys_info);
    return a.exec();
}
void testPrint_sys(QList<CSysMemInfoElement> &sys_info)
{
    int size = sys_info.size();
    int i = 0;
    for (i = 0; i < size; i++)
    {
        qDebug() << "pid:" << sys_info.at(i).m_Pid << "name:" << sys_info.at(i).m_name << "mem_use:" << sys_info.at(i).m_VmRSS << "occupy:" << sys_info.at(i).m_occupancyRate << "state:" << sys_info.at(i).m_State;
    }
}
