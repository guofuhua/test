#ifndef QFILEDELETELINE_H
#define QFILEDELETELINE_H

#include <QObject>
#include <QDateTime>

class QFileDeleteLine : public QObject
{
    Q_OBJECT
public:
    explicit QFileDeleteLine(QObject *parent = 0);
    bool deleteOneLine(void);
    bool addOneERR(int alarm_type, QDateTime time);
    bool readOneLine(int &alarm_type, QDateTime &time);
    bool isFileExists();
    bool getUpdateAlarm(int &alarm_type, QDateTime &time);
    const char *extractFirstValue(const char *source, QString &value);

signals:
    
public slots:
    
};

#endif // QFILEDELETELINE_H
