#include "PublicFun.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<time.h>

namespace SubScribe{

PublicFun::PublicFun()
{
}

LogCallBack PublicFun::m_stalogBackFun = NULL;
std::string PublicFun::m_logFile;

unsigned long PublicFun::getSystemUsedTime()
{
    struct timespec startTime1 = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &startTime1);
    return (unsigned long) (1000*( startTime1.tv_sec) +   startTime1.tv_nsec/1000000);
}

void PublicFun::log(std::string strLogText, bool bIsSuccess)
{
    if(m_stalogBackFun)
    {
        (*m_stalogBackFun)(m_logFile, strLogText.c_str(), bIsSuccess);
    } else {
        printf(" log fun is null \n");
    }
}

}
