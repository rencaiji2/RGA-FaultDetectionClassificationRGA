#include "TChartWidget.h"
#include <QDebug>
#include <QApplication>
#include <QRandomGenerator>
#include <QKeyEvent>
#include <QScrollBar>
#include <cmath>
#include <QDateTime>
#include <QToolTip>
#include <QtMath>

// TChartView 实现
TChartView::TChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
    , m_isDragging(false)
    , m_isSelecting(false)
    , m_rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    setDragMode(QGraphicsView::NoDrag);
    setMouseTracking(true);
    setInteractive(true);
}

void TChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 左键框选
        m_isSelecting = true;
        m_selectionStartPos = event->pos();
        m_rubberBand->setGeometry(QRect(m_selectionStartPos, QSize()));
        m_rubberBand->show();
    }
    else if (event->button() == Qt::MiddleButton ||
             (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)) {
        // 中键或Shift+左键拖拽
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    else if (event->button() == Qt::RightButton) {
        // 右键重置视图
        resetView();
    }

    QChartView::mousePressEvent(event);
}


void TChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isSelecting) {
        // 更新框选区域
        QRect rect = QRect(m_selectionStartPos, event->pos()).normalized();
        m_rubberBand->setGeometry(rect);
    }
    else if (m_isDragging) {
        // 平移视图
        QPointF delta = mapToScene(m_lastMousePos) - mapToScene(event->pos());
        chart()->scroll(delta.x(), -delta.y()); // 注意Y轴方向
        m_lastMousePos = event->pos();
    }

    QChartView::mouseMoveEvent(event);
}

void TChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isSelecting) {
        // 完成框选放大
        m_rubberBand->hide();
        m_isSelecting = false;

        QRect rect = m_rubberBand->geometry();
        if (rect.width() > 10 && rect.height() > 10) {
            // 转换为图表坐标系
            QPointF origin = mapToScene(rect.topLeft());
            QPointF bottomRight = mapToScene(rect.bottomRight());
            QRectF zoomRect(origin, bottomRight);
            zoomIn(zoomRect);
        }
    }
    else if ((event->button() == Qt::MiddleButton ||
              (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier))
             && m_isDragging) {
        // 停止拖拽
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }

    QChartView::mouseReleaseEvent(event);
}

void TChartView::wheelEvent(QWheelEvent *event)
{
    // 保存鼠标位置
    QPointF mousePos = event->posF();

    // 缩放逻辑
    qreal scaleFactor = 1.1;
    if (event->angleDelta().y() > 0) {
        chart()->zoom(scaleFactor);
    } else {
        chart()->zoom(1.0 / scaleFactor);
    }

    // 进行小幅度调整
    QRectF plotArea = chart()->plotArea();
    if (plotArea.width() > 0 && plotArea.height() > 0) {
        // 计算鼠标相对于绘图区域的位置比例
        qreal ratioX = (mousePos.x() - plotArea.left()) / plotArea.width();
        qreal ratioY = (mousePos.y() - plotArea.top()) / plotArea.height();

        // 根据比例进行适度调整
        qreal adjustX = (ratioX - 0.5) * plotArea.width() * 0.1;
        qreal adjustY = (ratioY - 0.5) * plotArea.height() * 0.1;

        chart()->scroll(adjustX, -adjustY);
    }

    event->accept();
}


void TChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoom(1.2);
        break;
    case Qt::Key_Minus:
        chart()->zoom(0.8);
        break;
    case Qt::Key_0:
    case Qt::Key_Escape:
        resetView();
        break;
    default:
        QChartView::keyPressEvent(event);
    }
}

void TChartView::zoomIn(const QRectF &rect)
{
    QRectF plotArea = chart()->plotArea();
    if (plotArea.width() <= 0 || plotArea.height() <= 0) {
        return;
    }

    // 计算缩放因子
    qreal zoomX = plotArea.width() / rect.width();
    qreal zoomY = plotArea.height() / rect.height();
    qreal zoomFactor = qMin(zoomX, zoomY);

    // 使用更保守的缩放因子
    zoomFactor = qBound(1.2, zoomFactor, 2.5);
    qDebug() << "zoomFactor:    " << zoomFactor;

    // 只进行缩放，不调整位置（避免坐标转换问题）
    chart()->zoom(zoomFactor);
}


void TChartView::zoomOut()
{
    chart()->zoom(0.8);
}

void TChartView::resetView()
{
    if (!chart()) {
        return;
    }

    // 保存当前轴的范围
    QList<QAbstractAxis*> xAxisList = chart()->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxisList = chart()->axes(Qt::Vertical);

    // 重置所有状态
    chart()->zoomReset();
    chart()->scroll(0, 0);

    chart()->update();

    qDebug() << "视图已完全重置";
}

// TChartWidget 实现
TChartWidget::TChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_chartView(nullptr)
    , m_chart(nullptr)
    ,m_xDateTimeAxis(nullptr)
    ,m_xValueAxis(nullptr)
    , m_yAxis(nullptr)
    , m_xAxis(nullptr)
{
    setupUI();
}

TChartWidget::~TChartWidget()
{

}

void TChartWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_chart = new QChart();
    m_chartView = new TChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(m_chartView);

    // 初始化默认配置
    m_xAxisConfig.title = "X轴";
    m_yAxisConfig.title = "Y轴";
}

void TChartWidget::setChartConfig(const ChartConfig& config)
{
    m_chartConfig = config;

    // 设置图表标题
    m_chart->setTitle(m_chartConfig.title);

    // 设置抗锯齿
    m_chartView->setRenderHint(QPainter::Antialiasing, m_chartConfig.antialiasing);

    // 设置背景色
    m_chart->setBackgroundBrush(m_chartConfig.backgroundColor);

    // 更新图例
    updateLegend();

    // 重新创建图表
    updateChart();
}

void TChartWidget::setXAxisConfig(const AxisConfig& config)
{
    m_xAxisConfig = config;
    updateAxis();
}

void TChartWidget::setYAxisConfig(const AxisConfig& config)
{
    m_yAxisConfig = config;
    updateAxis();
}

void TChartWidget::addSeries(const SeriesData& series)
{
    m_seriesData[series.name] = series;
    updateChart();
}

void TChartWidget::clearSeries()
{
    try {
        // 可能抛出异常的代码
        m_seriesData.clear();
        m_seriesMap.clear();
        updateChart();
    } catch (const std::exception& e) {
        qDebug() << "TChartWidget::clearSeries-Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "TChartWidget::clearSeries-Unknown exception occurred";
    }


}

void TChartWidget::setData(const QString& seriesName, const QVector<QPointF>& points)
{
    if (m_seriesData.contains(seriesName)) {
        m_seriesData[seriesName].points = points;
        updateChart();
    }
}

void TChartWidget::addDataPoint(const QString& seriesName, const QPointF& point)
{
    if (m_seriesData.contains(seriesName)) {
        m_seriesData[seriesName].points.append(point);
        updateChart();
    }
}

void TChartWidget::removeSeries(const QString& seriesName)
{
    m_seriesData.remove(seriesName);
    m_seriesMap.remove(seriesName);
    updateChart();
}

/**
 * @brief 设置图例配置
 * @param config 图例配置
 */
void TChartWidget::setLegendConfig(const LegendConfig& config)
{
    m_legendConfig = config;
    updateLegend();
    updateLegendPosition();
    applyLegendStyle();
}

/**
 * @brief 快速设置图例位置
 * @param position 图例位置
 */
void TChartWidget::setLegendPosition(LegendPosition position)
{
    m_legendConfig.position = position;
    updateLegendPosition();
}

/**
 * @brief 设置图例可见性
 * @param visible 是否可见
 */
void TChartWidget::setLegendVisible(bool visible)
{
    m_legendConfig.visible = visible;
    updateLegend();
}

/**
 * @brief 设置图例对齐方式
 * @param alignment 对齐方式
 */
void TChartWidget::setLegendAlignment(Qt::Alignment alignment)
{
    m_legendConfig.alignment = alignment;
    m_chart->legend()->setAlignment(alignment);
}

/**
 * @brief 设置带名称的XY数组数据
 * @param seriesName 数据系列名称
 * @param i_x X轴数据数组
 * @param i_y Y轴数据数组
 * @param color 系列颜色（可选）
 */
void TChartWidget::setXYArray(const QString &seriesName,
                              const QVector<double> &i_x,
                              const QVector<double> &i_y,
                              const QColor &color)
{
    // 检查图表类型是否支持XY数组
    if (!isXYChartType()) {
        qDebug() << "警告: setXYArray 仅支持折线图和散点图，当前类型为:" << m_chartConfig.type;
        return;
    }

    // 检查输入数据
    if (i_x.isEmpty() || i_y.isEmpty()) {
        qDebug() << "警告: 输入数据为空";
        return;
    }

    // 确定数据长度
    int dataLength = qMin(i_x.size(), i_y.size());

    // 创建数据点
    QVector<QPointF> points;
    points.reserve(dataLength);

    for (int i = 0; i < dataLength; ++i) {
        points.append(QPointF(i_x[i], i_y[i]));
    }

    // 创建系列数据
    SeriesData series;
    series.name = seriesName.isEmpty() ? "数据系列" : seriesName;
    series.points = points;

    // 设置颜色
    if (color.isValid()) {
        series.color = color;
    } else {
        // 根据系列数量生成不同颜色
        static QList<QColor> defaultColors = {
            Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan, Qt::yellow, Qt::darkRed, Qt::darkBlue
        };
        static int colorIndex = 0;
        series.color = defaultColors[colorIndex % defaultColors.size()];
        colorIndex++;
    }

    // 根据图表类型设置默认样式
    if (m_chartConfig.type == ScatterChart) {
        series.pointSize = 6.0;
    } else {
        series.lineWidth = 2.0;
    }

    // 添加系列并更新图表
    addSeries(series);

    qDebug() << "成功设置XY数组数据系列:" << seriesName << "，共" << dataLength << "个数据点";
}

/**
 * @brief 检查当前图表类型是否支持XY数组数据
 * @return true 如果是折线图或散点图，否则返回false
 */
bool TChartWidget::isXYChartType() const
{
    return (m_chartConfig.type == LineChart || m_chartConfig.type == ScatterChart);
}

void TChartWidget::updateChart()
{
    try {
        // 可能抛出异常的代码
        // 清除现有的系列
        for (auto series : m_seriesMap.values()) {
            m_chart->removeSeries(series);
        }
        m_seriesMap.clear();

        // 根据类型创建图表
        switch (m_chartConfig.type) {
        case LineChart:
            createLineChart();
            break;
        case ScatterChart:
            createScatterChart();
            break;
        case PieChart:
            createPieChart();
            break;
        case BarChart:
            createBarChart();
            break;
        case AreaChart:
            createAreaChart();
            break;
        }

        // 更新轴和图例
        updateAxis();
        updateLegend();
    } catch (const std::exception& e) {
        qDebug() << "TChartWidget::updateChart-Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "TChartWidget::updateChart-Unknown exception occurred";
    }
}

void TChartWidget::clearChart()
{
    try {
        // 可能抛出异常的代码
        clearSeries();
        m_chart->setTitle("");
        m_chart->removeAllSeries();
    } catch (const std::exception& e) {
        qDebug() << "TChartWidget::clearChart-Caught exception:" << e.what();
    } catch (...) {
        qDebug() << "TChartWidget::clearChart-Unknown exception occurred";
    }


}

void TChartWidget::replot()
{
    updateChart();
}

void TChartWidget::resetView()
{
    m_chartView->resetView();
}

void TChartWidget::removeSeries(QAbstractSeries *series)
{
    if(m_chart == nullptr || series == nullptr){
        return;
    }
    m_chart->removeSeries(series);
}

void TChartWidget::addSeries(QAbstractSeries *series)
{
    if(m_chart == nullptr || series == nullptr){
        return;
    }
    m_chart->addSeries(series);
}

void TChartWidget::createLineChart()
{
    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        QLineSeries *series = new QLineSeries();
        series->setName(data.name);
        series->setColor(data.color);
        QList<QPointF> pnts = QList<QPointF>::fromVector(data.points);
        series->append(pnts);
        series->setPen(QPen(data.color, data.lineWidth));

        //connect(series, &QLineSeries::clicked, this, &TChartWidget::handleSeriesClicked);

        m_chart->addSeries(series);
        m_seriesMap[data.name] = series;
    }

    // 创建轴
    if (!m_xAxis) {
        m_xAxis = new QValueAxis();
        m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    }

    if (!m_yAxis) {
        m_yAxis = new QValueAxis();
        m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    }

    // 关联系列到轴
    for (auto series : m_seriesMap.values()) {
        if (series->type() == QAbstractSeries::SeriesTypeLine) {
            series->attachAxis(m_xAxis);
            series->attachAxis(m_yAxis);
        }
    }
}

void TChartWidget::createScatterChart()
{
    // 清理现有系列
    for (auto series : m_seriesMap.values()) {
        m_chart->removeSeries(series);
    }
    m_seriesMap.clear();

    // 根据X轴类型创建不同的图表
    if (m_xAxisConfig.axisType == DateTimeAxis) {
        createScatterChartWithDateTime();
    } else {
        createScatterChartWithNumeric();
    }
}

void TChartWidget::createPieChart()
{
    QPieSeries *series = new QPieSeries();
    series->setName("Pie Series");

    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        if (!data.points.isEmpty()) {
            qreal value = data.points.first().y(); // 使用第一个点的Y值
            QPieSlice *slice = series->append(data.name, value);
            if (data.color.isValid()) {
                slice->setColor(data.color);
            }
        }
    }

    m_chart->addSeries(series);
    m_seriesMap["PieSeries"] = series;

    // 饼图不需要轴
    m_xAxis = nullptr;
    m_yAxis = nullptr;
}

void TChartWidget::createBarChart()
{
    QBarSeries *series = new QBarSeries();

    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        QBarSet *set = new QBarSet(data.name);
        if (data.color.isValid()) {
            set->setColor(data.color);
        }

        for (const QPointF& point : data.points) {
            *set << point.y();
        }

        series->append(set);
    }

    m_chart->addSeries(series);
    m_seriesMap["BarSeries"] = series;

    // 创建类别轴
    QStringList categories;
    if (!m_seriesData.isEmpty()) {
        const SeriesData& firstData = m_seriesData.first();
        for (const QPointF& point : firstData.points) {
            categories << QString::number(point.x());
        }
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_xAxis = axisX;
    m_yAxis = axisY;
}

void TChartWidget::createAreaChart()
{
    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        QAreaSeries *series = new QAreaSeries();
        QLineSeries *upperSeries = new QLineSeries();
        QList<QPointF> pnts = QList<QPointF>::fromVector(data.points);
        upperSeries->append(pnts);

        series->setName(data.name);
        series->setUpperSeries(upperSeries);
        if (data.color.isValid()) {
            series->setColor(data.color);
        }

        m_chart->addSeries(series);
        m_seriesMap[data.name] = series;
    }

    // 创建轴
    if (!m_xAxis) {
        m_xAxis = new QValueAxis();
        m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    }

    if (!m_yAxis) {
        m_yAxis = new QValueAxis();
        m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    }

    // 关联系列到轴
    for (auto series : m_seriesMap.values()) {
        if (series->type() == QAbstractSeries::SeriesTypeArea) {
            series->attachAxis(m_xAxis);
            series->attachAxis(m_yAxis);
        }
    }
}

void TChartWidget::updateAxis()
{
    if (m_xAxisConfig.axisType == DateTimeAxis) {
        updateDateTimeAxis();
    } else {
        updateNumericAxis();
    }
}

void TChartWidget::updateLegend()
{
    if (!m_chart || !m_chart->legend()) {
        return;
    }

    QLegend *legend = m_chart->legend();

    // 设置可见性
    legend->setVisible(m_legendConfig.visible);

    // 设置对齐方式
    Qt::Alignment alignment = mapLegendPositionToAlignment(m_legendConfig.position);
    legend->setAlignment(alignment);

    // 应用样式
    applyLegendStyle();

    // 设置图例标题（如果需要）
    if (!m_chartConfig.title.isEmpty()) {

    }
}

//QString TChartWidget::formatTooltip(const QPointF &point)
//{
//    QString msg = QString("x值:%1\ny值:%2")
//            .arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("yyyy-MM-dd hh:mm:ss"))
//            .arg(QString::number(point.y()));
//    return msg;
//}
QString TChartWidget::formatTooltip(const QPointF &point, QString name, int index)
{
    QString formatStr = QDateTime::fromMSecsSinceEpoch(point.x()).toString("yyyy-MM-dd hh:mm:ss")
            + "\n值: " + QString::number(point.y())
             + "\nRecipeNameEQ: " + mRecipeNameEQ[name][index]
             + "\nRecipeNameRGA: " + mRecipeNameRGA[name][index]
             + "\nStep: " + mStep[name][index]
             + "\nLotID: " + mLotID[name][index]
             + "\nSlotID: " + mSlotID[name][index]
             + "\nWaferID: " + mWaferID[name][index];

    qDebug().noquote() << "formatTooltip:   " << formatStr;

    return formatStr;
}
/**
 * @brief 更新图例位置
 */
void TChartWidget::updateLegendPosition()
{
    if (!m_chart || !m_chart->legend()) {
        return;
    }

    QLegend *legend = m_chart->legend();

    // 根据位置设置对齐方式
    Qt::Alignment alignment = mapLegendPositionToAlignment(m_legendConfig.position);
    legend->setAlignment(alignment);

    // 特殊处理自定义位置
    if (m_legendConfig.position == LegendCustom) {
        // 自定义位置需要通过其他方式处理
        // 这里可以添加更复杂的自定义位置逻辑
    }
}

/**
 * @brief 将图例位置映射到Qt对齐方式
 * @param position 图例位置
 * @return Qt对齐方式
 */
Qt::Alignment TChartWidget::mapLegendPositionToAlignment(LegendPosition position)
{
    switch (position) {
    case LegendTop:
        return Qt::AlignTop;
    case LegendBottom:
        return Qt::AlignBottom;
    case LegendLeft:
        return Qt::AlignLeft;
    case LegendRight:
        return Qt::AlignRight;
    case LegendTopLeft:
        return Qt::AlignTop | Qt::AlignLeft;
    case LegendTopRight:
        return Qt::AlignTop | Qt::AlignRight;
    case LegendBottomLeft:
        return Qt::AlignBottom | Qt::AlignLeft;
    case LegendBottomRight:
        return Qt::AlignBottom | Qt::AlignRight;
    case LegendInside:
        return Qt::AlignCenter;
    case LegendCustom:
        return m_legendConfig.alignment;
    default:
        return Qt::AlignRight;
    }
}

/**
 * @brief 应用图例样式
 */
void TChartWidget::applyLegendStyle()
{
    if (!m_chart || !m_chart->legend()) {
        return;
    }

    QLegend *legend = m_chart->legend();

    // 设置可见性
    legend->setVisible(m_legendConfig.visible);

    // 设置背景颜色
    if (m_legendConfig.backgroundColor != Qt::transparent) {
        legend->setBackgroundVisible(true);
        legend->setBrush(m_legendConfig.backgroundColor);
    }

    // 设置文字颜色
    legend->setLabelColor(m_legendConfig.textColor);

    // 设置字体
    legend->setFont(m_legendConfig.font);

    // 设置标记显示
    legend->setShowToolTips(m_legendConfig.showMarkers);

    // 设置标记大小（需要遍历所有系列）
    /*
    for (QLegendMarker *marker : legend->markers()) {
        // Qt Charts 不直接支持设置标记大小，需要通过系列设置
    }
    */

    // 设置间距
    // Qt Charts 的图例间距设置有限，这里可以作为参考
}

void TChartWidget::createDateTimeChart()
{

}

void TChartWidget::createScatterChartWithDateTime()
{
    // 清理现有轴
    cleanupAxes();

    // 添加数据系列
    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        QScatterSeries *series = new QScatterSeries();
        series->setName(data.name);
        series->setColor(data.color);
        series->setMarkerSize(data.pointSize);
        QList<QPointF> pnts = QList<QPointF>::fromVector(data.points);
        series->append(pnts);

        //connect(series, &QScatterSeries::clicked, this, &TChartWidget::handleSeriesClicked);
        //connect(series, &QScatterSeries::hovered, this, &TChartWidget::handleSeriesHovered);
        connect(series, &QScatterSeries::doubleClicked, this, &TChartWidget::handleSeriesDoubleClicked);

        connect(series, &QScatterSeries::clicked, [=](const QPointF& point){
                QList<QPointF> points = series->points();
                int indexSeries= -1;
                for (int i = 0; i < points.size(); ++i) {
                    const QPointF& pt = points.at(i);
                    if (std::abs(pt.x() - point.x()) < 1e-6 &&
                            std::abs(pt.y() - point.y()) < 1e-6) {
                        indexSeries= i;
                    }
                }
                if(indexSeries== -1)
                    return;
                QToolTip::showText(QCursor::pos(), formatTooltip(point, series->name(), indexSeries));
        });
        m_chart->addSeries(series);
        m_seriesMap[data.name] = series;
    }

    // 创建时间轴
    m_xDateTimeAxis = new QDateTimeAxis();
    m_xDateTimeAxis->setTitleText(m_xAxisConfig.title);
    m_xDateTimeAxis->setFormat("yyyy-MM-dd hh:mm:ss");
    m_xDateTimeAxis->setTickCount(m_xAxisConfig.tickCount);
    m_xDateTimeAxis->setLabelsAngle(m_xAxisConfig.labelAngle);  // 设置标签角度

    if (!m_xAxisConfig.autoRange) {
        m_xDateTimeAxis->setRange(QDateTime::fromMSecsSinceEpoch((qint64)m_xAxisConfig.min),
                                 QDateTime::fromMSecsSinceEpoch((qint64)m_xAxisConfig.max));
    }

    m_chart->addAxis(m_xDateTimeAxis, Qt::AlignBottom);

    // 创建Y轴
    createYAxis();

    // 关联轴
    for (auto series : m_seriesMap.values()) {
        series->attachAxis(m_xDateTimeAxis);
        series->attachAxis(m_yAxis);
    }
}

void TChartWidget::createScatterChartWithNumeric()
{
    // 清理现有轴
    cleanupAxes();

    // 添加数据系列
    for (auto it = m_seriesData.constBegin(); it != m_seriesData.constEnd(); ++it) {
        const SeriesData& data = it.value();
        QScatterSeries *series = new QScatterSeries();
        series->setName(data.name);
        series->setColor(data.color);
        series->setMarkerSize(data.pointSize);
        QList<QPointF> pnts = QList<QPointF>::fromVector(data.points);
        series->append(pnts);

        //connect(series, &QScatterSeries::clicked, this, &TChartWidget::handleSeriesClicked);

        connect(series, &QScatterSeries::clicked, [=](const QPointF& point){
                QList<QPointF> points = series->points();
                int indexSeries= -1;
                for (int i = 0; i < points.size(); ++i) {
                    const QPointF& pt = points.at(i);
                    if (std::abs(pt.x() - point.x()) < 1e-6 &&
                            std::abs(pt.y() - point.y()) < 1e-6) {
                        indexSeries= i;
                    }
                }
                if(indexSeries== -1)
                    return;
                QToolTip::showText(QCursor::pos(), formatTooltip(point, series->name(), indexSeries));
        });

        m_chart->addSeries(series);
        m_seriesMap[data.name] = series;
    }

    // 创建X轴
    m_xValueAxis = new QValueAxis();
    m_xValueAxis->setTitleText(m_xAxisConfig.title);
    if (!m_xAxisConfig.autoRange) {
        m_xValueAxis->setRange(m_xAxisConfig.min, m_xAxisConfig.max);
    }
    m_xValueAxis->setTickCount(m_xAxisConfig.tickCount);
    m_xValueAxis->setLabelsAngle(m_xAxisConfig.labelAngle);  // 设置标签角度
    m_chart->addAxis(m_xValueAxis, Qt::AlignBottom);

    // 创建Y轴
    createYAxis();

    // 关联轴
    for (auto series : m_seriesMap.values()) {
        series->attachAxis(m_xValueAxis);
        series->attachAxis(m_yAxis);
    }
}

void TChartWidget::updateDateTimeAxis()
{
    if (m_xDateTimeAxis) {
        m_xDateTimeAxis->setTitleText(m_xAxisConfig.title);
        m_xDateTimeAxis->setFormat("yyyy-MM-dd hh:mm:ss");
        m_xDateTimeAxis->setTickCount(m_xAxisConfig.tickCount);
        m_xDateTimeAxis->setLabelsAngle(m_xAxisConfig.labelAngle);  // 设置标签角度

        if (!m_xAxisConfig.autoRange) {
            m_xDateTimeAxis->setRange(QDateTime::fromMSecsSinceEpoch((qint64)m_xAxisConfig.min),
                                     QDateTime::fromMSecsSinceEpoch((qint64)m_xAxisConfig.max));
        }
    }

    if (m_yAxis) {
        m_yAxis->setTitleText(m_yAxisConfig.title);
        if (!m_yAxisConfig.autoRange) {
            m_yAxis->setRange(m_yAxisConfig.min, m_yAxisConfig.max);
        }
        m_yAxis->setTickCount(m_yAxisConfig.tickCount);
        m_yAxis->setLabelsAngle(m_yAxisConfig.labelAngle);  // 设置Y轴标签角度
    }
}

// 修改 updateNumericAxis 函数
void TChartWidget::updateNumericAxis()
{
    if (m_xValueAxis) {
        m_xValueAxis->setTitleText(m_xAxisConfig.title);
        if (!m_xAxisConfig.autoRange) {
            m_xValueAxis->setRange(m_xAxisConfig.min, m_xAxisConfig.max);
        }
        m_xValueAxis->setTickCount(m_xAxisConfig.tickCount);
        m_xValueAxis->setLabelsAngle(m_xAxisConfig.labelAngle);  // 设置标签角度
    }

    if (m_yAxis) {
        m_yAxis->setTitleText(m_yAxisConfig.title);
        if (!m_yAxisConfig.autoRange) {
            m_yAxis->setRange(m_yAxisConfig.min, m_yAxisConfig.max);
        }
        m_yAxis->setTickCount(m_yAxisConfig.tickCount);
        m_yAxis->setLabelsAngle(m_yAxisConfig.labelAngle);  // 设置Y轴标签角度
    }
}

void TChartWidget::createYAxis()
{
    m_yAxis = new QValueAxis();
    m_yAxis->setTitleText(m_yAxisConfig.title);
    if (!m_yAxisConfig.autoRange) {
        m_yAxis->setRange(m_yAxisConfig.min, m_yAxisConfig.max);
    }
    m_yAxis->setTickCount(m_yAxisConfig.tickCount);
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
}

void TChartWidget::cleanupAxes()
{
    // 移除所有现有轴
    if (m_xDateTimeAxis) {
        m_chart->removeAxis(m_xDateTimeAxis);
        delete m_xDateTimeAxis;
        m_xDateTimeAxis = nullptr;
    }
    if (m_xValueAxis) {
        m_chart->removeAxis(m_xValueAxis);
        delete m_xValueAxis;
        m_xValueAxis = nullptr;
    }
    if (m_yAxis) {
        m_chart->removeAxis(m_yAxis);
        delete m_yAxis;
        m_yAxis = nullptr;
    }
    m_xAxis = nullptr;
}

void TChartWidget::setDateTimeAxisLabelAngle(int angle)
{
    m_xAxisConfig.labelAngle = angle;
    if (m_xDateTimeAxis) {
        m_xDateTimeAxis->setLabelsAngle(angle);
    }
}


/*!
 * \brief TChartWidget::addSelectionMarker 添加选择mark
 * \param seriesName
 * \param point
 */
// 修正 addSelectionMarker 函数
void TChartWidget::addSelectionMarker(const QString& seriesName, const QPointF& point)
{
    QString pointKey = generatePointKey(seriesName, point);

    // 创建选中标记系列（如果不存在）
    if (!m_selectionMarkers.contains(pointKey)) {
        QScatterSeries* markerSeries = new QScatterSeries();
        markerSeries->setName(pointKey);
        markerSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        markerSeries->setMarkerSize(16.0);  // 增大一些以便更明显
        markerSeries->setColor(Qt::transparent);  // 透明填充
        markerSeries->setBorderColor(Qt::green);    // 绿色边框

        // 设置虚线边框 - 重要：使用正确的QPen设置
        QPen pen(Qt::darkYellow);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(3);  // 增加线宽使更明显
        markerSeries->setPen(pen);

        // 设置边框宽度
        //markerSeries->setMarkerSize(16); - 貌似没啥用

        // 添加点
        *markerSeries << point;

        // 添加到图表
        m_chart->addSeries(markerSeries);

        // 关键修正：正确关联到现有的轴
        // 遍历当前图表中的所有系列，找到正确的轴
        QList<QAbstractAxis*> xAxisList = m_chart->axes(Qt::Horizontal);
        QList<QAbstractAxis*> yAxisList = m_chart->axes(Qt::Vertical);

        if (!xAxisList.isEmpty() && !yAxisList.isEmpty()) {
            // 关联到与原系列相同的轴
            markerSeries->attachAxis(xAxisList.first());
            markerSeries->attachAxis(yAxisList.first());
        } else if (m_xAxis && m_yAxis) {
            // 备用方案：使用成员变量
            markerSeries->attachAxis(m_xAxis);
            markerSeries->attachAxis(m_yAxis);
        }

        // 确保标记系列可见
        markerSeries->setVisible(true);

        // 为标记添加点击事件处理[原因是mark比原先的点大，会覆盖原先点的点击事件导致点不会再次被点击，这样就不能取消]
        connect(markerSeries, &QScatterSeries::doubleClicked, [this, seriesName, point]() {
            // 当点击标记时，相当于点击原始点，应该取消选中
            qDebug() << "点击选中标记，取消选中:" << seriesName << point;
            removeSelectionMarker(seriesName, point);
            m_selectedPoints.removeOne(qMakePair(seriesName, point));
            emit pointSelected(seriesName, point, false);
        });

        m_selectionMarkers[pointKey] = markerSeries;
        m_selectedPointMap[pointKey] = point;

        qDebug() << "添加选中标记:" << pointKey << "坐标:" << point;

        // 强制更新图表
        m_chart->update();
    }
}

/*!
 * \brief TChartWidget::removeSelectionMarker 移除选中mark
 * \param seriesName
 * \param point
 */
void TChartWidget::removeSelectionMarker(const QString &seriesName, const QPointF &point)
{
    QString pointKey = generatePointKey(seriesName, point);

    if (m_selectionMarkers.contains(pointKey)) {
        QScatterSeries* markerSeries = m_selectionMarkers[pointKey];
        m_chart->removeSeries(markerSeries);
        delete markerSeries;

        m_selectionMarkers.remove(pointKey);
        m_selectedPointMap.remove(pointKey);

        qDebug() << "移除选中标记:" << pointKey;
    }
}

/*!
 * \brief TChartWidget::clearAllSelectionMarkers 清除选中mark
 */
void TChartWidget::clearAllSelectionMarkers()
{
    for (QScatterSeries* markerSeries : m_selectionMarkers.values()) {
        m_chart->removeSeries(markerSeries);
        delete markerSeries;
    }

    m_selectionMarkers.clear();
    m_selectedPointMap.clear();
    m_selectedPoints.clear();

    qDebug() << "清除所有选中标记";
}

QString TChartWidget::generatePointKey(const QString &seriesName, const QPointF &point) const
{
    QDateTime date = QDateTime::fromMSecsSinceEpoch(point.x());
    QString timeStr = date.toString("yyyyMMddhhmmsszzz");
    return QString("%1_%2_%3").arg(seriesName).arg(timeStr).arg(point.y());
}

QString TChartWidget::findSeriesNameByPoint(const QPointF &point) const
{
    // 遍历所有系列查找包含该点的系列
    for (auto it = m_seriesMap.constBegin(); it != m_seriesMap.constEnd(); ++it) {
        QString seriesName = it.key();
        QAbstractSeries* series = it.value();

        if (series->type() == QAbstractSeries::SeriesTypeScatter) {
            QScatterSeries* scatterSeries = qobject_cast<QScatterSeries*>(series);
            if (scatterSeries) {
                for (const QPointF& seriesPoint : scatterSeries->points()) {
                    // 比较点坐标（考虑一定的容差）
                    if (qAbs(seriesPoint.x() - point.x()) < 1.0 &&
                        qAbs(seriesPoint.y() - point.y()) < 1.0) {
                        return seriesName;
                    }
                }
            }
        }
    }
    return QString();
}

//void TChartWidget::handleSeriesClicked(const QPointF &point)
//{
//    // 查找是哪个系列触发的点击
//    QString seriesName = "Unknown";
//    QAbstractSeries* senderSeries = qobject_cast<QAbstractSeries*>(sender());
//    if (senderSeries) {
//        for (auto it = m_seriesMap.constBegin(); it != m_seriesMap.constEnd(); ++it) {
//            if (sender() == it.value()) {
//                seriesName = it.key();
//                break;
//            }
//        }
//    }

//    if (m_mouseMode == SelectMode) {
//        // 点选模式
//        QString pointKey = generatePointKey(seriesName, point);

//        // 检查点是否已选中
//        bool alreadySelected = m_selectedPointMap.contains(pointKey);

//        if (alreadySelected) {
//            // 已选中则取消选中
//            removeSelectionMarker(seriesName, point);
//            m_selectedPoints.removeOne(qMakePair(seriesName, point));
//            emit pointSelected(seriesName, point, false);
//            qDebug() << "取消选中点:" << seriesName << point;
//        } else {
//            // 未选中则添加选中
//            addSelectionMarker(seriesName, point);
//            m_selectedPoints.append(qMakePair(seriesName, point));
//            emit pointSelected(seriesName, point, true);
//            qDebug() << "选中点:" << seriesName << point;
//        }
//    } else {
//        // 缩放模式，保持原有行为或发出点击信号
//        emit pointClicked(seriesName, point);
//    }
//}

void TChartWidget::handleSeriesDoubleClicked(const QPointF &point)
{
    // 1.查找是哪个系列触发的双击
    QString seriesName = "Unknown";
    QAbstractSeries* senderSeries = qobject_cast<QAbstractSeries*>(sender());
    if (senderSeries) {
        for (auto it = m_seriesMap.constBegin(); it != m_seriesMap.constEnd(); ++it) {
            if (sender() == it.value()) {
                seriesName = it.key();
                break;
            }
        }
    }

    //2.绘制选择marker
    QString pointKey = generatePointKey(seriesName, point);

    // 检查点是否已选中
    bool alreadySelected = m_selectedPointMap.contains(pointKey);

    if (alreadySelected) {
        // 已选中则取消选中
        removeSelectionMarker(seriesName, point);
        m_selectedPoints.removeOne(qMakePair(seriesName, point));
        emit pointSelected(seriesName, point, false);
        qDebug() << "取消选中点:" << seriesName << point;
    } else {
        // 未选中则添加选中
        addSelectionMarker(seriesName, point);
        m_selectedPoints.append(qMakePair(seriesName, point));
        emit pointSelected(seriesName, point, true);
        qDebug() << "选中点:" << seriesName << point;
    }


    //3.通知主界面操作
    emit pointDoubleClicked(seriesName, point);

    /*
    //if (m_mouseMode == ZoomMode) {
        // 缩放模式下保持原有行为 - 相当于是点选一个点
        QObject* snt = sender();
        QScatterSeries* ser = static_cast<QScatterSeries*>(snt);
        if(ser != nullptr){
            qDebug().noquote() << "SeriesName:  " << ser->name();
        }
        QString recipeNameEQ = "NULL";
        QString recipeNameRGA = "NULL";
        QDateTime date = QDateTime::fromMSecsSinceEpoch(point.x());
        if(!m_seriesInfoMap.isEmpty()){
            QStringList infoList = m_seriesInfoMap.value(date);
            if(infoList.count() >= 2){
                recipeNameEQ = infoList.value(0,"NULL");
                recipeNameRGA = infoList.value(1,"NULL");
            }
        }


        //文件名格式 ：MASS_DATA_20250904172706(RecipeNameEQ@NULL)(RecipeNameRGA@ChC-Idle.tuneFile)
        QString formarStr = QString("%1,%2,%3")
                .arg(date.toString("yyyyMMddhhmmss"))
                .arg(recipeNameEQ)
                .arg(recipeNameRGA);

        QStringList info = {formarStr};
        emit sProcessStart(info);//有了这3个基础数据，后处理软件就可以组建文件名称
        QString x = date.toString("yyyy-MM-dd hh:mm:ss.zzz");
        qDebug().noquote() << QString("doubleClicked at x:%1,EQ:%2,RGA:%3").arg(x).arg(recipeNameEQ).arg(recipeNameRGA);

//    } else {
//        // 点选模式下发出双击信号
        emit pointDoubleClicked(seriesName, point);
//    }
        */
}

//void TChartWidget::handleSeriesHovered(const QPointF &point, bool state)
//{
//    Q_UNUSED(state)
//    QObject* snt = sender();
//    QScatterSeries* ser = static_cast<QScatterSeries*>(snt);
//    if(ser != nullptr){
//        //qDebug().noquote() << "SeriesName:  " << ser->name();
//    }

//    //qDebug() << "Series clicked at:" << point;
//    // 可以在这里发射自定义信号
//    QString x = QDateTime::fromMSecsSinceEpoch(point.x()).toString("yyyy-MM-dd hh:mm:ss.zzz");
//    QString y = QString::number( point.y(),'f',4);
//    //qDebug().noquote() << "Series clicked at-xy:  " << x << " - " << y;
//    QToolTip::showText(QCursor::pos(), formatTooltip(point));
//}


//以下  area告警区域
void TChartWidget::initializeAreaSeries()
{
    if (!m_chart || m_areasInitialized) {
        return;
    }

    // 创建边界线系列
    m_upperTopLine = new QLineSeries();
    m_upperBottomLine = new QLineSeries();
    m_lowerTopLine = new QLineSeries();
    m_lowerBottomLine = new QLineSeries();

    m_upperTopLine->setName("upperTopLine");
    m_upperBottomLine->setName("upperBottomLine");
    m_lowerTopLine->setName("lowerTopLine");
    m_lowerBottomLine->setName("lowerBottomLine");

    m_upperTopLine->setColor(Qt::red);
    m_upperBottomLine->setColor(Qt::red);
    m_lowerTopLine->setColor(Qt::red);
    m_lowerBottomLine->setColor(Qt::red);
    // 隐藏边界线
    m_upperTopLine->setVisible(false);
    m_upperBottomLine->setVisible(false);
    m_lowerTopLine->setVisible(false);
    m_lowerBottomLine->setVisible(false);

    // 创建面积图系列
    m_upperAreaSeries = new QAreaSeries(m_upperTopLine, m_upperBottomLine);
    m_upperAreaSeries->setName("UpperArea");
    m_upperAreaSeries->setVisible(false);
    m_upperAreaSeries->setPen(QPen(Qt::NoPen));
    m_upperAreaSeries->setBrush(QBrush(QColor(255, 0, 0, 30))); // 默认淡红色
    m_upperAreaSeries->setOpacity(0.3);

    m_lowerAreaSeries = new QAreaSeries(m_lowerTopLine, m_lowerBottomLine);
    m_lowerAreaSeries->setName("LowerArea");
    m_lowerAreaSeries->setVisible(false);
    m_lowerAreaSeries->setPen(QPen(Qt::NoPen));
    m_lowerAreaSeries->setBrush(QBrush(QColor(255, 0, 0, 30))); // 默认淡红色
    m_lowerAreaSeries->setOpacity(0.3);

    // 添加到图表
    m_chart->addSeries(m_upperAreaSeries);
    m_chart->addSeries(m_lowerAreaSeries);

    // 关联轴（假设轴已经设置完成）
    if (m_xAxis && m_yAxis) {
        m_upperAreaSeries->attachAxis(m_xAxis);
        m_upperAreaSeries->attachAxis(m_yAxis);
        m_lowerAreaSeries->attachAxis(m_xAxis);
        m_lowerAreaSeries->attachAxis(m_yAxis);
    }

    m_areasInitialized = true;

    // 设置默认值
    updateAreaValues(0.0, 1.0, -1.0, 0.0);
}

void TChartWidget::setShowUpperArea(bool show)
{
    m_showUpperArea = show;
    if (m_upperAreaSeries) {
        m_upperAreaSeries->setVisible(show && m_areasInitialized);
    }
    updateAreaDisplay();
}

void TChartWidget::setShowLowerArea(bool show)
{
    m_showLowerArea = show;
    if (m_lowerAreaSeries) {
        m_lowerAreaSeries->setVisible(show && m_areasInitialized);
    }
    updateAreaDisplay();
}

void TChartWidget::setShowAreas(bool upperShow, bool lowerShow)
{
    setShowUpperArea(upperShow);
    setShowLowerArea(lowerShow);
}

void TChartWidget::updateAreaValues(double upBase, double upLine, double downBase, double downLine)
{
    m_upBase = upBase;
    m_upLine = upLine;
    m_downBase = downBase;
    m_downLine = downLine;

    // 如果还没有初始化，先初始化
    if (!m_areasInitialized) {
        initializeAreaSeries();
    }

    // 更新数据点
    if (m_areasInitialized) {
        // 获取X轴范围
        qreal xMin = 0, xMax = 100; // 默认值
        if (m_xAxis) {
            if (QValueAxis* valueAxis = qobject_cast<QValueAxis*>(m_xAxis)) {
                xMin = valueAxis->min();
                xMax = valueAxis->max();
            } else if (QDateTimeAxis* dateAxis = qobject_cast<QDateTimeAxis*>(m_xAxis)) {
                xMin = dateAxis->min().toMSecsSinceEpoch();
                xMax = dateAxis->max().toMSecsSinceEpoch();
            }
        }

        // 更新上部区域数据
        if (m_upperTopLine && m_upperBottomLine) {
            m_upperTopLine->clear();
            m_upperBottomLine->clear();

            /*
            //调测
            {
                QPointF startPnt(xMin, qMax(upBase, upLine));
                QPointF endPnt(xMax, qMax(upBase, upLine));
                QList<QPointF> pnts = {startPnt,endPnt};
                m_upperTopLine->replace(pnts);
                qDebug().noquote() << "m_upperTopLine:  " << pnts;
            }
            {
                QPointF startPnt(xMin, qMin(upBase, upLine));
                QPointF endPnt(xMax, qMin(upBase, upLine));
                QList<QPointF> pnts = {startPnt,endPnt};
                m_upperBottomLine->replace(pnts);
                qDebug().noquote() << "m_upperBottomLine:  " << pnts;
            }

            QLineSeries* upLine = m_upperAreaSeries->upperSeries();
            QList<QPointF> upPnts = upLine->points();

            QLineSeries* lowLine = m_upperAreaSeries->lowerSeries();
            QList<QPointF> lowPnts = lowLine->points();

            qDebug().noquote() << "upPnts:  " << upPnts;
            qDebug().noquote() << "lowPnts:  " << lowPnts;
            */

            *m_upperTopLine << QPointF(xMin, qMax(upBase, upLine)) << QPointF(xMax, qMax(upBase, upLine));
            *m_upperBottomLine << QPointF(xMin, qMin(upBase, upLine)) << QPointF(xMax, qMin(upBase, upLine));
        }

        // 更新下部区域数据
        if (m_lowerTopLine && m_lowerBottomLine) {
            m_lowerTopLine->clear();
            m_lowerBottomLine->clear();

            *m_lowerTopLine << QPointF(xMin, qMax(downBase, downLine)) << QPointF(xMax, qMax(downBase, downLine));
            *m_lowerBottomLine << QPointF(xMin, qMin(downBase, downLine)) << QPointF(xMax, qMin(downBase, downLine));
        }
    }

    updateAreaDisplay();
}

void TChartWidget::updateAreaDisplay()
{
    if (!m_areasInitialized) {
        return;
    }

    if (m_upperAreaSeries) {
        m_upperAreaSeries->setVisible(m_showUpperArea);
    }

    if (m_lowerAreaSeries) {
        m_lowerAreaSeries->setVisible(m_showLowerArea);
    }

    if (m_chart) {
        m_chart->update();
    }
}

void TChartWidget::setAreaColors(const QColor &upperColor, const QColor &lowerColor)
{
    if (!m_areasInitialized) {
        initializeAreaSeries();
    }

    if (m_upperAreaSeries) {
        m_upperAreaSeries->setBrush(QBrush(upperColor));
    }

    if (m_lowerAreaSeries) {
        m_lowerAreaSeries->setBrush(QBrush(lowerColor));
    }
}

