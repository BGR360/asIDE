TARGET = Intellisense
TEMPLATE = lib

DEFINES += LIBRARY_LIBRARY

SOURCES += syntaxhighlighter.cpp \
    documenttokenizer.cpp \
    token.cpp

HEADERS += syntaxhighlighter.h \ 
    documenttokenizer.h \
    token.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}