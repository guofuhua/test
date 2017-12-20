#-------------------------------------------------
#
# Project created by QtCreator 2017-08-04T09:06:19
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = TCPTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    QTestSlot.cpp \
    ../../../../Common/Log/QLog.cpp \
    ../../../../ProjectCommon/TcpDataTransmit/QTcpDataTransmit.cpp

HEADERS += \
    QTestSlot.h \
    ../../../../ProjectCommon/TcpDataTransmit/QTcpDataTransmit.h \
    ../../../../Common/Log/QLog.h

INCLUDEPATH += /project/Common
INCLUDEPATH += /project/ProjectCommon
