#-------------------------------------------------
#
# Project created by QtCreator 2017-08-15T08:35:50
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = subscribe_client
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Client_test.cpp \
    QConnectClient.cpp \
    SubscribeClient.cpp \
    QTcpRequestClient.cpp \
    ../SubscribeCommon/QTcpServerExt.cpp \
    ../SubscribeCommon/QTcpConnect.cpp \
    ../SubscribeCommon/PublicFun.cpp \
    QClientInfoinfo.cpp

HEADERS += \
    Client_test.h \
    QConnectClient.h \
    SubscribeClient.h \
    QTcpRequestClient.h \
    ../SubscribeCommon/QTcpServerExt.h \
    ../SubscribeCommon/QTcpConnect.h \
    ../SubscribeCommon/PublicFun.h \
    QClientInfo.h
