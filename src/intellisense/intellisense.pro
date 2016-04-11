TARGET = Intellisense
TEMPLATE = lib

DEFINES += INTELLISENSE_LIBRARY

SOURCES += syntaxhighlighter.cpp \
    documenttokenizer.cpp \
    token.cpp \
    documentlabelindex.cpp

HEADERS += syntaxhighlighter.h \ 
    documenttokenizer.h \
    token.h \
    intellisense_global.h \
    documentlabelindex.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
