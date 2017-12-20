#-------------------------------------------------
#
# Project created by QtCreator 2017-11-01T12:58:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Polymorphism
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    CTrainItem.cpp \
    CItemWaysideStop.cpp \
    CItemStop.cpp

HEADERS += \
    CTrainItem.h \
    CItemWaysideStop.h \
    LocalCommon/predefine.h \
    CItemStop.h

INCLUDEPATH +=../../../../Common
INCLUDEPATH +=../../../../ThirdPart
INCLUDEPATH +=../../../../ProjectCommon
