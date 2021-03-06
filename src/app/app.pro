# //////////////////////////////////////////////////////////////////////////////////////////////////////////////
# //                                                                                                          //
# //  The MIT License (MIT)                                                                                   //
# //  Copyright (c) 2016 Benjamin Reeves                                                                      //
# //                                                                                                          //
# //  Permission is hereby granted, free of charge, to any person obtaining a copy of this software           //
# //  and associated documentation files (the "Software"), to deal in the Software without restriction,       //
# //  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
# //  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,   //
# //  subject to the following conditions:                                                                    //
# //                                                                                                          //
# //  The above copyright notice and this permission notice shall be included in all copies or substantial    //
# //  portions of the Software.                                                                               //
# //                                                                                                          //
# //  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT   //
# //  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     //
# //  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, //
# //  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     //
# //  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                  //
# //                                                                                                          //
# //////////////////////////////////////////////////////////////////////////////////////////////////////////////

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = asIDE
TEMPLATE = app

CONFIG += c++11

ICON = ../../images/app-icon/app.icns

SOURCES += main.cpp \
    mainwindow.cpp \
    aseconfigdialog.cpp \
    codeeditwidget.cpp \
    labelviewdialog.cpp \
    tokenviewdialog.cpp \
    instructionviewdialog.cpp \
    linenumberarea.cpp \
    autocompleter.cpp

HEADERS  += mainwindow.h \
    aseconfigdialog.h \
    codeeditwidget.h \
    labelviewdialog.h \
    tokenviewdialog.h \
    instructionviewdialog.h \
    linenumberarea.h \
    autocompleter.h

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

