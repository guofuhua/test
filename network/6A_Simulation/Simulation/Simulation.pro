#-------------------------------------------------
#
# Project created by QtCreator 2017-12-25T10:30:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simulation
TEMPLATE = app

TRANSLATIONS += chese.ts\
                english.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    SimulationDialog.cpp

HEADERS  += mainwindow.h \
    SimulationDialog.h
