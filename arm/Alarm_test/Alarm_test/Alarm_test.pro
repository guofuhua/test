#-------------------------------------------------
#
# Project created by QtCreator 2017-06-29T13:47:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Alarm_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../../ProjectCommon/ShareMem/CShrMem_DriverState.cpp \
    ../../../ProjectCommon/ShareMem/CShrMem_DevState.cpp \
    ../../../Common/ShareMem/CShareMem.cpp \
    ../../../Common/Tools/QTFSystemUtils.cpp

HEADERS += \
    ../../../ProjectCommon/ShareMem/CShrMem_DriverState.h \
    ../../../ProjectCommon/ShareMem/CShrMem_DevState.h \
    ../../../Common/ShareMem/CShareMem.h \
    ../../../Common/Tools/QTFSystemUtils.h
