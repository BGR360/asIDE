QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = asIDE
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp \
    mainwindow.cpp \
    aseconfigdialog.cpp \
    codeeditwidget.cpp \
    labelviewdialog.cpp \
    tokenviewdialog.cpp \
    instructionviewdialog.cpp

HEADERS  += mainwindow.h \
    aseconfigdialog.h \
    codeeditwidget.h \
    labelviewdialog.h \
    tokenviewdialog.h \
    instructionviewdialog.h

FORMS    = ../../forms/mainwindow.ui \
    ../../forms/aseconfigdialog.ui \
    ../../forms/codeeditwidget.ui \
    ../../forms/labelviewdialog.ui \
    ../../forms/tokenviewdialog.ui \
    ../../forms/instructionviewdialog.ui

RESOURCES += ../../resources.qrc

LIBS += -L../intellisense -lIntellisense

INCLUDEPATH += ../intellisense

# Default rules for deployment.
include(../../deployment.pri)
