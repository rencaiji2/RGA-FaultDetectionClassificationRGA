#include "sChartWidget.h"
#include <QGridLayout>
#include <QTimerEvent>
#include <qwt_plot_renderer.h>
//#include <sobject.h>
//#include <sproject.h>
#include <QMenuBar>
#include <qwt_plot_grid.h>
#include <QSettings>
#include <QFileDialog>
#include <QGestureEvent>
#include <qwt_picker_machine.h>
#include <LibWidget/sInputdialog.h>
#include <QCoreApplication>

using namespace MyWidget;
sChartWidget::sChartWidget(_STYLE_CHART _STYLE_CHART1, QWidget *parent, QWidget *pMenu):
    QWidget(parent),
    m_STYLE_CHART(_STYLE_CHART1)
{
    setAutoFillBackground(true);
    mPublicProperty.insert("Title","");
    mPublicProperty.insert("RangeX","auto");
    mPublicProperty.insert("RangeY","auto");

mScaleLeftProp.first= 0;
mScaleLeftProp.second= 0;
mScaleBottomProp.first= 0;
mScaleLeftProp.second= 0;

    mPlotProp.insert("RangeX","auto");
    mPlotProp.insert("RangeY","auto");
connect(this, SIGNAL(sResetScale()),this,SLOT(onResetScale()),Qt::QueuedConnection);
//connect( zoomer, SIGNAL( zoomed( QRectF ) ),
//             this, SLOT( select_axis_zoomer( QRectF ) ) );
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0,0,0,0);
    pLayout->setSpacing(0);
    QHBoxLayout* pTitleLayout = new QHBoxLayout(this);
    pTitleLayout->setContentsMargins(0,0,0,2);
    if(pMenu)
        pTitleLayout->addWidget(pMenu);
    else
        pTitleLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding,QSizePolicy::Expanding));
    QHBoxLayout* pTitleChild = new QHBoxLayout(this);
    pTitleChild->setContentsMargins(0,0,0,0);
    pTitleChild->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding,QSizePolicy::Expanding));
    pStackedLayout = new QStackedLayout(this);
    pStackedLayout->setContentsMargins(0,0,0,0);
    QVBoxLayout* pCentralLayout = new QVBoxLayout(this);
    pCentralLayout->setContentsMargins(0,0,0,0);

    //    if(m_STYLE_CHART == _MIM_CHART){
    //        QVBoxLayout* pVBLO = new QVBoxLayout(this);
    //        QFont font;
    //        font.setPointSize(10);
    //        font.setFamily("Microsoft YaHei");
    //        QHBoxLayout* pHBLO1 = new QHBoxLayout(this);
    //        QLabel* lable1 = new QLabel("峰位:");
    //        mLE_PEAK_POS = new QLineEdit();
    //        lable1->setFont(font);
    //        mLE_PEAK_POS->setFont(font);
    //        mLE_PEAK_POS->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //        pHBLO1->setContentsMargins(0,0,0,0);
    //        pHBLO1->setSpacing(0);
    //        pHBLO1->addStretch(1);
    //        pHBLO1->addWidget(lable1, 1);
    //        pHBLO1->addWidget(mLE_PEAK_POS, 2);
    //        pHBLO1->addStretch(1);

    //        QHBoxLayout* pHBLO2 = new QHBoxLayout(this);
    //        QLabel* lable2 = new QLabel("半高峰宽:");
    //        mLE_PEAK_HWW = new QLineEdit();
    //        lable2->setFont(font);
    //        mLE_PEAK_HWW->setFont(font);
    //        mLE_PEAK_HWW->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //        pHBLO2->setContentsMargins(0,0,0,0);
    //        pHBLO2->setSpacing(0);
    //        pHBLO2->addStretch(1);
    //        pHBLO2->addWidget(lable2, 1);
    //        pHBLO2->addWidget(mLE_PEAK_HWW, 2);
    //        pHBLO2->addStretch(1);

    //        QHBoxLayout* pHBLO3 = new QHBoxLayout(this);
    //        QLabel* lable3 = new QLabel("峰面积:");
    //        mLE_PEAK_AREA = new QLineEdit();
    //        lable3->setFont(font);
    //        mLE_PEAK_AREA->setFont(font);
    //        mLE_PEAK_AREA->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //        pHBLO3->setContentsMargins(0,0,0,0);
    //        pHBLO3->setSpacing(0);
    //        pHBLO3->addStretch(1);
    //        pHBLO3->addWidget(lable3, 1);
    //        pHBLO3->addWidget(mLE_PEAK_AREA, 2);
    //        pHBLO3->addStretch(1);

    //        pVBLO->addLayout(pHBLO1);
    //        pVBLO->addLayout(pHBLO2);
    //        pVBLO->addLayout(pHBLO3);

    //        pCentralLayout->addLayout(pStackedLayout, 5);
    //        pCentralLayout->addLayout(pVBLO, 1);
    //    }else
    pCentralLayout->addLayout(pStackedLayout);



    pLayout->addLayout(pTitleLayout,0);
    pLayout->addLayout(pCentralLayout,1);//pLayout->addLayout(pStackedLayout,1);
    pLayout->setStretch(1,1);
    setLayout(pLayout);

    //    mChartXIC = new sChartXIC();                        //构造XIC编辑控件
    //    connect(mChartXIC, SIGNAL(ChangedManager(QString, bool )),
    //            this,SLOT(on_ChangedManager(QString, bool )));

    m_Histogram_plot = new TVPlot(this);                //构造谱图控件
    m_Histogram_plot->setProperty("__uid", "ins");
    m_Histogram_plot->setAxisScaleDraw(QwtPlot::yLeft, new DistroScaleDraw());
    m_Histogram_plot->setAxisAutoScale(QwtPlot::yLeft, true);
    m_Histogram_plot->setAxisScaleDraw(QwtPlot::xBottom, new DistroScaleDraw());
    m_Histogram_plot->setAxisAutoScale(QwtPlot::xBottom, true);
    //    m_Histogram_plot->setAxisTitle(QwtPlot::yLeft, tr("气压/mbar"));
    //    m_Histogram_plot->setAxisTitle(QwtPlot::xBottom, tr("质量/u"));
    //    m_Histogram_plot->setAxisFont(QwtPlot::yLeft, QFont(tr("Times New Roman")));
    //    m_Histogram_plot->setAxisFont(QwtPlot::xBottom, QFont(tr("Times New Roman")));

    m_Histogram_plot->setCanvasBackground(QBrush(QColor(255,255,255)));
    m_Histogram_plot->canvas()->installEventFilter(this);

    m_Histogram_plot->setMinimumSize(0,0);

    mBaseLine= new QwtPlotMarker();
    mBaseLine->setLineStyle(QwtPlotMarker::HLine);
    mBaseLine->setLinePen(QPen(Qt::black, 1, Qt::SolidLine));
    mBaseLine->attach(m_Histogram_plot);
    mBaseLine->hide();

    //mQwtPickerClickPointMachine= new QwtPickerClickPointMachine();
    mPicker = new QwtPlotPicker(m_Histogram_plot->canvas());//mPicker = new  DistroQwtPicker(m_Histogram_plot->canvas());
    mPicker->setStateMachine(new QwtPickerClickPointMachine);
    //mPicker->setStateMachine(mQwtPickerClickPointMachine );
    connect(mPicker,SIGNAL(selected(const QPointF &)),this,SLOT(onSelected(const QPointF &)));

    //QwtPlotGrid*
    mGrid= new QwtPlotGrid();                //网格
    mGrid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));  //大格子
    mGrid->attach(m_Histogram_plot);
    pStackedLayout->addWidget(m_Histogram_plot);

    QFont mfont;
    mfont.setPointSize(18);
    mfont.setFamily("Microsoft YaHei");
    if(m_STYLE_CHART==_TUNING_CHART){
        //pTitle= new sMyButton(this);
    }else{
        pTitle= new sMyButton(QPixmap(":/Chart/picture/title.png"),mfont,this);

        pTitle->setMinimumWidth(150);
        connect(pTitle,SIGNAL(ButtonClicked()),this,SLOT(onTitleClicked()));
        pTitle->setLayout(pTitleChild);
        if(pMenu){
            //pTitleLayout->addWidget(pMenu);
            QVBoxLayout* tempLayout = new QVBoxLayout(this);
            pTitleLayout->addItem(tempLayout);
            tempLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,QSizePolicy::Expanding));
            tempLayout->addWidget(pTitle);
        }else
            pTitleLayout->addWidget(pTitle);
        //pProp = new sMyButton(QPixmap(":/Chart/picture/Widget_menu.png"),mfont,pTitle);
        //connect(pProp,SIGNAL(ButtonClicked()),this,SLOT(setPlot()));
        pSave = new sMyButton(QPixmap(":/Chart/picture/Widget_save.png"),mfont,pTitle);
        connect(pSave,SIGNAL(ButtonClicked()),this,SLOT(exportPlot()));
        //pScalDown = new sMyButton(QPixmap(":/Chart/picture/Widget_zoom_in.png"),mfont,pTitle);
        //connect(pScalDown,SIGNAL(ButtonClicked()),this,SLOT(onScalDown()));
        //pScalUp= new sMyButton(QPixmap(":/Chart/picture/Widget_zoom_out.png"),mfont,pTitle);
        //connect(pScalUp,SIGNAL(ButtonClicked()),this,SLOT(onScalUp()));

        //    pScreenShot = new sMyButton(QPixmap(":/Chart/picture/Widget_screenShot.png"),mfont,pTitle);
        //    pScreenShot->hide();
        //    connect(pScreenShot,SIGNAL(ButtonClicked()),this,SLOT(exportPlot()));

        pStackedLayout->addWidget(&mFileWidget);
        connect(&mFileWidget,SIGNAL(selectFile(bool,QString)),this,SLOT(onSelectFile(bool,QString)));

        
        if(m_STYLE_CHART==_TIC_LEGEND){//如果构造为TIC组件，则增加XIC编辑按键、右标
            showLegend(true);
#if 0
            QHBoxLayout* pLayout = new QHBoxLayout(m_Histogram_plot->canvas());
            //tempWidget=new QWidget(this);
            //tempWidget->setMinimumWidth(50);
            pLayout->addStretch();
            //pLayout->insertSpacerItem(0, new QSpacerItem(100, 100, QSizePolicy::Minimum, QSizePolicy::Minimum));
            pLayout->addSpacerItem(new QSpacerItem(100, 100, QSizePolicy::Minimum, QSizePolicy::Minimum));
            //pLayout->addWidget(tempWidget);pLayout->addWidget(m_legend->contentsWidget());
            m_Histogram_plot->canvas()->setLayout(pLayout);
            m_Histogram_plot->setContentsMargins(0,0,9,0);

            //mTICList= new sTICList(this);
            //mTICList->hide();
            //pCentralLayout->addWidget(mTICList);
            //            connect(mTICList, SIGNAL(ChangedManager(QString, bool )),
            //                    this,SLOT(on_ChangedManager(QString, bool )));
            //            connect(mTICList, SIGNAL(HideXIC(uint32_t , QString , bool )),
            //                    this,SLOT(onHideXIC(uint32_t , QString , bool )));
#endif
        }
        pTitleChild->addWidget(pProp);
        pTitleChild->addWidget(pSave);
        pTitleChild->addWidget(pScalDown);
        pTitleChild->addWidget(pScalUp);
        //    pTitleChild->addWidget(pScreenShot);
        setProperty("isTitleClicked", true);
    }
    grabGesture(Qt::PinchGesture);
}

sChartWidget::~sChartWidget()
{
    lockXIC();
//    QMutableMapIterator< uint32_t, QMAP_GRAPH_XIC > tempIter(mXIC);
//    while (tempIter.hasNext()) {
//        QMutableMapIterator<sMapString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//        while (tempIter1.hasNext())
//            delete tempIter1.next().value();
//    }
    for(auto& key: mXIC.keys()){
        for(auto& key1: mXIC[key].keys()){
            if(mXIC[key][key1])
                delete mXIC[key][key1];
        }
    }
    unLockXIC();
    if(mGrid)
        delete mGrid;
    mGrid= nullptr;

    if(mBaseLine != nullptr)//必须要在m_Histogram_plot 前删除
        delete mBaseLine;
    mBaseLine = nullptr;

    if(m_Histogram_plot!=nullptr)
        delete m_Histogram_plot;
    m_Histogram_plot=nullptr;
    //    if(mQwtPickerClickPointMachine)
    //        delete mQwtPickerClickPointMachine;
    //    mQwtPickerClickPointMachine= nullptr;
}

int sChartWidget::addToolButton(MyWidget::sMyButton* pButton, int index)
{
    QHBoxLayout* playout= static_cast<QHBoxLayout*>(pTitle->layout());
    if((!playout)||(!pButton))
        return -1;
    if(index== -1)
        playout->addWidget(pButton);
    else{
        playout->insertWidget(index, pButton);
    }
    return playout->indexOf(pButton);
}

int sChartWidget::findToolButton(MyWidget::sMyButton* pButton)
{
    QHBoxLayout* playout= static_cast<QHBoxLayout*>(pTitle->layout());
    if((!playout)||(!pButton))
        return -1;
    return playout->indexOf(pButton);
}

void sChartWidget::onTitleClicked()
{
    if(property("isTitleClicked").toBool()){
        for (int cc = pTitle->layout()->count()-1; cc >= 0; --cc){
            QLayoutItem *it = pTitle->layout()->itemAt(cc);
            MyWidget::sMyButton *pButton = qobject_cast<MyWidget::sMyButton *>(it->widget());
            if (pButton)
                pButton->hide();
        }
        setProperty("isTitleClicked", false);
    }else{
        for (int cc = pTitle->layout()->count()-1; cc >= 0; --cc){
            QLayoutItem *it = pTitle->layout()->itemAt(cc);
            MyWidget::sMyButton *pButton = qobject_cast<MyWidget::sMyButton *>(it->widget());
            if (pButton)
                pButton->show();
        }
        setProperty("isTitleClicked", true);
    }
}

void sChartWidget::saveToFile(QString pathName, QSizeF sizePic)
{
    //QwtPlotRenderer renderer;
    //renderer.renderDocument(m_Histogram_plot, pathName+".jpg", sizePic, 600);
    QFile file(pathName+".csv");
    if(file.open(QFile::WriteOnly|QIODevice::Text)){
        QStringList tmpListKey;
        QList<std::vector<double>*> tmpListValue;
        mutex_XIC.lock();
        for(auto& key: mXIC.keys()){
            for(auto& key1: mXIC[key].keys()){
                _GRAPH_XIC* pGRAPH_XIC= mXIC[key][key1];
                tmpListKey<< QString("Mass%1").arg(key1);// + " MassRange=" +QString::number(pGRAPH_XIC->massRange);
                tmpListValue<< &(pGRAPH_XIC->yListXIC);
            }
        }
//        QMutableMapIterator< uint32_t, QMAP_GRAPH_XIC > tempIter(mXIC);
//        while (tempIter.hasNext()) {
//            QMutableMapIterator<sMapString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//            while (tempIter1.hasNext()) {
//                _GRAPH_XIC* pGRAPH_XIC= tempIter1.next().value();
//                tmpListKey<< QString("Mass%1").arg(tempIter1.key());// + " MassRange=" +QString::number(pGRAPH_XIC->massRange);
//                tmpListValue<< &(pGRAPH_XIC->yListXIC);
//            }
//        }
        mutex_XIC.unlock();

        int sizeListValue= tmpListValue.size();
        mutex.lock();
        QTextStream stream(&file);
        uint32_t buffSize=mValuesX.size();
        if((buffSize>0)&&(buffSize==mValuesY.size())){
            if(mFileHead.size() > 0)
                stream << mFileHead<< "\n";
            stream << "_X,_Y,"<< tmpListKey.join(",")<< "\n";
            for(uint32_t i=0; i<buffSize; ++i){
                stream << QString::number(mValuesX.at(i)) << ","<< QString::number(mValuesY.at(i));// << "\n";
                for(uint32_t j=0; j< sizeListValue; ++j){
                    stream << ","<<  QString::number(tmpListValue[j]->at(i));
                }
                stream << "\n";
            }
        }
        mutex.unlock();
        file.close();
    }
}

void sChartWidget::renderTo(QPixmap& pixmap)
{
    QwtPlotRenderer renderer;
    renderer.renderTo(m_Histogram_plot, pixmap);
    return;
}

void sChartWidget::exportPlot()
{
    QString pathName= QCoreApplication::applicationDirPath()+ "/data";//QDir::currentPath()


    if(mFileWidget.getFrameCount()== 1)
        saveToFile(pathName+"/tmpfile");
    pStackedLayout->setCurrentIndex(1);

    //QDateTime dateTime = QDateTime::currentDateTime();
    //QLocale locale = QLocale::English;
    //QString strFormat = dateTime.toString("H.m.s-a-yyyy.MMMM.dd");
    mFileWidget.creatNewFile(pathName, QDateTime::currentDateTime().toString("H.m.s-a-yyyy.MMMM.dd"));
}

void sChartWidget::onScalDown()
{
    if(mCurrentScal/2< 0){//if(mCurrentScal/2< 0.01){
        mPlotProp["RangeY"]="auto";
        setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
    }else{
        mCurrentScal/= 2;
        mPlotProp["RangeY"]="0,"+QString::number(mCurrentScal);
        setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
        m_Histogram_plot->setZoomBase(true);
    }
}

void sChartWidget::onScalUp()
{
    //    if(mCurrentScal*2> 1000000) //上限限制
    //        return;
    mCurrentScal*=2;
    mPlotProp["RangeY"]="0,"+QString::number(mCurrentScal);
    setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
    m_Histogram_plot->setZoomBase(true);
}

void sChartWidget::onSelectFile(bool selected,QString fileName)
{
    pStackedLayout->setCurrentIndex(0);
    QString pathName= QCoreApplication::applicationDirPath()+ "/data/";//QDir::currentPath()
    if(selected){
        if(mFileWidget.getFrameCount()== 1){
            QFile::rename(pathName+ "tmpfile.jpg",pathName+fileName+".jpg");
            QFile::rename(pathName+ "tmpfile.csv",pathName+fileName+".csv");
            mCountSave=0;
        }else{
            QFile::remove(pathName+ "tmpfile.jpg");
            QFile::remove(pathName+ "tmpfile.csv");
            mCurrentFile= pathName+fileName;
            mCountSave= mFileWidget.getFrameCount();
        }
    }else{
        if(mFileWidget.getFrameCount()== 1){
            QFile::remove(pathName+ "tmpfile.jpg");
            QFile::remove(pathName+ "tmpfile.csv");
        }
        mCountSave=0;
    }
}

void sChartWidget::setPlot()
{
    QStringList lstName, lstValue, lstButton;
    lstName << "RangeX"<<"RangeY";
    lstValue << mPlotProp["RangeX"]
            << mPlotProp["RangeY"];
    lstButton << "OK" << "Cancel";
    SInputDialog dlgSet(lstName, lstValue, lstButton, this);
    dlgSet.setWindowTitle("Setting plot");
    if(dlgSet.execEx() != "OK")
        return;
    QString strRange = dlgSet.parameter("RangeX");
    if(!strRange.isEmpty()){
        mPlotProp["RangeX"]=strRange;
        setScale(QwtPlot::xBottom, mPlotProp["RangeX"]);
    }
    strRange = dlgSet.parameter("RangeY");
    if(!strRange.isEmpty()){
        mPlotProp["RangeY"]=strRange;
        if(strRange.compare("auto", Qt::CaseInsensitive) == 0){
            setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
        }else{
            //                    QStringList lstRange = strRange.split(',');
            //                    if(lstRange.size() >= 2)
            //                        m_Histogram_plot->setAxisScale(QwtPlot::yLeft, lstRange[0].toDouble(),
            //                                lstRange[1].toDouble());
            setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
            m_Histogram_plot->setZoomBase(true);
        }
    }
}

void sChartWidget::setStyleM(TVPlot::MPlotStyle type, QString title, double xStart,double xEnd)
{
    m_Histogram_plot->setStyle(type);
    if(m_STYLE_CHART!=_TUNING_CHART){
        if(mPublicProperty["Title"] != title)
            setTitle(title);
    }
//    QString strRange= QString::number(xStart)+","+QString::number(xEnd);
//    if(mPublicProperty["RangeX"]!= strRange){
//        mPublicProperty["RangeX"]=strRange;
//        if(mPlotProp["RangeX"] == "auto" ||
//                mPlotProp["RangeX"].isEmpty()){
//            setScale(QwtPlot::xBottom, strRange);
//        }else {
//            setScale(QwtPlot::xBottom, mPlotProp["RangeX"]);
//        }
//    }
    mScaleBottomProp.first= xStart;
    mScaleBottomProp.second= xEnd;
    m_Histogram_plot->setAxisScale(QwtPlot::xBottom, mScaleBottomProp.first,
                                   mScaleBottomProp.second);

    //m_Histogram_plot->setAxisScale(QwtPlot::yLeft, -100,
    //                               100);
    //m_Histogram_plot->setAxisAutoScale(QwtPlot::yLeft);
}

QString sChartWidget::GetClassProp(){
    return mPublicProperty["Title"]+"/"
            +mPlotProp["RangeX"]+"/"
            +mPlotProp["RangeY"];
}

void sChartWidget::setScale(QwtPlot::Axis axic1, QString strScale)
{
    if(m_Histogram_plot!= nullptr && !strScale.isEmpty()){
        if(strScale.compare("auto", Qt::CaseInsensitive) == 0){
            m_Histogram_plot->setAxisAutoScale(axic1, true);
        }else{
            QStringList lstRange = strScale.split(',');
            if(lstRange.size() >= 2){
                m_Histogram_plot->setAxisScale(axic1, lstRange[0].toDouble(),
                        lstRange[1].toDouble());
                //m_Histogram_plot->setZoomBase(true);
            }
        }
    }
}

void sChartWidget::setSample(std::vector<double>const& ValuesX,
                              std::vector<double>& ValuesY,
                             double baseLine, bool isShowBaseLine,
                              bool save)
{
    m_Histogram_plot->setPlotSize(1);
    if(isShowBaseLine){
        mBaseLine->setYValue(baseLine);
        mBaseLine->show();
    }else
        mBaseLine->hide();
    m_Histogram_plot->populate(ValuesX,ValuesY, 0, baseLine);
    if(save)
    if(mutex.tryLock()){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        /* Modified by WQH
         * 解决采集过程中切换Y轴显示单位出现的软件闪退异常
         */
        if(mValuesX.size() != 0 && ValuesX.size() != 0 &&
                mValuesY.size() != 0 && ValuesY.size() != 0) {
            memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
            memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
        }
        mutex.unlock();
    }
}
void sChartWidget::setSample(QVector<double>const& ValuesX,
                             QVector<double>& ValuesY,
                             double baseLine, bool isShowBaseLine,
                             bool save)
{
    m_Histogram_plot->setPlotSize(1);
    if(isShowBaseLine){
        mBaseLine->setYValue(baseLine);
        mBaseLine->show();
    }else
        mBaseLine->hide();
    m_Histogram_plot->populate(ValuesX.data(),ValuesY.data(), ValuesX.size(), 0, baseLine);
    if(save)
    if(mutex.tryLock()){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        /* Modified by WQH
         * 解决采集过程中切换Y轴显示单位出现的软件闪退异常
         */
        if(mValuesX.size() != 0 && ValuesX.size() != 0 &&
                mValuesY.size() != 0 && ValuesY.size() != 0) {
            memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
            memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
        }
        mutex.unlock();
    }
}

void sChartWidget::clearPlot()
{
    m_Histogram_plot->setPlotSize(0);
}

int sChartWidget::addSample(std::vector<double>const& ValuesX,
               std::vector<double>& ValuesY,
               double baseLine, bool isShowBaseLine, bool save)
{
    m_Histogram_plot->setPlotSize(m_Histogram_plot->getPlotSize()+ 1);
    if(isShowBaseLine){
        mBaseLine->setYValue(baseLine);
        mBaseLine->show();
    }else
        mBaseLine->hide();
    m_Histogram_plot->populate(ValuesX,ValuesY, m_Histogram_plot->getPlotSize()-1, baseLine);
    if(save)
    if(mutex.tryLock()){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
        memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
        mutex.unlock();
    }
    return m_Histogram_plot->getPlotSize();
}

void sChartWidget::removeSample(int index)
{
    m_Histogram_plot->removePlot(index);
}

void sChartWidget::setSamples(QList<std::vector<double>>const& ValuesX,
                              QList<std::vector<double>>& ValuesY,
                              double baseLine, bool isShowBaseLine,
                              bool save)
{
    int lengthList= ValuesX.size();
    if((lengthList< 1)||(lengthList!= ValuesY.size()))
        return;
    m_Histogram_plot->setPlotSize(lengthList);

    if(isShowBaseLine){
        mBaseLine->setYValue(baseLine);
        mBaseLine->show();
    }else
        mBaseLine->hide();
    for(int i= 0; i< lengthList; ++i){
        m_Histogram_plot->populate(ValuesX[i],ValuesY[i], i, baseLine);
    }
    if(!save)
        return;
    if(mutex.tryLock()){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
        memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
        mutex.unlock();
    }
}
void sChartWidget::setSamples(QList<QVector<double>>const& ValuesX,
                QList<QVector<double>>& ValuesY,
                double baseLine, bool isShowBaseLine, bool save)
{
    int lengthList= ValuesX.size();
    if((lengthList< 1)||(lengthList!= ValuesY.size()))
        return;
    m_Histogram_plot->setPlotSize(lengthList);

    if(isShowBaseLine){
        mBaseLine->setYValue(baseLine);
        mBaseLine->show();
    }else
        mBaseLine->hide();
    for(int i= 0; i< lengthList; ++i){
        m_Histogram_plot->populate(ValuesX[i].data(),ValuesY[i].data(), ValuesX[i].size(), i, baseLine);
    }
    if(save)
    if(mutex.tryLock()){
        mValuesX.resize(ValuesX.size());
        mValuesY.resize(ValuesY.size());
        memcpy(&(mValuesX.at(0)),&(ValuesX.at(0)),ValuesX.size()*sizeof(double));
        memcpy(&(mValuesY.at(0)),&(ValuesY.at(0)),ValuesY.size()*sizeof(double));
        mutex.unlock();
    }
}
//void sChartWidget::setSamples( const double *xData, const double *yData, int size, int index, bool save)
//{
//    mBaseLine->hide();
//    if(save){
//        m_Histogram_plot->populate(xData, yData, index, size);
//        if(mutex.tryLock()){
//            mValuesX.resize(size);
//            mValuesY.resize(size);
//            memcpy(&(mValuesX.at(0)),xData,size*sizeof(double));
//            memcpy(&(mValuesY.at(0)),yData,size*sizeof(double));
//            mutex.unlock();
//        }
//    }
//    m_Histogram_plot->populate(xData, yData, index, size);
//}
void sChartWidget::resizeEvent(QResizeEvent *event)
{
    if(mCoordinateMask)
        mCoordinateMask->_hide();
    clearLine();
    this->update();
}

bool sChartWidget::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)
    if (ev->type() == QEvent::MouseButtonPress ||
            ev->type() == QEvent::MouseButtonRelease ){//||ev->type() == QEvent::MouseButtonDblClick
        QMouseEvent *mouseev = static_cast<QMouseEvent *>(ev);
        if(mouseev->button() == Qt::LeftButton) {
            if(ev->type() == QEvent::MouseButtonPress){
                mStartPos=mouseev->pos();
            }else if(ev->type() == QEvent::MouseButtonRelease){
                int distance = (mouseev->pos() - mStartPos).manhattanLength();
                if (distance < 5){//QApplication::startDragDistance()
                    emit selected(mCurrentX);//bPickerM=true;
                    //return true;
                }
            }
            return false;//false控制鼠标启用
        }else if(mouseev->button() == Qt::RightButton){
//            if(m_Histogram_plot!=nullptr){
//                //                if(mPlotProp["RangeX"] == "auto" ||
//                //                        mPlotProp["RangeX"].isEmpty())
//                //                    setScale(QwtPlot::xBottom, mPublicProperty["RangeX"]);
//                //                else
//                //                    setScale(QwtPlot::xBottom, mPlotProp["RangeX"]);
//                //                setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
//                if(mScaleLeftProp.first== mScaleLeftProp.second){
//                    m_Histogram_plot->setAxisAutoScale(QwtPlot::yLeft, true);
//                }else{
//                    m_Histogram_plot->setAxisScale(QwtPlot::yLeft, mScaleLeftProp.first,
//                                                   mScaleLeftProp.second);
//                }
//                if(mScaleBottomProp.first== mScaleBottomProp.second){
//                    m_Histogram_plot->setAxisAutoScale(QwtPlot::xBottom, true);
//                }else{
//                    m_Histogram_plot->setAxisScale(QwtPlot::xBottom, mScaleBottomProp.first,
//                                                   mScaleBottomProp.second);

//                }
//                m_Histogram_plot->updateMarker();
//            }
//            return true;
            emit sResetScale();
            return false;
        }else
            return false;
    }
    return false;
}


void sChartWidget::onSelected(const QPointF &polygon)
{
    mCurrentX= polygon.x();
}

void sChartWidget::addLine(double x, Qt::GlobalColor lineColor, QwtPlotMarker::LineStyle style, QString label)
{
    QwtPlotMarker *mX = new QwtPlotMarker();
    //mX->setLabel(QString::fromLatin1(""));
    //mX->setLabelAlignment(Qt::AlignLeft, Qt::AlignBottom);
    //mX->setLabelOrientation(Qt::Vertical);
    mX->setLineStyle(style);
    mX->setLinePen(QPen(lineColor, 1, Qt::DashDotDotLine));
    if(QwtPlotMarker::VLine== style)
        mX->setXValue(x);
    else if(QwtPlotMarker::HLine== style)
        mX->setYValue(x);
    if(!label.isEmpty()){
        QFont font;
        font.setPointSizeF(24);
        font.setBold(true);
        QwtText tx(label);
        tx.setFont(font);
        mX->setLabel(tx);
        mX->setLabelAlignment(/*Qt::AlignLeft |*/ Qt::AlignTop);
    }
    mX->attach(m_Histogram_plot);
    mLineMark.append(mX);
    m_Histogram_plot->replot();
}

void sChartWidget::clearLine(){
    foreach (auto var, mLineMark) {
        if(var){
            delete var;
            var=nullptr;
        }
    }
    mLineMark.clear();
    m_Histogram_plot->replot();
}

QList<double> sChartWidget::getLineMark()
{
    QList<double> tempList;
    foreach (auto var, mLineMark) {
        if(var)
            tempList.append(var->xValue());
        else
            tempList.clear();
    }
    return tempList;
}

void sChartWidget::updateXIC(QString XicString)
{
    QMap<uint32_t, QMAP_PARAM_XIC> xics;
    QStringList lstStr = XicString.split('/');
    if(lstStr.size()<2)
        return;
    double massRange= lstStr[0].toDouble();
    QStringList lstCurves = lstStr[1].split('@');
    foreach (QString strCurve, lstCurves) {
        QStringList lstCurveInfo = strCurve.split(':');
        if(lstCurveInfo.size() < 5)
            continue;
        uint32_t tempEvent= lstCurveInfo[0].toInt();
        QString tempMass= lstCurveInfo[1];
        xics[tempEvent][tempMass]= new _PARAM_XIC(massRange,
                                                  lstCurveInfo[2].toUInt(nullptr, 16),
                lstCurveInfo[3].toDouble(),
                lstCurveInfo[4].toDouble());
    }
    updateXIC(xics, false);
}

void sChartWidget::updateXIC(QMap<uint32_t, QMAP_PARAM_XIC>& xics, bool crossThread)
{
    QMutexLocker locker(&mutex_XIC);

    _GRAPH_XIC* tempGRAPH_XIC= nullptr;
    foreach (auto tmpKey, mXIC.keys()) {
        if(xics.contains(tmpKey)){
            foreach (auto tmpKey1, mXIC[tmpKey].keys()){
                if(xics[tmpKey].contains(tmpKey1))
                    continue;
                tempGRAPH_XIC= mXIC[tmpKey][tmpKey1];
                if(tempGRAPH_XIC->curve)
                    delete tempGRAPH_XIC->curve;
                delete mXIC[tmpKey][tmpKey1];
                mXIC[tmpKey].remove(tmpKey1);
            }
        }else{
            if(mXIC[tmpKey].size()>0){
                foreach (auto tmpKey1, mXIC[tmpKey].keys()){
                    tempGRAPH_XIC= mXIC[tmpKey][tmpKey1];
                    if(tempGRAPH_XIC->curve)
                        delete tempGRAPH_XIC->curve;
                    delete tempGRAPH_XIC;//mXIC[tmpKey][tmpKey1];
                    mXIC[tmpKey].remove(tmpKey1);
                }
                mXIC[tmpKey].clear();
            }
            mXIC.remove(tmpKey);
        }
    }

    foreach (uint32_t tmpKey, xics.keys()) {
        if(mXIC.contains(tmpKey)){
            foreach (auto& tmpKey1, xics[tmpKey].keys()){
                if(mXIC[tmpKey].contains(tmpKey1)){
                    *mXIC[tmpKey][tmpKey1]= *xics[tmpKey][tmpKey1];
                    if(crossThread)
                        continue;
                    _GRAPH_XIC* pGRAPH_XIC= mXIC[tmpKey][tmpKey1];
                    if(pGRAPH_XIC->curve)
                        continue;
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }else{
                    mXIC[tmpKey][tmpKey1]= new _GRAPH_XIC(xics[tmpKey][tmpKey1]);
                    if(crossThread)
                        continue;
                    _GRAPH_XIC* pGRAPH_XIC= mXIC[tmpKey][tmpKey1];
                    if(pGRAPH_XIC->curve)
                        continue;
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }
            }
        }else{
            mXIC[tmpKey]= QMAP_GRAPH_XIC();
            foreach (auto tmpKey1, xics[tmpKey].keys()){
                mXIC[tmpKey][tmpKey1]= new _GRAPH_XIC(xics[tmpKey][tmpKey1]);
                if(crossThread)
                    continue;

                _GRAPH_XIC* pGRAPH_XIC= mXIC[tmpKey][tmpKey1];
                if(!pGRAPH_XIC->curve){
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }
            }
        }
    }
}

void sChartWidget::updateOther(QString XicString)
{
    QMap<uint32_t, QMAP_PARAM_XIC> xics;
    QStringList lstCurves = XicString.split('@');
    foreach (QString strCurve, lstCurves) {
        QStringList lstCurveInfo = strCurve.split(':');
        if(lstCurveInfo.size() < 5)
            continue;
        uint32_t tempEvent= lstCurveInfo[0].toInt();
        QString tempMass= lstCurveInfo[1];
        xics[tempEvent][tempMass]= new _PARAM_XIC(0.5,
                                                  lstCurveInfo[2].toUInt(nullptr, 16),
                lstCurveInfo[3].toDouble(),
                lstCurveInfo[4].toDouble());
    }
    updateOther(xics, false);
}

void sChartWidget::updateOther(QMap<uint32_t, QMAP_PARAM_XIC>& xics, bool crossThread)
{
    QMutexLocker locker(&mutex_XIC);

    _GRAPH_XIC* tempGRAPH_XIC= nullptr;
    foreach (auto tmpKey, mOther.keys()) {
        if(xics.contains(tmpKey)){
            foreach (auto tmpKey1, mOther[tmpKey].keys()){
                if(xics[tmpKey].contains(tmpKey1))
                    continue;
                tempGRAPH_XIC= mOther[tmpKey][tmpKey1];
                if(tempGRAPH_XIC->curve)
                    delete tempGRAPH_XIC->curve;
                delete mOther[tmpKey][tmpKey1];
                mOther[tmpKey].remove(tmpKey1);
            }
        }else{
            if(mOther[tmpKey].size()>0){
                foreach (auto tmpKey1, mOther[tmpKey].keys()){
                    tempGRAPH_XIC= mOther[tmpKey][tmpKey1];
                    if(tempGRAPH_XIC->curve)
                        delete tempGRAPH_XIC->curve;
                    delete tempGRAPH_XIC;//mOther[tmpKey][tmpKey1];
                    mOther[tmpKey].remove(tmpKey1);
                }
                mOther[tmpKey].clear();
            }
            mOther.remove(tmpKey);
        }
    }

    foreach (uint32_t tmpKey, xics.keys()) {
        if(mOther.contains(tmpKey)){
            foreach (auto& tmpKey1, xics[tmpKey].keys()){
                if(mOther[tmpKey].contains(tmpKey1)){
                    *mOther[tmpKey][tmpKey1]= *xics[tmpKey][tmpKey1];
                    if(crossThread)
                        continue;
                    _GRAPH_XIC* pGRAPH_XIC= mOther[tmpKey][tmpKey1];
                    if(pGRAPH_XIC->curve)
                        continue;
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }else{
                    mOther[tmpKey][tmpKey1]= new _GRAPH_XIC(xics[tmpKey][tmpKey1]);
                    if(crossThread)
                        continue;
                    _GRAPH_XIC* pGRAPH_XIC= mOther[tmpKey][tmpKey1];
                    if(pGRAPH_XIC->curve)
                        continue;
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }
            }
        }else{
            mOther[tmpKey]= QMAP_GRAPH_XIC();
            foreach (auto tmpKey1, xics[tmpKey].keys()){
                mOther[tmpKey][tmpKey1]= new _GRAPH_XIC(xics[tmpKey][tmpKey1]);
                if(crossThread)
                    continue;

                _GRAPH_XIC* pGRAPH_XIC= mOther[tmpKey][tmpKey1];
                if(!pGRAPH_XIC->curve){
                    pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tmpKey).arg(tmpKey1));
                    pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                    if(m_Histogram_plot)
                        pGRAPH_XIC->curve->attach(m_Histogram_plot);
                }
            }
        }
    }
}

void sChartWidget::onAddXIC(uint32_t event,
                            QString mass,
                            double massRange,
                            uint color,
                            double Gain,
                            double Offset, bool crossThread)
{
    QMutexLocker locker(&mutex_XIC);

    if(mXIC.contains(event)){
        if(mXIC[event].contains(mass)){
            _GRAPH_XIC* pGRAPH_XIC= mXIC[event][mass];
            pGRAPH_XIC->massRange=massRange;
            pGRAPH_XIC->color=color;
            pGRAPH_XIC->Gain=Gain;
            pGRAPH_XIC->Offset=Offset;
            if(!pGRAPH_XIC->curve){
                pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            }
        }else{
            mXIC[event][mass]= new _GRAPH_XIC(massRange,
                                              color,
                                              Gain,
                                              Offset);
            if(crossThread)
                return;

            _GRAPH_XIC* pGRAPH_XIC= mXIC[event][mass];
            if(!pGRAPH_XIC->curve){
                pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(event).arg(mass));
                pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                if(m_Histogram_plot)
                    pGRAPH_XIC->curve->attach(m_Histogram_plot);
            }

        }
    }else{
        mXIC[event][mass]= new _GRAPH_XIC(massRange,
                                          color,
                                          Gain,
                                          Offset);
        if(crossThread)
            return;

        _GRAPH_XIC* pGRAPH_XIC= mXIC[event][mass];
        if(!pGRAPH_XIC->curve){
            pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(event).arg(mass));
            pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            if(m_Histogram_plot)
                pGRAPH_XIC->curve->attach(m_Histogram_plot);
        }

    }
}

void sChartWidget::onRemoveXIC(uint32_t event, QString mass)
{
    QMutexLocker locker(&mutex_XIC);
    if(!mXIC.contains(event))
        return;
    if(!mXIC[event].contains(mass))
        return;
    _GRAPH_XIC* pGRAPH_XIC= mXIC[event][mass];
    if(pGRAPH_XIC->curve)
        delete pGRAPH_XIC->curve;
    delete pGRAPH_XIC;//mXIC[tmpKey][tmpKey1];
    mXIC[event].remove(mass);
}
void sChartWidget::onAddOther(uint32_t event,
                            QString mass,
                            double massRange,
                            uint color,
                            double Gain,
                            double Offset, bool crossThread)
{
    QMutexLocker locker(&mutex_XIC);

    if(mOther.contains(event)){
        if(mOther[event].contains(mass)){
            _GRAPH_XIC* pGRAPH_XIC= mOther[event][mass];
            pGRAPH_XIC->massRange=massRange;
            pGRAPH_XIC->color=color;
            pGRAPH_XIC->Gain=Gain;
            pGRAPH_XIC->Offset=Offset;
            if(pGRAPH_XIC->curve){
                pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            }
        }else{
            mOther[event][mass]= new _GRAPH_XIC(massRange,
                                              color,
                                              Gain,
                                              Offset);
            if(crossThread)
                return;

            _GRAPH_XIC* pGRAPH_XIC= mOther[event][mass];
            if(!pGRAPH_XIC->curve){
                pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(event).arg(mass));
                pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
                if(m_Histogram_plot)
                    pGRAPH_XIC->curve->attach(m_Histogram_plot);
            }

        }
    }else{
        mOther[event][mass]= new _GRAPH_XIC(massRange,
                                          color,
                                          Gain,
                                          Offset);
        if(crossThread)
            return;

        _GRAPH_XIC* pGRAPH_XIC= mOther[event][mass];
        if(!pGRAPH_XIC->curve){
            pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(event).arg(mass));
            pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            if(m_Histogram_plot)
                pGRAPH_XIC->curve->attach(m_Histogram_plot);
        }

    }
}

void sChartWidget::onRemoveOther(uint32_t event, QString mass)
{
    QMutexLocker locker(&mutex_XIC);
    if(!mOther.contains(event))
        return;
    if(!mOther[event].contains(mass))
        return;
    _GRAPH_XIC* pGRAPH_XIC= mOther[event][mass];
    if(pGRAPH_XIC->curve)
        delete pGRAPH_XIC->curve;
    delete pGRAPH_XIC;//mXIC[tmpKey][tmpKey1];
    mOther[event].remove(mass);
}
//bool sChartWidget::drawXIC(std::vector<double>& pX, uint32_t sizeTIC, QString strXIC, int timeout)
//{
//    if(sizeTIC<1){
//        unLockXIC();return false;
//    }
//    if(!tryLockXIC(timeout)){
//        unLockXIC();return false;
//    }
//    QMutableMapIterator< uint32_t, QMAP_GRAPH_XIC > tempIter(mXIC);

//    while (tempIter.hasNext()) {
//        QMutableMapIterator<sMapString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//        while (tempIter1.hasNext()) {
//            _GRAPH_XIC* pGRAPH_XIC= tempIter1.next().value();
//            if(!pGRAPH_XIC->curve){
//                pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(tempIter.key()).arg(tempIter1.key()));
//                pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
//                if(m_Histogram_plot)
//                    pGRAPH_XIC->curve->attach(m_Histogram_plot);
//            }
//            if(pGRAPH_XIC->yListXIC.size()== sizeTIC){
//                pGRAPH_XIC->curve->setSamples(pX.data(), pGRAPH_XIC->yListXIC.data(), sizeTIC);
//                pGRAPH_XIC->curve->hide();
//            }
//        }
//    }

//    if(!strXIC.isEmpty()){
//        QMap<uint32_t, QMAP_PARAM_XIC> xics;
//        QStringList lstStr = strXIC.split('/');
//        if(lstStr.size()<2){
//            unLockXIC();return false;
//        }
//        double massRange= lstStr[0].toDouble();
//        QStringList lstCurves = lstStr[1].split('@');
//        foreach (QString strCurve, lstCurves) {
//            QStringList lstCurveInfo = strCurve.split(':');
//            if(lstCurveInfo.size() < 5)
//                continue;
//            uint32_t tempEvent= lstCurveInfo[0].toInt();
//            QString tempMass= lstCurveInfo[1];
//            xics[tempEvent][tempMass]= new _PARAM_XIC(massRange,
//                                                      lstCurveInfo[2].toUInt(nullptr, 16),
//                    lstCurveInfo[3].toDouble(),
//                    lstCurveInfo[4].toDouble());
//        }

//        QMap<uint32_t, QMAP_PARAM_XIC>::iterator iterEventMapXIC;
//        QMap<QString, _PARAM_XIC*>::iterator iterMassMapXIC;

//        tempIter= mXIC;
//        //QMutableMapIterator< uint32_t, QMap<QString, _GRAPH_XIC*> > tempIter(mXIC);
//        while (tempIter.hasNext()) {
//            QMutableMapIterator<sMapString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//            iterEventMapXIC= xics.find(tempIter.key());
//            if (iterEventMapXIC == xics.end())
//                continue;
//            QMap<QString, _PARAM_XIC*>& pMassMapXIC= xics[tempIter.key()];
//            while (tempIter1.hasNext()) {
//                _GRAPH_XIC* pGRAPH_XIC= tempIter1.next().value();
//                iterMassMapXIC= pMassMapXIC.find(tempIter1.key());
//                if (iterMassMapXIC == pMassMapXIC.end())
//                    continue;
//                pGRAPH_XIC->curve->show();
//            }
//        }
//    }
//    unLockXIC();
//    return true;
//}

bool sChartWidget::drawXIC(std::vector<double>& pX,
                           QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                           uint32_t sizeTIC,
                           int timeout)
{
    if(sizeTIC<1)
        return false;
    if(!tryLockXIC(timeout))
        return false;
    for(const auto& uintkey: mXIC.keys()){
        if (xics.find(uintkey) == xics.end())
            continue;
        for(const auto& strkey: mXIC[uintkey].keys()){
            if (xics[uintkey].find(strkey) == xics[uintkey].end())
                continue;
            _GRAPH_XIC* pGRAPH_XIC= mXIC[uintkey][strkey];
            if(!pGRAPH_XIC->curve){
                pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(uintkey).arg(strkey));
                if(m_Histogram_plot)
                    pGRAPH_XIC->curve->attach(m_Histogram_plot);
            }
            pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            _PARAM_XIC* p_PARAM_XIC= xics[uintkey][strkey];
            if(p_PARAM_XIC->yListXIC.size()== sizeTIC){
                pGRAPH_XIC->curve->setSamples(pX.data(), p_PARAM_XIC->yListXIC.data(), sizeTIC);
            }
        }
    }
    unLockXIC();
    return true;
}

bool sChartWidget::drawOther(std::vector<double>& pX,
                           QMap<uint32_t, QMAP_PARAM_XIC>& xics,
                           uint32_t sizeTIC,
                           int timeout)
{
    if(sizeTIC<1)
        return false;
    if(!tryLockXIC(timeout))
        return false;
    for(const auto& uintkey: mOther.keys()){
        if (!xics.contains(uintkey))
            continue;
        for(const auto& strkey: mOther[uintkey].keys()){
            if (!xics[uintkey].contains(strkey))
                continue;
            _GRAPH_XIC* pGRAPH_XIC= mOther[uintkey][strkey];
            if(!pGRAPH_XIC->curve){
                pGRAPH_XIC->curve = new QwtPlotCurve(QString("E%1Mass%2").arg(uintkey).arg(strkey));
                if(m_Histogram_plot)
                    pGRAPH_XIC->curve->attach(m_Histogram_plot);
            }
            pGRAPH_XIC->curve->setPen(pGRAPH_XIC->color, mWidth);
            _PARAM_XIC* p_PARAM_XIC= xics[uintkey][strkey];
            if(p_PARAM_XIC->yListXIC.size()== sizeTIC){
                pGRAPH_XIC->curve->setSamples(pX.data(), p_PARAM_XIC->yListXIC.data(), sizeTIC);
            }
        }
    }
    unLockXIC();
    return true;
}

void sChartWidget::clearBuffXIC()
{
    lockXIC();
    for(auto& uintKey: mXIC.keys()){
        for(auto& strKey: mXIC[uintKey].keys()){
            mXIC[uintKey][strKey]->yListXIC.clear();
        }
    }
    unLockXIC();
}

//void sChartWidget::updateArea(
//    const QString &title,
//    const QVector<QwtIntervalSample>& samples,
//    const QColor &color )
//{
//    if(mQwtPlotIntervalCurve)
//        delete mQwtPlotIntervalCurve;
//    mQwtPlotIntervalCurve = new QwtPlotIntervalCurve( title );
//    mQwtPlotIntervalCurve-> setTitle(title);

//    mQwtPlotIntervalCurve-> setRenderHint( QwtPlotItem::RenderAntialiased );
//    mQwtPlotIntervalCurve-> setPen( Qt::white );

//    QColor bg( color );
//    bg.setAlpha( 150 );
//    mQwtPlotIntervalCurve-> setBrush( QBrush( bg ) );
//    mQwtPlotIntervalCurve-> setStyle( QwtPlotIntervalCurve::Tube );

//    mQwtPlotIntervalCurve-> setSamples( samples );
//    mQwtPlotIntervalCurve-> attach( m_Histogram_plot );
//}

//void sChartWidget::clearCurveXIC()
//{
//    lockXIC();
//    QMutableMapIterator< uint32_t, QMap<QString, _GRAPH_XIC*> > tempIter(mXIC);
//    while (tempIter.hasNext()) {
//        QMutableMapIterator<QString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//        while (tempIter1.hasNext()) {
//            _GRAPH_XIC* pGRAPH_XIC= tempIter1.next().value();
//            if(pGRAPH_XIC->curve)
//                delete (QwtPlotCurve*)pGRAPH_XIC->curve;
//            pGRAPH_XIC->curve = nullptr;
//            pGRAPH_XIC->yListXIC.clear();
//        }
//    }
//    unLockXIC();
//}

//int sChartWidget::XIC_CurveCnt()
//{
//    int cnt = 0;
//    lockXIC();
////方式1：
////    QList<uint> keyList = mXIC.keys();
////    for(int i =0; i < keyList.size(); i++){
////       QMap<QString, _GRAPH_XIC*> mapXIC =  mXIC.value(keyList[i]);
////       QList<QString> keyList2 = mapXIC.keys();
////       cnt += keyList2.size();
////    }
////方式2：
//    QMutableMapIterator< uint32_t, QMap<QString, _GRAPH_XIC*> > tempIter(mXIC);
//    while (tempIter.hasNext()) {
//        QMutableMapIterator<QString, _GRAPH_XIC*> tempIter1(tempIter.next().value());
//        while (tempIter1.hasNext()) {
//            cnt++;
//            tempIter1.next();
//        }
//    }
//    unLockXIC();
//    return cnt;
//}

//void sChartWidget::set_PEAK_INFO(QString posStr, QString hwwStr, QString areaStr)
//{
//    if(mLE_PEAK_POS){
//        mLE_PEAK_POS->setText(posStr);
//    }
//    if(mLE_PEAK_HWW){
//        mLE_PEAK_HWW->setText(hwwStr);
//    }
//    if(mLE_PEAK_AREA){
//        mLE_PEAK_AREA->setText(areaStr);
//    }
//}
