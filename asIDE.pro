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
    #src/documenthandler.cpp \
    src/syntaxhighlighter.cpp \
    src/aseconfigdialog.cpp

HEADERS  += src/mainwindow.h \
    #src/documenthandler.h
    src/syntaxhighlighter.h \
    src/aseconfigdialog.h

FORMS    += forms/mainwindow.ui \
    forms/aseconfigdialog.ui

RESOURCES += resources.qrc

# Default rules for deployment.
include(deployment.pri)
