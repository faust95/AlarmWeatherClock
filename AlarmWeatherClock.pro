#-------------------------------------------------
#
# Project created by QtCreator 2016-10-03T00:08:56
#
#-------------------------------------------------

QT       += core gui multimedia qml positioning quick network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets qml positioning

TARGET = AlarmWeatherClock
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    about.cpp \
    timer.cpp \
    alarm.cpp \
    scheduler.cpp \
    schedulercontainer.cpp \
    settings.cpp \
    snoozer.cpp \
    settingsdialog.cpp \
    weatherdata.cpp

HEADERS  += mainwindow.h \
    about.h \
    timer.h \
    alarm.h \
    scheduler.h \
    schedulercontainer.h \
    settings.h \
    snoozer.h \
    settingsdialog.h \
    weatherdata.h

FORMS    += mainwindow.ui \
    about.ui \
    snoozer.ui \
    settingsdialog.ui

RESOURCES += \
    alarmresources.qrc
