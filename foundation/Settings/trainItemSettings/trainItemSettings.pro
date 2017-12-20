#-------------------------------------------------
#
# Project created by QtCreator 2017-11-14T14:13:22
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = trainItemSettings
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../../AV4/AV4_TrainItemAnalysis_Model/code/Setting/QTrainItemCfg.cpp \
    ../../../AV4/AV4_AlarmManage_Model/code/Setting/QAlarmSetting.cpp

HEADERS += \
    ../../../AV4/AV4_TrainItemAnalysis_Model/code/Setting/QTrainItemCfg.h \
    ../../../AV4/AV4_AlarmManage_Model/code/Setting/QAlarmSetting.h \
    ../../../AV4/AV4_AlarmManage_Model/code/LocalCommon/predefine.h

INCLUDEPATH +=../../../../Common
INCLUDEPATH +=../../../../ThirdPart
INCLUDEPATH +=../../../../ProjectCommon
INCLUDEPATH +=../../../../Modules/AV4/AV4_AlarmManage_Model/code
