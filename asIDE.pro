TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += src/main.cpp \
    src/documenthandler.cpp \
    src/syntaxhighlighter.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = qml

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/documenthandler.h \
    src/syntaxhighlighter.h
