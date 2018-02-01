#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T00:05:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = hello
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ffmpegdecode.cpp \
    storage.cpp \
    threadpool/threadpool.c \
    test_judge.cpp

HEADERS += \
    ffmpegdecode.h \
    storage.h \
    threadpool/threadpool.h

INCLUDEPATH = third_lib/ffmpeg-3.0.2/include
LIBS += -L/home/linux/test/helloworld/hello/third_lib/ffmpeg-3.0.2/lib -lavformat -lavcodec -lavutil
