#include "qfiledeleteline.h"
#include "predefine.h"
#include <QFile>
#include <QTextStream>
#define UPGRADE_ERR_FILE ("./upgrade_err.txt")

QFileDeleteLine::QFileDeleteLine(QObject *parent) :
    QObject(parent)
{
}

//删除文件的第一行，适用于小文件
bool QFileDeleteLine::deleteOneLine()
{
    QFile file(UPGRADE_ERR_FILE);
    bool isEmpty = false;

    if (!file.exists())
    {
        WARN("no file:%s\n", file.fileName().toStdString().c_str());
        return true;
    }

    if (!file.open(QFile::ReadOnly))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return false;
    }

    QString strall;
    QTextStream textstream(&file);
    if (textstream.atEnd())
    {
        isEmpty = true;
    }
    else
    {
        strall = textstream.readLine();
        if (textstream.atEnd())
        {
            isEmpty = true;
        }
        else
        {
            strall = textstream.readAll();
        }
    }
    file.close();

    if (isEmpty)
    {
        if (file.remove())
        {
            return true;
        }
        else
        {
            ERR("remove %s failed\n", file.fileName().toStdString().c_str());
            return false;
        }
    }

    if (!file.open(QFile::WriteOnly))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return false;
    }
    QTextStream bakTextStream(&file);
    bakTextStream << strall;
    file.close();
    INFO("delete %s first line\n", file.fileName().toStdString().c_str());
    return true;
}

bool QFileDeleteLine::addOneERR(int alarm_type, QDateTime time)
{
    QFile file(UPGRADE_ERR_FILE);
    if (!file.open(QFile::Append))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return false;
    }
    QString text = QString("type=%1, time=%2\n").arg(alarm_type).arg(time.toString("yyyy-MM-dd hh:mm:ss"));
    QTextStream bakTextStream(&file);
    bakTextStream << text;
    file.close();
    INFO("append file:%s", text.toStdString().c_str());
    return true;
}

//从文件中读取第一行，并解析出其中的参数
bool QFileDeleteLine::readOneLine(int &alarm_type, QDateTime &time)
{
    QFile file(UPGRADE_ERR_FILE);
    bool ret = false;

    if (!file.exists())
    {
        WARN("no file:%s\n", file.fileName().toStdString().c_str());
        return true;
    }

    if (!file.open(QFile::ReadOnly))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return false;
    }

    //从文件中读取第一行
    QString line;
    QTextStream textstream(&file);
    line = textstream.readLine();
    file.close();

    QString key = "type";
    QString value;
    if (line.contains(key))
    {
        if (NULL != extractFirstValue(line.mid(line.indexOf(key)).toStdString().c_str(), value))
        {
            bool ok;
            int type = value.toInt(&ok, 10);
            if (ok)
            {
                alarm_type = type;
                ret = true;
            }
        }
    }
    key = "time";
    if (line.contains(key))
    {
        if (NULL != extractFirstValue(line.mid(line.indexOf(key)).toStdString().c_str(), value))
        {
            time = time.fromString(value, "yyyy-MM-dd hh:mm:ss");
            ret = true;
        }
    }
    return ret;
}

bool QFileDeleteLine::isFileExists()
{
    return QFile::exists(UPGRADE_ERR_FILE);
}

const char * QFileDeleteLine::extractFirstValue(const char *source, QString &value)
{
    const char *p = source;
    char str_value[128];
    const char *p_value = NULL;
    if (NULL == source)
    {
        return NULL;
    }
    memset(str_value, 0, sizeof(str_value));

    //获取从'='到','或结尾之间的字串
    while(('\n' != *p) && (',' != *p) && ('\0' != *p))
    {
        if ('=' == *p)
        {
            p_value = p;
        }
        p++;
    }

    if (NULL != p_value)
    {
        p_value++;//去掉'='
        unsigned int len = p - p_value;
        if (sizeof(str_value) - 1 < len)
            len = sizeof(str_value) - 1;
        memcpy(str_value, p_value, len);
    }
    value = QString(str_value);
    return p_value;
}


bool QFileDeleteLine::getUpdateAlarm(int &alarm_type, QDateTime &time)
{
    if (readOneLine(alarm_type, time))
    {
        if (deleteOneLine())
        {
            return true;
        }
    }
    return false;
}
