#ifndef TCHARWIDGET_H
#define TCHARWIDGET_H

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QScatterSeries>
#include <QPieSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QCategoryAxis>
#include <QLegend>
#include <QVBoxLayout>
#include <QVector>
#include <QPointF>
#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QRectF>
#include <QRubberBand>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QAreaSeries>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE

// 自定义图表视图类，处理鼠标交互
class TChartView : public QChartView
{
    Q_OBJECT

public:
    explicit TChartView(QChart *chart, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    bool m_isDragging;
    bool m_isSelecting;
    QPoint m_lastMousePos;
    QPoint m_selectionStartPos;
    QRubberBand *m_rubberBand;

public:
    void zoomIn(const QRectF &rect);
    void zoomOut();
    void resetView();
};

class TChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TChartWidget(QWidget *parent = nullptr);
    ~TChartWidget();

    // 图例位置枚举
    enum LegendPosition {
        LegendTop,          // 顶部
        LegendBottom,       // 底部
        LegendLeft,         // 左侧
        LegendRight,        // 右侧
        LegendTopLeft,      // 左上角
        LegendTopRight,     // 右上角
        LegendBottomLeft,   // 左下角
        LegendBottomRight,  // 右下角
        LegendInside,       // 图表内部（默认居中）
        LegendCustom        // 自定义位置
    };
    Q_ENUM(LegendPosition)

    // 图例配置结构
    struct LegendConfig {
        bool visible;                    // 是否显示
        LegendPosition position;        // 图例位置
        Qt::Alignment alignment;        // 对齐方式
        QColor backgroundColor;         // 背景颜色
        QColor textColor;              // 文字颜色
        QFont font;                    // 字体
        bool showMarkers;              // 是否显示标记
        qreal markerSize;              // 标记大小
        qreal spacing;                 // 间距
        QPointF customPosition;        // 自定义位置（当position为LegendCustom时使用）

        LegendConfig() : visible(true),
                        position(LegendRight),
                        alignment(Qt::AlignCenter),
                        backgroundColor(Qt::transparent),
                        textColor(Qt::black),
                        showMarkers(true),
                        markerSize(10.0),
                        spacing(5.0) {}
    };

    // 图表类型枚举
    enum ChartType {
        LineChart,      // 折线图
        ScatterChart,   // 散点图
        PieChart,       // 饼图
        BarChart,       // 柱状图
        AreaChart       // 面积图
    };
    Q_ENUM(ChartType)

    // 数据系列结构
    struct SeriesData {
        QString name;
        QVector<QPointF> points;
        QColor color;
        qreal lineWidth;
        qreal pointSize;

        SeriesData() : lineWidth(2.0), pointSize(5.0) {}
    };

    // 轴类型枚举
        enum AxisType {
            NumericAxis,    // 数值轴
            DateTimeAxis    // 时间轴
        };

    // 扩展轴配置结构
    struct AxisConfig {
        QString title;
        QString format;
        qreal min;
        qreal max;
        bool autoRange;
        int tickCount;
        AxisType axisType;  // 新增：轴类型
        int labelAngle;  // 新增：标签角度

        AxisConfig() : min(0), max(100), autoRange(true), tickCount(5), axisType(NumericAxis),labelAngle(0) {}
    };

    // 图表配置结构
    struct ChartConfig {
        ChartType type;
        QString title;
        bool showLegend;
        Qt::Alignment legendAlignment;
        QColor backgroundColor;
        bool antialiasing;

        ChartConfig() : type(LineChart), showLegend(true),
                       legendAlignment(Qt::AlignRight),
                       backgroundColor(Qt::white),
                       antialiasing(true) {}
    };

    // 添加鼠标模式枚举
//    enum MouseMode {
//        ZoomMode = 0,      // 缩放模式（默认）
//        SelectMode = 1     // 点选模式
//    };
    // 设置鼠标模式
//    void setMouseMode(MouseMode mode){
//        m_mouseMode = mode;
//    }
//    MouseMode getMouseMode() const {
//    return m_mouseMode;
//}
    // 添加获取当前选中点的函数
    QList<QPair<QString, QPointF>> getSelectedPoints() const{
        return m_selectedPoints;
    }
    /*!
     * \brief TChartWidget::clearSelection 清空选中点mark
     */
    void clearSelection(){
        clearAllSelectionMarkers();
    }

    // 设置图表配置
    void setChartConfig(const ChartConfig& config);

    // 设置轴配置
    void setXAxisConfig(const AxisConfig& config);
    void setYAxisConfig(const AxisConfig& config);

    // 设置数据系列
    void addSeries(const SeriesData& series);
    void clearSeries();

    // 数据操作
    void setData(const QString& seriesName, const QVector<QPointF>& points);
    void addDataPoint(const QString& seriesName, const QPointF& point);
    void removeSeries(const QString& seriesName);

    // 设置图例配置
    void setLegendConfig(const LegendConfig& config);

    // 快速设置图例位置
    void setLegendPosition(LegendPosition position);
    void setLegendVisible(bool visible);
    void setLegendAlignment(Qt::Alignment alignment);

    // 获取图例配置
    LegendConfig getLegendConfig() const { return m_legendConfig; }

    // 新增：设置XY数组数据（仅对折线图和散点图有效）
    void setXYArray(const QString& seriesName,
                                 const QVector<double>& i_x,
                                 const QVector<double>& i_y,
                                 const QColor& color);

    // 辅助函数：检查当前图表类型是否支持XY数组
    bool isXYChartType() const;

    // 图表操作
    void updateChart();
    void clearChart();
    void replot();
    void resetView(); // 新增：重置视图

    // 获取图表对象（用于高级自定义）
    QChart* getChart() { return m_chart; }
    TChartView* getChartView() { return m_chartView; }

//private slots:
//    void handleSeriesClicked(const QPointF &point);
    QHash <QString, QList<uint>> mOptionMap;//20250925用于后处理匹配时间
    QHash <QString, QStringList> mRecipeNameEQ;
    QHash <QString, QStringList> mRecipeNameRGA;
    QHash <QString, QStringList> mStep;
    QHash <QString, QStringList> mLotID;
    QHash <QString, QStringList> mSlotID;
    QHash <QString, QStringList> mWaferID;
    QHash <QString, QStringList> mChamberID;

    //辅助数据 - 作废
    QMap<QDateTime,QStringList>& getOptionMap(){
        return m_seriesInfoMap;
    }
    void setOptionMap(QMap<QDateTime,QStringList>& i_seriesInfoMap)
    {
        m_seriesInfoMap.clear();
        m_seriesInfoMap = i_seriesInfoMap;
    }

    QMap<QString, SeriesData> getSeriesData()
    {
        return m_seriesData;
    }

    //重新关联轴 【主要是层级的调整，chart是先创建的在底层】
    // 获取所有系列的函数
    QList<QAbstractSeries*> getAllSeries() const
    {
        if (m_chart) {
            return m_chart->series();
        }
        return QList<QAbstractSeries*>();
    }

    //移除- 注意不是删除【只是相当于把数据轴从chart中弄出来，指针还在】
    void removeSeries(QAbstractSeries* series);
    void addSeries(QAbstractSeries* series);

    //关于告警区域---------------------
    // 在 TChartWidget.h 的 private 部分添加成员变量
private:
    // 背景区域系列
    QAreaSeries* m_upperAreaSeries = nullptr;     // 上部区域
    QAreaSeries* m_lowerAreaSeries = nullptr;     // 下部区域

    // 边界线系列（用于面积图）
    QLineSeries* m_upperTopLine = nullptr;        // 上部区域上边界
    QLineSeries* m_upperBottomLine = nullptr;     // 上部区域下边界
    QLineSeries* m_lowerTopLine = nullptr;        // 下部区域上边界
    QLineSeries* m_lowerBottomLine = nullptr;     // 下部区域下边界

    // 区域值
    double m_upBase = 0;                    // 上部区域基线
    double m_upLine = 5;                    // 上部区域线
    double m_downBase = -5;                  // 下部区域基线
    double m_downLine = -10;                  // 下部区域线

    // 显示控制
    bool m_showUpperArea = true;               // 是否显示上部区域
    bool m_showLowerArea = true;               // 是否显示下部区域

    // 初始化标志
    bool m_areasInitialized = false;            // 区域是否已初始化

// 在 TChartWidget.h 的 public 部分添加公共函数
public:
    // 区域背景相关函数
    void initializeAreaSeries();                    // 初始化区域系列
    void setShowUpperArea(bool show);              // 设置上部区域显示
    void setShowLowerArea(bool show);              // 设置下部区域显示
    void setShowAreas(bool upperShow, bool lowerShow); // 同时设置两个区域显示
    void updateAreaValues(double upBase, double upLine, double downBase, double downLine); // 更新区域值
    void updateAreaDisplay();                       // 更新区域显示
    void setAreaColors(const QColor& upperColor = QColor(255, 0, 0, 30),
                      const QColor& lowerColor = QColor(255, 0, 0, 30)); // 设置区域颜色
    bool isUpperAreaVisible() const { return m_showUpperArea; }
    bool isLowerAreaVisible() const { return m_showLowerArea; }

    // 获取区域系列（用于外部控制层次）
    QAreaSeries* getUpperAreaSeries() const { return m_upperAreaSeries; }
    QAreaSeries* getLowerAreaSeries() const { return m_lowerAreaSeries; }


    //-----------end---------------------

signals:
    void sProcessStart(QStringList);//格式化的标志符的列表【用于后处理软件的启动配置】
    // 点选相关的信号
    //void pointClicked(const QString& seriesName, const QPointF& point);  // 点击点
    void pointDoubleClicked(const QString& seriesName, const QPointF& point);  // 双击点
    void pointSelected(const QString& seriesName, const QPointF& point, bool selected);  // 点选状态改变

private slots:
    //void handleSeriesClicked(const QPointF &point);
    void handleSeriesDoubleClicked(const QPointF &point);
    //void handleSeriesHovered(const QPointF &point, bool state);

private:
    void setupUI();
    void createLineChart();
    void createScatterChart();
    void createPieChart();
    void createBarChart();
    void createAreaChart();
    void updateAxis();
    void updateLegend();
    //QString formatTooltip(const QPointF& point);
    QString formatTooltip(const QPointF& point, QString name, int index);
    //图例显示相关
    void updateLegendPosition();
    Qt::Alignment mapLegendPositionToAlignment(LegendPosition position);
    void applyLegendStyle();
    //
    void createDateTimeChart();      // 创建时间轴图表

    TChartView *m_chartView;  // 修改为自定义视图类
    QChart *m_chart;

    // 配置
    ChartConfig m_chartConfig;
    AxisConfig m_xAxisConfig;
    AxisConfig m_yAxisConfig;

    // 添加成员变量
    QDateTimeAxis *m_xDateTimeAxis;
    QValueAxis *m_xValueAxis;
    QValueAxis *m_yAxis;

    // 添加新的私有函数
    void createScatterChartWithDateTime();
    void createScatterChartWithNumeric();
    void updateDateTimeAxis();
    void updateNumericAxis();
    void createYAxis();
    void cleanupAxes();

    void setDateTimeAxisLabelAngle(int angle);

    // 数据系列
    QMap<QString, SeriesData> m_seriesData;
    QMap<QString, QAbstractSeries*> m_seriesMap;

    // 轴
    QAbstractAxis *m_xAxis;
    // 图例显示相关
    LegendConfig m_legendConfig;
    //---------------------

    //辅助信息 2025-0916新增recipe相关 做一个map，完善数据
    //QStringList就两个值 recipeNameEQ，recipeNameRGA
    QMap<QDateTime,QStringList> m_seriesInfoMap;

    //----------------------点选相关-----------------------------------
    //MouseMode m_mouseMode = MouseMode::ZoomMode;
    // 选中点相关
    QList<QPair<QString, QPointF>> m_selectedPoints;
    QMap<QString, QScatterSeries*> m_selectionMarkers;  // 选中点标记系列
    QMap<QString, QPointF> m_selectedPointMap;  // 选中点映射（用于快速查找）

    // 添加选中点标记
    void addSelectionMarker(const QString& seriesName, const QPointF& point);
    void removeSelectionMarker(const QString& seriesName, const QPointF& point);
    void clearAllSelectionMarkers();
    QString generatePointKey(const QString& seriesName, const QPointF& point) const;

    // 辅助函数
    QString findSeriesNameByPoint(const QPointF& point) const;
    //-----------------------点选相关-------------------------------------------------
};

#endif // TCHARWIDGET_H
