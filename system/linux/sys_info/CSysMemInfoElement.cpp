#include "CSysMemInfoElement.h"

CSysMemInfoElement::CSysMemInfoElement()
{
    m_name = "";//应用程序或命令的名字
    m_State = "";//任务的状态，运行/睡眠/僵死/
    m_Tgid = 0;//线程组号
    m_Pid = 0;//进程ID
    m_PPid = 0;//父进程ID
    m_TracerPid = 0;//接收跟踪该进程信息的进程的ID号
    m_FDSize = 0;//文件描述符的最大个数
    m_VmPeak = 0;//代表当前进程运行过程中占用内存的峰值
    m_VmSize = 0;//代表进程现在正在占用的内存
    m_VmLck = 0;//代表进程已经锁住的物理内存的大小.锁住的物理内存不能交换到硬盘
    m_VmHWM = 0;//是程序得到分配到物理内存的峰值
    m_VmRSS = 0;//应用程序正在使用的物理内存的大小，就是用ps命令的参数rss的值 (rss)
    m_VmData = 0;//表示进程数据段的大小
    m_VmStk = 0;//表示进程堆栈段的大小
    m_VmExe = 0;//表示进程代码的大小
    m_VmLib = 0;//表示进程所使用LIB库的大小
    m_VmPTE = 0;//占用的页表的大小
    m_threads = 0;//表示当前进程组的线程数量
    m_SigQ = 0;//待处理信号的个数/目前最大可以处理的信号的个数
    m_occupancyRate = 0.0;//内存占用率
}
