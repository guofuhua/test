#-------------------------------------------------
#
# Project created by QtCreator 2017-10-10T09:55:40
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = AV4EXP_LKJFileReader_Model
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp \
    ../../../../Common/Tools/QTFSystemUtils.cpp \
    ../../../../Common/SerialPort/CSerialportDevice.cpp \
    QLKJRuntimeFile.cpp \
    QAnalysisOrgFile.cpp \
    QFieldConvert.cpp \
    QCZBMAnalysis.cpp \
    QRuntimeFileSync.cpp \
    ../../../../Common/Tools/QTFDevUtils.cpp \
    QSerialReadThread.cpp \
    QSerialWrite.cpp \
    QLKJFileCfg.cpp \
    ../../../../ProjectCommon/TcpDataTransmit/QTcpDataTransmit.cpp \
    ../../../../ProjectCommon/SystemSetting/QSystemSetting.cpp \
    Log/QLogProcess.cpp \
    Log/QLog.cpp \
    QUploadRunRecord.cpp \
    QTcpSendRunRecord.cpp \
    ../../../../Common/Subscribe/SubscribeServer/SubscribeServer.cpp \
    ../../../../Common/Subscribe/SubscribeServer/QTcpServerExtServer.cpp \
    ../../../../Common/Subscribe/SubscribeServer/QTcpConnectServer.cpp \
    ../../../../Common/Subscribe/SubscribeServer/PublicFunServer.cpp

HEADERS  += \
    ../../../../Common/Tools/QTFSystemUtils.h \
    ../../../../Common/SerialPort/CSerialportDevice.h \
    QLKJRuntimeFile.h \
    QAnalysisOrgFile.h \
    QFieldConvert.h \
    QCZBMAnalysis.h \
    QRuntimeFileSync.h \
    ../../../../Common/Tools/QTFDevUtils.h \
    QSerialReadThread.h \
    LocalCommon.h \
    QSerialWrite.h \
    QLKJFileCfg.h \
    ../../../../ProjectCommon/TcpDataTransmit/QTcpDataTransmit.h \
    ../../../../ProjectCommon/SystemSetting/QSystemSetting.h \
    Log/QLogProcess.h \
    Log/QLog.h \
    QUploadRunRecord.h \
    QTcpSendRunRecord.h \
    ../../../../Common/Subscribe/SubscribeServer/SubscribeServer.h \
    ../../../../Common/Subscribe/SubscribeServer/QTcpServerExtServer.h \
    ../../../../Common/Subscribe/SubscribeServer/QTcpConnectServer.h \
    ../../../../Common/Subscribe/SubscribeServer/PublicFunServer.h

INCLUDEPATH +=../../../../Common
INCLUDEPATH +=../../../../ThirdPart
INCLUDEPATH +=../../../../ProjectCommon
LIBS += ../../../../ThirdPart/JSON/lib/libjsoncpp.so
LIBS += ../../../../ThirdPart/JSON/lib/libcurl.so.4.2.0
