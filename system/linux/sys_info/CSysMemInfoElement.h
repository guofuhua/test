#ifndef CSYSMEMINFOELEMENT_H
#define CSYSMEMINFOELEMENT_H
#include<qstring.h>
#include<qchar.h>

class CSysMemInfoElement
{
public:
    CSysMemInfoElement();

public:
    QString m_name;//应用程序或命令的名字
    QString m_State;//任务的状态，运行/睡眠/僵死/
    int m_Tgid;//线程组号
    int m_Pid;//进程ID
    int m_PPid;//父进程ID
    int m_TracerPid;//接收跟踪该进程信息的进程的ID号
    int m_FDSize;//文件描述符的最大个数
    int m_VmPeak;//代表当前进程运行过程中占用内存的峰值
    int m_VmSize;//代表进程现在正在占用的内存
    int m_VmLck;//代表进程已经锁住的物理内存的大小.锁住的物理内存不能交换到硬盘
    int m_VmHWM;//是程序得到分配到物理内存的峰值
    int m_VmRSS;//应用程序正在使用的物理内存的大小，就是用ps命令的参数rss的值 (rss)
    int m_VmData;//表示进程数据段的大小
    int m_VmStk;//表示进程堆栈段的大小
    int m_VmExe;//表示进程代码的大小
    int m_VmLib;//表示进程所使用LIB库的大小
    int m_VmPTE;//占用的页表的大小
    int m_threads;//表示当前进程组的线程数量
    int m_SigQ;//待处理信号的个数/目前最大可以处理的信号的个数
    float m_occupancyRate;//内存占用率
};

#endif // CSYSMEMINFOELEMENT_H
