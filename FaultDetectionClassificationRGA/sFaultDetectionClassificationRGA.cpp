
#include "cSerializeJS.h"
#include "sFaultDetectionClassificationRGA.h"
#include <limits>

sFaultDetectionClassificationRGA::sFaultDetectionClassificationRGA(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.splitter_2->setSizes({100, 500});
    ui.tabWidget->setStyleSheet("QTabBar::tab { height: 36px; }");/*tabBar()->setFixedHeight(36);*/
    ui.UI_CB_EQ_RECIPE->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.UI_CB_CHAMBERID->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.UI_CB_LOTID->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.UI_CB_WAFERID->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initUI();
    mConfigEQ= new sConfigEQ();
    connect(mConfigEQ, &sConfigEQ::sUpdateDeviceList, this, &sFaultDetectionClassificationRGA::onUpdateDeviceList);
    mConfigEQ->loadChartFDC();

    mRecipeManageFDC= new sRecipeManageFDC(this);
    ui.UI_LAYOUT_RECIPE_CONFIG_FDC->addWidget(mRecipeManageFDC);
    mRecipeManageFDC->loadChartFDC();
    connect(mRecipeManageFDC, &sRecipeManageFDC::sGetRecipeListEQ, [&](QString& EquipmentGroupName, QString& ip){
        if(!mApiClient.isConnected())
            mApiClient.connectToServer(ip);
        QByteArray array= cExecuteStruct::createRecipeListEQ_Client(EquipmentGroupName, ip);
        mApiClient.sendData(array);
    });
    connect(mRecipeManageFDC, &sRecipeManageFDC::sGetRecipeListRGA, [&](QString& EquipmentGroupName, QString& ip){
        if(!mApiClient.isConnected())
            mApiClient.connectToServer(ip);
        QByteArray array= cExecuteStruct::createRecipeListRGA_Client(EquipmentGroupName, ip);
        mApiClient.sendData(array);
    });
    //    QString folderPath= QCoreApplication::applicationDirPath()+"/Recipe/132";
    //    cSerializeJS::loadTreeFromFile(ui.UI_TW_RECIPE_TREE_RM, folderPath);

    mApiServer.start();
    connect(&mApiClient, &sApiClient::isReceived, this, &sFaultDetectionClassificationRGA::onResponseReceived);
    //connect(&mApiClient, &sApiClient::responseReceived, this, &sFaultDetectionClassificationRGA::onResponseReceived);
    //mApiClient.connectToServer("192.168.69.27");

    connect(this, &sFaultDetectionClassificationRGA::sUpdateCharWgt, [&](){
        parsedCharWgtData(mDataMap);//解析数据 - 计算
    });

    //加载CL与chamerID对应关系配置
    loadChamerIDConf();
}

sFaultDetectionClassificationRGA::~sFaultDetectionClassificationRGA()
{
    if(mConfigEQ)
        delete mConfigEQ;
    mConfigEQ= nullptr;

//    if(mRecipeManageFDC)
//        delete mRecipeManageFDC;
//    mRecipeManageFDC= nullptr;
}



void sFaultDetectionClassificationRGA::on_UI_PB_OK_FDC_clicked()
{
    //2025-0909 临时解决多次加载数据，崩溃问题，崩溃出在图表控件的轴删除与创建上，但是找了半天找不到
    //临时重置全部
    // 第一步：清理现有的图表控件
    cleanupChartWidget();

    // 第二步：重新初始化图表控件
    initChartWgt();
    //-临时方案 直接重置全部的控件，而不是只对chart部分重置,但是界面会闪一下，先这样吧,布局背景色改成白色，稍微好一点--
    mDataMap.clear();
    mChamberIDDataMap.clear();
    //赋值时默认使用区间
    QDate startDate = ui.dateEditStart->date();
    QDate endDate = ui.dateEditEnd->date();
    if(ui.rBtnTimeStep->isChecked()){//如果使用的是 近期
        startDate = getStartDate();
        endDate = QDate::currentDate();
    }
    QString RecipeEQ= ui.UI_CB_EQ_RECIPE->currentText();
    QString lotID= ui.UI_CB_LOTID->currentText();
    QString waferID= ui.UI_CB_WAFERID->currentText();
    QString chamberID= ui.UI_CB_CHAMBERID->currentText();

    QMap<QString, QMap<QString, QStringList>> Curve;//机台<"CHE" VALUE:"N2/H2O,M28/M32,N,N">
    QMap<QString, QString> IP;
    sConfigEQ::traverseItemCheckboxes(ui.UI_TW_EQ_LIST_FDC, Curve, IP);
    if(Curve.isEmpty())
        return;
    for(auto& keyEquipmentGroup: Curve.keys()){
        QMap<QString, QStringList>& chamber= Curve[keyEquipmentGroup];//KEY:"CHE" VALUE:"N2/H2O,M28/M32,N,N"
        if(chamber.isEmpty())
            continue;
        if(IP[keyEquipmentGroup]=="127.0.0.1"){
            for(auto& keyChamber: chamber.keys()){
                bool ok;
                loadLocalData(keyChamber, chamber[keyChamber], keyEquipmentGroup, RecipeEQ, startDate, endDate,
                              lotID, waferID, chamberID,ok);

            }
        }else{
            if(!mApiClient.isConnected())
                mApiClient.connectToServer(IP[keyEquipmentGroup]);
            QByteArray array= cExecuteStruct::createQueryClient(keyEquipmentGroup, RecipeEQ, startDate, endDate,
                                                                lotID, waferID, chamberID, chamber);
            mApiClient.sendData(array);
            mApiClientCMD<< IP[keyEquipmentGroup];
        }
    }
    if(mApiClientCMD.isEmpty())
        emit sUpdateCharWgt();
}

void sFaultDetectionClassificationRGA::onResponseReceived(/*const QJsonObject &response*/)
{

    QByteArray data;
    mApiClient.getDataOnce(data);

    QString action;
    QJsonObject request;
    QString ip;
    QString EquipmentGroupName;
    cExecuteStruct::_TYPE_ACTION TYPE_ACTION= cExecuteStruct::getAction(data, request, action);
    switch (TYPE_ACTION) {
    case cExecuteStruct::_query_ACTION:{
        mApiClientCMD.removeFirst();
        if(!cExecuteStruct::parsedQueryClient(data, request, EquipmentGroupName))
            return;
        parsedQuery(request, EquipmentGroupName);
        if(mApiClientCMD.isEmpty())
            emit sUpdateCharWgt();
        break;
    }
    case cExecuteStruct::_recipeListEQ_ACTION:{
        QStringList RecipeListEQ;
        if(!cExecuteStruct::parsedRecipeListEQ_Client(data, request, EquipmentGroupName, ip, RecipeListEQ))
            return;
        if(!mRecipeManageFDC)
            return;
        mRecipeManageFDC->updateRecipeListEQ(RecipeListEQ, ip);
        break;
    }
    case cExecuteStruct::_recipeListRGA_ACTION:{
        QStringList RecipeListRGA;
        if(!cExecuteStruct::parsedRecipeListRGA_Client(data, request, EquipmentGroupName, ip, RecipeListRGA))
            return;
        if(!mRecipeManageFDC)
            return;
        mRecipeManageFDC->updateRecipeListRGA(RecipeListRGA, ip);
        break;
    }
    default:{
        break;
    }
    }
}

void sFaultDetectionClassificationRGA::loadLocalData(const QString& chamberName, const QStringList& listChannel,
                                                     const QString& keyEquipmentGroup,
                                                     const QString& recipeEQ, const QDate& startDate, const QDate& endDate,
                                                     const QString& lotID,
                                                     const QString& waferID,
                                                     const QString& chamberID, bool &o_ok)
{
    cDatabaseFDC* mDatabaseFDC= sDatabaseFDC::getWidgetFDC();
    if(!mDatabaseFDC)
        return;
    QList<uint> dateList;
    QStringList timeList, customName, customData,//6 0.54,0.52
            totalPressure, RecipeNameEQ, RecipeNameRGA, Step,//10
            LotID, SlotID, WaferID, ChamberID, SlitValve;//15
    o_ok = mDatabaseFDC->getDatabyTimeArea(dateList,
                                              timeList,
                                              customName,
                                              customData,
                                              totalPressure,
                                              RecipeNameEQ,
                                              RecipeNameRGA,
                                              Step,//10
                                              LotID,//11
                                              SlotID,//12
                                              WaferID,//13
                                              ChamberID,//14
                                              SlitValve,//15
                                              recipeEQ,
                                              startDate,
                                              endDate,
                                              lotID,
                                              waferID,
                                              chamberID,
                                              chamberName);
    if(!o_ok){
        QMessageBox::warning(nullptr, tr("提示"), keyEquipmentGroup+ " "+ chamberName/*keyChamber*/+ tr("该区间无数据！"));
        return;//continue;
    }
    qDebug().noquote() << tr("总数据长度:%1").arg(dateList.count());

    parsedData(listChannel, customName, customData,
               keyEquipmentGroup, chamberName,
               RecipeNameEQ, RecipeNameRGA, Step,
               LotID, SlotID, WaferID, ChamberID,
               dateList, timeList);
}

void sFaultDetectionClassificationRGA::loadSystemConf()
{
    QString path = QCoreApplication::applicationDirPath() + "\\system.conf";
    m_systemMap = cJsonFileOperate::jsonFile2QVarint(path).toMap();
}


//void sFaultDetectionClassificationRGA::on_pushButton_4_clicked()
//{
//    QVariantList params;
//    mApiClient.executeQuery(params);
//    //mApiClient.disconnectFromServer();
//}

void sFaultDetectionClassificationRGA::on_UI_PB_DEVICE_CONFIG_FDC_clicked()
{
    mConfigEQ->show();
    //ui.UI_TW_EQ_LIST_FDC
}

void sFaultDetectionClassificationRGA::initUI()
{
    initChartWgt();
    //initChart();

    //某些控件的处理
    ui.dateEditStart->setDate(QDate::currentDate());
    ui.dateEditEnd->setDate(QDate::currentDate().addDays(7));

    loadChartFDCXML();
    loadRecipeDirInfo();
    loadSystemConf();

    //配置管理的
    ui.tableWgt_ChmaerID->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui.tableWgt_ChmaerID, &QTableWidget::customContextMenuRequested,
            this, &sFaultDetectionClassificationRGA::onConfTablecustomContextMenuRequested);
}

void sFaultDetectionClassificationRGA::initChart()
{
    //mApiServer = new cApiServer(this);
    chart = new QChart();

    axisX = new QDateTimeAxis();
    axisX->setTitleText("日期");
    axisX->setFormat("yyyy-MM-dd hh:mm"); //:ss.zzz
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis();
    axisY->setTitleText("数值");
    axisY->setLabelFormat("%.2e");  // 两位小数
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); // 抗锯齿
    // 启用鼠标缩放/平移
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    //chartView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    //chartView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    ui.UI_LAYOUT_CHART_FDC->addWidget(chartView);
}

void sFaultDetectionClassificationRGA::initChartWgt()
{
    /*
    // 图表控件
    m_chartWgt = new TChartWidget();
    ui.UI_LAYOUT_CHART_FDC->addWidget(m_chartWgt);
    */

    // 确保之前的控件已经清理
    if (m_chartWgt) {
        delete m_chartWgt;
        m_chartWgt = nullptr;
    }

    // 图表控件
    m_chartWgt = new TChartWidget(this);

    // 设置对象名称（便于调试）
    m_chartWgt->setObjectName("chartWidget");
 //m_chartWgt->setMouseMode(static_cast<TChartWidget::MouseMode>(1));

    // 添加到布局
    ui.UI_LAYOUT_CHART_FDC->addWidget(m_chartWgt);

    if (m_chartWgt) {

    }
    // 初始化控制线相关变量
    m_showControlLines = true;
    m_avgLine = nullptr;
    m_uclLine = nullptr;
    m_lclLine = nullptr;
    m_uclLineManual = nullptr;
    m_lclLineManual = nullptr;

    //带参数启动后处理[这个connect需要放到这边，因为m_chartWgt会被重置]
    connect(m_chartWgt,SIGNAL(sProcessStart(QStringList)),this,SLOT(onPostHandleProcessStart(QStringList)));

    //-----------点选模式------------------------------
    connect(m_chartWgt, &TChartWidget::pointSelected,
            this, &sFaultDetectionClassificationRGA::onPointSelected);
//    connect(m_chartWgt, &TChartWidget::pointClicked,
//            this, &sFaultDetectionClassificationRGA::onPointClicked);
    connect(m_chartWgt, &TChartWidget::pointDoubleClicked,
            this, &sFaultDetectionClassificationRGA::onPointDoubleClicked);
    //-------------------点选模式----------------------
    m_chartWgt->mOptionMap.clear();
    m_chartWgt->mRecipeNameEQ.clear();
    m_chartWgt->mRecipeNameRGA.clear();
    m_chartWgt->mStep.clear();
    m_chartWgt->mLotID.clear();
    m_chartWgt->mSlotID.clear();
    m_chartWgt->mWaferID.clear();
    qDebug() << "图表控件初始化完成";
}

void sFaultDetectionClassificationRGA::parsedCharWgtData(QMap<QString, QList<QPair<QDateTime, double> > > &pMap)
{
    if(pMap.isEmpty()){
        QMessageBox::warning(nullptr,tr("错误"),tr("parsedCharWgtData-无有效的数据！"));
        return;
    }
    /*
     * pDef->m_dataInfoMap输出: QVariantMap {
            "name1": {
                    "avg": 1.2,//平均值
                    "org_y_list": ["1", "2", "3", "4"],//原始y
                    "avg_type": 0,//算法类型
                    "y_list": ["1.2", "3.2", "3.3"]//0,1算法下分别对应：org_y_list一样，两边差值后的数据
                    "standard_deviation": 2.054,//标准差[这边是用的y_list就算的],
                    "ucl":1.0, //控制限上限
                    "lcl":0.2//控制限下限
                }
            },
            "name2":{}
     * */
    //算法类型  - 0 原始数据平均  ， 1 差值数据平均
    PublicDef* pDef = sPublicDefSingleton::GetInstance();
    int avgType = ui.comAlgType->currentIndex();
    pDef->m_dataInfoMap = PublicAlgorithm::processAverageData(pMap,avgType);
    //数据输出
    //cJsonFileOperate::toJsonFile(pDef->m_dataInfoMap,"typeAvgInfo.json");

    //显示散点图
    plotTimeSeriesData(m_chartWgt, pMap, "RGA数据监控"); 

    //ChamberID控件
    calCLAlg(pDef->m_dataInfoMap);

    if(m_version == 0)
    {
        // 只在第一次创建控制线，后续更新即可
        // 移除重复的 createControlLines() 调用
        if (!m_avgLine || !m_uclLine || !m_lclLine) {
            createControlLines();
        }
    }

    on_comCL_mode_currentIndexChanged(0);//触发一次

}


/**
 * @brief 将时间序列数据绘制到TChartWidget上
 * @param chartWidget TChartWidget实例
 * @param pMap 时间序列数据
 * @param chartTitle 图表标题（可选）
 */
void sFaultDetectionClassificationRGA::plotTimeSeriesData(TChartWidget *chartWidget,
                                                          const QMap<QString, QList<QPair<QDateTime, double> > > &pMap,
                                                          const QString &chartTitle)
{
    if (!chartWidget) {
        qDebug() << "错误: chartWidget 为空";
        return;
    }

    // 设置图表配置为散点图
    TChartWidget::ChartConfig chartConfig;
    chartConfig.type = TChartWidget::ScatterChart;
    chartConfig.title = chartTitle;
    chartConfig.showLegend = true;
    chartConfig.legendAlignment = Qt::AlignRight;
    chartWidget->setChartConfig(chartConfig);

    // 清除现有数据
    //chartWidget->clearSeries();
    chartWidget->clearChart();

    if (pMap.isEmpty()) {
        return;
    }

    // 计算时间范围和数值范围
    QDateTime minDateTime, maxDateTime;
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    bool firstPoint = true;

    // 遍历所有数据系列，计算范围
    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        const QList<QPair<QDateTime, double>>& dataList = it.value();
        for (const QPair<QDateTime, double>& point : dataList) {
            if (firstPoint) {
                minDateTime = point.first;
                maxDateTime = point.first;
                minY = point.second;
                maxY = point.second;
                firstPoint = false;
            } else {
                if (point.first < minDateTime) minDateTime = point.first;
                if (point.first > maxDateTime) maxDateTime = point.first;
                if (point.second < minY) minY = point.second;
                if (point.second > maxY) maxY = point.second;
            }
        }
    }

    // 设置X轴配置（时间轴）- 关键修改：指定轴类型
    TChartWidget::AxisConfig xAxisConfig;
    xAxisConfig.title = "时间";
    xAxisConfig.autoRange = false;
    xAxisConfig.min = minDateTime.toMSecsSinceEpoch();
    xAxisConfig.max = maxDateTime.toMSecsSinceEpoch();
    xAxisConfig.axisType = TChartWidget::DateTimeAxis;  // 指定为时间轴
    xAxisConfig.labelAngle = -30;  // 设置为-30度（斜30度角）
    chartWidget->setXAxisConfig(xAxisConfig);

    // 设置Y轴配置
    TChartWidget::AxisConfig yAxisConfig;
    yAxisConfig.title = "数值";
    yAxisConfig.autoRange = false;
    yAxisConfig.min = minY - (maxY - minY) * 0.1; // 添加10%边距
    yAxisConfig.max = maxY + (maxY - minY) * 0.1;
    yAxisConfig.axisType = TChartWidget::NumericAxis;  // Y轴为数值轴
    yAxisConfig.labelAngle = 0;  // Y轴标签保持水平
    chartWidget->setYAxisConfig(yAxisConfig);

    // 预定义颜色列表
    /*
    QList<QColor> colors = {
        Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan,
        Qt::yellow, Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta
    };
    */
    QList<QColor> colors = {
        Qt::cyan, Qt::blue, Qt::green, Qt::magenta,Qt::red,
        Qt::yellow, Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta
    };

    // 添加每个数据系列
    int colorIndex = 0;
    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        QString seriesName = it.key();
        const QList<QPair<QDateTime, double>>& dataList = it.value();

        if (dataList.isEmpty()) {
            continue;
        }

        // 创建数据点
        QVector<QPointF> points;
        points.reserve(dataList.size());

        for (const QPair<QDateTime, double>& dataPoint : dataList) {
            // 将QDateTime转换为毫秒数
            qreal x = dataPoint.first.toMSecsSinceEpoch();
            qreal y = dataPoint.second;
            points.append(QPointF(x, y));
        }

        // 创建系列数据
        TChartWidget::SeriesData series;
        series.name = seriesName;
        series.points = points;
        series.color = colors[colorIndex % colors.size()];
        series.pointSize = 10.0; // 散点大小

        // 添加到图表
        chartWidget->addSeries(series);

        colorIndex++;
    }

    // 更新图表
    chartWidget->updateChart();
}


void sFaultDetectionClassificationRGA::plotTimeSeriesDataAdvanced(TChartWidget *chartWidget,
                                                                  const QMap<QString, QList<QPair<QDateTime, double> > > &pMap,
                                                                  const sFaultDetectionClassificationRGA::TimeSeriesPlotOptions &options)
{
    if (!chartWidget) {
        qDebug() << "错误: chartWidget 为空";
        return;
    }

    // 设置图表配置
    TChartWidget::ChartConfig chartConfig;
    chartConfig.type = TChartWidget::ScatterChart;
    chartConfig.title = options.title;
    chartConfig.showLegend = options.showLegend;
    chartWidget->setChartConfig(chartConfig);

    // 设置图例位置
    chartWidget->setLegendPosition(options.legendPos);

    // 清除现有数据
    chartWidget->clearSeries();

    if (pMap.isEmpty()) {
        return;
    }

    // 计算范围
    QDateTime minDateTime, maxDateTime;
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    bool firstPoint = true;

    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        const QList<QPair<QDateTime, double>>& dataList = it.value();
        for (const QPair<QDateTime, double>& point : dataList) {
            if (firstPoint) {
                minDateTime = point.first;
                maxDateTime = point.first;
                minY = point.second;
                maxY = point.second;
                firstPoint = false;
            } else {
                if (point.first < minDateTime) minDateTime = point.first;
                if (point.first > maxDateTime) maxDateTime = point.first;
                if (point.second < minY) minY = point.second;
                if (point.second > maxY) maxY = point.second;
            }
        }
    }

    // 设置X轴配置（时间轴）- 关键修改：指定轴类型
    TChartWidget::AxisConfig xAxisConfig;
    xAxisConfig.title = "时间";
    xAxisConfig.autoRange = options.autoRangeX;
    if (!options.autoRangeX) {
        xAxisConfig.min = minDateTime.toMSecsSinceEpoch();
        xAxisConfig.max = maxDateTime.toMSecsSinceEpoch();
    }
    xAxisConfig.tickCount = 10;
    xAxisConfig.axisType = TChartWidget::DateTimeAxis;  // 指定为时间轴
    xAxisConfig.labelAngle = -45;  // 设置为-45度（斜45度角）
    chartWidget->setXAxisConfig(xAxisConfig);

    // 设置Y轴配置
    TChartWidget::AxisConfig yAxisConfig;
    yAxisConfig.title = "数值";
    yAxisConfig.autoRange = options.autoRangeY;
    if (!options.autoRangeY) {
        double margin = (maxY - minY) * options.yMarginPercent / 100.0;
        yAxisConfig.min = minY - margin;
        yAxisConfig.max = maxY + margin;
    }
    yAxisConfig.tickCount = 8;
    yAxisConfig.axisType = TChartWidget::NumericAxis;  // Y轴为数值轴
    yAxisConfig.labelAngle = 0;  // Y轴标签保持水平
    chartWidget->setYAxisConfig(yAxisConfig);

    // 预定义颜色列表
    QList<QColor> defaultColors = {
        Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan,
        Qt::yellow, Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta
    };

    // 添加数据系列
    int colorIndex = 0;
    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        QString seriesName = it.key();
        const QList<QPair<QDateTime, double>>& dataList = it.value();

        if (dataList.isEmpty()) {
            continue;
        }

        // 创建数据点
        QVector<QPointF> points;
        points.reserve(dataList.size());

        for (const QPair<QDateTime, double>& dataPoint : dataList) {
            qreal x = dataPoint.first.toMSecsSinceEpoch();
            qreal y = dataPoint.second;
            points.append(QPointF(x, y));
        }

        // 确定颜色
        QColor seriesColor;
        if (options.customColors.contains(seriesName)) {
            seriesColor = options.customColors[seriesName];
        } else {
            seriesColor = defaultColors[colorIndex % defaultColors.size()];
        }

        // 创建系列数据
        TChartWidget::SeriesData series;
        series.name = seriesName;
        series.points = points;
        series.color = seriesColor;
        series.pointSize = options.pointSize;

        // 添加到图表
        chartWidget->addSeries(series);

        colorIndex++;
    }

    // 更新图表
    chartWidget->updateChart();
}

void sFaultDetectionClassificationRGA::blockComCLModeSignal(bool i_bIsBlocked)
{
    m_blockFlag = i_bIsBlocked;
}

/*!
 * \brief sFaultDetectionClassificationRGA::calCLAlg 绘制CL的3条直线
 * \param i_infoMap
 */
void sFaultDetectionClassificationRGA::calCLAlg(const QVariantMap &i_infoMap)
{
    blockComCLModeSignal(true);//禁止信号绑定
    QStringList keys = i_infoMap.keys();
    QString all = "ALL";
    keys.removeOne(all);
    if(keys.count() > 0 && ui.comCL_mode != nullptr){
        ui.comCL_mode->clear();
        ui.comCL_mode->addItems(keys);
        //ALL加在最后面
        ui.comCL_mode->addItem("ALL");
    }
    blockComCLModeSignal(false);//信号绑定

}

void sFaultDetectionClassificationRGA::createControlLines()
{
    // 添加空指针检查
    if (!m_chartWgt) {
        qDebug() << "错误: m_chartWgt 为空";
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        qDebug() << "错误: chart 为空";
        return;
    }

    try {
        // 安全地删除已存在的控制线
        if (m_avgLine) {
            if (chart->series().contains(m_avgLine)) {
                chart->removeSeries(m_avgLine);
            }
            delete m_avgLine;
            m_avgLine = nullptr;
        }

        if (m_uclLine) {
            if (chart->series().contains(m_uclLine)) {
                chart->removeSeries(m_uclLine);
            }
            delete m_uclLine;
            m_uclLine = nullptr;
        }

        if (m_lclLine) {
            if (chart->series().contains(m_lclLine)) {
                chart->removeSeries(m_lclLine);
            }
            delete m_lclLine;
            m_lclLine = nullptr;
        }

        // 创建新的控制线
        m_avgLine = new QLineSeries();
        m_avgLine->setName("AVG");
        m_avgLine->setColor(Qt::green);
        m_avgLine->setPen(QPen(Qt::green, 2, Qt::DashLine));

        m_uclLine = new QLineSeries();
        m_uclLine->setName("UCL");
        m_uclLine->setColor(Qt::red);
        m_uclLine->setPen(QPen(Qt::red, 2, Qt::DashLine));

        m_lclLine = new QLineSeries();
        m_lclLine->setName("LCL");
        m_lclLine->setColor(Qt::red);
        m_lclLine->setPen(QPen(Qt::red, 2, Qt::DashLine));

        // 添加到图表
        chart->addSeries(m_avgLine);
        chart->addSeries(m_uclLine);
        chart->addSeries(m_lclLine);

        // 关联轴
        QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
        QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);

        if (!xAxisList.isEmpty() && !yAxisList.isEmpty()) {
            // 确保系列还没有关联轴
            if (m_avgLine->attachedAxes().isEmpty()) {
                m_avgLine->attachAxis(xAxisList.first());
                m_avgLine->attachAxis(yAxisList.first());
            }

            if (m_uclLine->attachedAxes().isEmpty()) {
                m_uclLine->attachAxis(xAxisList.first());
                m_uclLine->attachAxis(yAxisList.first());
            }

            if (m_lclLine->attachedAxes().isEmpty()) {
                m_lclLine->attachAxis(xAxisList.first());
                m_lclLine->attachAxis(yAxisList.first());
            }
        }

        // 设置显示状态
        m_avgLine->setVisible(m_showControlLines);
        m_uclLine->setVisible(m_showControlLines);
        m_lclLine->setVisible(m_showControlLines);

        qDebug() << "控制线创建完成";

    } catch (const std::exception& e) {
        qDebug() << "createControlLines - Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "createControlLines - Unknown exception occurred";
    }
}


void sFaultDetectionClassificationRGA::updateControlLines(const QString& seriesName)
{
    try {
        if(m_version == 0)
        {
            // 可能抛出异常的代码
            if (!m_avgLine || !m_uclLine || !m_lclLine) {
                createControlLines();
            }

            PublicDef* pDef = sPublicDefSingleton::GetInstance();
            QVariantMap infoMap = pDef->m_dataInfoMap;
            double uclManual = pDef->m_uclValueManual;
            double lclManual = pDef->m_lclValueManual;

            if (infoMap.contains(seriesName)) {
                QVariantMap dataMap = infoMap.value(seriesName).toMap();
                double avg = dataMap["avg"].toDouble();
                double ucl = dataMap["ucl"].toDouble();
                double lcl = dataMap["lcl"].toDouble();

                // 获取X轴范围来创建水平线
                QChart* chart = m_chartWgt->getChart();
                QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
                if (!xAxisList.isEmpty()) {
                    QAbstractAxis* xAxis = xAxisList.first();

                    // 获取X轴范围
                    qreal xMin, xMax;
                    if (xAxis->type() == QAbstractAxis::AxisTypeDateTime) {
                        QDateTimeAxis* dtAxis = qobject_cast<QDateTimeAxis*>(xAxis);
                        if (dtAxis) {
                            xMin = dtAxis->min().toMSecsSinceEpoch();
                            xMax = dtAxis->max().toMSecsSinceEpoch();
                        } else {
                            xMin = 0;
                            xMax = 1000;
                        }
                    } else {
                        QValueAxis* valAxis = qobject_cast<QValueAxis*>(xAxis);
                        if (valAxis) {
                            xMin = valAxis->min();
                            xMax = valAxis->max();
                        } else {
                            xMin = 0;
                            xMax = 1000;
                        }
                    }

                    //最大值最小值赋值【其他地方绘制直线用到】
                    pDef->m_max_x = xMax;
                    pDef->m_min_x = xMin;

                    // 创建水平线数据点
                    QVector<QPointF> avgPoints, uclPoints, lclPoints;
                    avgPoints << QPointF(xMin, avg) << QPointF(xMax, avg);
                    uclPoints << QPointF(xMin, ucl) << QPointF(xMax, ucl);
                    lclPoints << QPointF(xMin, lcl) << QPointF(xMax, lcl);

                    // 更新线数据
                    m_avgLine->replace(avgPoints);
                    m_uclLine->replace(uclPoints);
                    m_lclLine->replace(lclPoints);

                    // 设置显示状态
                    m_avgLine->setVisible(m_showControlLines);
                    m_uclLine->setVisible(m_showControlLines);
                    m_lclLine->setVisible(m_showControlLines);

                    qDebug() << QString("更新控制线: AVG=%1, UCL=%2, LCL=%3").arg(avg).arg(ucl).arg(lcl);
                }
            } else {
                // 如果选择了"ALL"或其他不存在的系列，隐藏控制线
                m_avgLine->setVisible(false);
                m_uclLine->setVisible(false);
                m_lclLine->setVisible(false);
            }
        }

    } catch (const std::exception& e) {
        qDebug() << "updateControlLines-Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "updateControlLines-Unknown exception occurred";
    }
}


void sFaultDetectionClassificationRGA::setControlLinesVisible(bool visible)
{
    m_showControlLines = visible;
    if (m_avgLine) m_avgLine->setVisible(visible);
    if (m_uclLine) m_uclLine->setVisible(visible);
    if (m_lclLine) m_lclLine->setVisible(visible);
}

void sFaultDetectionClassificationRGA::createControlLinesManual()
{
    // 添加空指针检查
    if (!m_chartWgt) {
        qDebug() << "错误: m_chartWgt 为空";
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        qDebug() << "错误: chart 为空";
        return;
    }

    try {
        // 安全地删除已存在的控制线

        if (m_uclLineManual) {
            if (chart->series().contains(m_uclLineManual)) {
                chart->removeSeries(m_uclLineManual);
            }
            delete m_uclLineManual;
            m_uclLineManual = nullptr;
        }

        if (m_lclLineManual) {
            if (chart->series().contains(m_lclLineManual)) {
                chart->removeSeries(m_lclLineManual);
            }
            delete m_lclLineManual;
            m_lclLineManual = nullptr;
        }

        // 创建新的控制线

        m_uclLineManual = new QLineSeries();
        m_uclLineManual->setName("UCL");
        m_uclLineManual->setColor(Qt::red);
        m_uclLineManual->setPen(QPen(Qt::red, 2, Qt::SolidLine));

        m_lclLineManual = new QLineSeries();
        m_lclLineManual->setName("LCL");
        m_lclLineManual->setColor(Qt::red);
        m_lclLineManual->setPen(QPen(Qt::red, 2, Qt::SolidLine));

        QString chamberID = ui.UI_CB_CHAMBERID->currentText();
        QString seriesName = ui.comCL_mode->currentText();
        //点击CL线显示信息
        connect(m_uclLineManual, &QLineSeries::clicked, [=](const QPointF& point){
                Q_UNUSED(point)
                QString msg = QString(tr("ChamberID:%1\n数据轴:%2\nUCL:%3")
                                      .arg(chamberID).arg(seriesName).arg(m_alarmUpValue));
                QToolTip::showText(QCursor::pos(), msg);
        });
        connect(m_lclLineManual, &QLineSeries::clicked, [=](const QPointF& point){
                Q_UNUSED(point)
                QString msg = QString(tr("ChamberID:%1\n数据轴:%2\nLCL:%3")
                                      .arg(chamberID).arg(seriesName).arg(m_alarmDownValue));
                QToolTip::showText(QCursor::pos(), msg);
        });

        // 添加到图表
        chart->addSeries(m_uclLineManual);
        chart->addSeries(m_lclLineManual);

        // 关联轴
        QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
        QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);

        if (!xAxisList.isEmpty() && !yAxisList.isEmpty()) {
            // 确保系列还没有关联轴
            if (m_uclLineManual->attachedAxes().isEmpty()) {
                m_uclLineManual->attachAxis(xAxisList.first());
                m_uclLineManual->attachAxis(yAxisList.first());
            }

            if (m_lclLineManual->attachedAxes().isEmpty()) {
                m_lclLineManual->attachAxis(xAxisList.first());
                m_lclLineManual->attachAxis(yAxisList.first());
            }
        }

        // 设置显示状态
        m_uclLineManual->setVisible(m_showControlLines);
        m_lclLineManual->setVisible(m_showControlLines);

        qDebug() << "控制线创建完成";

    } catch (const std::exception& e) {
        qDebug() << "createControlLinesManual - Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "createControlLinesManual - Unknown exception occurred";
    }
}

void sFaultDetectionClassificationRGA::clearControlLineManual()
{
    // 添加空指针检查
    if (!m_chartWgt) {
        qDebug() << "错误: m_chartWgt 为空";
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        qDebug() << "错误: chart 为空";
        return;
    }

    try {
        // 安全地删除已存在的控制线

        if (m_uclLineManual) {
            if (chart->series().contains(m_uclLineManual)) {
                chart->removeSeries(m_uclLineManual);
            }
            delete m_uclLineManual;
            m_uclLineManual = nullptr;
        }

        if (m_lclLineManual) {
            if (chart->series().contains(m_lclLineManual)) {
                chart->removeSeries(m_lclLineManual);
            }
            delete m_lclLineManual;
            m_lclLineManual = nullptr;
        }

    } catch (const std::exception& e) {
        qDebug() << "clearControlLineManual - Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "clearControlLineManual - Unknown exception occurred";
    }
}

void sFaultDetectionClassificationRGA::updateControlLinesManual(const QString &seriesName)
{
    //重新绘制CL
    QString chamberID = ui.UI_CB_CHAMBERID->currentText();
    QMap<QString,QPair<QString,QString>> pairMap = m_chamerIDConf.value(chamberID);

    QString key = "";
    //取当前的CL值
    QStringList tmpList = seriesName.split(" ");
    if(tmpList.count() == 3){
        key = tmpList.value(2);
    }

    QPair<QString,QString> clPair = pairMap.value(key);
    double lcl = clPair.first.toDouble();
    double ucl = clPair.second.toDouble();

    //赋值告警值，【给背景面积图绘制使用】
    m_alarmUpValue = ucl;
    m_alarmDownValue = lcl;

    PublicDef* pDef = sPublicDefSingleton::GetInstance();
    //开始绘制
    // 获取X轴范围来创建水平线
    QChart* chart = m_chartWgt->getChart();
    QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
    if (!xAxisList.isEmpty()) {
        QAbstractAxis* xAxis = xAxisList.first();

        // 获取X轴范围
        qreal xMin, xMax;
        if (xAxis->type() == QAbstractAxis::AxisTypeDateTime) {
            QDateTimeAxis* dtAxis = qobject_cast<QDateTimeAxis*>(xAxis);
            if (dtAxis) {
                xMin = dtAxis->min().toMSecsSinceEpoch();
                xMax = dtAxis->max().toMSecsSinceEpoch();
            } else {
                xMin = 0;
                xMax = 1000;
            }
        } else {
            QValueAxis* valAxis = qobject_cast<QValueAxis*>(xAxis);
            if (valAxis) {
                xMin = valAxis->min();
                xMax = valAxis->max();
            } else {
                xMin = 0;
                xMax = 1000;
            }
        }

        //最大值最小值赋值【其他地方绘制直线用到】
        pDef->m_max_x = xMax;
        pDef->m_min_x = xMin;

        // 创建水平线数据点
        QVector<QPointF> uclPoints, lclPoints;
        uclPoints << QPointF(xMin, ucl) << QPointF(xMax, ucl);
        lclPoints << QPointF(xMin, lcl) << QPointF(xMax, lcl);

        // 更新线数据
        m_uclLineManual->replace(uclPoints);
        m_lclLineManual->replace(lclPoints);

        // 设置显示状态
        m_uclLineManual->setVisible(m_showControlLines);
        m_lclLineManual->setVisible(m_showControlLines);

        qDebug() << QString("updateControlLinesManual-更新控制线: UCL=%1, LCL=%2").arg(ucl).arg(lcl);
    }

    qDebug().noquote() << "seriesName:  " << seriesName;
    qDebug().noquote() << "chamberID:  " << chamberID;
    qDebug().noquote() << "pairMap:  " << pairMap.keys();
    qDebug().noquote() << "key:  " << key;
    qDebug().noquote() << "lcl:  " << lcl;
    qDebug().noquote() << "ucl:  " << ucl;
}

QDate sFaultDetectionClassificationRGA::getStartDate()
{
    QDate startDate = QDate::currentDate();
    int flag = ui.UI_CB_DATE_FDC->currentIndex();// 0 1 2 分别代表 近三月 近一月 近一周
    switch (flag)
    {
    case 0:
        startDate = startDate.addMonths(-3);
        break;
    case 1:
        startDate = startDate.addMonths(-1);
        break;
    case 2:
        startDate = startDate.addDays(-7);
        break;
    default:
        startDate = startDate.addDays(7);
        break;

    }

    return startDate;
}

void sFaultDetectionClassificationRGA::cleanupChartWidget()
{
    // 1. 断开所有信号连接
    if (m_chartWgt) {
        // 断开可能的信号连接
        disconnect(m_chartWgt, nullptr, this, nullptr);
    }

    // 2. 从布局中移除控件
    if (ui.UI_LAYOUT_CHART_FDC) {
        QLayoutItem* item;
        while ((item = ui.UI_LAYOUT_CHART_FDC->takeAt(0)) != nullptr) {
            QWidget* widget = item->widget();
            if (widget) {
                widget->setParent(nullptr); // 从父对象中移除
                widget->deleteLater();      // 安全删除
            }
            delete item;
        }
    }

    // 3. 重置相关指针
    m_chartWgt = nullptr;
    m_avgLine = nullptr;
    m_uclLine = nullptr;
    m_lclLine = nullptr;
    m_uclLineManual = nullptr;
    m_lclLineManual = nullptr;

    // 4. 重置标志位
    m_showControlLines = true;

    qDebug() << "图表控件清理完成";
}

QString sFaultDetectionClassificationRGA::createComboboxItemText(const QString &seriesName, const QPointF &point)
{
    //取消时需要把combobox中的数据同步取消
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(point.x());

    QHash <QString, QList<uint>> optionMap = m_chartWgt->mOptionMap;
    //日期 到 日[这个没办法获得具体数据,point的传入是到毫秒,dataLst就无法对应单个点，虽然往combox加入也会过滤]
    //QList<uint> dataLst = optionMap.value(seriesName);
    //时间 到毫秒
    QList<QPair<QDateTime, double>> pntsList = mDataMap.value(seriesName);
    QStringList recipeEQList = m_chartWgt->mRecipeNameEQ.value(seriesName);
    QStringList recipeRGAList = m_chartWgt->mRecipeNameRGA.value(seriesName);
    QStringList stepList = m_chartWgt->mStep.value(seriesName);
    QStringList lotIDList = m_chartWgt->mLotID.value(seriesName);
    QStringList slotIDList = m_chartWgt->mSlotID.value(seriesName);
    QStringList waferIDList = m_chartWgt->mWaferID.value(seriesName);

    int index = PublicDef::findDateTimeIndexLinear(pntsList,dateTime);
    QString dateTimeStr = dateTime.toString("yyyyMMddhhmmss");
    QString recipeEQ = recipeEQList.value(index);
    QString recipeRGA = recipeRGAList.value(index);
    QString step = stepList.value(index);
    QString lotID = lotIDList.value(index);
    QString slotID = slotIDList.value(index);
    QString waferID = waferIDList.value(index);

    //组建数据给combobox 已秒这个时间级别添加【不然就跟双击点的数量对不上，至于打开后处理在制定具体的检索文件规则】
    //注意这个格式如果修改需要同步修改【1.publicDef中的解析方法 2.后处理的接入参数创建 3.后处理获取参数组建】
    /*
    QString itemStr = QString("%1(EQ@%2)(RGA@%3)(WaferID@%4)(LotID@%5)(SlotID@%6)")
            .arg(dateTimeStr).arg(recipeEQ).arg(recipeRGA).arg(waferID).arg(lotID).arg(slotID);
    */
    QString itemStr = PublicDef::formatComboboxItemText(dateTimeStr,recipeEQ,
                                                       recipeRGA,waferID,lotID,slotID);

    return itemStr;
}

void sFaultDetectionClassificationRGA::loadChartFDCXML()
{
    QString fdcFile_xml = QCoreApplication::applicationDirPath() + "\\ChartFDC";
    if(!QFile::exists(fdcFile_xml)){
        return;
    }

    QFile file(fdcFile_xml);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_chartFDCXML_content = file.readAll();
    }
    file.close();
}

void sFaultDetectionClassificationRGA::loadRecipeDirInfo()
{
    //QString 是文件名【基础名称，无路径】,QMap是name与值[first 是lcl sencond是 ucl]
    //QMap<QString,QMap<QString,QPair<double,double>>>  m_recipeDirInfoMap;//recipe文件夹中的配方信息
    m_recipeDirInfoMap.clear();
    QString recipeDir = QCoreApplication::applicationDirPath() + "\\Recipe";
    QMap<QString, QString> recipeStrMap = PublicDef::getIniFilesWithJsCalculate(recipeDir);
    foreach (QString fileName, recipeStrMap.keys())
    {
        QMap<QString,QPair<QString,QString>> pairMap;

        QString JsCalculate = recipeStrMap.value(fileName);
        QStringList tmpList = JsCalculate.split(";");
        if(tmpList.count() > 0){
            foreach (QString val, tmpList) {
                QStringList singleList = val.split(",");
                if(singleList.count() >= 4){
                    QString name = singleList.value(0);
                    QString lcl = singleList.value(2);
                    QString ucl = singleList.value(3);

                    pairMap.insert(name,qMakePair(lcl,ucl));
                }
            }

        }
        m_recipeDirInfoMap.insert(fileName,pairMap);
    }
}

/*!
 * \brief sFaultDetectionClassificationRGA::getRecipeValueByChartFDC
 * \param GroupKey
 * \param ChamberName
 * \param channelName
 */
QString sFaultDetectionClassificationRGA::getRecipeValueByChartFDC(const QString &GroupKey, const QString &ChamberName, const QString &channelName)
{
    QString channelValue = "";
    QMap<QString, EquipmentGroupInfo> infoMap = XMLReadHelp::parseEquipmentGroupsFromXML(m_chartFDCXML_content);

    EquipmentGroupInfo groupInfo = infoMap.value(GroupKey);
    QList<ChannelInfo> channelInfo = groupInfo.channels;
    foreach (ChannelInfo info, channelInfo) {
        if(info.chamberName.toLower() == ChamberName.toLower() &&
                info.name.toLower() == channelName.toLower())
        {
            channelValue = info.value;
        }
    }

    if(m_debugBool){
        qDebug().noquote() << "infoMap: " << infoMap.keys();
    }
    return channelValue;
}

/*!
 * \brief sFaultDetectionClassificationRGA::parsedCL_manual 绘制多段手动CL线
 */
void sFaultDetectionClassificationRGA::parsedCL_manual(QList<QDateTime> &validStartX, QList<QDateTime> &validEndX,
                                                       QList<double> &validY_lcl, QList<double> &validY_ucl, QList<QString> &validChamberID)
{
    QString seriesName = ui.comCL_mode->currentText();
    if(seriesName.toLower() == "all"){
        return;
    }

    validStartX.clear();
    validEndX.clear();
    validY_lcl.clear();
    validY_ucl.clear();
    validChamberID.clear();

    //轴的数据
    QList<QPair<QDateTime, double>> pntsList = mDataMap.value(seriesName);
    if(pntsList.count() <= 0){
        QMessageBox::warning(nullptr,tr("错误"),tr("parsedCL_manual中数据！"));
        return;
    }

    //按recipeRGA规整信息，注意这边的顺序跟点的x轴顺序是一致的（数据就是这么存储的）
    //x轴的时间也是连续的，就是说在同一个seriesName点的x坐标唯一
    QStringList recipeRGAList = m_chartWgt->mRecipeNameRGA.value(seriesName);
    QPair<QList<QStringList>, QList<QList<int>>> recipeRGAPairs = PublicDef::groupConsecutiveItems(recipeRGAList);

    //QStringList chamberIDList = m_chartWgt->mChamberID.value(seriesName);
    //QPair<QList<QStringList>, QList<QList<int>>> chamberIDPairs = PublicDef::groupConsecutiveItems(chamberIDList);

    //这两个list长度必相等,且通位置下的长度也是一样的
    QList<QStringList> optionRecipeRGA = recipeRGAPairs.first;
    QList<QList<int>> optionIndexs = recipeRGAPairs.second;

    int maxIndex = optionIndexs.last().last();//用于单点是否越界的判断

    //QList<QStringList> optionChamberIDList = chamberIDPairs.first;
    //QList<QList<int>> optionChamberIDIndexs = chamberIDPairs.second;

    //从seriesName中解析ChamberName 与 channelName [SeriesName= keyEquipmentGroup+" "+ keyChamber+" "+ keyChannel;]
    QString EquipmentGroupName = "";
    QString ChamberName = "";
    QString channelName = "";
    QStringList tmpList = seriesName.split(" ");
    if(tmpList.count() >= 3){
        EquipmentGroupName = tmpList.value(0);
        ChamberName = tmpList.value(1);
        channelName = tmpList.value(2);
    }
    //equipmentGroupNo当前配置里面没有值
    //seriesName-parsed-equipmentGroupNo:,ChamberName:CHE,channelName:N2/Ar
    qDebug().noquote() << QString("seriesName-parsed-EquipmentGroupName:%1,ChamberName:%2,channelName:%3")
                          .arg(EquipmentGroupName).arg(ChamberName).arg(channelName);

    //注意：根据EquipmentGroupName得到EquipmentGroupNo [这个应该是临时方案，后续数据结构扩展的话，可能有其他方式]
    QString EquipmentGroupNo = "0";
    QMap<QString, EquipmentGroupInfo> infoMap = XMLReadHelp::parseEquipmentGroupsFromXML(m_chartFDCXML_content);
    //XMLReadHelp::createGroupKey规则一样,这边默认不同的no不可以起同一个名字，这样也不合理
    foreach (QString key, infoMap.keys()) {
        QStringList tmpList = key.split("_");
        if(tmpList.count() == 2){
            QString tmpNo = tmpList.value(0);
            QString tmpName = tmpList.value(1);
            if(tmpName.toLower() == EquipmentGroupName.toLower()){
                EquipmentGroupNo = tmpNo;
            }
        }
    }

    QList<QPair<QDateTime, QString>>& pChamberIDList= mChamberIDDataMap[seriesName];

    m_isSingleList.clear();//用来标识是 连续点  还是 单个点
    //2025-1003_10:06 全部点都绘制【连续点，单点】单点前后借点
    for (int i = 0;i < optionRecipeRGA.count();i++)
    {
        //1.默认多个点 先赋值
        QStringList rga = optionRecipeRGA.value(i);
        QString curRecipeName = rga.value(0);//rga值都是一样的
        //获取x轴的起止值
        QList<int> curIndexList = optionIndexs.value(i);
        int startIndex = curIndexList.value(0);
        int endIndex = curIndexList.last();

        bool isSingle = false;
        //如果是单点 - 前后借点绘制
        if(rga.count() == 1){
            isSingle = true;

            //起始点，终止点 各位两边借1个
            startIndex = startIndex - 1;
            endIndex = endIndex + 1;
            startIndex = startIndex > 0 ? startIndex:0;//防越界
            endIndex = endIndex > maxIndex ? maxIndex:endIndex;
        }
        m_isSingleList.append(isSingle);//标识是否单点

        //开始
        //取值 x
        QPair<QDateTime, double> startPair = pntsList.value(startIndex);
        QPair<QDateTime, double> endPair = pntsList.value(endIndex);
        QDateTime startTime = startPair.first;//开始x
        QDateTime endTime = endPair.first;//结束x
        //取值 y
        QString uclStr = "N";
        QString lclStr = "N";
        //1.取配方
        //1.1 优先从recipe文件夹中取 - 直接使用channelName获取
        bool ok = false;
        //"N2/H2O,M28/M18,N,N;O2/H2O,M32/M18,1,2;N2/Ar,M28/M40,N,N"，拆分的mao
        //{"N2/H2O",pair(N,N)},{"O2/H2O",pair(1,2)}...
        QMap<QString, QPair<QString, QString> > clMap = getCurrentRecipeMap(curRecipeName,ok);
        //1.2 如果配方文件夹不存该配方文件 - 从配置中获得
        if(clMap.contains(channelName)){
            QPair<QString, QString> clInfo = clMap.value(channelName);
            lclStr = clInfo.first;
            uclStr = clInfo.second;
        }
        else
        {
            //1.2 如果配方文件夹不存该配方文件 - 从配置中获得
            QString groupKey = XMLReadHelp::createGroupKey(EquipmentGroupNo,EquipmentGroupName);
            QString channelValue =  getRecipeValueByChartFDC(groupKey,ChamberName,channelName);
            ChannelValueInfo valueInfo = XMLReadHelp::parseChannelValue(channelValue);
            lclStr = valueInfo.param1;
            uclStr = valueInfo.param2;
        }

        //判断是否为有效数据
        if(lclStr.toLower() == "n" || uclStr.toLower() == "n")
        {
            continue;
        }

        validStartX.append(startTime);
        validEndX.append(endTime);
        validY_lcl.append(lclStr.toDouble());
        validY_ucl.append(uclStr.toDouble());

        //2. 界面上上可操作的所有 组建线的flag
        QStringList eqNames = m_chartWgt->mRecipeNameEQ.value(seriesName);
        QStringList lotIDs = m_chartWgt->mLotID.value(seriesName);
        //m_chartWgt->mSlotID.value(seriesName);
        QStringList waferIDs = m_chartWgt->mWaferID.value(seriesName);
        QString flagStr = QString("%1_%2_%3_%4")
                .arg(eqNames.value(startIndex))
                .arg(lotIDs.value(startIndex))
                .arg(waferIDs.value(startIndex))
                .arg(pChamberIDList.value(startIndex).second);
        validChamberID.append(flagStr);
    }


    //2025-1003 之前的逻辑，先不删除，也不启用
    bool isOrg = false;
    if(isOrg)
    {
        //开始组建数据

        //1.准备绘制CL线的数据 - 若只有一个点就不绘制，不然好多散点在一个配方下就1个点，会导致绘制暴多
        //1.1 去单点
        QList<QStringList> optionRecipeRGAParsed;
        QList<QList<int>> optionIndexsParsed;
        for (int i=0;i < optionRecipeRGA.count();i++) {
            QStringList rga = optionRecipeRGA.value(i);

            if(rga.count() > 1){
                optionRecipeRGAParsed.append(optionRecipeRGA.value(i));
                optionIndexsParsed.append(optionIndexs.value(i));
            }
        }


        QList<QPair<QDateTime, QString>>& pChamberIDList= mChamberIDDataMap[seriesName];

        /*
        QList<QStringList> optionChamberIDListParsed;
        QList<QList<int>> optionChamberIDIndexsParsed;
        for (int i=0;i < optionChamberIDList.count();i++) {
            QStringList rga = optionChamberIDList.value(i);

            if(rga.count() > 1){
                optionChamberIDListParsed.append(optionChamberIDList.value(i));
                optionChamberIDIndexsParsed.append(optionChamberIDIndexs.value(i));
            }
        }
        */

        //1.2 组建
        //长度就是需要实际绘制的手动CL - 多点连续的
        for (int i = 0;i < optionRecipeRGAParsed.count();i++)
        {
            QStringList rga = optionRecipeRGA.value(i);
            QString curRecipeName = rga.value(0);//rga值都是一样的
            //获取x轴的起止值
            QList<int> curIndexList = optionIndexsParsed.value(i);
            int startIndex = curIndexList.value(0);
            int endIndex = curIndexList.last();
            //取值 x
            QPair<QDateTime, double> startPair = pntsList.value(startIndex);
            QPair<QDateTime, double> endPair = pntsList.value(endIndex);
            QDateTime startTime = startPair.first;//开始x
            QDateTime endTime = endPair.first;//结束x
            //取值 y
            QString uclStr = "N";
            QString lclStr = "N";
            //1.取配方
            //1.1 优先从recipe文件夹中取 - 直接使用channelName获取
            bool ok = false;
            //"N2/H2O,M28/M18,N,N;O2/H2O,M32/M18,1,2;N2/Ar,M28/M40,N,N"，拆分的mao
            //{"N2/H2O",pair(N,N)},{"O2/H2O",pair(1,2)}...
            QMap<QString, QPair<QString, QString> > clMap = getCurrentRecipeMap(curRecipeName,ok);
            //1.2 如果配方文件夹不存该配方文件 - 从配置中获得
            if(clMap.contains(channelName)){
                QPair<QString, QString> clInfo = clMap.value(channelName);
                lclStr = clInfo.first;
                uclStr = clInfo.second;
            }
            else
            {
                //1.2 如果配方文件夹不存该配方文件 - 从配置中获得
                QString groupKey = XMLReadHelp::createGroupKey(EquipmentGroupNo,EquipmentGroupName);
                QString channelValue =  getRecipeValueByChartFDC(groupKey,ChamberName,channelName);
                ChannelValueInfo valueInfo = XMLReadHelp::parseChannelValue(channelValue);
                lclStr = valueInfo.param1;
                uclStr = valueInfo.param2;
            }

            //判断是否为有效数据
            if(lclStr.toLower() == "n" || uclStr.toLower() == "n")
            {
                continue;
            }

            validStartX.append(startTime);
            validEndX.append(endTime);
            validY_lcl.append(lclStr.toDouble());
            validY_ucl.append(uclStr.toDouble());

            //2025-1001 逻辑上这边的start与end应该是一个值
            //【即配方跟随chamerID变化，返回来讲就是chamerID与配方的对齐关系是一致的,groupConsecutiveItems】
            //以下组建线的flag  1,2选一个
            bool use = false;
            if(use)
            {
                //1.单一 chamerID 组建线的flag
                validChamberID.append(pChamberIDList.value(startIndex).second);
                //validChamberID.append(pChamberIDList.value(endIndex).second);
            }
            else
            {
                //2. 界面上上可操作的所有 组建线的flag
                QStringList eqNames = m_chartWgt->mRecipeNameEQ.value(seriesName);
                QStringList lotIDs = m_chartWgt->mLotID.value(seriesName);
                //m_chartWgt->mSlotID.value(seriesName);
                QStringList waferIDs = m_chartWgt->mWaferID.value(seriesName);
                QString flagStr = QString("%1_%2_%3_%4")
                        .arg(eqNames.value(startIndex))
                        .arg(lotIDs.value(startIndex))
                        .arg(waferIDs.value(startIndex))
                        .arg(pChamberIDList.value(startIndex).second);
                validChamberID.append(flagStr);
            }


            //测试输出
            if(m_debugBool)
            {
                qDebug().noquote() << QString("drawCL_manual - lclStr:%1-uclStr:%2")
                                      .arg(lclStr).arg(uclStr);
                QString msg = QString("drawCL_manual-parsed-startIndex:%1,endIndex:%2,startTime:%3,endTime:%4")
                                .arg(startIndex).arg(endIndex)
                        .arg(startTime.toString("yyyyMMdd hh:mm:ss"))
                        .arg(endTime.toString("yyyyMMdd hh:mm:ss"));

                qDebug().noquote() << msg;
            }

        }

        //关键信息输出 - 测试用
        if(m_debugBool){
            qDebug().noquote() << "validStartX: " << validStartX.count();
            qDebug().noquote() << "validEndX: " << validEndX.count();
            qDebug().noquote() << "validY_lcl: " << validY_lcl.count();
            qDebug().noquote() << "validY_ucl: " << validY_ucl.count();
            //1.汇总一共需要多少个配方
            QStringList recipeNeeds;
            foreach (QStringList tmp, optionRecipeRGA) {
                QString tmpName = tmp.value(0);
                if(!recipeNeeds.contains(tmpName)){
                    recipeNeeds.append(tmpName);
                }
            }
            qDebug().noquote() << "recipeNeeds:  " << recipeNeeds.join(",");
            //1.配置中的手动数据CL - 这个是备用项，前提是recipe文件夹中不存在才使用配置
            QString equipmentGroupNo = "0";
            QString ChamberName = "CHE";
            QString channelName = "O2/H2O";
            QString channelValue =  getRecipeValueByChartFDC(equipmentGroupNo,ChamberName,channelName);
            ChannelValueInfo valueInfo = XMLReadHelp::parseChannelValue(channelValue);
            //核一下与数据库是不是对应的
            QList<int> tmpList = optionIndexs.value(0);
            int startIndex = tmpList.value(0);
            int endIndex = tmpList.last();

            QString startTimeStr = pntsList.value(startIndex).first.toString("yyyyMMdd hh:mm:ss");
            QString endTimeStr = pntsList.value(endIndex).first.toString("yyyyMMdd hh:mm:ss");
            qDebug().noquote() << "startIndex:  " << startIndex;
            qDebug().noquote() << "endIndex:  " << endIndex;
            qDebug().noquote() << "startTimeStr:  " << startTimeStr;
            qDebug().noquote() << "endTimeStr:  " << endTimeStr;
            qDebug().noquote() << "drawCL_manual-seriesName:    " << seriesName;
            qDebug().noquote() << "drawCL_manual-optionRecipeRGA-optionIndexs:  "
                               << optionRecipeRGA.count() << optionIndexs.count();
            qDebug().noquote() << "drawCL_manual-optionRecipeRGAParsed-optionIndexsParsed:  "
                               << optionRecipeRGAParsed.count() << optionIndexsParsed.count();
        }
    }

}

/*!
 * \brief sFaultDetectionClassificationRGA::drawManualCL_lines 绘制多段线
 * \param validStartX
 * \param validEndX
 * \param validY_lcl
 * \param validY_ucl
 */
void sFaultDetectionClassificationRGA::drawManualCL_lines(QList<QDateTime> &validStartX, QList<QDateTime> &validEndX, QList<double> &validY_lcl, QList<double> &validY_ucl,QList<QString>& validChamberID)
{
    // 检查参数一致性
    int count = validStartX.size();
    if (count != validEndX.size() || count != validY_lcl.size() || count != validY_ucl.size()) {
        qDebug() << "错误: 参数长度不一致";
        return;
    }

    // 获取图表对象
    if (!m_chartWgt) {
        qDebug() << "错误: m_chartWgt 为空";
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        qDebug() << "错误: chart 为空";
        return;
    }

    // 清除之前创建的手动CL线
    clearManualCLLines();

    // 如果没有数据，直接返回
    if (count == 0) {
        return;
    }

    // 获取轴对象
    QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);

    if (xAxisList.isEmpty() || yAxisList.isEmpty()) {
        qDebug() << "错误: 无法获取轴对象";
        return;
    }

    QAbstractAxis* xAxis = xAxisList.first();
    QAbstractAxis* yAxis = yAxisList.first();

    /*注意事项
     * chamerID有两种，默认第2种
     * 1. QString flagStr = pChamberIDList.value(startIndex).second
     * 2. QString flagStr = QString("%1_%2_%3_%4")
                .arg(eqNames.value(startIndex))
                .arg(lotIDs.value(startIndex))
                .arg(waferIDs.value(startIndex))
                .arg(pChamberIDList.value(startIndex).second);
    */

    int flagSignleCount = m_isSingleList.count();
    QStringList singleMsg;
    // 创建新的手动CL线
    for (int i = 0; i < count; i++) {
        qint64 xStart = validStartX[i].toMSecsSinceEpoch();
        qint64 xEnd = validEndX[i].toMSecsSinceEpoch();
        double lcl = validY_lcl[i];
        double ucl = validY_ucl[i];
        QString chamerID = validChamberID[i];

        bool isSingle = m_isSingleList.value(i);

        QColor defaultColor = Qt::darkRed;
        if(isSingle){
            defaultColor = Qt::red;
        }

        // 创建LCL线
        QLineSeries* lclLine = new QLineSeries();
        lclLine->setName(QString("LCL_Manual_%1").arg(i));
        lclLine->setProperty("flag",chamerID);
        //lclLine->setColor(Qt::darkRed);
        //lclLine->setPen(QPen(Qt::darkRed, 2, Qt::SolidLine));
        lclLine->setColor(defaultColor);
        lclLine->setPen(QPen(defaultColor, 2, Qt::SolidLine));

        // 设置数据点
        QVector<QPointF> lclPoints;
        lclPoints << QPointF(xStart, lcl) << QPointF(xEnd, lcl);
        lclLine->replace(lclPoints);

        // 添加到图表
        chart->addSeries(lclLine);
        lclLine->attachAxis(xAxis);
        lclLine->attachAxis(yAxis);

        // 显示与否
        lclLine->setVisible(m_showControlLines);

        //点击CL线显示信息
        connect(lclLine, &QLineSeries::clicked, [=](const QPointF& point){
                QString flag = lclLine->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString eqName = tmpList.value(0);
                    QString lotID = tmpList.value(1);
                    QString waferID = tmpList.value(2);
                    QString chamerID = tmpList.value(3);
                    flag = QString("EQ Name:%1\nLotID:%2\nwaferID:%3\nchamerID:%4")
                            .arg(eqName).arg(lotID).arg(waferID).arg(chamerID);
                }
                QToolTip::showText(QCursor::pos(), flag);
        });

        // 保存到列表中以便管理
        m_manualLCLLines.append(lclLine);

        // 创建UCL线
        QLineSeries* uclLine = new QLineSeries();
        uclLine->setName(QString("UCL_Manual_%1").arg(i));
        uclLine->setProperty("flag",chamerID);
        //uclLine->setColor(Qt::darkRed);
        //uclLine->setPen(QPen(Qt::darkRed, 2, Qt::SolidLine));
        uclLine->setColor(defaultColor);
        uclLine->setPen(QPen(defaultColor, 2, Qt::SolidLine));

        // 设置数据点
        QVector<QPointF> uclPoints;
        uclPoints << QPointF(xStart, ucl) << QPointF(xEnd, ucl);
        uclLine->replace(uclPoints);

        // 添加到图表
        chart->addSeries(uclLine);
        uclLine->attachAxis(xAxis);
        uclLine->attachAxis(yAxis);

        // 显示与否
        uclLine->setVisible(m_showControlLines);

        connect(uclLine, &QLineSeries::clicked, [=](const QPointF& point){
            QString flag = uclLine->property("flag").toString();
            QStringList tmpList = flag.split("_");
            if(tmpList.count() >= 4){
                QString eqName = tmpList.value(0);
                QString lotID = tmpList.value(1);
                QString waferID = tmpList.value(2);
                QString chamerID = tmpList.value(3);
                flag = QString("EQ Name:%1\nLotID:%2\nwaferID:%3\nchamerID:%4")
                        .arg(eqName).arg(lotID).arg(waferID).arg(chamerID);
            }
            QToolTip::showText(QCursor::pos(), flag);
        });

        // 保存到列表中以便管理
        m_manualUCLLines.append(uclLine);

        //输出一下单点的信息，方便调测
        if(isSingle)
        {
            QString xStart = validStartX[i].toString("yyyyMMdd hh:mm:ss");
            QString xEnd = validEndX[i].toString("yyyyMMdd hh:mm:ss");
            double lcl = validY_lcl[i];
            double ucl = validY_ucl[i];
            QString chamerID = validChamberID[i];

            QString msg = QString("startTime:%1_endTime:%2_lcl:%3_ucl:%4_chamerID:%5_index:%6")
                                .arg(xStart).arg(xEnd).arg(lcl).arg(ucl).arg(chamerID).arg(i);

            singleMsg.append(msg);
        }
    }

    // 隐藏所有手动CL线的图例显示
    hideManualCLLineLegends();

    qDebug() << "绘制了" << count << "对手动CL线";

    //输出到文件
    singleMsg.append(QString("Cl Line Total Count:%1").arg(flagSignleCount));
    cJsonFileOperate::jsonStr2File("singlePnts_CL.txt",singleMsg.join("\n"));
}

/*!
 * \brief sFaultDetectionClassificationRGA::getCurrentRecipeMap
 * \param recipeName 数据库中的配方名，与配方文件夹中的名是一样的，都有后缀
 * \param ok
 * \return
 */
QMap<QString, QPair<QString, QString> > sFaultDetectionClassificationRGA::getCurrentRecipeMap(const QString &recipeName, bool &ok)
{
    QMap<QString, QPair<QString, QString> > resMap;
    //fileBaseName没有后缀，所以用contains而不是==
    foreach (QString fileBaseName, m_recipeDirInfoMap.keys()) {
        if(recipeName.toLower().contains(fileBaseName.toLower())){
            resMap = m_recipeDirInfoMap.value(fileBaseName);
            ok = true;
            break;
        }
    }
    ok = false;
    return resMap;
}

void sFaultDetectionClassificationRGA::setManualCLLinesVisible(bool visible)
{
    // 设置LCL线可见性
    for (QLineSeries* line : m_manualLCLLines) {
        if (line) {
            line->setVisible(visible);
        }
    }

    // 设置UCL线可见性
    for (QLineSeries* line : m_manualUCLLines) {
        if (line) {
            line->setVisible(visible);
        }
    }

    hideManualCLLineLegends();
}

void sFaultDetectionClassificationRGA::setManualCLLinesVisible(QString flagVal, int flag, bool visible)
{
    /*
     * QString flag = lclLine->property("flag").toString();
                    QStringList tmpList = flag.split("_");
                    if(tmpList.count() >= 4){
                        QString eqName = tmpList.value(0);
                        QString lotID = tmpList.value(1);
                        QString waferID = tmpList.value(2);
                        QString chamerID = tmpList.value(3);
                        flag = QString("EQ Name:%1\nLotID:%2\nwaferID:%3\nchamerID:%4")
                                .arg(eqName).arg(lotID).arg(waferID).arg(chamerID);
                    }
     **/
    if(flag == 0){
        //EQ Name 检索
        //LCL
        foreach (QLineSeries* line, m_manualLCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString eqName = tmpList.value(0);
                    if(flagVal.toLower() == eqName.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }

        //UCL
        foreach (QLineSeries* line, m_manualUCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString eqName = tmpList.value(0);
                    if(flagVal.toLower() == eqName.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }
    }
    else if(flag == 1)
    {
        //lotID检索
        //LCL
        foreach (QLineSeries* line, m_manualLCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString slotID = tmpList.value(1);
                    if(flagVal.toLower() == slotID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }

        //UCL
        foreach (QLineSeries* line, m_manualUCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString slotID = tmpList.value(1);
                    if(flagVal.toLower() == slotID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }
    }
    else if(flag == 2)
    {
        //waferID检索
        //LCL
        foreach (QLineSeries* line, m_manualLCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString waferID = tmpList.value(2);
                    if(flagVal.toLower() == waferID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }

        //UCL
        foreach (QLineSeries* line, m_manualUCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString waferID = tmpList.value(2);
                    if(flagVal.toLower() == waferID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }
    }
    else if(flag == 3)
    {
        //chamerID检索
        //LCL
        foreach (QLineSeries* line, m_manualLCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString chamerID = tmpList.value(3);
                    if(flagVal.toLower() == chamerID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }

        //UCL
        foreach (QLineSeries* line, m_manualUCLLines) {
            if(line != nullptr)
            {
                QString flag = line->property("flag").toString();
                QStringList tmpList = flag.split("_");
                if(tmpList.count() >= 4){
                    QString chamerID = tmpList.value(3);
                    if(flagVal.toLower() == chamerID.toLower()){
                        line->setVisible(visible);
                    }
                    else
                    {
                        line->setVisible(!visible);
                    }
                }
            }
        }
    }
    else
    {
        //全部
        setManualCLLinesVisible(visible);
    }

    hideManualCLLineLegends();
}

void sFaultDetectionClassificationRGA::clearManualCLLines()
{
    if (!m_chartWgt) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        return;
    }

    // 删除所有LCL线
    for (QLineSeries* line : m_manualLCLLines) {
        if (line && chart->series().contains(line)) {
            chart->removeSeries(line);
        }
        delete line;
    }
    m_manualLCLLines.clear();

    // 删除所有UCL线
    for (QLineSeries* line : m_manualUCLLines) {
        if (line && chart->series().contains(line)) {
            chart->removeSeries(line);
        }
        delete line;
    }
    m_manualUCLLines.clear();
}

void sFaultDetectionClassificationRGA::hideManualCLLineLegends()
{
    // 方法1: 通过设置图例不显示特定系列
    if (m_chartWgt && m_chartWgt->getChart()) {
        QLegend* legend = m_chartWgt->getChart()->legend();
        if (legend) {
            // 隐藏所有手动CL线的图例
            for (QLineSeries* line : m_manualLCLLines) {
                if (line) {
                    QList <QLegendMarker*> markers = legend->markers(line);
                    if(markers.count() > 0){
                        legend->markers(line).first()->setVisible(false);
                    }

                }
            }
            for (QLineSeries* line : m_manualUCLLines) {
                if (line) {
                    QList <QLegendMarker*> markers = legend->markers(line);
                    if(markers.count() > 0){
                        legend->markers(line).first()->setVisible(false);
                    }
                }
            }
        }
    }
}

/*!
 * \brief sFaultDetectionClassificationRGA::on_comCL_mode_currentIndexChanged
 * \param index
 */
void sFaultDetectionClassificationRGA::on_comCL_mode_currentIndexChanged(int index)
{
    //更新控制限CL数据
    Q_UNUSED(index)

    if(getBlockComCLModeSignal()){//如果是信号block就return
        return;
    }
    if(m_version == 0)
    {
        PublicDef* pDef = sPublicDefSingleton::GetInstance();
        QVariantMap infoMap = pDef->m_dataInfoMap;
        cJsonFileOperate::toJsonFile(infoMap,"123321.json");
        QString curName = ui.comCL_mode->currentText();
        if(infoMap.contains(curName)){
            QVariantMap dataMap = infoMap.value(curName).toMap();
            double avg = dataMap["avg"].toDouble();
            double ucl = dataMap["ucl"].toDouble();
            double lcl = dataMap["lcl"].toDouble();
            qDebug().noquote() << QString(tr("当前选择的是:%1,avg:%2,ucl:%3,ucl:%4")
                                          .arg(curName)
                                          .arg(avg)
                                          .arg(ucl)
                                          .arg(lcl));
            updateControlLines(curName);// 更新控制线
        }
        else{
            //
        }

        //绘制手动多段控制限CL
        QList<QDateTime> validStartX;
        QList<QDateTime> validEndX;
        QList<double> validY_lcl;
        QList<double> validY_ucl;
        QList<QString> validChamerID;
        //1.有效数据解析
        parsedCL_manual(validStartX,validEndX,validY_lcl,validY_ucl,validChamerID);
        //2.绘制
        drawManualCL_lines(validStartX,validEndX,validY_lcl,validY_ucl,validChamerID);
    }
    else if(m_version == 1)
    {
        //重新绘制CL
        QString chamberID = ui.UI_CB_CHAMBERID->currentText();
        if(chamberID.toLower() == "all"){
            QMessageBox::warning(nullptr,tr("提示"),tr("ALL的选择不会绘制CL控制线！"));
            //清空已有控制线
            clearControlLineManual();
            return;
        }
        QMap<QString,QPair<QString,QString>> pairMap = m_chamerIDConf.value(chamberID);
        QString seriesName = ui.comCL_mode->currentText();
        if(seriesName.isEmpty() || seriesName.toLower() == "all"){
            //实际就是软件刚刚打开的时候也触发了，
            QMessageBox::warning(nullptr,tr("提示"),tr("ALL选择不会绘制CL控制限，清空当前CL线！"));
            clearControlLineManual();
            return;
        }

        //创建CL的2条线
        createControlLinesManual();

        updateControlLinesManual(seriesName);

        //绘制告警区域
        //setAlarmValues(m_alarmUpValue,m_alarmDownValue);
    }

}

void sFaultDetectionClassificationRGA::on_chkShowCL_clicked(bool checked)
{
    setControlLinesVisible(checked);

    //测试需要删除
    QMap<QString, TChartWidget::SeriesData> seriesDataMap = m_chartWgt->getSeriesData();
    foreach (QString seriesName, seriesDataMap.keys()) {
        qDebug().noquote() << "seriesName:  " << seriesName;
    }
}

//void sFaultDetectionClassificationRGA::on_UI_PB_RECIPE_CONFIG_FDC_clicked()
//{
//if(!mRecipeManageFDC)
//    return;
//mRecipeManageFDC->show();
//}

void sFaultDetectionClassificationRGA::on_btnOpenPostSoftware_clicked()
{
    onExecuteSelectionClicked();
}

void sFaultDetectionClassificationRGA::on_btnClearPnts_clicked()
{
    if(m_chartWgt != nullptr){
        m_chartWgt->clearSelection();
    }

    ui.UI_PNTS_COMBOX->clear();
}

void sFaultDetectionClassificationRGA::on_UI_CB_EQ_RECIPE_currentTextChanged(const QString &arg1)
{
    if(arg1.toLower() == "all"){
        setManualCLLinesVisible(true);
    }
    else
    {
        setManualCLLinesVisible(arg1,0,true);
    }
}

void sFaultDetectionClassificationRGA::on_UI_CB_CHAMBERID_currentTextChanged(const QString &arg1)
{
    if(arg1.toLower() == "all"){
        setManualCLLinesVisible(true);
    }
    else
    {
        setManualCLLinesVisible(arg1,3,true);
    }

}

void sFaultDetectionClassificationRGA::on_UI_CB_LOTID_currentTextChanged(const QString &arg1)
{
    if(arg1.toLower() == "all"){
        setManualCLLinesVisible(true);
    }
    else
    {
        setManualCLLinesVisible(arg1,1,true);
    }
}

void sFaultDetectionClassificationRGA::on_UI_CB_WAFERID_currentTextChanged(const QString &arg1)
{
    if(arg1.toLower() == "all"){
        setManualCLLinesVisible(true);
    }
    else
    {
        setManualCLLinesVisible(arg1,2,true);
    }
}

/*!
 * \brief sFaultDetectionClassificationRGA::on_btnDefine_clicked
 */
void sFaultDetectionClassificationRGA::on_btnDefine_clicked()
{
    QString xmlConfPath  = QCoreApplication::applicationDirPath() + "\\ChartFDC";
    if(!QFile::exists(xmlConfPath)){
        QMessageBox::warning(nullptr,tr("错误"),tr("核心配置文件ChartFDC不存在，请检查！"));
        return;
    }

    QMap<QString, EquipmentGroupInfo> infoMap = XMLReadHelp::parseEquipmentGroupsFromXML(m_chartFDCXML_content);

    sDefinePlotDataDlg* dlg = new sDefinePlotDataDlg(infoMap,m_chamerIDConfMap);
    if(dlg->exec() == QDialog::Accepted)
    {
        QVariantMap dataMap = dlg->getData();
        QString msg = cJsonFileOperate::variant2Json(dataMap);
        qDebug().noquote() << msg;
        if(dataMap.isEmpty()){
            QMessageBox::warning(nullptr,tr("提示"),tr("无有效的选择！"));
            return;
        }

        //选中条目【具体的 N2/Ar  N2/H2O  O2/H2O 】
        setChkItem(dataMap);

        //chamberID控件的值处理 - 临时方法[防止sDefinePlotDataDlg中 CHE 与 TM这种交叉，即tree选择与com选择岔开来了]
        //CHE的chamberID就自动为CHE
        int itemCount = ui.UI_CB_CHAMBERID->count();
        QStringList itemTexts;
        for (int var = 0; var < itemCount; ++var) {
            itemTexts.append(ui.UI_CB_CHAMBERID->itemText(var));
        }
        QString chamberName = dataMap["chamberName"].toString();
        if(chamberName.toUpper() == "CHE")
        {
            int index = itemTexts.indexOf("CHE");
            if(index > 0){
                ui.UI_CB_CHAMBERID->setCurrentIndex(index);
            }
        }
        else
        {
            //TM的要是选的不是TM相关的就选中第一个TM相关的
            QString selectedTxt = dataMap["com_box"].toString();
            int validIndex = -1;
            if(!selectedTxt.contains("TM1")){
                for (int i=0;i<itemTexts.count();i++) {
                    QString tmp = itemTexts.value(i);
                    if(tmp.toUpper().contains("TM1")){
                        validIndex = i;
                        break;
                    }
                }
                if(validIndex > 0){
                    ui.UI_CB_CHAMBERID->setCurrentIndex(validIndex);
                }
            }
            else
            {
                int index = itemTexts.indexOf(selectedTxt);
                if(index > 0){
                    ui.UI_CB_CHAMBERID->setCurrentIndex(index);
                }
            }
        }

        //2.执行载入
        on_UI_PB_OK_FDC_clicked();
    }
}

void sFaultDetectionClassificationRGA::setChkItem(const QVariantMap &i_dataMap)
{
    int topLevelCount = ui.UI_TW_EQ_LIST_FDC->topLevelItemCount();
    //清空全部chk[注意没有递归，只适合当前配置的xml]
    for (int i =0;i < topLevelCount;i++)
    {
        QTreeWidgetItem* topLevelItem = ui.UI_TW_EQ_LIST_FDC->topLevelItem(i);
        int cCount = topLevelItem->childCount();
        for (int cIndex =0;cIndex < cCount;cIndex++)
        {
            QTreeWidgetItem* childItem = topLevelItem->child(cIndex);
            PublicDef::clearAllCheckState(childItem);
        }
    }

    //开始
    QString equipmentGroupName = i_dataMap["equipmentGroupName"].toString();
    QString chamberName = i_dataMap["chamberName"].toString();
    QString name = i_dataMap["name"].toString();
    QString chamberID_selected = i_dataMap["com_box"].toString();
    //1.界面勾选

    for (int i =0;i < topLevelCount;i++) {
        QTreeWidgetItem* topLevelItem = ui.UI_TW_EQ_LIST_FDC->topLevelItem(i);
        QString grpName = topLevelItem->text(0);
        qDebug().noquote() << "grpName:   " << grpName;
        if(grpName.toLower() == equipmentGroupName.toLower()){
            int cCount = topLevelItem->childCount();
            for (int cIndex =0;cIndex < cCount;cIndex++) {
                //这边是  CHE TM1这个级别
                QTreeWidgetItem* childItem = topLevelItem->child(cIndex);
                QString cName = childItem->text(0);
                qDebug().noquote() << "cName:   " << cName;
                if(cName.toLower() == chamberName.toLower())
                {
                    //到了具体的 N2/Ar  N2/H2O  O2/H2O  这边了
                    int itemCount = childItem->childCount();
                    for (int tmp = 0;tmp < itemCount;tmp++) {
                        QTreeWidgetItem* tmpItem = childItem->child(tmp);
                        QString tmpName = tmpItem->text(0);
                        if(tmpName.toLower() == name.toLower()){
                            tmpItem->setCheckState(0,Qt::Checked);
                        }
                    }
                    break;
                }
            }
        }
    }
}
