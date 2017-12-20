#ifndef PUBLICFUN_H
#define PUBLICFUN_H
#include <QString>

namespace SubScribe{

typedef void (*LogCallBack)(const std::string& file, const char *str, bool bIsSuccess);

class PublicFun
{
public:
    PublicFun();
    static unsigned long getSystemUsedTime();

    static LogCallBack m_stalogBackFun;
    static std::string m_logFile;
    static void log(std::string strLogText, bool bIsSuccess);
};
}
#endif // PUBLICFUNCLIENT_H
