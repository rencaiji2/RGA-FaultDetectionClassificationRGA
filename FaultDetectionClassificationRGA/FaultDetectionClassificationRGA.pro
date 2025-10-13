QT       += core gui charts sql network script xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IPInputDialog.cpp \
    LibAlgorithm/PublicAlgorithm.cpp \
    LibWidget/TChartWidget.cpp \
    XMLReadHelp.cpp \
    cApiServer.cpp \
    #cRequestTask.cpp \
    cRequestTask.cpp \
    cSerializeJS.cpp \
    main.cpp \
    #sDatabaseManager.cpp \
    publicdef.cpp \
    sApiClient.cpp \
    sConfigEQ.cpp \
    sConfigure.cpp \
    sConfigure/sControlCPM.cpp \
    sConfigure/sControlSIM.cpp \
    sConfigure/sControlScan.cpp \
    sConfigure/sEditSIM.cpp \
    sConfigure/sJsCalculate.cpp \
    sDefinePlotDataDlg.cpp \
    sFaultDetectionClassificationRGA.cpp \
    sFaultDetectionClassificationRGA/sChamerIDConf.cpp \
    sFaultDetectionClassificationRGA/sParsedQuery.cpp \
    sFaultDetectionClassificationRGA/sSeries.cpp \
    sFaultDetectionClassificationRGA/sWarningArea.cpp \
    sRecipeManage.cpp \
    sRecipeManageFDC.cpp

HEADERS += \
    IPInputDialog.h \
    LibAlgorithm/PublicAlgorithm.h \
    LibWidget/TChartWidget.h \
    XMLReadHelp.h \
    cApiServer.h \
    cDatabaseFDC.h \
    #cRequestTask.h \
    #sDatabaseManager.h \
    cExecuteStruct.h \
    cRequestTask.h \
    cSerializeJS.h \
    cStructRGA.h \
    publicdef.h \
    sApiClient.h \
    sConfigEQ.h \
    sConfigure.h \
    sConfigure/cConfigRGA.h \
    sConfigure/cParamCPM.h \
    sConfigure/sEditSIM.h \
    sConfigure/sJsCalculate.h \
    sDefinePlotDataDlg.h \
    sFaultDetectionClassificationRGA.h \
    sRecipeManage.h \
    sRecipeManageFDC.h

FORMS += \
    #sDatabaseManager.ui \
    sApiClient.ui \
    sConfigEQ.ui \
    sConfigure.ui \
    sConfigure/sEditSIM.ui \
    sConfigure/sJsCalculate.ui \
    sDefinePlotDataDlg.ui \
    sFaultDetectionClassificationRGA.ui \
    sRecipeManage.ui \
    sRecipeManageFDC.ui

win32{
    include(D:/QT/GlobalStruct/libGlobalStruct.pri)
}
unix{
    include(/home/zhouxu/work/GlobalStruct/libGlobalStruct.pri)
}

include($$PWD/sTool/sTool.pri)
include($$PWD/LibGlobalToolsR/LibGlobalTools.pri)
include($$PWD/LibWidget/LibWidget.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
