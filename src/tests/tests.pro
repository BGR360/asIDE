QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Tests
CONFIG += console

TEMPLATE = app

SOURCES += main.cpp \
    documenttokenizertest.cpp \
    documentlabelindextest.cpp

LIBS += -L../intellisense -lIntellisense

INCLUDEPATH += ../intellisense

HEADERS += \
    documenttokenizertest.h \
    documentlabelindextest.h
