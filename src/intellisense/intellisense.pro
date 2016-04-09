TARGET = Intellisense
TEMPLATE = lib

DEFINES += INTELLISENSE_LIBRARY

SOURCES += syntaxhighlighter.cpp \
    documenttokenizer.cpp \
    token.cpp

HEADERS += syntaxhighlighter.h \ 
    documenttokenizer.h \
    token.h \
    intellisense_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
