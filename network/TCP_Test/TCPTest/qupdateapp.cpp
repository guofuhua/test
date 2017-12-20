#include "qupdateapp.h"
#include "predefine.h"
#include <QSettings>
#include <QFile>
#include <QCryptographicHash>
#include <QTextStream>
#include <stdlib.h>
#include <QStringList>
#include <QDebug>

#define MD5_CONFIG  ("./md5.ini")
#define  UPDOFILE          "./UpDo.sh"

QUpdateApp::QUpdateApp(QObject *parent) :
    QObject(parent)
{
}

//md5校验升级包内的程序
bool QUpdateApp::checkUpgradeFile(QString appfile)
{
    bool ret = false;
    int cmdret = 0;
    QString untar("tar -xzvf ");

    //解压升级包，如果失败超过3次，则解压失败
    untar.append(appfile);
    for (int i = 0; i < 3; i++)
    {
        cmdret = RunShellCommand(untar.toStdString().c_str());
        if (0 == cmdret)
        {
            ret = true;
            break;
        }
        sleep(1);
    }

    if (ret)
    {
        ret = md5Check();
        if (ret)
        {
            if (0 != RunUpDoFIleLine())
            {
                ERR("run UpDo file failed!!\n");
                ret = false;
            }
        }
        else
        {
            ERR("md5 check failed!!\n");
        }
    }
    else
    {
        ERR("uncompress upgrade package failed!!!\n");
    }
    return ret;
}

//运行shell命令
int QUpdateApp::RunShellCommand(const char *command)
{
    int status;
    status = system(command);

    if (-1 == status)
    {
        printf("run sh file: %s error!", command);
    }
    else
    {
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
            return -1;
        }
    }
    return status;
}

//md5校验升级包内的文件
bool QUpdateApp::md5Check()
{
    //如果不存在md5文件，不进行校验，直接返回成功
    if (!QFile::exists(MD5_CONFIG))
    {
        WARN("%s not exists!\n", MD5_CONFIG);
        return true;
    }

    //读取所有的md5文件列表，一一进行校验
    QSettings _settings(MD5_CONFIG, QSettings::IniFormat);
    QStringList allkey = _settings.allKeys();
    for (int j = 0; j < allkey.size(); ++j)
    {
        if (QFile::exists(allkey.at(j)))
        {
            //计算文件的md5值
            QFile file(allkey.at(j));
            file.open(QIODevice::ReadOnly);
            QByteArray md5 = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
            file.close();

            QString Calculate_md5 = QString(md5.toHex().constData());
            QString ini_md5 = _settings.value(allkey.at(j)).toString();
            if (ini_md5 != Calculate_md5)
            {
                ERR("upgrade file %s check md5 failed!\n", allkey.at(j).toStdString().c_str());
                return false;
            }
        }
        else
        {
            ERR("upgrade file %s not exists!! upgrade failed!!\n", allkey.at(j).toStdString().c_str());
            return false;
        }
    }

    return true;
}

//运行UpDo.sh文件
int QUpdateApp::RunUpDoFIleLine()
{
    int cmdret = 0;
    QFile file(UPDOFILE);
    //UpDo.sh不存在，直接返回0
    if (!file.exists())
    {
        WARN("no updo file:%s\n", UPDOFILE);
        return 0;
    }
    //UpDo.sh打开失败，返回－1
    if (!file.open(QFile::ReadOnly))
    {
        ERR("updo file :%s open failed\n", file.fileName().toStdString().c_str());
        cmdret = -1;
        return cmdret;
    }
    //逐行执行shell命令,
    QTextStream textstream(&file);
    QString line;
    do {
        line = textstream.readLine();
        if(line.contains('#'))
        {//有#的当注释行处理，不执行shell语句
            INFO("note line:%s\n", line.toStdString().c_str());
        }
        else
        {//执行shell命令
            INFO("shell line:%s\n", line.toStdString().c_str());
            cmdret = RunShellCommand(line.toStdString().c_str());
            if (0 != cmdret)
            {//shell命令执行失败，退出
                ERR("shell line exec failed:%s \n", line.toStdString().c_str());
                break;
            }
        }
    } while (!line.isNull());
    file.close();

    return cmdret;
}
