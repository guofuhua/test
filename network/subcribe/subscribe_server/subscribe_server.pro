#-------------------------------------------------
#
# Project created by QtCreator 2017-08-15T08:37:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = subscribe_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../../../Common/Log/QLog.cpp \
    Server_test.cpp \
    SubscribeServer.cpp \
    QTcpPushClient.h.cpp \
    QTcpReplyClient.cpp \
    ../SubscribeCommon/QTcpServerExt.cpp \
    ../SubscribeCommon/QTcpConnect.cpp \
    ../SubscribeCommon/PublicFun.cpp

HEADERS += \
    ../../../../Common/Log/QLog.h \
    Server_test.h \
    SubscribeServer.h \
    QTcpPushClient.h \
    QTcpReplyClient.h \
    ../SubscribeCommon/QTcpServerExt.h \
    ../SubscribeCommon/QTcpConnect.h \
    ../SubscribeCommon/PublicFun.h


LIBS += ../../../../ThirdPart/JSON/lib/libjsoncpp.so
