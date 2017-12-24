#ifndef QRUNTIMEFILESYNC_H
#define QRUNTIMEFILESYNC_H

#include <QObject>
#include <QTimer>
//#include <stdio.h>

//---begin
#define DEBUG (1)

#if (DEBUG)
#define FUNC_IN             //do {qDebug("[%s][%s(in )][%d]\n", __FILE__, __FUNCTION__, __LINE__);}while(0);
#define FUNC_OUT            //do {qDebug("[%s][%s(out)][%d]\n", __FILE__, __FUNCTION__, __LINE__);}while(0);
#define TRACE(format,...)   do {qDebug("[%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define WARN(format,...)    do {qDebug("[WARNING][%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define DEB(format,...)     do {qDebug("[DEBUG][%s][%s][%d] ",   __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define DEB_1(format,...)   do {qDebug("[DEBUG][%s][%s][%d] ",   __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define DEB_2(format,...)   do {qDebug("[DEBUG][%s][%s][%d] ",   __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define ERR(format,...)     do {qDebug("[ERROR][%s][%s][%d] ",   __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define INFO(format,...)    do {qDebug("[INFO] "); qDebug(format, ##__VA_ARGS__);}while(0);
#else
#define FUNC_IN
#define FUNC_OUT
#define TRACE(format,...)
#define DEB(format,...)
#define WARN(format,...)    //do {qDebug("[WARNING][%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define ERR(format,...)     //do {qDebug("[ERROR][%s][%s][%d] ",   __FILE__, __FUNCTION__, __LINE__); qDebug(format, ##__VA_ARGS__);}while(0);
#define INFO(format,...)    //do {qDebug("[INFO] "); qDebug(format, ##__VA_ARGS__);}while(0);
#endif
//---end

class QRuntimeFileSync : public QObject
{
    Q_OBJECT
public:
    QRuntimeFileSync();
    QTimer *m_timer;
    QTimer *m_timer2;
    QTimer *m_timer_quick;
    QString strerr;
    void dealTask();
    void dealQuickTask();
    void doSync();
signals:
    void signalinit();

public slots:
    void slotCheckCmdReply();
    void slotDoSync();
    void slotQuick();
    void slotinit();
};

#endif // QRUNTIMEFILESYNC_H
