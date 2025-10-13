#include "sFaultDetectionClassificationRGA.h"

void sFaultDetectionClassificationRGA::onCreateAlarmArea()
{
    createBackgroundAreas(m_alarmUpValue,m_alarmDownValue);
}

// 创建背景区域
void sFaultDetectionClassificationRGA::createBackgroundAreas(double up, double down)
{
    if (!m_chartWgt || !m_chartWgt->getChart()) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();

    // 保存限制值
    m_alarmUpValue = up;
    m_alarmDownValue = down;

    // 清除现有的背景区域
    clearBackgroundAreas();

    // 获取图表的绘图区域
    QRectF plotArea = chart->plotArea();

    // 获取Y轴
    QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);
    if (yAxisList.isEmpty()) {
        return;
    }

    QValueAxis* yAxis = qobject_cast<QValueAxis*>(yAxisList.first());
    if (!yAxis) {
        return;
    }

    double yMin = yAxis->min();
    double yMax = yAxis->max();

    // 创建上部背景区域（从 up 到 yMax）
    if (up < yMax) {
        QRectF upperRect = plotArea;
        // 计算上部区域的高度比例
        double upperHeightRatio = (yMax - up) / (yMax - yMin);
        upperRect.setTop(plotArea.top());  // 保持顶部不变
        upperRect.setBottom(plotArea.top() + plotArea.height() * (1.0 - (up - yMin) / (yMax - yMin)));

        m_upperBackground = new BackgroundAreaItem(upperRect, QColor(255, 0, 0, 50));  // 淡红色
        chart->scene()->addItem(m_upperBackground);

        // 转换坐标系统
        QPointF sceneTopLeft = chart->mapToScene(plotArea.topLeft().toPoint());
        m_upperBackground->setPos(sceneTopLeft);
    }

    // 创建下部背景区域（从 yMin 到 down）
    if (down > yMin) {
        QRectF lowerRect = plotArea;
        // 计算下部区域的高度比例
        double lowerHeightRatio = (down - yMin) / (yMax - yMin);
        lowerRect.setTop(plotArea.top() + plotArea.height() * (down - yMin) / (yMax - yMin));
        lowerRect.setBottom(plotArea.bottom());

        m_lowerBackground = new BackgroundAreaItem(lowerRect, QColor(255, 0, 0, 50));  // 淡红色
        chart->scene()->addItem(m_lowerBackground);

        // 转换坐标系统
        QPointF sceneTopLeft = chart->mapToScene(plotArea.topLeft().toPoint());
        m_lowerBackground->setPos(sceneTopLeft);
    }
}

// 更新背景区域（当图表缩放或平移时调用）
void sFaultDetectionClassificationRGA::updateBackgroundAreas()
{
    if (!m_chartWgt || !m_chartWgt->getChart()) {
        return;
    }

    QChart* chart = m_chartWgt->getChart();
    QRectF plotArea = chart->plotArea();

    // 获取Y轴
    QList<QAbstractAxis*> yAxisList = chart->axes(Qt::Vertical);
    if (yAxisList.isEmpty()) {
        return;
    }

    QValueAxis* yAxis = qobject_cast<QValueAxis*>(yAxisList.first());
    if (!yAxis) {
        return;
    }

    double yMin = yAxis->min();
    double yMax = yAxis->max();

    // 更新上部背景区域
    if (m_upperBackground && m_alarmUpValue < yMax) {
        QRectF upperRect = plotArea;
        upperRect.setTop(plotArea.top());
        upperRect.setBottom(plotArea.top() + plotArea.height() * (1.0 - (m_alarmUpValue - yMin) / (yMax - yMin)));
        m_upperBackground->setRect(upperRect);
    }

    // 更新下部背景区域
    if (m_lowerBackground && m_alarmDownValue > yMin) {
        QRectF lowerRect = plotArea;
        lowerRect.setTop(plotArea.top() + plotArea.height() * (m_alarmDownValue - yMin) / (yMax - yMin));
        lowerRect.setBottom(plotArea.bottom());
        m_lowerBackground->setRect(lowerRect);
    }
}

// 清除背景区域
void sFaultDetectionClassificationRGA::clearBackgroundAreas()
{
    if (m_upperBackground) {
        delete m_upperBackground;
        m_upperBackground = nullptr;
    }

    if (m_lowerBackground) {
        delete m_lowerBackground;
        m_lowerBackground = nullptr;
    }
}

// 设置背景区域（公共接口）
void sFaultDetectionClassificationRGA::setBackgroundAreas(double up, double down)
{
    createBackgroundAreas(up, down);
}

// 在适当的地方调用更新（比如图表更新后）
void sFaultDetectionClassificationRGA::onChartUpdated()
{
    updateBackgroundAreas();
}
