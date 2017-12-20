#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T09:52:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = sys_info_1
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    CGetProcessInfo.cpp \
    CSysMemInfoElement.cpp

HEADERS += \
    CGetProcessInfo.h \
    CSysMemInfoElement.h
