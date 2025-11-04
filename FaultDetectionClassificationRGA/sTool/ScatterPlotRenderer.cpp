#include "ScatterPlotRenderer.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <cmath>
#include <limits>

// 保存到文件
/*!
 * \brief ScatterPlotRenderer::savePlotsToFile
 * \param i_startDate  界面中的开始时间
 * \param i_endDate 界面中的结束时间
 * \param dataMap
 * \param config
 * \return
 */
bool ScatterPlotRenderer::savePlotsToFile(const QDate &i_startDate, const QDate &i_endDate, const QMap<QString, QList<QPair<QDateTime, double>>> &dataMap,
                                         const PlotConfig &config)
{
    // 确保输出目录存在
    QDir dir(config.outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "无法创建输出目录:" << config.outputDir;
            return false;
        }
    }

    bool allSuccess = true;

    // 为每个数据系列生成图片
    auto images = renderPlotsToImages(dataMap, config);
    for (auto it = images.constBegin(); it != images.constEnd(); ++it) {
        QString fileName = it.key();
        // 清理文件名中的非法字符
        fileName.replace("/", "_").replace("\\", "_").replace(":", "_");
        fileName.replace("*", "_").replace("?", "_").replace("\"", "_");
        fileName.replace("<", "_").replace(">", "_").replace("|", "_");

        fileName += "_" + i_startDate.toString("MM_dd") + "-" + i_endDate.toString("MM_dd");

        QString filePath = config.outputDir + "/" + fileName + ".png";
        if (!it.value().save(filePath, "PNG")) {
            qDebug() << "保存图片失败:" << filePath;
            allSuccess = false;
        } else {
            qDebug() << "成功保存图片:" << filePath;
        }
    }

    return allSuccess;
}

/*!
 * \brief ScatterPlotRenderer::renderPlotsToImages 渲染告警邮件的chart图
 * \param dataMap
 * \param config
 * \return
 */
QMap<QString, QImage> ScatterPlotRenderer::renderPlotsToImages(const QMap<QString, QList<QPair<QDateTime, double>>> &dataMap,
                                                              const PlotConfig &config)
{
    QMap<QString, QImage> images;

    for (auto it = dataMap.constBegin(); it != dataMap.constEnd(); ++it) {
        QString title = it.key();
        const QList<QPair<QDateTime, double>> &data = it.value();

        if (data.isEmpty()) {
            continue;
        }

        QImage image = renderSinglePlot(title, data, config);
        if (!image.isNull()) {
            images[title] = image;
        }
    }

    return images;
}

// 渲染单个图表到QImage
/*
QImage ScatterPlotRenderer::renderSinglePlot(const QString &title,
                                            const QList<QPair<QDateTime, double>> &data,
                                            const PlotConfig &config)
{
    if (data.isEmpty()) {
        return QImage();
    }

    // 创建图片
    QImage image(config.width, config.height, QImage::Format_ARGB32);
    image.fill(config.backgroundColor);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 计算数据范围
    DataRange range = calculateDataRange(data);

    // 确保Y轴范围包含上下限值
    if (config.showLimits) {
        range.minY = std::min(range.minY, config.down);
        range.maxY = std::max(range.maxY, config.up);
    }

    // 计算绘图区域（留出边距）
    int margin = 70;
    QRect plotRect(margin, margin + 40,
                   config.width - 2 * margin,
                   config.height - 2 * margin - 80);

    // 绘制各个元素
    if (config.showGrid) {
        drawGrid(painter, range, plotRect, config);
    }

    drawAxes(painter, range, plotRect, config);

    if (config.showLimits) {
        drawLimitLines(painter, range, plotRect, config);
    }

    drawDataPoints(painter, data, range, plotRect, config);
    drawTitle(painter, title, QRect(0, 10, config.width, 30), config);

    if (config.showLegend) {
        QRect legendRect(config.width - 150, config.height - 60, 140, 50);
        drawLegend(painter, legendRect, config);
    }

    painter.end();

    return image;
}
*/

/*!
 * \brief ScatterPlotRenderer::renderSinglePlot 渲染告警邮件的chart图
 * \param title
 * \param data
 * \param config
 * \return
 */
QImage ScatterPlotRenderer::renderSinglePlot(const QString &title,
                                            const QList<QPair<QDateTime, double>> &data,
                                            const PlotConfig &config)
{
    // 创建图片
    QImage image(config.width, config.height, QImage::Format_ARGB32);
    image.fill(config.backgroundColor);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 计算绘图区域（留出边距）
    int margin = 70;
    QRect plotRect(margin, margin + 40,
                   config.width - 2 * margin,
                   config.height - 2 * margin - 80);

    // 绘制标题
    //drawTitle(painter, title, QRect(0, 10, config.width, 30), config);

    if (data.isEmpty()) {
        // 如果没有数据，绘制空白图表
        // 绘制坐标轴
        painter.setPen(QPen(config.axisColor, 2));
        painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.right(), plotRect.bottom()); // X轴
        painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.left(), plotRect.top());    // Y轴

        // 添加"无数据"提示文本
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 12));
        QString noDataText = "无数据";
        QRect textRect(plotRect.left() + plotRect.width()/2 - 30,
                      plotRect.top() + plotRect.height()/2 - 10,
                      60, 20);
        painter.drawText(textRect, Qt::AlignCenter, noDataText);

        // 绘制图例（如果需要）
        if (config.showLegend) {
            QRect legendRect(config.width - 150, config.height - 60, 140, 50);
            drawLegend(painter, legendRect, config);
        }

        painter.end();
        return image;
    }

    // 有数据的情况，按原有逻辑处理
    DataRange range = calculateDataRange(data);

    // 确保Y轴范围包含上下限值
    if (config.showLimits) {
        range.minY = std::min(range.minY, config.down);
        range.maxY = std::max(range.maxY, config.up);
    }

    // 绘制各个元素
    if (config.showGrid) {
        drawGrid(painter, range, plotRect, config);
    }

    drawAxes(painter, range, plotRect, config);

    if (config.showLimits) {
        drawLimitLines(painter, range, plotRect, config);
    }

    drawDataPoints(painter, data, range, plotRect, config);

    if (config.showLegend) {
        QRect legendRect(config.width - 150, config.height - 60, 140, 50);
        drawLegend(painter, legendRect, config);
    }

    painter.end();

    return image;
}

// 计算数据范围
ScatterPlotRenderer::DataRange ScatterPlotRenderer::calculateDataRange(const QList<QPair<QDateTime, double>> &data)
{
    DataRange range;

    if (data.isEmpty()) {
        range.minX = range.maxX = 0;
        range.minY = range.maxY = 0;
        return range;
    }

    // 初始化
    range.minDateTime = data.first().first;
    range.maxDateTime = data.first().first;
    range.minY = data.first().second;
    range.maxY = data.first().second;

    // 遍历所有数据点
    for (const auto &point : data) {
        const QDateTime &dt = point.first;
        double value = point.second;

        if (dt < range.minDateTime) range.minDateTime = dt;
        if (dt > range.maxDateTime) range.maxDateTime = dt;
        if (value < range.minY) range.minY = value;
        if (value > range.maxY) range.maxY = value;
    }

    range.minX = range.minDateTime.toMSecsSinceEpoch();
    range.maxX = range.maxDateTime.toMSecsSinceEpoch();

    // 添加边距
    double xMargin = (range.maxX - range.minX) * 0.05;
    double yMargin = (range.maxY - range.minY) * 0.1;

    range.minX -= xMargin;
    range.maxX += xMargin;
    range.minY -= yMargin;
    range.maxY += yMargin;

    // 防止除零错误
    if (range.maxX == range.minX) {
        range.maxX = range.minX + 1;
    }
    if (range.maxY == range.minY) {
        range.maxY = range.minY + 1;
    }

    return range;
}

// 预定义颜色列表
QList<QColor> ScatterPlotRenderer::getDefaultColors()
{
    return {
        Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan,
        Qt::yellow, Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta,
        Qt::darkCyan, Qt::darkYellow, Qt::black
    };
}

// 内部绘制函数
void ScatterPlotRenderer::drawPlot(QPainter &painter,
                                  const QString &title,
                                  const QList<QPair<QDateTime, double>> &data,
                                  const DataRange &range,
                                  const PlotConfig &config)
{
    Q_UNUSED(painter);
    Q_UNUSED(title);
    Q_UNUSED(data);
    Q_UNUSED(range);
    Q_UNUSED(config);
}

// 绘制坐标轴
void ScatterPlotRenderer::drawAxes(QPainter &painter,
                                  const DataRange &range,
                                  const QRect &plotRect,
                                  const PlotConfig &config)
{
    painter.setPen(QPen(config.axisColor, 2));
    painter.setFont(config.axisFont);

    // 绘制坐标轴
    painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.right(), plotRect.bottom()); // X轴
    painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.left(), plotRect.top());    // Y轴

    // 绘制X轴刻度和标签
    int xTickCount = 5;
    for (int i = 0; i <= xTickCount; ++i) {
        int x = plotRect.left() + (plotRect.width() * i) / xTickCount;
        painter.drawLine(x, plotRect.bottom(), x, plotRect.bottom() + 5);

        // 时间标签
        qint64 timeValue = range.minX + (range.maxX - range.minX) * i / xTickCount;
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(timeValue);
        QString timeStr = dt.toString("yyyyMMdd_hh:mm:ss");
        QRect textRect(x - 30, plotRect.bottom() + 8, 160, 20);
        painter.drawText(textRect, Qt::AlignCenter, timeStr);
    }

    // 绘制Y轴刻度和标签
    int yTickCount = 6;
    for (int i = 0; i <= yTickCount; ++i) {
        int y = plotRect.bottom() - (plotRect.height() * i) / yTickCount;
        painter.drawLine(plotRect.left() - 5, y, plotRect.left(), y);

        // 数值标签
        double value = range.minY + (range.maxY - range.minY) * i / yTickCount;
        QString valueStr = QString::number(value, 'f', 2);
        QRect textRect(plotRect.left() - 60, y - 10, 50, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, valueStr);
    }

    // 绘制轴标签
    painter.drawText(plotRect.left() + plotRect.width()/2 - 20, plotRect.bottom() + 50, "时间");
    painter.save();
    painter.rotate(-90);
    painter.drawText(-plotRect.top() - plotRect.height()/2 - 20, plotRect.left() - 60, "数值");
    painter.restore();
}

// 绘制网格
void ScatterPlotRenderer::drawGrid(QPainter &painter,
                                  const DataRange &range,
                                  const QRect &plotRect,
                                  const PlotConfig &config)
{
    painter.setPen(QPen(config.gridColor, 1, Qt::DotLine));

    // 绘制垂直网格线
    int xTickCount = 5;
    for (int i = 0; i <= xTickCount; ++i) {
        int x = plotRect.left() + (plotRect.width() * i) / xTickCount;
        painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
    }

    // 绘制水平网格线
    int yTickCount = 6;
    for (int i = 0; i <= yTickCount; ++i) {
        int y = plotRect.bottom() - (plotRect.height() * i) / yTickCount;
        painter.drawLine(plotRect.left(), y, plotRect.right(), y);
    }
}

// 绘制数据点
void ScatterPlotRenderer::drawDataPoints(QPainter &painter,
                                        const QList<QPair<QDateTime, double>> &data,
                                        const DataRange &range,
                                        const QRect &plotRect,
                                        const PlotConfig &config)
{
    for (const auto &point : data) {
        double x = dateTimeToX(point.first, range, plotRect);
        double y = valueToY(point.second, range, plotRect);

        // 确保点在绘图区域内
        if (x >= plotRect.left() && x <= plotRect.right() &&
            y >= plotRect.top() && y <= plotRect.bottom()) {

            // 根据值是否超出范围选择颜色
            QColor pointColor;
            if (isOutlier(point.second, config)) {
                pointColor = config.outlierPointColor; // 红色表示异常点
            } else {
                pointColor = config.normalPointColor;  // 蓝色表示正常点
            }

            painter.setPen(QPen(pointColor, config.pointSize));
            painter.setBrush(pointColor);
            painter.drawEllipse(QPointF(x, y), config.pointSize/2.0, config.pointSize/2.0);
        }
    }
}

// 绘制上下限线
void ScatterPlotRenderer::drawLimitLines(QPainter &painter,
                                        const DataRange &range,
                                        const QRect &plotRect,
                                        const PlotConfig &config)
{
    // 绘制下限线
    if (config.down > -std::numeric_limits<double>::max()) {
        double y = valueToY(config.down, range, plotRect);
        if (y >= plotRect.top() && y <= plotRect.bottom()) {
            painter.setPen(QPen(config.downLimitColor, 2, Qt::DashLine));
            painter.drawLine(plotRect.left(), y, plotRect.right(), y);

            // 添加下限标签
            painter.setPen(config.downLimitColor);
            painter.setFont(QFont("Arial", 9));
            QString label = QString("下限: %1").arg(config.down, 0, 'f', 2);
            painter.drawText(plotRect.left() + 10, y - 5, label);
        }
    }

    // 绘制上限线
    if (config.up < std::numeric_limits<double>::max()) {
        double y = valueToY(config.up, range, plotRect);
        if (y >= plotRect.top() && y <= plotRect.bottom()) {
            painter.setPen(QPen(config.upLimitColor, 2, Qt::DashLine));
            painter.drawLine(plotRect.left(), y, plotRect.right(), y);

            // 添加上限标签
            painter.setPen(config.upLimitColor);
            painter.setFont(QFont("Arial", 9));
            QString label = QString("上限: %1").arg(config.up, 0, 'f', 2);
            painter.drawText(plotRect.left() + 10, y - 5, label);
        }
    }
}

// 绘制标题
void ScatterPlotRenderer::drawTitle(QPainter &painter,
                                   const QString &title,
                                   const QRect &titleRect,
                                   const PlotConfig &config)
{
    painter.setPen(Qt::black);
    painter.setFont(config.titleFont);
    painter.drawText(titleRect, Qt::AlignCenter, title);
}

// 绘制图例
void ScatterPlotRenderer::drawLegend(QPainter &painter,
                                    const QRect &legendRect,
                                    const PlotConfig &config)
{
    painter.setFont(QFont("Arial", 9));

    // 绘制正常点图例
    painter.setPen(config.normalPointColor);
    painter.setBrush(config.normalPointColor);
    painter.drawEllipse(QPointF(legendRect.left() + 10, legendRect.top() + 10),
                       config.pointSize/2.0, config.pointSize/2.0);
    painter.setPen(Qt::black);
    painter.drawText(legendRect.left() + 20, legendRect.top() + 15, "正常值");

    // 绘制异常点图例
    painter.setPen(config.outlierPointColor);
    painter.setBrush(config.outlierPointColor);
    painter.drawEllipse(QPointF(legendRect.left() + 10, legendRect.top() + 30),
                       config.pointSize/2.0, config.pointSize/2.0);
    painter.setPen(Qt::black);
    painter.drawText(legendRect.left() + 20, legendRect.top() + 35, "异常值");
}

// 时间转坐标
double ScatterPlotRenderer::dateTimeToX(const QDateTime &dt,
                                       const DataRange &range,
                                       const QRect &plotRect)
{
    qint64 timeValue = dt.toMSecsSinceEpoch();
    return plotRect.left() + (double)(timeValue - range.minX) / (range.maxX - range.minX) * plotRect.width();
}

// 数值转坐标
double ScatterPlotRenderer::valueToY(double value,
                                    const DataRange &range,
                                    const QRect &plotRect)
{
    return plotRect.bottom() - (double)(value - range.minY) / (range.maxY - range.minY) * plotRect.height();
}

// 判断是否为异常点
bool ScatterPlotRenderer::isOutlier(double value, const PlotConfig &config)
{
    return (value < config.down || value > config.up);
}
