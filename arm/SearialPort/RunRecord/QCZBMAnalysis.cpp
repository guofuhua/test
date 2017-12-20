#include "QCZBMAnalysis.h"

QCZBMAnalysis::QCZBMAnalysis(QObject *parent) :
    QObject(parent)
{
}

QCZBMAnalysis::~QCZBMAnalysis()
{
}

QString QCZBMAnalysis::GetStationName(uchar ByteJiaoLu, int siStationID)
{
    return QString("%1-%2").arg((int)ByteJiaoLu).arg(siStationID);
}
