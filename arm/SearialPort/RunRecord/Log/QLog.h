#ifndef QLOG_H
#define QLOG_H

#include <QObject>
#include <QThread>

class QLog : public QObject
{
    Q_OBJECT
public:
    //单例
    static QLog* getInstance()
    {
        static QLog instance;
        return &instance;
    }
    void logToSDCard(const std::string& file, const char *str);
    void logToSDCard(const char *str);
private:
    explicit QLog(QObject *parent = 0);
    QThread m_thread;

signals:
    void sigLogToSDCard(QString file, QString text);
    void sigLogToSDCardDefault(QString text);
    
public slots:
    
};

#endif // QLOG_H
