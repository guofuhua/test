#ifndef QLOGPROCESS_H
#define QLOGPROCESS_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMap>

#include <string>
#include <iostream>
//日志记录类
class QLogProcess : public QObject
{
    Q_OBJECT
public:
    //单例
    static QLogProcess* getInstance()
    {
        static QLogProcess instance;
        return &instance;
    }

    //写入日志到内存
    void logToSDCard(const std::string& file, const char *str);
    void logToSDCard(const char *str);


private:
    explicit QLogProcess(QObject *parent = 0);
    void env_check(const char* pro);
    void backSDFile(std::string &strFile, int layerDeep);

    bool getSDcardPath();

    //日志文件默认大小
    const  long m_iLogFileDefaultSize;
    bool m_sd_mount;
    QString m_strSD_path;
    QString m_strSD_log_path;
    QString m_defult_log;
    int m_iBackLay;  //备份层数

signals:
    
public slots:
    void slotLogToSDCard(QString file, QString text);
    void slotLogToSDCardDefault(QString text);
};

#endif // QLOGPROCESS_H
