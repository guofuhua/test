#-------------------------------------------------
#
# Project created by QtCreator 2017-10-16T13:39:16
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = setU6A
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../../../ProjectCommon/SystemSetting/QSystemSetting.cpp \
    ../../../../ProjectCommon/ShareMem/QShrMem_TAXInfo.cpp


INCLUDEPATH +=../../../../ProjectCommon

HEADERS += \
    ../../../../ProjectCommon/SystemSetting/QSystemSetting.h \
    ../../../../ProjectCommon/ShareMem/CShrMem_SystemMode.h \
    ../../../../ProjectCommon/ShareMem/QShrMem_TAXInfo.h
