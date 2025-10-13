#include "sFaultDetectionClassificationRGA.h"

// 创建告警区域
void sFaultDetectionClassificationRGA::createAlarmAreas()
{
    // 清除现有的告警区域
    clearAlarmAreas();

    if (!m_chartWgt) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        return;
    }

    // 获取轴对象
    QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);

    if (xAxisList.isEmpty() || yAxisList.isEmpty()) {
        return;
    }

    QAbstractAxis* xAxis = xAxisList.first();
    QAbstractAxis* yAxis = yAxisList.first();

    // 创建上边界线（用于上告警区域）
    m_upperBoundSeries = new QLineSeries();
    m_upperBoundSeries->setName("UpperBound");
    m_upperBoundSeries->setVisible(false); // 隐藏边界线本身

    // 创建下边界线（用于下告警区域）
    m_lowerBoundSeries = new QLineSeries();
    m_lowerBoundSeries->setName("LowerBound");
    m_lowerBoundSeries->setVisible(false); // 隐藏边界线本身

    // 创建基准线（X轴）
    QLineSeries* baseLineUpper = new QLineSeries();
    baseLineUpper->setName("BaseLineUpper");
    baseLineUpper->setVisible(false);

    QLineSeries* baseLineLower = new QLineSeries();
    baseLineLower->setName("BaseLineLower");
    baseLineLower->setVisible(false);

    // 添加到图表
    chart->addSeries(m_upperBoundSeries);
    chart->addSeries(m_lowerBoundSeries);
    chart->addSeries(baseLineUpper);
    chart->addSeries(baseLineLower);

    // 关联轴
    m_upperBoundSeries->attachAxis(xAxis);
    m_upperBoundSeries->attachAxis(yAxis);
    m_lowerBoundSeries->attachAxis(xAxis);
    m_lowerBoundSeries->attachAxis(yAxis);
    baseLineUpper->attachAxis(xAxis);
    baseLineUpper->attachAxis(yAxis);
    baseLineLower->attachAxis(xAxis);
    baseLineLower->attachAxis(yAxis);

    // 创建面积图
    m_upperAlarmArea = new QAreaSeries(m_upperBoundSeries, baseLineUpper);
    m_upperAlarmArea->setName("UpperAlarmArea");
    m_upperAlarmArea->setColor(QColor(255, 0, 0, 15)); // 淡红色，透明度30
    m_upperAlarmArea->setBorderColor(Qt::transparent);

    m_lowerAlarmArea = new QAreaSeries(baseLineLower, m_lowerBoundSeries);
    m_lowerAlarmArea->setName("LowerAlarmArea");
    m_lowerAlarmArea->setColor(QColor(255, 0, 0, 15)); // 淡红色，透明度30
    m_lowerAlarmArea->setBorderColor(Qt::transparent);

    // 添加面积图到图表
    chart->addSeries(m_upperAlarmArea);
    chart->addSeries(m_lowerAlarmArea);

    // 关联轴
    m_upperAlarmArea->attachAxis(xAxis);
    m_upperAlarmArea->attachAxis(yAxis);
    m_lowerAlarmArea->attachAxis(xAxis);
    m_lowerAlarmArea->attachAxis(yAxis);

    // 设置可见性
    m_upperAlarmArea->setVisible(m_showAlarmArea);
    m_lowerAlarmArea->setVisible(m_showAlarmArea);

    // 更新告警区域
    updateAlarmAreas();
}

// 更新告警区域（根据当前X轴范围和告警值）
void sFaultDetectionClassificationRGA::updateAlarmAreas()
{
    if (!m_chartWgt || !m_upperAlarmArea || !m_lowerAlarmArea) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        return;
    }

    // 获取X轴范围
    QList<QAbstractAxis*> xAxisList = chart->axes(Qt::Horizontal);
    if (xAxisList.isEmpty()) {
        return;
    }

    QAbstractAxis* xAxis = xAxisList.first();
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

    // 更新上告警区域数据
    if (m_upperBoundSeries && m_upperAlarmArea->upperSeries()) {
        QVector<QPointF> upperPoints;
        upperPoints << QPointF(xMin, m_alarmUpValue) << QPointF(xMax, m_alarmUpValue);
        m_upperBoundSeries->replace(upperPoints);

        QVector<QPointF> baseUpperPoints;
        // 设置一个足够大的Y值来填充上部区域
        double upperFillValue = m_alarmUpValue + (m_alarmUpValue - m_alarmDownValue) * 2;
        baseUpperPoints << QPointF(xMin, upperFillValue) << QPointF(xMax, upperFillValue);
        qobject_cast<QLineSeries*>(m_upperAlarmArea->lowerSeries())->replace(baseUpperPoints);
    }

    // 更新下告警区域数据
    if (m_lowerBoundSeries && m_lowerAlarmArea->lowerSeries()) {
        QVector<QPointF> lowerPoints;
        lowerPoints << QPointF(xMin, m_alarmDownValue) << QPointF(xMax, m_alarmDownValue);
        m_lowerBoundSeries->replace(lowerPoints);

        QVector<QPointF> baseLowerPoints;
        // 设置一个足够小的Y值来填充下部区域
        double lowerFillValue = m_alarmDownValue - (m_alarmUpValue - m_alarmDownValue) * 2;
        baseLowerPoints << QPointF(xMin, lowerFillValue) << QPointF(xMax, lowerFillValue);
        qobject_cast<QLineSeries*>(m_lowerAlarmArea->upperSeries())->replace(baseLowerPoints);
    }

    // 设置可见性
    if (m_upperAlarmArea) m_upperAlarmArea->setVisible(m_showAlarmArea);
    if (m_lowerAlarmArea) m_lowerAlarmArea->setVisible(m_showAlarmArea);
}

// 设置告警区域可见性
void sFaultDetectionClassificationRGA::setAlarmAreasVisible(bool visible)
{
    m_showAlarmArea = visible;

    if (m_upperAlarmArea) m_upperAlarmArea->setVisible(visible);
    if (m_lowerAlarmArea) m_lowerAlarmArea->setVisible(visible);
}

// 清除告警区域
void sFaultDetectionClassificationRGA::clearAlarmAreas()
{
    if (!m_chartWgt) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    if (!chart) {
        return;
    }

    // 删除上告警区域
    if (m_upperAlarmArea) {
        if (chart->series().contains(m_upperAlarmArea)) {
            chart->removeSeries(m_upperAlarmArea);
        }
        delete m_upperAlarmArea;
        m_upperAlarmArea = nullptr;
    }

    // 删除下告警区域
    if (m_lowerAlarmArea) {
        if (chart->series().contains(m_lowerAlarmArea)) {
            chart->removeSeries(m_lowerAlarmArea);
        }
        delete m_lowerAlarmArea;
        m_lowerAlarmArea = nullptr;
    }

    // 删除边界线
    if (m_upperBoundSeries) {
        if (chart->series().contains(m_upperBoundSeries)) {
            chart->removeSeries(m_upperBoundSeries);
        }
        delete m_upperBoundSeries;
        m_upperBoundSeries = nullptr;
    }

    if (m_lowerBoundSeries) {
        if (chart->series().contains(m_lowerBoundSeries)) {
            chart->removeSeries(m_lowerBoundSeries);
        }
        delete m_lowerBoundSeries;
        m_lowerBoundSeries = nullptr;
    }

    // 删除基准线
    QList<QLineSeries*> toDelete;
    for (QAbstractSeries* series : chart->series()) {
        if (QLineSeries* lineSeries = qobject_cast<QLineSeries*>(series)) {
            if (lineSeries->name() == "BaseLineUpper" || lineSeries->name() == "BaseLineLower") {
                toDelete.append(lineSeries);
            }
        }
    }

    for (QLineSeries* series : toDelete) {
        chart->removeSeries(series);
        delete series;
    }
}

// 更新告警区域颜色
void sFaultDetectionClassificationRGA::updateAlarmAreaColors(const QColor& upperColor, const QColor& lowerColor)
{
    if (m_upperAlarmArea) {
        m_upperAlarmArea->setColor(upperColor);
    }
    if (m_lowerAlarmArea) {
        m_lowerAlarmArea->setColor(lowerColor);
    }
}

// 设置告警值的公共接口函数
void sFaultDetectionClassificationRGA::setAlarmValues(double up, double down)
{
    m_alarmUpValue = up;
    m_alarmDownValue = down;

    // 如果告警区域不存在，创建它们
    if (!m_upperAlarmArea || !m_lowerAlarmArea) {
        createAlarmAreas();
    } else {
        // 否则更新现有区域
        updateAlarmAreas();
    }
}
