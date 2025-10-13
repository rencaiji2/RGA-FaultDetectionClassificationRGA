#ifndef SCHARTWIDGET_H
#define SCHARTWIDGET_H

#include <QGestureEvent>
#include <QMutex>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QLineEdit>
#include <qwt_scale_engine.h>
#include <LibWidget/sMyButton.h>
#include <LibWidget/sFileWidget.h>
#include <LibWidget/sChartWidget/sChartChildTools.h>
#include "sTvplot.h"
#include "sCoordinateMask.h"
#include <cPublicStruct.h>
#include <qwt_date_scale_draw.h>

#define SMS_CHANNEL_COUNT 64

/**
 * @brief The sChartWidget class
 * 通用谱图控件类
 */
class sChartWidget:public QWidget
{
    Q_OBJECT
public:
    QWidget* tempWidget;
    enum _STYLE_PLOT: quint32 {_NORMAL_PLOT, _ORIGINAL_PLOT, _PROCESS_PLOT, _STICK_PLOT, _MASS_BAR_PLOT };
    enum _STYLE_CHART{_NORMAL_CHART,_TIC_CHART,_TUNING_CHART, _TIC_LEGEND, _MIM_CHART};
    enum _STYLE_SCALE_LEFT{CARTESIAN_SCALE_LEFT, LOGARITHMIC_SCALE_LEFT};
    explicit sChartWidget(_STYLE_CHART _STYLE_CHART1, QWidget *parent= nullptr,
                          QWidget* pMenu= nullptr);
    ~sChartWidget();
    void setPlotSize(int size){
        if(m_Histogram_plot)
            m_Histogram_plot->setPlotSize(size);
    }
    QList<QwtPlotCurve*>* getPlotCurves(){
        if(m_Histogram_plot)
            return &(m_Histogram_plot->mQwtPlotCurves);
        return nullptr;
    }
    void setRenderHint(QwtPlotItem::RenderHint hint, bool on = true){
        if(!m_Histogram_plot)
            return;
        int lengthList= m_Histogram_plot->getPlotSize();
        for(int i= 0; i< lengthList; ++i){
            QwtPlotCurve* pQwtPlotCurve= m_Histogram_plot->mQwtPlotCurves[i];
            if(pQwtPlotCurve)
                pQwtPlotCurve->setRenderHint(hint, on );
        }
    }
    void setPen(const QColor &color, qreal width = 0.0, int index=-1){
        if(!m_Histogram_plot)
            return;
        int lengthList= m_Histogram_plot->getPlotSize();
        if(index< 0)
            for(int i= 0; i< lengthList; ++i){
                QwtPlotCurve* pQwtPlotCurve= m_Histogram_plot->mQwtPlotCurves[i];
                if(pQwtPlotCurve)// Qt::black
                    pQwtPlotCurve->setPen(color, width);
            }
        else if(index< lengthList){
            QwtPlotCurve* pQwtPlotCurve= m_Histogram_plot->mQwtPlotCurves[index];
            if(pQwtPlotCurve)// Qt::black
                pQwtPlotCurve->setPen(color, width);
        }
        //m_Histogram_plot->replot();
    }

int addToolButton(MyWidget::sMyButton* pButton, int index=-1);
int findToolButton(MyWidget::sMyButton* pButton);

    QMap<QString,QString> mPublicProperty;
    QMap<QString,QString> mPlotProp;
    QPair<double,double> mScaleLeftProp, mScaleBottomProp;
    //void propertyChanged(QString QString1);
    QString GetClassProp();
    bool isShow(){
        return bShow;
    }
    void setWidthXIC(qreal width){
        mWidth= width;
    }
    void setBackgroundColor(QColor pQColor){
        mQColor=pQColor;
        QPalette palette;
        palette.setColor(QPalette::Background, mQColor);//QColor(148,163,220));
        this->setPalette(palette);
    }
    QColor getBackgroundColor(){
        return mQColor;
    }
    void clearData(){
        mBaseLine->hide();
        std::vector<double> emptyVector(0);
        int lengthList= m_Histogram_plot->getPlotSize();
        for(int i= 0; i< lengthList; ++i){
            m_Histogram_plot->populate(emptyVector,emptyVector, i);
        }
        //m_Histogram_plot->populate(emptyVector,emptyVector);
        //m_Histogram_plot->replot();
    }
    void setStyleM(TVPlot::MPlotStyle type){
        return m_Histogram_plot->setStyle(type);
    }
    virtual void setStyleM(TVPlot::MPlotStyle type, QString title, double xStart,double xEnd);
//    void setDateAxicX(){
//        QwtDateScaleDraw *timeScale = new QwtDateScaleDraw(Qt::LocalTime);
//        QString scaleFormat("hh:mm:ss\nyy-MM-dd");

//        timeScale->setDateFormat(QwtDate::Second, scaleFormat);

//        m_Histogram_plot->setAxisScaleDraw(QwtPlot::xBottom, timeScale);
//    }
    void setSecondAxicX(){
        QwtScaleDraw* secondScale= new QwtScaleDraw();
        m_Histogram_plot->setAxisScaleDraw(QwtPlot::xBottom, secondScale);
    }

    void setBasetimeAxisX(QDateTime time){
        if(!time.isValid())
            return;
        //mTimeScaleDraw.setBaseTime(time);
        //m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom, &mTimeScaleDraw);
//        if(pTimeScaleDraw)
//            delete pTimeScaleDraw;
//        pTimeScaleDraw= new TimeScaleDraw(time);
//        m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom, pTimeScaleDraw);
        m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw(time));
    }
    void setScientificNotationAxisY(bool enable= true){
        m_Histogram_plot->setAxisScaleDraw( QwtPlot::yLeft, new ScientificNotationScaleDraw(enable));
    }
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
    void setAxisTitle( int axisId, const QwtText & pQwtText){
        if(m_Histogram_plot)
            m_Histogram_plot->setAxisTitle(axisId, pQwtText);
    }
    QwtText axisTitle( int axisId ) const{
        if(m_Histogram_plot)
            return m_Histogram_plot->axisTitle(axisId);
        return QwtText("");
    }
    void setScaleStyle(_STYLE_SCALE_LEFT STYLE_SCALE_LEFT){// 设置纵坐标为笛卡尔坐标
        if(!m_Histogram_plot)
            return;
        switch (STYLE_SCALE_LEFT) {
        case CARTESIAN_SCALE_LEFT:
            //m_Histogram_plot->setAxisScaleDraw(QwtPlot::yLeft, new DistroScaleDraw());
            m_Histogram_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
            break;
        case LOGARITHMIC_SCALE_LEFT:
            //m_Histogram_plot->setAxisScaleDraw(QwtPlot::yLeft, new DistroScaleDraw(true));
            m_Histogram_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
            break;
        default:return;
        }

    //m_Histogram_plot->setAxisTitle(QwtPlot::yLeft, tr("强度"));
    m_Histogram_plot->replot();
    }
    void showLegend(bool show){
        if(m_legend)
            delete m_legend;
        m_legend= nullptr;
        if(show)
            m_legend = new DistroQwtLegend(this);
        m_Histogram_plot->insertLegend(m_legend, QwtPlot::RightLegend);
    }
    //void set_PEAK_INFO(QString posStr, QString hwwStr, QString areaStr);

    void setSample(std::vector<double>const& ValuesX,
                   std::vector<double>& ValuesY,
                   double baseLine=0.0, bool isShowBaseLine= false, bool save= true);
    void setSample(QVector<double>const& ValuesX,
                   QVector<double>& ValuesY,
                                 double baseLine, bool isShowBaseLine,
                                  bool save);
    void clearPlot();
    int addSample(std::vector<double>const& ValuesX,
                   std::vector<double>& ValuesY,
                   double baseLine=0.0, bool isShowBaseLine= false, bool save= true);
    void removeSample(int indexPlot);
    void setSamples(QList<std::vector<double>>const& ValuesX,
                    QList<std::vector<double>>& ValuesY,
                    double baseLine=0.0, bool isShowBaseLine= false, bool save= true);
    void setSamples(QList<QVector<double>>const& ValuesX,
                    QList<QVector<double>>& ValuesY,
                    double baseLine=0.0, bool isShowBaseLine= false, bool save= true);
    //void setSamples( const double *xData, const double *yData, int size, int index, bool save= true);
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,QVector<QString>& Marker){
        m_Histogram_plot->SetMarker(ValuesX,ValuesY,Marker);
    }
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,std::vector<double>& Marker){
        m_Histogram_plot->SetMarker(ValuesX,ValuesY,Marker);
    }
    void setScale(QwtPlot::Axis axic1, QString ScaleBottom);
    void getScale(QwtPlot::Axis axic1, double& min, double& max){
        QwtInterval ytemp = m_Histogram_plot->axisScaleDiv(axic1).interval();
            min = ytemp.minValue();
            max = ytemp.maxValue();
    }
    void setZoomBase(bool setBase){
        m_Histogram_plot->setZoomBase(setBase);
    }
    void updateArea(const QString &title, const QVector<QwtIntervalSample>& samples, const QColor &color );
    /**
     * @brief updateXIC 根据XicString重新构造mXIC,此时mXIC中的yListXIC均为空
     * @param XicString
     */
    void updateXIC(QString XicString);
    /**
     * @brief updateXIC 根据xics重新构造mXIC,此时将交换mXIC->yListXIC和xics->yListXIC
     * @param xics
     * @param crossThread 若为true，则不构造mXIC->curve,将在drawXIC函数中进行构造
     */
    void updateXIC(QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                   bool crossThread= true);//从文件中构建XIC结构时需跨线程，绘图放到描数据线程中
    /**
     * @brief updateXIC 根据XicString重新构造mXIC,此时mXIC中的yListXIC均为空
     * @param XicString
     */
    void updateOther(QString XicString);
    /**
     * @brief updateXIC 根据xics重新构造mXIC,此时将交换mXIC->yListXIC和xics->yListXIC
     * @param xics
     * @param crossThread 若为true，则不构造mXIC->curve,将在drawXIC函数中进行构造
     */
    void updateOther(QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                   bool crossThread= true);//从文件中构建XIC结构时需跨线程，绘图放到描数据线程中
    QMap<uint32_t, QMAP_GRAPH_XIC>* getXIC(){
        return &mXIC;
    }
    QString getStrXIC(){
        double massRange=0;
        QStringList tmpList;//, list_PARAM_XIC;
        for(auto& event: mXIC.keys()){
            for(auto& mass: mXIC[event].keys()){
                _PARAM_XIC* p_PARAM_XIC= mXIC[event][mass];
                if(p_PARAM_XIC){
                    massRange=p_PARAM_XIC->massRange;
                    QStringList tmpList1;
                    tmpList1<< QString::number(event)
                            <<mass
                           <<QString::number(p_PARAM_XIC->color,16)
                          <<QString::number(p_PARAM_XIC->Gain)
                         <<QString::number(p_PARAM_XIC->Offset);
                    tmpList<< tmpList1.join(':');
                }
            }
        }
        return QString::number(massRange)+ '/'+ tmpList.join('@');
    }
    void clearBuffXIC();
    //void clearCurveXIC();
    /**
     * @brief drawXIC 使用成员mXIC中的yListXIC进行绘图
     * @param pX
     * @param sizeTIC
     * @param timeout
     * @return
     */
//    bool drawXIC(std::vector<double>& pX,
//                 uint32_t sizeTIC,
//                 QString strXIC={},
//                 int timeout= 50);
    /**
     * @brief drawXIC 使用参数xics中的yListXIC进行绘图，若成员XIC中无xics中所对应的条目，则跳过
     * @param pX
     * @param xics
     * @param sizeTIC
     * @param timeout
     * @return
     */
    bool drawXIC(std::vector<double>& pX,
                 QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                 uint32_t sizeTIC,
                 int timeout= 50);
    /**
     * @brief drawXIC 使用参数xics中的yListXIC进行绘图，若成员XIC中无xics中所对应的条目，则跳过
     * @param pX
     * @param xics
     * @param sizeTIC
     * @param timeout
     * @return
     */
    bool drawOther(std::vector<double>& pX,
                 QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                 uint32_t sizeTIC,
                 int timeout= 50);
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
    void addLine(double x,
                 Qt::GlobalColor lineColor= Qt::black,
                 QwtPlotMarker::LineStyle style= QwtPlotMarker::VLine,
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

//    void updateXAxis(QDateTime dateTime){
//        m_Histogram_plot->setAxisScaleDraw( QwtPlot::xBottom,
//            new TimeScaleDraw( dateTime) );
//    }

//    void setTICCurveVisible(bool flag){
//        if(m_Histogram_plot != nullptr)
//            m_Histogram_plot->setQwtPlotCurveVisible(flag);
//    }

//    int XIC_CurveCnt();

public slots:
    void clearLine();
    void onAddXIC(uint32_t event,
                  QString mass,
                  double massRange,
                  uint color,
                  double Gain,
                  double Offset,
                  bool crossThread= true);
    void onRemoveXIC(uint32_t event, QString mass);
    void onAddOther(uint32_t event,
                  QString mass,
                  double massRange,
                  uint color,
                  double Gain,
                  double Offset,
                  bool crossThread= true);
    void onRemoveOther(uint32_t event, QString mass);
    void onShowPlotCurve(bool show){
        if(!m_Histogram_plot)
            return;
        QwtPlotCurve* pQwtPlotCurve= nullptr;
        int lengthList= m_Histogram_plot->getPlotSize();
        if(show){
            for(int i= 0; i< lengthList; ++i){
                pQwtPlotCurve= m_Histogram_plot->mQwtPlotCurves[i];
                if(pQwtPlotCurve)
                    pQwtPlotCurve->show();
            }
        }else{
            for(int i= 0; i< lengthList; ++i){
                pQwtPlotCurve= m_Histogram_plot->mQwtPlotCurves[i];
                if(pQwtPlotCurve)
                    pQwtPlotCurve->hide();
            }
        }
    }
public:
    //bool isTitleClicked             = false;  //用来判断Title控件是否点击，显/隐其他按键
    MyWidget::sMyButton* pTitle     = nullptr;//Title控件
    MyWidget::sMyButton* pProp      = nullptr;//属性按键
    MyWidget::sMyButton* pEditXIC   = nullptr;//XIC编辑按键
    MyWidget::sMyButton* pSave      = nullptr;//谱图数据保存按键
    MyWidget::sMyButton* pScalDown  = nullptr;//
    MyWidget::sMyButton* pScalUp    = nullptr;//

    QList<double> getLineMark();

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
    //void ShowPlot();
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
    void onResetScale(){
        if(!m_Histogram_plot)
        return;
        {
            //                if(mPlotProp["RangeX"] == "auto" ||
            //                        mPlotProp["RangeX"].isEmpty())
            //                    setScale(QwtPlot::xBottom, mPublicProperty["RangeX"]);
            //                else
            //                    setScale(QwtPlot::xBottom, mPlotProp["RangeX"]);
            //                setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
            if(mScaleLeftProp.first== mScaleLeftProp.second){
                m_Histogram_plot->setAxisAutoScale(QwtPlot::yLeft, true);
            }else{
                m_Histogram_plot->setAxisScale(QwtPlot::yLeft, mScaleLeftProp.first,
                                               mScaleLeftProp.second);
            }
            if(mScaleBottomProp.first== mScaleBottomProp.second){
                m_Histogram_plot->setAxisAutoScale(QwtPlot::xBottom, true);
            }else{
                m_Histogram_plot->setAxisScale(QwtPlot::xBottom, mScaleBottomProp.first,
                                               mScaleBottomProp.second);

            }
            m_Histogram_plot->updateMarker();
        }
    }
//    void changeUI_PB_OK_text(QString str){
//        if(mCoordinateMask)
//            mCoordinateMask->change_UI_PB_OK_Text(str);
//    }
protected:
    TVPlot* m_Histogram_plot        = nullptr;//谱图控件
    std::vector<double> mValuesX;//current data for save
    std::vector<double> mValuesY;//current data for save
    double mCurrentX=0.0;

private:
    enum _NAME_BUTTON{_PROP_BUTTON = 0, _EXPORT_BUTTON, _XIC_BUTTON, _CLOSE_BUTTON, NUM_BUTTON};
    _STYLE_CHART m_STYLE_CHART= _NORMAL_CHART;
    QStackedLayout* pStackedLayout  = nullptr;//谱图控件与XIC编辑控件布局//同层
    //SignalProcessing mSignalProcessing;       //数据处理类

    QwtPlotGrid* mGrid= nullptr;
    // QwtPickerClickPointMachine* mQwtPickerClickPointMachine=nullptr;

    //TimeScaleDraw mTimeScaleDraw;
    //TimeScaleDraw* pTimeScaleDraw=nullptr;
    QwtPlotMarker* mBaseLine=nullptr;
    DistroQwtLegend* m_legend             = nullptr;//谱图控件右标，TIC组件专用
    double mCurrentScal=1000;
    QwtPlotPicker *mPicker=nullptr;//DistroQwtPicker *mPicker=nullptr;
    qreal mWidth= 1;



    QList<QwtPlotMarker*> mLineMark;//QList<QPair<QwtPlotMarker*, QwtPlotMarker*>> mLineMark;

    QMutex mutex;//current data locker
    QMutex mutex_XIC;
    bool bShow=true;
    QColor mQColor=QColor(148,163,220);
    QMap<uint32_t, QMAP_GRAPH_XIC> mXIC;//用于绘图
    QMap<uint32_t, QMAP_GRAPH_XIC> mOther;//用于绘图
    sFileWidget mFileWidget;
    QString mFileHead;

    QwtPlotIntervalCurve* mQwtPlotIntervalCurve=nullptr;
    sCoordinateMask* mCoordinateMask=nullptr;

protected:
    QPoint mStartPos;
    void resizeEvent(QResizeEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *ev);

signals:
    //void ChangedXIC(QString);
    void sResetScale();
    void CommandSignal(QString ,sChartWidget* ,void* );
    void selected( const double);
};

#endif // SCHARTWIDGET_H
