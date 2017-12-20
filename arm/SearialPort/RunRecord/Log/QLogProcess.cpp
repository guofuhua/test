#include "QLogProcess.h"
//#include "../qdevicestate.h"

#include <QDebug>
#include <QDateTime>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <QDir>
#include "Include/mydebug.h"

//extern bool  g_bRamFSFalut;
extern const char *g_proName;
bool getDevPath(QString &devPath, const char *devPre);

QLogProcess::QLogProcess(QObject *parent) :
    QObject(parent)
  ,m_iLogFileDefaultSize(6*1024*1024)
{
    m_iBackLay = 8;
    env_check(g_proName);
}

//
//FUCTION:获取sd卡路径
//PARAMETER：sdPath sd卡路径
//RETURN：是否成功
bool QLogProcess::getSDcardPath()
{
    if(getDevPath(m_strSD_path, "/media/mmcblk"))
    {
        return true;
    }
    return false;
}

void QLogProcess::slotLogToSDCard(QString file, QString text)
{
    logToSDCard(file.toStdString(), text.toStdString().c_str());
}

void QLogProcess::slotLogToSDCardDefault(QString text)
{
    logToSDCard(text.toStdString().c_str());
}


//////m_strSD_path =/media/mmcblk0p1
void QLogProcess::env_check(const char* pro)
{
    m_sd_mount = getSDcardPath();
    if(m_sd_mount){
        QDir log_dir(m_strSD_path + "/log");
        if(log_dir.exists() == false){
            log_dir.mkdir(m_strSD_path + "/log");
        }
        QDir log_dir_pro(m_strSD_path + "/log/" + pro);
        if(log_dir_pro.exists() == false){
            log_dir_pro.mkdir(m_strSD_path + "/log/" + pro);
        }

        m_strSD_log_path = m_strSD_path + "/log/" + pro + "/";
        m_defult_log =  QString("%1.log").arg(pro);
    }
}


//
//FUCTION: 　写入日志直接写SD卡
//PARAMETER：file  日志文件   str   写入内容
//RETURN：No
void QLogProcess::logToSDCard(const std::string& file, const char *str)
{
    //std::cout << "got log file:" << file << " data:" << str ;
    //如无SD卡 //    checkSDCanUse();

    if(m_sd_mount == false)
    {
        //env_check();
        m_sd_mount = getSDcardPath();
        return ;
    }

    if(strlen(str)<=0) return;

    std::string strFullFile;

    //写入SD卡文件
    strFullFile = m_strSD_log_path.toStdString() + file;
//    std::cout << "got log file:" << strFullFile << " data:" << str ;
    FILE *fp = fopen(strFullFile.c_str(), "a");
    if (!fp)
        return;

    std::string srtTime = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss").toStdString() + ":: " ;

    fwrite(srtTime.c_str(), sizeof(char), strlen(srtTime.c_str()), fp);
    fwrite(str, sizeof(char), strlen(str), fp);
    fclose(fp);
    //SD卡文件

    struct stat st;
    if (!access(strFullFile.c_str(), F_OK))
    {
        if (stat(strFullFile.c_str(), &st) == 0)
        {
            if (st.st_size > m_iLogFileDefaultSize)
            {
                backSDFile(strFullFile, m_iBackLay);
            }
        }
    }

    //system("sync");
}

void QLogProcess::logToSDCard(const char *str)
{

    logToSDCard(m_defult_log.toStdString(), str);
}


//
//FUCTION: 　写入日志直接写SD卡备份文件
//PARAMETER： strFile :文件  layerDeep: 备份深度 比如 4 logfile.1.bak logfile.2.bak logfile.3.bak
//RETURN：No
void QLogProcess::backSDFile(std::string &strFile, int layerDeep)
{

    QString srtCmd,qstrFile;
    if(layerDeep<=0)
    {
        srtCmd = QString("rm -f %1 ").arg(strFile.c_str());
       //移动文件
        system(srtCmd.toStdString().c_str());
        return;
    }
    if(layerDeep > 300)
    {
        layerDeep = 300;
    }
    for(int i=layerDeep; i>1; i-- )
    {

        //源文件
        qstrFile = QString("%1.%2.bak").arg(strFile.c_str()).arg(i-1);
        //文件不存在
        if (access(qstrFile.toStdString().c_str(), F_OK) != 0)
        {
            continue;
        }

        srtCmd = QString("mv %1 ").arg(qstrFile);

        //目标文件
        qstrFile = QString("%1.%2.bak").arg(strFile.c_str()).arg(i);

        srtCmd = srtCmd + qstrFile;

        //移动文件
        system(srtCmd.toStdString().c_str());
    }


    srtCmd = QString("mv %1 ").arg(strFile.c_str());
    //目标文件
    qstrFile = QString("%1.%2.bak").arg(strFile.c_str()).arg(1);
    srtCmd = srtCmd + qstrFile;
    //移动文件
    system(srtCmd.toStdString().c_str());

}


//
//FUCTION:获取设备路径
//PARAMETER：devPath 设备路径, devPre 设备路径前缀
//RETURN：是否成功
bool getDevPath(QString &devPath, const char *devPre)
{
    bool rs = false;
    char * line = NULL;
    size_t len = 0;
    FILE *ptr = NULL;

    if((ptr=popen("df", "r"))!=NULL)
    {
        QString t_devPath;
        while ( getline(&line, &len, ptr) != -1)
        {
            t_devPath = line;
            //            qDebug() << "df line " << uPath;
            if(!t_devPath.contains(devPre))
            {
                continue;
            }
            t_devPath = t_devPath.section('/', -2);
            //去掉换行符
            devPath = QString("/") + t_devPath.left(t_devPath.length() - 1);

            rs = true;
        }
        if(line != NULL)
        {
            free(line);
        }
        pclose(ptr);
    } else {
        ERR(" popen df error\n");
    }
    return rs;
}
