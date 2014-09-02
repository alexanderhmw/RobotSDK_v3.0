#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T17:14:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConfigApplication
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

unix{
    DESTDIR = ../../../Build/Tools

    target.path = /opt/RobotSDK/Tools
    INSTALLS += target
}

win32{
    DESTDIR = ../../../Build/Tools
}
