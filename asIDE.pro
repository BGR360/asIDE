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
    src/gui/mainwindow.cpp \
    src/gui/aseconfigdialog.cpp \
    src/gui/codeeditwidget.cpp \
    src/intellisense/syntaxhighlighter.cpp \
    src/intellisense/documenttokenizer.cpp

HEADERS  += src/gui/mainwindow.h \
    src/gui/aseconfigdialog.h \
    src/gui/codeeditwidget.h \
    src/intellisense/syntaxhighlighter.h \ 
    src/intellisense/token.h \
    src/intellisense/documenttokenizer.h

FORMS    += forms/mainwindow.ui \
    forms/aseconfigdialog.ui \
    forms/codeeditwidget.ui

RESOURCES += resources.qrc

# Default rules for deployment.
include(deployment.pri)
