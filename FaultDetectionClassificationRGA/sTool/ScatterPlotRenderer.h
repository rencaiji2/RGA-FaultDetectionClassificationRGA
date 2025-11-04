#ifndef SCATTERPLOTRENDERER_H
#define SCATTERPLOTRENDERER_H

#include <QMap>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QColor>
#include <QFont>

class ScatterPlotRenderer
{
public:
    struct PlotConfig {
        PlotConfig(){}
        int width = 800;              // 图片宽度
        int height = 600;             // 图片高度
        QColor backgroundColor = Qt::white;  // 背景颜色
        QColor gridColor = Qt::lightGray;    // 网格颜色
        QColor axisColor = Qt::black;        // 坐标轴颜色
        QColor normalPointColor = Qt::blue;  // 正常点颜色
        QColor outlierPointColor = Qt::red;  // 异常点颜色
        QColor downLimitColor = Qt::red;   // 下限线颜色
        QColor upLimitColor = Qt::red;     // 上限线颜色
        QFont titleFont = QFont("Arial", 14, QFont::Bold);  // 标题字体
        QFont axisFont = QFont("Arial", 5); // 坐标轴字体
        int pointSize = 6;            // 散点大小
        bool showGrid = true;         // 是否显示网格
        bool showLegend = true;       // 是否显示图例
        bool showLimits = true;       // 是否显示上下限线
        QString outputDir = "./plots"; // 输出目录
        double down = -std::numeric_limits<double>::max(); // 下限值
        double up = std::numeric_limits<double>::max();    // 上限值
    };

    struct DataRange {
        double minX, maxX;
        double minY, maxY;
        QDateTime minDateTime, maxDateTime;
    };

    // 保存到文件
    static bool savePlotsToFile(const QDate& i_startDate,const QDate& i_endDate,
                                const QMap<QString, QList<QPair<QDateTime, double>>> &dataMap,
                               const PlotConfig &config = PlotConfig());

    // 渲染到QImage列表
    static QMap<QString, QImage> renderPlotsToImages(const QMap<QString, QList<QPair<QDateTime, double>>> &dataMap,
                                                    const PlotConfig &config = PlotConfig());

    // 渲染单个图表到QImage
    static QImage renderSinglePlot(const QString &title,
                                  const QList<QPair<QDateTime, double>> &data,
                                  const PlotConfig &config = PlotConfig());

    // 计算数据范围
    static DataRange calculateDataRange(const QList<QPair<QDateTime, double>> &data);

    // 预定义颜色列表
    static QList<QColor> getDefaultColors();

private:
    // 内部绘制函数
    static void drawPlot(QPainter &painter,
                        const QString &title,
                        const QList<QPair<QDateTime, double>> &data,
                        const DataRange &range,
                        const PlotConfig &config);

    // 绘制坐标轴
    static void drawAxes(QPainter &painter,
                        const DataRange &range,
                        const QRect &plotRect,
                        const PlotConfig &config);

    // 绘制网格
    static void drawGrid(QPainter &painter,
                        const DataRange &range,
                        const QRect &plotRect,
                        const PlotConfig &config);

    // 绘制数据点
    static void drawDataPoints(QPainter &painter,
                              const QList<QPair<QDateTime, double>> &data,
                              const DataRange &range,
                              const QRect &plotRect,
                              const PlotConfig &config);

    // 绘制上下限线
    static void drawLimitLines(QPainter &painter,
                              const DataRange &range,
                              const QRect &plotRect,
                              const PlotConfig &config);

    // 绘制标题
    static void drawTitle(QPainter &painter,
                         const QString &title,
                         const QRect &titleRect,
                         const PlotConfig &config);

    // 绘制图例
    static void drawLegend(QPainter &painter,
                          const QRect &legendRect,
                          const PlotConfig &config);

    // 时间转坐标
    static double dateTimeToX(const QDateTime &dt,
                             const DataRange &range,
                             const QRect &plotRect);

    // 数值转坐标
    static double valueToY(double value,
                          const DataRange &range,
                          const QRect &plotRect);

    // 判断是否为异常点
    static bool isOutlier(double value, const PlotConfig &config);
};

#endif // SCATTERPLOTRENDERER_H
