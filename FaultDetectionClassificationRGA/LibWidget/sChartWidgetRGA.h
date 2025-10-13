#ifndef SCHARTWIDGET_H
#define SCHARTWIDGET_H
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_intervalcurve.h>
#include <QGestureEvent>
#include <QMutex>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <qwt_scale_draw.h>
#include <qwt_plot_grid.h>
#ifdef Q_OS_WIN32
#include "D:/QT/GlobalStruct/cPublicCCS.h"
#else
#include "/home/zhouxu/work/GlobalStruct/cPublicCCS.h"
#endif
#include <LibWidget/sMyButton.h>
#include "sTvplot.h"
//#include "sChartWidget/sChartXIC.h"
#include "LibWidget/sFileWidget.h"

#include "sCoordinateMask.h"

#include <QLineEdit>

#define SMS_CHANNEL_COUNT 64

struct _GRAPH_XIC:_PARAM_XIC{//:PEAK_VALUE{
    QwtPlotCurve* curve=nullptr; /**< TODO */
    _GRAPH_XIC(std::size_t length=0):
        _PARAM_XIC(length){
    }
    _GRAPH_XIC(_PARAM_XIC& pPARAM_XIC){
        massRange= pPARAM_XIC.massRange;
        color= pPARAM_XIC.color;
        Gain= pPARAM_XIC.Gain;
        Offset= pPARAM_XIC.Offset;
        yListXIC.swap(pPARAM_XIC.yListXIC);
    }
    _GRAPH_XIC(_PARAM_XIC* pPARAM_XIC){
        massRange= pPARAM_XIC->massRange;
        color= pPARAM_XIC->color;
        Gain= pPARAM_XIC->Gain;
        Offset= pPARAM_XIC->Offset;
        yListXIC.swap(pPARAM_XIC->yListXIC);
    }
    _GRAPH_XIC(double dRange,
               uint dColor,
               double dGain,
               double dOffset,
               std::size_t length=0):
        _PARAM_XIC(dRange,dColor,dGain,dOffset,length){
    }
    _GRAPH_XIC& operator=(const _GRAPH_XIC& pGRAPH_XIC){
        if (this != &pGRAPH_XIC){
            massRange= pGRAPH_XIC.massRange;
            color= pGRAPH_XIC.color;
            Gain= pGRAPH_XIC.Gain;
            Offset= pGRAPH_XIC.Offset;
            curve= pGRAPH_XIC.curve;
            //            yListXIC.resize(pGRAPH_XIC.yListXIC.size());
            //            memcpy(yListXIC.data(), pGRAPH_XIC.yListXIC.data(), pGRAPH_XIC.yListXIC.size());
            _GRAPH_XIC* p= (_GRAPH_XIC*)(&pGRAPH_XIC);
            yListXIC.swap(p->yListXIC);
        }
        return *this;
    }
    _GRAPH_XIC& operator=(const _PARAM_XIC& pPARAM_XIC){
        if (this != &pPARAM_XIC){
            massRange= pPARAM_XIC.massRange;
            color= pPARAM_XIC.color;
            Gain= pPARAM_XIC.Gain;
            Offset= pPARAM_XIC.Offset;
            //            curve= pGRAPH_XIC.curve;
            //            yListXIC.resize(pGRAPH_XIC.yListXIC.size());
            //            memcpy(yListXIC.data(), pGRAPH_XIC.yListXIC.data(), pGRAPH_XIC.yListXIC.size());
            _PARAM_XIC* p= (_PARAM_XIC*)(&pPARAM_XIC);
            yListXIC.swap(p->yListXIC);
        }
        return *this;
    }
};
/**
 * @brief The DistroScaleDraw class
 * 用来设置谱图控件颜色的类
 */
class DistroScaleDraw:public QwtScaleDraw
{
public:
    DistroScaleDraw():QwtScaleDraw(),mColor(QColor(0,0,0)){}
    ~DistroScaleDraw(){}
    void setColor(QColor pColor){
        mColor= pColor;
    }
    virtual QwtText label(double value) const{
        QwtText text(QString::number(value));
        text.setColor(mColor);
        return text;
    }
private:
    QColor mColor;
};

class DistroQwtLegend:public QwtLegend
{
public:
    DistroQwtLegend( QWidget *parent = nullptr ):QwtLegend(parent){
        //        QVBoxLayout* pLayout = new QVBoxLayout(&mWidget);
        //        pLayout->setContentsMargins(0,0,0,0);
        //        pLayout->setSpacing(0);
        //        pLayout->addWidget(contentsWidget());
        //        mWidget.setLayout(pLayout);
        //mDockWidget.setWidget(&mWidget);
        //mDockWidget.show();
        //mWidget.show();
    }
    ~DistroQwtLegend(){}
    //QWidget mWidget;
    //QDockWidget mDockWidget;
    void setColor(QString pColor){
        mColor= pColor;
        contentsWidget()->setStyleSheet(mColor);
    }
private:
    QString mColor= "color:#E4DAD0;";
};

//class TimeScaleDraw: public QwtScaleDraw
//{
//public:
//    TimeScaleDraw(){}
//    ~TimeScaleDraw(){}
//    void setBaseTime(const QTime &base){
//        baseTime= base;
//    }
//    virtual QwtText label( double v ) const{
//        return baseTime.addSecs( static_cast<int>( v ) ).toString();
//    }
//private:
//    QTime baseTime;
//};
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QDateTime &base ):
        baseDateTime( base ){}
    ~TimeScaleDraw(){}
    virtual QwtText label( double v ) const{
        QDateTime upTime = baseDateTime.addSecs( static_cast<int>( v ) );
        return upTime.toString("   MM/dd\nhh:mm:ss");
    }
private:
    QDateTime baseDateTime;
};

/**
 * @brief The sChartWidget class
 * 通用谱图控件类
 */
class sChartWidget:public QWidget
{
    Q_OBJECT
public:
    QWidget* tempWidget;
    enum _STYLE_CHART{_NORMAL_CHART,_TIC_CHART,_TUNING_CHART, _TIC_LEGEND, _MIM_CHART};
    explicit sChartWidget(_STYLE_CHART _STYLE_CHART1, QWidget *parent= nullptr,
                          QWidget* pMenu= nullptr);
    ~sChartWidget();
    QwtPlotCurve* getPlotCurve(){
        if(m_Histogram_plot)
            return m_Histogram_plot->pQwtPlotCurve;
        return nullptr;
    }
    QMap<QString,QString> mPublicProperty;
    QMap<QString,QString> mPlotProp;
    void propertyChanged(QString QString1);
    QString GetClassProp();
    bool isShow(){
        return bShow;
    }
    void setBackgroundColor(QColor pQColor){
        mQColor=pQColor;
        QPalette palette;
        palette.setColor(QPalette::Background, mQColor);//QColor(148,163,220));
        this->setPalette(palette);
    }
    QColor getColor(){
        return mQColor;
    }
    void clearData(){
        mBaseLine->hide();
        std::vector<double> emptyVector(0);
        m_Histogram_plot->populate(emptyVector,emptyVector);
        m_Histogram_plot->replot();
    }
    void setStyleM(TVPlot::MPlotStyle type){
        return m_Histogram_plot->setStyle(type);
    }
    void setStyleM(TVPlot::MPlotStyle type, QString title, double xStart,double xEnd);
//    void setXAxisBasetime(QTime time){
//        if(!time.isValid())
//            return;
//        mTimeScaleDraw.setBaseTime(time);
//        m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom, &mTimeScaleDraw);
//    }
    void setTitle(QString title){
        mPublicProperty["Title"]=title;
        pTitle->setText(title);
    }
    QString getTitle(){
        QString tmpStr=pTitle->getText();
        QStringList tmpList=tmpStr.split("  ");
        if(tmpList.size()<2)
            return tmpStr;
        else
            return tmpList[0]+"\n"+tmpList[1];
    }
    void showLegend(bool show){
        if(m_legend)
            delete m_legend;
        m_legend= nullptr;
        if(show)
            m_legend = new DistroQwtLegend(this);
        m_Histogram_plot->insertLegend(m_legend, QwtPlot::RightLegend);
    }
    void set_PEAK_INFO(QString posStr, QString hwwStr, QString areaStr);

    void setSamples(std::vector<double>const& ValuesX, std::vector<double>& ValuesY, double baseLine=0.0, bool isShowBaseLine= false);
    void setSamples( const double *xData, const double *yData, int size, bool save= true);
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,QVector<QString>& Marker){
        m_Histogram_plot->SetMarker(ValuesX,ValuesY,Marker);
    }
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,std::vector<double>& Marker){
        m_Histogram_plot->SetMarker(ValuesX,ValuesY,Marker);
    }
    void setScale(QwtPlot::Axis axic1, QString ScaleBottom);
    void setZoomBase(bool setBase){
        m_Histogram_plot->setZoomBase(setBase);
    }
    void updateArea(const QString &title, const QVector<QwtIntervalSample>& samples, const QColor &color );

    void updateXIC(QString XicString);
    void updateXIC(QMap<uint32_t, QMap<QString, _PARAM_XIC*>>& ,bool crossThread= true);//从文件中构建XIC结构时需跨线程，绘图放到描数据线程中

    QMap<uint32_t, QMap<QString, _GRAPH_XIC*>>* getXIC(){
        return &mXIC;
    }
    void clearBuffXIC();
    void clearCurveXIC();
    bool drawXIC(std::vector<double>& pX, uint32_t sizeTIC, int timeout= 50);
    void lockXIC(){
        return mutex_XIC.lock();
    }
    bool tryLockXIC(int timeout = 0){
        return mutex_XIC.tryLock(timeout);
    }
    void unLockXIC(){
        return mutex_XIC.unlock();
    }
    QwtPicker *getPicker(){//DistroQwtPicker *getPicker(){
        return mPicker;
    }
    void addLine(double x, Qt::GlobalColor lineColor= Qt::black, QwtPlotMarker::LineStyle style= QwtPlotMarker::VLine,
                 QString label= QString());

    void removeLine(){
        if(mLineMark.size()>0){
            if(mLineMark.last()){
                delete mLineMark.last();
                mLineMark.last()=nullptr;
            }
            mLineMark.removeLast();
        }
        m_Histogram_plot->replot();
    }

    void updateXAxis(QDateTime dateTime){
        m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom,
            new TimeScaleDraw( dateTime) );
    }

    void setTICCurveVisible(bool flag){
        if(m_Histogram_plot != nullptr)
            m_Histogram_plot->setQwtPlotCurveVisible(flag);
    }

    int XIC_CurveCnt();

public slots:
    void clearLine();
public:
    QList<double> getLineMark();

    void copy2BuffXIC(const std::vector<double>& ValuesX, const std::vector<double>& ValuesY){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
        memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
    }

    void setFileHead(QByteArray& fileHead){
        mFileHead= QString::fromUtf8(fileHead);
    }
    void setFileHead(QString& fileHead){
        mFileHead= fileHead;
    }
    uint32_t mCountSave=0;
    QString mCurrentFile;
    void saveToFile(QString pathName, QSizeF sizePic= QSizeF( 810, 540 ));
    void renderTo(QPixmap& pixmap);
    sCoordinateMask* createCoordinateMask(QWidget* topParant=nullptr){
        if(mCoordinateMask)
            delete mCoordinateMask;
        mCoordinateMask= new sCoordinateMask(this, topParant);//
        return mCoordinateMask;
    }
    sCoordinateMask* getCoordinateMask(){
        return mCoordinateMask;
    }

public slots:
    void onTitleClicked();
    void exportPlot();
    void onScalDown();
    void onScalUp();
    void onSelectFile(bool,QString);
    void setPlot();
    void ShowPlot();
    void on_ShowManager(){
        pStackedLayout->setCurrentIndex(2);
    }
    void on_hide(){
        this->hide();
        emit CommandSignal("hide",this ,(void*)0 );
    }
    void onSelected(const QPointF& );
    void showCoordinateMask(){
        if(mCoordinateMask)
            mCoordinateMask->_show();
    }
    void hideCoordinateMask(){
        if(mCoordinateMask)
            mCoordinateMask->_hide();
        clearLine();
    }
    void changeUI_PB_OK_text(QString str){
        if(mCoordinateMask)
            mCoordinateMask->change_UI_PB_OK_Text(str);
    }
private:
    enum _NAME_BUTTON{_PROP_BUTTON = 0, _EXPORT_BUTTON, _XIC_BUTTON, _CLOSE_BUTTON, NUM_BUTTON};
    _STYLE_CHART m_STYLE_CHART= _NORMAL_CHART;
    QStackedLayout* pStackedLayout  = nullptr;//谱图控件与XIC编辑控件布局//同层
    //SignalProcessing mSignalProcessing;       //数据处理类

    QwtPlotGrid* mGrid= nullptr;
    // QwtPickerClickPointMachine* mQwtPickerClickPointMachine=nullptr;
    TVPlot* m_Histogram_plot        = nullptr;//谱图控件
    //TimeScaleDraw mTimeScaleDraw;
    QwtPlotMarker* mBaseLine        = nullptr;
    DistroQwtLegend* m_legend       = nullptr;//谱图控件右标，TIC组件专用
    double mCurrentScal=1000;
    QwtPlotPicker *mPicker=nullptr;//DistroQwtPicker *mPicker=nullptr;

    bool isTitleClicked             = false;  //用来判断Title控件是否点击，显/隐其他按键
    MyWidget::sMyButton* pTitle     = nullptr;//Title控件
    MyWidget::sMyButton* pProp      = nullptr;//属性按键
    MyWidget::sMyButton* pEditXIC   = nullptr;//XIC编辑按键
    MyWidget::sMyButton* pSave      = nullptr;//谱图数据保存按键
    MyWidget::sMyButton* pScalDown  = nullptr;//
    MyWidget::sMyButton* pScalUp    = nullptr;//

    QList<QwtPlotMarker*> mLineMark;//QList<QPair<QwtPlotMarker*, QwtPlotMarker*>> mLineMark;

    QMutex mutex;//current data locker
    QMutex mutex_XIC;
    bool bShow=true;
    QColor mQColor=QColor(148,163,220);
    std::vector<double> mValuesX;//current data for save
    std::vector<double> mValuesY;//current data for save
    QMap<uint32_t, QMap<QString, _PARAM_XIC*>> mTempXIC;//用于暴露外部接收数据
    QMap<uint32_t, QMap<QString, _GRAPH_XIC*>> mXIC;//用于绘图
    sFileWidget mFileWidget;
    QString mFileHead;
    double mCurrentX=0.0;
    QwtPlotIntervalCurve* mQwtPlotIntervalCurve=nullptr;
    sCoordinateMask* mCoordinateMask=nullptr;

    QLineEdit* mLE_PEAK_POS = nullptr;
    QLineEdit* mLE_PEAK_HWW = nullptr;
    QLineEdit* mLE_PEAK_AREA = nullptr;

private://触摸屏
    bool m_bGesture=false;
    bool bMouseM=false;
    QPoint mStartPos;
    int mRectW=0;
    double mCurrentStartX=0,
    mCurrentEndX=0;
    int mCenterX=0;
    int mLastStartX=0;
    int mLastEndX=0;
    qreal mScale=1;
    qreal mAngle=0;

    bool isFirstCenter=true;
    double canvasW=0;
    int tmpStartX=0;
    int tmpEndX=0;

    void pinchTriggered(QPinchGesture *gesture);
    bool gestureEvent(QGestureEvent *event);

protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    void resizeEvent(QResizeEvent *event);
signals:
    //void ChangedXIC(QString);
    void CommandSignal(QString ,sChartWidget* ,void* );
    void selected( const double);
};

#endif // SCHARTWIDGET_H
