#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T17:39:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = asIDE
TEMPLATE = app

CONFIG += c++11


SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/syntaxhighlighter.cpp \
    src/aseconfigdialog.cpp \
    src/codeeditwidget.cpp

HEADERS  += src/mainwindow.h \
    src/syntaxhighlighter.h \
    src/aseconfigdialog.h \
    src/codeeditwidget.h

FORMS    += forms/mainwindow.ui \
    forms/aseconfigdialog.ui \
    forms/codeeditwidget.ui

RESOURCES += resources.qrc

# Default rules for deployment.
include(deployment.pri)
