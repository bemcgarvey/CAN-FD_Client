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

INCLUDEPATH += src/

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
    src/usbdevice.cpp \
    src/rxmessage.cpp \
    src/txmessage.cpp \
    src/rxthread.cpp \
    src/rxdatacolumndelegate.cpp \
    src/rxiddelegate.cpp \
    src/rxtablemodel.cpp \
    src/configdialog.cpp \
    src/toolstatus.cpp \
    src/errordialog.cpp \
    src/txtreeitem.cpp \
    src/txtreemodel.cpp \
    src/txiddelegate.cpp \
    src/txdataframe.cpp \
    src/txdatadelegate.cpp \
    src/configurefifosdialog.cpp \
    src/USBDeviceWatcher.cpp \
    src/filterdialog.cpp \
    src/canfilter.cpp \
    src/filtervalidator.cpp \
    src/aboutdialog.cpp \
    src/dblclicklabel.cpp

HEADERS += \
        src/mainwindow.h \
    src/usbdevice.h \
    src/rxmessage.h \
    src/txmessage.h \
    src/rxthread.h \
    src/rxdatacolumndelegate.h \
    src/rxiddelegate.h \
    src/rxtablemodel.h \
    src/configdialog.h \
    src/mcp2517fd.h \
    src/toolstatus.h \
    src/errordialog.h \
    src/txtreeitem.h \
    src/txtreemodel.h \
    src/txiddelegate.h \
    src/txdataframe.h \
    src/txdatadelegate.h \
    src/configurefifosdialog.h \
    src/USBDeviceWatcher.h \
    src/filterdialog.h \
    src/canfilter.h \
    src/filtervalidator.h \
    src/aboutdialog.h \
    src/dblclicklabel.h

FORMS += \
        src/mainwindow.ui \
    src/configdialog.ui \
    src/errordialog.ui \
    src/txdataframe.ui \
    src/configurefifosdialog.ui \
    src/filterdialog.ui \
    src/aboutdialog.ui

win32: LIBS += -lCfgMgr32
win32: LIBS += -lwinusb
win32: LIBS += -luser32
win32:RC_ICONS += src/images/icon1.ico

RESOURCES += \
    src/resources.qrc
