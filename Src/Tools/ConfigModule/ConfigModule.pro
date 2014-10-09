#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T17:14:09
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConfigModule
TEMPLATE = app

DEFINES += PRODEFROBOTSDKMODULE=$$(RobotSDK_ModuleDev)

SOURCES += \
    scaninterfacefunction.cpp \
    main.cpp

HEADERS  += \
    scaninterfacefunction.h

FORMS    += \
    scaninterfacefunction.ui

OTHER_FILES += \
    RobotSDK_Main.pri \
    RobotSDK_Install.pri

unix{
    DESTDIR = ../../../Build/Tools

    target.path = $$(HOME)/SDK/RobotSDK/Tools
    INSTALLS += target

    PUBLISHFILES.path = $$(HOME)/SDK/RobotSDK/Tools
    PUBLISHFILES.files = $$OTHER_FILES
    INSTALLS += PUBLISHFILES
}

win32{
    DESTDIR = ../../../Build/Tools
}
