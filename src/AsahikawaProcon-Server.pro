#-------------------------------------------------
#
# Project created by QtCreator 2015-03-20T02:08:40
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
#QMAKE_CXXFLAGS += -std=c++11

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsahikawaProcon-Server
TEMPLATE = app


SOURCES += main.cpp\
    GameMap.cpp \
    MainWindow.cpp \
    GameSystem.cpp \
    StartupDialog.cpp \
    TcpClient.cpp \
    BaseClient.cpp \
    ComClient.cpp \
    GameBoard.cpp \
    ManualClientDialog.cpp \
    ManualClient.cpp \
    MapEditerDialog.cpp \
    SettingDialog.cpp \
    ClientSettingForm.cpp \
    StableLog.cpp \
    DesignDialog.cpp

HEADERS  += \
    GameMap.h \
    MainWindow.h \
    GameSystem.h \
    StartupDialog.h \
    TcpClient.h \
    BaseClient.h \
    ComClient.h \
    GameBoard.h \
    ManualClientDialog.h \
    ManualClient.h \
    MapEditerDialog.h \
    SettingDialog.h \
    ClientSettingForm.h \
    StableLog.h \
    DesignDialog.h

FORMS    += \
    MainWindow.ui \
    GameBoard.ui \
    ManualClientDialog.ui \
    MapEditerDialog.ui \
    SettingDialog.ui \
    ClientSettingForm.ui \
    DesignDialog.ui \
    StartupDialog.ui

RESOURCES += \
    resource.qrc

win32:{
    RC_FILE += icon.rc
}
mac:{
    ICON += icon.icns
}
