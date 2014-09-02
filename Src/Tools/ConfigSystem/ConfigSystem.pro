#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T17:09:44
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConfigSystem
TEMPLATE = app
CONFIG += qt

SOURCES += main.cpp\
    configuration.cpp \
    registerdirwidget.cpp \
    registerdirwidgetitem.cpp

HEADERS  += \
    configuration.h \
    registerdirwidget.h \
    registerdirwidgetitem.h

FORMS    += mainwindow.ui \
    configuration.ui \
    projectsetting.ui \
    registerdirwidget.ui \
    registerdirwidgetitem.ui

unix{
    DESTDIR = ../../../Build/Tools

    target.path = /opt/RobotSDK/Tools
    INSTALLS += target
}

win32{
    DESTDIR = ../../../Build/Tools
    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
}


