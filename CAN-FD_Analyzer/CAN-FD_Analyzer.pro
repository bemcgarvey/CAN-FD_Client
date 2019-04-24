#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T10:41:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAN-FD_Analyzer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

config += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    usbdevice.cpp \
    rxmessage.cpp \
    txmessage.cpp \
    rxthread.cpp \
    rxdatacolumndelegate.cpp \
    rxiddelegate.cpp \
    rxtablemodel.cpp \
    configdialog.cpp \
    toolstatus.cpp \
    errordialog.cpp \
    txtreeitem.cpp \
    txtreemodel.cpp \
    txiddelegate.cpp \
    txdataframe.cpp \
    txdatadelegate.cpp \
    configurefifosdialog.cpp \
    USBDeviceWatcher.cpp \
    filterdialog.cpp \
    canfilter.cpp \
    filtervalidator.cpp \
    aboutdialog.cpp \
    dblclicklabel.cpp

HEADERS += \
        mainwindow.h \
    usbdevice.h \
    rxmessage.h \
    txmessage.h \
    rxthread.h \
    rxdatacolumndelegate.h \
    rxiddelegate.h \
    rxtablemodel.h \
    configdialog.h \
    mcp2517fd.h \
    toolstatus.h \
    errordialog.h \
    txtreeitem.h \
    txtreemodel.h \
    txiddelegate.h \
    txdataframe.h \
    txdatadelegate.h \
    configurefifosdialog.h \
    USBDeviceWatcher.h \
    filterdialog.h \
    canfilter.h \
    filtervalidator.h \
    aboutdialog.h \
    dblclicklabel.h

FORMS += \
        mainwindow.ui \
    configdialog.ui \
    errordialog.ui \
    txdataframe.ui \
    configurefifosdialog.ui \
    filterdialog.ui \
    aboutdialog.ui

win32: LIBS += -lCfgMgr32
win32: LIBS += -lwinusb
win32: LIBS += -luser32
win32:RC_ICONS += images/icon1.ico

RESOURCES += \
    resources.qrc
