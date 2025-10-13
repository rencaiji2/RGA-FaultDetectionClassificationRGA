#INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/sChartWidget/sChartChildTools.cpp \
    $$PWD/sConfirmWidget.cpp \
    $$PWD/sCoordinateMask.cpp \
    $$PWD/sChartWidget/sChartXIC.cpp \
    $$PWD/sChartWidget/sManagerXIC.cpp \
    $$PWD/sIPAddressBox.cpp \
    $$PWD/sMyButton.cpp \
    $$PWD/sInputdialog.cpp \
    $$PWD/sFileDir.cpp \
    $$PWD/sFileWidget.cpp \
    $$PWD/ListWidget/ListItem.cpp \
    $$PWD/ListWidget/sTICList.cpp \
    $$PWD/sChartWidget.cpp \
    $$PWD/sChartWidgetTouchScreen.cpp \
    $$PWD/sNumberValidator.cpp \
    $$PWD/sTranslucentTemplate.cpp \
    $$PWD/sTransparentWindow.cpp \
    $$PWD/sTvplot.cpp \
    $$PWD/sCalandar/STDateWidget.cpp \
    $$PWD/sCalandar/STCalandarWidget.cpp \
    $$PWD/sWidgetLoadFile.cpp \
    $$PWD/sFileManager.cpp \
    $$PWD/sFileSystem.cpp

HEADERS  += \
    $$PWD/sChartWidget/sChartChildTools.h \
    $$PWD/sConfirmWidget.h \
    $$PWD/sCoordinateMask.h \
    $$PWD/sChartWidget/sChartXIC.h \
    $$PWD/sChartWidget/sManagerXIC.h \
    $$PWD/sIPAddressBox.h \
    $$PWD/sMyButton.h \
    $$PWD/sInputdialog.h \
    $$PWD/sFileDir.h \
    $$PWD/sFileWidget.h \
    $$PWD/ListWidget/ListItem.h \
    $$PWD/ListWidget/sTICList.h \
    $$PWD/sChartWidget.h \
    $$PWD/sChartWidgetTouchScreen.h \
    $$PWD/sNumberValidator.h \
    $$PWD/sTranslucentTemplate.h \
    $$PWD/sTransparentWindow.h \
    $$PWD/sTvplot.h \
    $$PWD/sCalandar/STDateWidget.h \
    $$PWD/sCalandar/STCalandarWidget.h \
    $$PWD/sWidgetLoadFile.h \
    $$PWD/sFileManager.h \
    $$PWD/sFileSystem.h


FORMS    += \
    $$PWD/sConfirmWidget.ui \
    $$PWD/sCoordinateMask.ui \
    $$PWD/sChartWidget/sChartXIC.ui \
    $$PWD/sChartWidget/sManagerXIC.ui \
    $$PWD/sFileDir.ui \
    $$PWD/sFileWidget.ui \
    $$PWD/ListWidget/ListItem.ui \
    $$PWD/ListWidget/sTICList.ui \
    $$PWD/sTranslucentTemplate.ui \
    $$PWD/sTransparentWindow.ui \
    $$PWD/sWidgetLoadFile.ui \
    $$PWD/sFileManager.ui \
    $$PWD/sFileSystem.ui	

RESOURCES += \
    $$PWD/LibWidget.qrc

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

win32:include ( C:/Qwt-6.1.1-svn/features/qwt.prf )
unix:INCLUDEPATH += /usr/local/qwt-6.1.2/include
unix:LIBS += -L/usr/local/qwt-6.1.2/lib -lqwt
