
HEADERS += \
    $$PWD/include/cGlobalTools.h \
    $$PWD/include/sThread.h

unix:!macx|win32: LIBS += -L$$PWD/lib/ -lGlobalTools

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
