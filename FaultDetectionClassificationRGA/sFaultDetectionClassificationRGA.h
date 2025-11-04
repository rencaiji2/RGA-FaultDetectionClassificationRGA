#ifndef SFAULTDETECTIONCLASSIFICATIONRGA_H
#define SFAULTDETECTIONCLASSIFICATIONRGA_H

//#include "cDatabaseFDC.h"
#include "cApiServer.h"
#include "sApiClient.h"
#include "sConfigEQ.h"
#include "sRecipeManageFDC.h"
#include "ui_sFaultDetectionClassificationRGA.h"
#include <QMainWindow>
#include <QtCharts>
#include <QDate>
#include <QAction>
#include <QMenu>
#include <cmath>
#include "LibWidget/TChartWidget.h"
#include "cExecuteStruct.h"
#include "publicdef.h"
#include "XMLReadHelp.h"
#include "LibAlgorithm/PublicAlgorithm.h"
#include "sDefinePlotDataDlg.h"
#include "sTool/EmailSender.h"
#include "sTool/OutlookEmailSender.h"
#include <sTool/UniversalEmailSender.h>
#include "sTool/ReportHandler.h"
#include "sTool/ScatterPlotRenderer.h"
#include "sTool/HtmlReportGenerator.h"
#include "sTool/OutlookProcessSender.h"
#include <QUrl>

using namespace QtCharts;

//QT_BEGIN_NAMESPACE
//namespace Ui { class sFaultDetectionClassificationRGA; }
//QT_END_NAMESPACE

class sFaultDetectionClassificationRGA : public QMainWindow
{
    Q_OBJECT

public:
    sFaultDetectionClassificationRGA(QWidget *parent = nullptr);
    ~sFaultDetectionClassificationRGA();

    //日常邮件发送用
    void setScheduledTime(const QTime& time);  // 设置定时时间
    void setScheduledEnabled(bool enabled);    // 启用/禁用定时功能
    QTime getScheduledTime() const;           // 获取定时时间
    bool isScheduledEnabled() const;          // 检查是否启用

    /**
     * @brief 高级版本：支持更多自定义选项
     * @param chartWidget TChartWidget实例
     * @param pMap 时间序列数据
     * @param options 绘制选项
     */
    struct TimeSeriesPlotOptions {
        QString title;                          // 图表标题
        bool showLegend;                       // 是否显示图例
        TChartWidget::LegendPosition legendPos; // 图例位置
        qreal pointSize;                       // 散点大小
        bool autoRangeX;                       // X轴自动范围
        bool autoRangeY;                       // Y轴自动范围
        double yMarginPercent;                 // Y轴边距百分比
        QMap<QString, QColor> customColors;    // 自定义颜色

        TimeSeriesPlotOptions()
            : title("时间序列数据")
            , showLegend(true)
            , legendPos(TChartWidget::LegendRight)
            , pointSize(8.0)
            , autoRangeX(false)
            , autoRangeY(false)
            , yMarginPercent(10.0)
        {}
    };
signals:
    void sUpdateCharWgt();
    void sDoDailyMail();//通知发日常邮件

private slots:
    void on_UI_PB_OK_FDC_clicked();
    void on_UI_PB_DEVICE_CONFIG_FDC_clicked();
    //CL数据源的选择功能【会触发CL3个线的绘制】
    void on_comCL_mode_currentIndexChanged(int index);

    void on_chkShowCL_clicked(bool checked);
    void onResponseReceived(/*const QJsonObject &response*/);
    void onUpdateDeviceList(){
        mConfigEQ->getParam(ui.UI_TW_EQ_LIST_FDC);
        ui.UI_TW_EQ_LIST_FDC->expandAll();
    }
    //重新绘制手动CL的两根线
    void onUpdateCL_Manual();

    //chart双击点-或者框选点事件的调用
    void onPostHandleProcessStart(QStringList i_option);

    //--------------------------点选模式----------------------
    //void setMouseMode(int mode);  // 0: 缩放, 1: 点选
    void onPointSelected(const QString& seriesName, const QPointF& point, bool selected);
    //void onPointClicked(const QString& seriesName, const QPointF& point);
    void onPointDoubleClicked(const QString& seriesName, const QPointF& point);
    void onExecuteSelectionClicked();  // 执行选中点处理
    void onClearSelectionClicked();    // 清除选中点
    //--------------------点选模式---------------------------------------------

    void on_btnOpenPostSoftware_clicked();
    //进程状态slot
    void onPostSoftwareProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){
        qDebug().noquote() << tr("退出状态码：%1").arg(exitCode) + " - " << tr("退出状态枚举：%1").arg(exitStatus);
    }

    void on_btnClearPnts_clicked();

    void on_UI_CB_EQ_RECIPE_currentTextChanged(const QString &arg1);

    void on_UI_CB_CHAMBERID_currentTextChanged(const QString &arg1);

    void on_UI_CB_LOTID_currentTextChanged(const QString &arg1);

    void on_UI_CB_WAFERID_currentTextChanged(const QString &arg1);

    void on_btnOpen_clicked();

    void on_btnSave_clicked();

    //配置管理表格的菜单
    void onConfTablecustomContextMenuRequested(const QPoint& pos);
    void onCopyAction();
    void onDeleteAction();

    void on_btnDefine_clicked();
    //2025-1008 找到需要chk的条目
    void setChkItem(const QVariantMap& i_dataMap);

    void drawAlarmArea();

    void on_btnTestEmail_clicked();  // 新增测试邮件按钮槽函数
    void sendEmailViaSystem(const QString& recipient, const QString& subject, const QString& body);

    void on_btnSendMail_clicked();
    void onSendAlarmMail(QVariant i_info);
    void exportScatterPlotsToImages();
    QMap<QString, QImage> getScatterPlotImages();
    QImage getSingleScatterPlotImage(const QString &seriesName);
    void exportScatterPlotsWithLimits();
    void exportScatterPlotsWithCustomLimits();
    //html
    void sendHtmlReportEmail();
    void sendEmailWithHtmlContent(const QString& htmlContent);
    void saveHtmlReportToFile();
    void on_btnClearLog_clicked();
    void onShowHanderLogMsg(QString i_log);

    //日常邮件发送
    void onSendDailyMail();
    void on_chkEnabledMail_clicked(bool checked);

private:
    void initUI();
    void initChart();//散点图原始数据chart
    void initChartWgt();//分析后计算的chart
    void parsedCharWgtData(QMap<QString, QList<QPair<QDateTime, double>>>& pMap);
    //散点图显示
    void plotTimeSeriesData(TChartWidget* chartWidget,
                            const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,
                            const QString& chartTitle = "时间序列数据");
    void plotTimeSeriesDataAdvanced(TChartWidget* chartWidget,
                                    const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,
                                    const TimeSeriesPlotOptions& options = TimeSeriesPlotOptions());

    //关于CL的操作 与 CL数据源切换的操作
    void blockComCLModeSignal(bool i_bIsBlocked);
    bool getBlockComCLModeSignal(){
        return m_blockFlag;
    }
    void calCLAlg(const QVariantMap& i_infoMap);
    // 添加辅助函数
    void createControlLines();
    void updateControlLines(const QString& seriesName);
    void setControlLinesVisible(bool visible);
    bool getControlLinesVisible() const;

    //北京版本的，CL线是直线[相当于原来的手动线]
    void createControlLinesManual();
    void clearControlLineManual();
    void updateControlLinesManual(const QString& seriesName);

    //当时间选择近期时截止时间的计算函数
    QDate getStartDate();

    //临时解决多次导入数据软件崩溃的方法
    void cleanupChartWidget();

    //---------------------
    //根据点击的点的信息创建combobox的显示字符串
    QString createComboboxItemText(const QString &seriesName, const QPointF &point);
    //2025-0927 新的CL_manual绘制逻辑相关[按recipeNameRGA的归总多段]
    void loadChartFDCXML();//FDC配置文件中的CL信息
    void loadRecipeDirInfo();//加载已存在的配方文件中的信息
    QString getRecipeValueByChartFDC(const QString& GroupKey,const QString& ChamberName,const QString& channelName);//解析配置中的信息
    void parsedCL_manual(QList<QDateTime>& validStartX,QList<QDateTime>& validEndX,
                         QList<double>& validY_lcl,QList<double>& validY_ucl,QList<QString>& validChamberID);
    void drawManualCL_lines(QList<QDateTime>& validStartX,QList<QDateTime>& validEndX,
                            QList<double>& validY_lcl,QList<double>& validY_ucl,QList<QString>& validChamberID);
    QString m_chartFDCXML_content = "";//xml的文件内容
    //QString 是文件名【基础名称，无路径】,QMap是name与值[first 是lcl sencond是 ucl]值可能是N这个字符
    QMap<QString,QMap<QString,QPair<QString,QString>>>  m_recipeDirInfoMap;//recipe文件夹中的配方信息
    QMap<QString,QPair<QString,QString>> getCurrentRecipeMap(const QString& recipeName,bool& ok);
    // 手动CL线管理
    QList<QLineSeries*> m_manualLCLLines;  // 手动LCL线列表
    QList<QLineSeries*> m_manualUCLLines;  // 手动UCL线列表

    void setManualCLLinesVisible(bool visible);
    void setManualCLLinesVisible(QString flagVal,int flag,bool visible);
    void clearManualCLLines();
    void hideManualCLLineLegends();

    QList<bool> m_isSingleList;//用来标识是 连续点  还是 单个点
    //--------------多段绘制over---------------------

    //chamerID的配置相关
    void loadChamerIDConf();
    void saveChamerIDConf();
    void updateChamerIDConf(const QVariantMap& i_dataMap);

    //key:QString 为chamerID,value: key-O2/H2O，value-下限+上限
    QMap<QString,QMap<QString,QPair<QString,QString>>> m_chamerIDConf;
    QVariantMap m_chamerIDConfMap;
    int m_version = 1;//2025-1005 临时增加 0  代表 广东版即原版  1 代表北京版，临时版
    double m_alarmUpValue = 0.0;    // 告警上线值
    double m_alarmDownValue = 0.0;  // 告警下线值
private:
    bool m_debugBool = true;//控制是否输出
    Ui::sFaultDetectionClassificationRGA ui;
    //cDatabaseFDC* mDatabaseFDC= nullptr;//测试使用
    cApiServer mApiServer;//cApiServer* mApiServer= nullptr;
    sApiClient mApiClient;//临时可用方案，只能有一个网络设备
    QList<QString> mApiClientCMD;
    QChart *chart= nullptr;
    QDateTimeAxis *axisX= nullptr;
    QValueAxis *axisY= nullptr;
    QMap <QString, QList<QPair<QDateTime, double>>> mDataMap;//
    QMap <QString, QList<QPair<QDateTime, QString>>> mChamberIDDataMap;//
    QMap<QString, QScatterSeries*> mScatterSeries;
    sConfigEQ* mConfigEQ= nullptr;
    //QString mCurrentTable;
    bool createNewSeries(QMap<QString, QList<QPair<QDateTime, double>>>& pMap);
    //QString formatTooltip(const QPointF& point, QString name, int index);
    //2025-0905 新增
    TChartWidget* m_chartWgt = nullptr;

    bool m_blockFlag = true;//用于控制CL数据源是否执行信号block的标志位
    //CL绘制相关
    bool m_showControlLines = true;    // 控制线显示标志位
    QLineSeries* m_avgLine = nullptr;     // AVG线
    QLineSeries* m_uclLine = nullptr;     // UCL线
    QLineSeries* m_lclLine = nullptr;     // LCL线

    QLineSeries* m_uclLineManual = nullptr;     // 手动UCL线
    QLineSeries* m_lclLineManual = nullptr;     // 手动LCL线

    QStringList mRecipeEQ;
    QStringList mLotID;
    QStringList mWaferID;
    QStringList mChamberID;
    bool parsedData(const QStringList& listChannel, QStringList& customName, QStringList& customData,
                    const QString& keyEquipmentGroup, const QString& keyChamber, //const QString& keyChannel,
                    const QStringList& RecipeNameEQ, const QStringList& RecipeNameRGA, const QStringList& Step,
                    const QStringList& LotID, const QStringList& SlotID, const QStringList& WaferID, const QStringList& ChamberID,
                    const QList<uint>& dateList, const QStringList& timeList/*, int length, const QVector<double>& pData*/);
    void parsedQuery(const QJsonObject& request, const QString& EquipmentGroupName);
    void addChartSeries(const QString& keyEquipmentGroup, const QString& keyChamber, const QString& keyChannel,
                        const QStringList& RecipeNameEQ, const QStringList& RecipeNameRGA, const QStringList& Step,
                        const QStringList& LotID, const QStringList& SlotID, const QStringList& WaferID,const QStringList& ChamberID,
                        const QList<uint>& dateList, const QStringList& timeList, int length, const QVector<double>& pData);
    void loadLocalData(const QString& chamberName, const QStringList& listChannel, const QString& keyEquipmentGroup,
                       const QString& recipeEQ, const QDate& startDate, const QDate& endDate, const QString& lotID,
                       const QString& waferID,
                       const QString& chamberID,bool& o_ok);
    sRecipeManageFDC* mRecipeManageFDC;

    //对应system.conf文件
    QVariantMap m_systemMap;
    void loadSystemConf();

    //设置超限点的标注颜色
    void setOverLimitedPoints();

    //告警邮件检测
    ReportHandler* m_reportHander = nullptr;
    void setLogMsg(QString i_logMsg);
    bool m_warningShow = true;//为了兼容后台查数据，在没有数据时是否弹框显示

    //日常定时邮件
    QTimer* m_scheduledTimer = nullptr;
    QTime m_scheduledTime;
    bool m_scheduledEnabled = false;
    bool m_isSendDailyOver = true;//防止一分钟内多发-时间精确
    void checkScheduledTime();
    // 通用算法版本
    void calculateGridDimensions(int imgCount, int& rows, int& cols)
    {
        if (imgCount <= 0) {
            rows = 0;
            cols = 0;
            return;
        }

        // 计算列数：图片数的平方根向上取整
        cols = static_cast<int>(ceil(sqrt(imgCount)));

        // 计算行数：能容纳所有图片的最小行数
        rows = static_cast<int>(ceil(static_cast<double>(imgCount) / cols));
    }
};

#endif // SFAULTDETECTIONCLASSIFICATIONRGA_H
