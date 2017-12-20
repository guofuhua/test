#include "CGetProcessInfo.h"
#include "qdir.h"
#include<qdebug.h>
#include<qstring.h>
#include<algorithm>
CGetProcessInfo::CGetProcessInfo()
{
}

int CGetProcessInfo::getTotalMem()
{
    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    fd = fopen ("/proc/meminfo", "r"); //以R读的方式打开文件再赋给指针fd

    //获取memtotal:总内存占用大小
    char name[32];//存放项目名称
    int memtotal;//存放内存峰值大小
    fgets (line_buff, sizeof(line_buff), fd);//读取memtotal这一行的数据,memtotal在第1行
    sscanf (line_buff, "%s %d", name,&memtotal);
    fclose(fd);     //关闭文件fd
    return memtotal;
}

int CGetProcessInfo::getFreeMem()
{
    QString file = "/proc/meminfo";//文件名

    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    fd = fopen (file.toAscii(), "r"); //以R读的方式打开文件再赋给指针fd

    //获取memtotal:总内存占用大小
    char name[32];//存放项目名称
    int memtotal;//存放内存峰值大小
    int memfree;
    int membuff;
    int memcache;
    fgets (line_buff, sizeof(line_buff), fd);//读取memtotal这一行的数据,memtotal在第1行
    sscanf (line_buff, "%s %d", name,&memtotal);
    fgets (line_buff, sizeof(line_buff), fd);//读取memfree这一行的数据,memfree在第2行
    sscanf (line_buff, "%s %d", name,&memfree);
    fgets (line_buff, sizeof(line_buff), fd);//读取buffers这一行的数据,buffers在第3行
    sscanf (line_buff, "%s %d", name,&membuff);
    fgets (line_buff, sizeof(line_buff), fd);//读取cached这一行的数据,cached在第4行
    sscanf (line_buff, "%s %d", name,&memcache);
    fclose(fd);     //关闭文件fd
    int free = memtotal - memfree - membuff - memcache;
    fprintf (stderr, "====%s：%d Kb====\n", "total available",free);
    return free;
}

QVector<int> CGetProcessInfo::getLinuxProcessPid()
{
    QVector<int> m_pid_group;
    QDir *dir=new QDir("/proc");
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList());
    bool ok;
    int i = 0;
    int pid = 0;
    int toalfile = fileInfo->count();
    m_pid_group.clear();
    for(i = 0;i < toalfile; i++)
    {
        pid = fileInfo->at(i).fileName().toInt(&ok, 10);
        if(ok)
        {
            m_pid_group.append(pid);
        }
    }

    return m_pid_group;
}

void CGetProcessInfo::testPrintPidArray()
{
    int i = 0;
    QVector<int> m_pid_group = getLinuxProcessPid();
    qDebug() << "total task:" << m_pid_group.size();
    for(i = 0; i < m_pid_group.size(); i++)
    {
        qDebug() << "pid:" << m_pid_group.at(i);
    }
    return;
}

bool compareData(int &barAmount1, int &barAmount2)
{
    if (barAmount1 < barAmount2)
    {
        return true;
    }
    return false;
}

QList<CSysMemInfoElement> CGetProcessInfo::getSysMemInfoList()
{
    QList<CSysMemInfoElement> m_sysMeminfo;
    QVector<int> m_pid_group = getLinuxProcessPid();
    int i = 0;
    int totalmem = getTotalMem();
    qSort(m_pid_group.begin(), m_pid_group.end(),compareData);
    m_sysMeminfo.clear();
    for(i = 0; i < m_pid_group.size(); i++)
    {
        CSysMemInfoElement sys_process_mem = getProcessMeminfo(m_pid_group.at(i));
        sys_process_mem.m_occupancyRate = 100 * (sys_process_mem.m_VmRSS * 1.0) / (totalmem * 1.0);
        m_sysMeminfo.append(sys_process_mem);
    }
    return m_sysMeminfo;
}

CSysMemInfoElement CGetProcessInfo::getProcessMeminfo(int pid)
{
    CSysMemInfoElement sys_info;
    char file[64] = {0};//文件名
    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    sprintf(file,"/proc/%d/status",pid);//文件中第11行包含着

    //fprintf (stderr, "current pid:%d\n", pid);
    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd

    char name[32];//存放项目名称

    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %s", name,file);
    sys_info.m_name = file;
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %s", name,file);
    sys_info.m_State = file;
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_Tgid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_Pid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_PPid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_TracerPid);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_FDSize);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmPeak);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmSize);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmLck);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmHWM);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmRSS);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmData);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmStk);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmExe);
    fgets (line_buff, sizeof(line_buff), fd);//读取VmRSS这一行的数据,VmRSS在第15行
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmLib);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmPTE);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_threads);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_SigQ);
    fclose(fd);     //关闭文件fd
    return sys_info;
}

void CGetProcessInfo::sortByOccupy(QList<CSysMemInfoElement> &sys_info_list)
{
    int low = 0;
    int high = sys_info_list.size() - 1;//设置变量的初始值
    int j;
    while (low < high)
    {
        for (j = low; j < high; ++j)//正向冒泡,找到最大者
        {
            if (sys_info_list.at(j).m_VmRSS > sys_info_list.at(j + 1).m_VmRSS)
            {
                sys_info_list.swap(j, j+1);
            }
        }
        --high;//修改high值, 前移一位
        for (j = high; j > low; --j)//反向冒泡,找到最小者
        {
            if (sys_info_list.at(j).m_VmRSS < sys_info_list.at(j - 1).m_VmRSS)
            {
                sys_info_list.swap(j, j-1);
            }
        }
        ++low;//修改low值,后移一位
    }
    return;
}
