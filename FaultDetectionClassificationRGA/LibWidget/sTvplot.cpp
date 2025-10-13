#include "sTvplot.h"
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include <qpen.h>
#include <stdlib.h>
#include <cPublicCCS.h>
//#ifdef Q_OS_WIN32
//#include "D:/QT/GlobalStruct/cPublicCCS.h"
//#else
//#include "/home/zhouxu/work/GlobalStruct/cPublicCCS.h"
//#endif

class Histogram: public QwtPlotHistogram
{
public:
    Histogram( const QString &, const QColor & );
    void setColor( const QColor & );
    void setValues( uint32_t numValues, const double * );
    void setValues( std::vector<double>const& ValuesX,std::vector<double>& ValuesY );
};

Histogram::Histogram( const QString &title, const QColor &symbolColor ):
    QwtPlotHistogram( title )
{
    setStyle( static_cast<QwtPlotHistogram::HistogramStyle>(QwtPlotHistogram::Outline) );
    setColor( symbolColor );
}

void Histogram::setColor( const QColor &color )
{
    QColor c = color;
    //c.setAlpha( 180 );//透明
    setBrush( QBrush( c ) );
}

void Histogram::setValues( uint32_t numValues, const double *values )
{
    QVector<QwtIntervalSample> samples( numValues );
    for ( uint32_t i = 0; i < numValues; i++ ){
        QwtInterval interval( double( i ),i+0.000001  );// i+ 5.0
        interval.setBorderFlags( QwtInterval::ExcludeMaximum);
        samples[i] = QwtIntervalSample(values[i], interval  );
    }
    setSamples(new QwtIntervalSeriesData( samples ));
}

void Histogram::setValues( std::vector<double>const& ValuesX,std::vector<double>& ValuesY )
{
    uint32_t numValues=ValuesX.size();
    QVector<QwtIntervalSample> samples( numValues );
    for ( uint32_t i = 0; i < numValues; i++ ){
        QwtInterval interval( ValuesX[i],ValuesX[i]+0.000001  );// i+ 5.0
        interval.setBorderFlags( QwtInterval::ExcludeMaximum);
        samples[i] = QwtIntervalSample(ValuesY[i], interval  );
    }
    setSamples(new QwtIntervalSeriesData( samples ));
}

TVPlot::TVPlot( QWidget *parent, MPlotStyle type):
    QwtPlot( parent ),
    //pMarker(nullptr),
    phistogram(nullptr)
{
    plotLayout()->setAlignCanvasToScales( true );
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableX( false );
    grid->enableY( true );
    grid->enableXMin( false );
    grid->enableYMin( false );
    grid->setMajorPen( Qt::black, 0, Qt::DotLine );
    grid->attach( this );
    if(type==Histograms){
        phistogram=new Histogram( "Histogram", Qt::red );
        phistogram->attach( this );
    }else{
        QwtPlotCurve* pQwtPlotCurve= new QwtPlotCurve("TIC");
        pQwtPlotCurve->setRenderHint(QwtPlotItem::RenderAntialiased, false);
        pQwtPlotCurve->attach(this);
        pQwtPlotCurve->setStyle((QwtPlotCurve::CurveStyle)type);
        mQwtPlotCurves<< pQwtPlotCurve;
    }
    mStyle=type;
    m_zoomer = new QwtPlotZoomer( this->canvas() );
    m_zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn );
    m_zoomer->setRubberBandPen( QColor( Qt::red ) );
    m_zoomer->setTrackerPen( QColor( Qt::red ) );
    connect(m_zoomer,SIGNAL(zoomed( const QRectF&)),this,SLOT(onZoomed(const QRectF&)));


    replot(); // creating the legend items
    setAutoReplot( true );
}

TVPlot::~TVPlot()
{
    qDeleteAll(pMarker);
    pMarker.clear();
    foreach (QwtPlotCurve* pPlotCurve, mQwtPlotCurves) {
        if(pPlotCurve)
            delete pPlotCurve;
    }
//    if(pQwtPlotCurve!=nullptr)
//        delete pQwtPlotCurve;
    if(phistogram!=nullptr)
        delete phistogram;
    if(m_zoomer!=nullptr)
        delete m_zoomer;
}

void TVPlot::populate(std::vector<double>const& ValuesX,std::vector<double>& ValuesY, int index, double baseLine)
{
    int lengthList= mQwtPlotCurves.size();
    for(int i= 0; i< lengthList; ++i){
        if(index!= i)
            continue;
        QwtPlotCurve* pPlotCurve= mQwtPlotCurves[i];
        if(pPlotCurve){
            if(mStyle==Lines){
                pPlotCurve->setSamples(ValuesX.data(), ValuesY.data(),ValuesX.size());
            }else if(mStyle==Sticks||mStyle==Histograms){
                if(mStyle==Sticks){
                    pPlotCurve->setBaseline(baseLine);
                    pPlotCurve->setSamples(ValuesX.data(), ValuesY.data(),ValuesX.size());
                }else
                    phistogram->setValues(ValuesX, ValuesY);
        #if 0
                tmpValuesY.resize(ValuesY.size());
                tmpValuesX.resize(ValuesX.size());
                memcpy(&tmpValuesY[0], &ValuesY[0], ValuesY.size() * sizeof(double));
                memcpy(&tmpValuesX[0], &ValuesX[0], ValuesX.size() * sizeof(double));
                marker.resize(ValuesY.size());
                for(int i=0;i<njuneValues;i++)
                    marker[i]=i;
                int nMarkers=20;
                if(njuneValues>nMarkers){
                    sortGICAP::widthSort(&(tmpValuesY[0]),&(marker[0]),njuneValues,nMarkers);
                    for(int i=0;i<nMarkers;i++)
                        tmpValuesX[i]=ValuesX[marker[i]];
                }else
                    nMarkers= njuneValues;

                if(pMarker.size()!=nMarkers){
                    while(pMarker.size()< nMarkers){
                        pMarker.append(new QwtPlotMarker());
                    }
                    while(pMarker.size()> nMarkers){
                        delete pMarker.last();
                        pMarker.removeLast();
                    }
                }
                for(int i=0;i<nMarkers;i++){
                    pMarker[i]->setLabel(QString::number(tmpValuesX[i]));
                    pMarker[i]->setLabelAlignment(Qt::AlignTop);//Qt::AlignLeft |
                    pMarker[i]->setLineStyle(QwtPlotMarker::NoLine);
                    pMarker[i]->setXValue(tmpValuesX[i]);
                    pMarker[i]->setYValue(tmpValuesY[i]);
                    pMarker[i]->attach(this);
                }
        #endif
            }
        }
        break;
    }
    QwtPlot::updateAxes();//replot();20250526
}
bool TVPlot::showMarker(double ValuesX, double ValuesY, QSize& size, QVector<QVector<quint8> >& bitMap, QwtPlotMarker* tmpMarker,
                _AXIS_GRAPH& pAXIS_GRAPH)
{
    QSizeF tmpSize= tmpMarker->label().textSize();
//    QwtInterval tempInterval=this->axisScaleDiv(QwtPlot::xBottom).interval();
//    pAXIS_GRAPH.xmin = tempInterval.minValue();
//    pAXIS_GRAPH.xmax = tempInterval.maxValue();
//    pAXIS_GRAPH.ymin = tempInterval.minValue();
//    pAXIS_GRAPH.ymax = tempInterval.maxValue();
    int startX= int((ValuesX-pAXIS_GRAPH.xmin)/(pAXIS_GRAPH.xmax-pAXIS_GRAPH.xmin)*size.width())
            -tmpSize.width()/2-1;
    if(startX< 0)
        startX= 0;
    int endX=startX+ tmpSize.width()+ 2;
    if(endX> size.width())
        endX= size.width();
    int startY= int((ValuesY-pAXIS_GRAPH.ymin)/(pAXIS_GRAPH.ymax-pAXIS_GRAPH.ymin)*size.height())
            -tmpSize.height()/2-1;
    if(startY< 0)
        startY= 0;
    int endY=startY+ tmpSize.height()+ 2;
    if(endY> size.height())
        endY= size.height();
    for (int j= startY; j< endY; ++j)
        for (int k= startX; k< endX; ++k)
            if(bitMap[j][k])
                return false;
    for (int j= startY; j< endY; ++j)
        for (int k= startX; k< endX; ++k)
            bitMap[j][k]= 1;
    return true;
}

void TVPlot::updateMarker()
{
    int32_t nMarkers=mMarker.size();
    if(nMarkers<1)
        return;
    QSize size= this->canvas()->size();
    _AXIS_GRAPH tmpAXIS_GRAPH;
    QwtInterval tempInterval=this->axisScaleDiv(QwtPlot::xBottom).interval();
    tmpAXIS_GRAPH.xmin = tempInterval.minValue();
    tmpAXIS_GRAPH.xmax = tempInterval.maxValue();
    tempInterval=this->axisScaleDiv(QwtPlot::yLeft).interval();
    tmpAXIS_GRAPH.ymin = tempInterval.minValue();
    tmpAXIS_GRAPH.ymax = tempInterval.maxValue();
    QVector<QVector<quint8> > bitMap(size.height(), QVector<quint8>(size.width()));
    for(int i= nMarkers-1; i> -1; --i){
        pMarker[i]->setLabel(mMarker[i]);
        //pMarker[i]->setLabelAlignment(Qt::AlignTop);//Qt::AlignLeft |
        //pMarker[i]->setLineStyle(QwtPlotMarker::NoLine);
        pMarker[i]->setXValue(mValuesX[i]);
        pMarker[i]->setYValue(mValuesY[i]);
        //pMarker[i]->attach(this);
        if(showMarker(mValuesX[i], mValuesY[i], size, bitMap, pMarker[i], tmpAXIS_GRAPH)){
            pMarker[i]->show();
        }else
            pMarker[i]->hide();
    }
    if((int32_t)(mValuesY.size())>nMarkers){
        pMarker[nMarkers-1]->setXValue(mValuesX[nMarkers]);
        pMarker[nMarkers-1]->setYValue(mValuesY[nMarkers]);
    }
    replot();
}

void TVPlot::SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,QVector<QString>& Marker)
{
    int32_t nMarkers=Marker.size();
    if(ValuesX.size()!=ValuesY.size()||
            (int32_t)(ValuesY.size())!=nMarkers)
        return;
    mValuesX.resize(ValuesX.size());
    mValuesY.resize(ValuesY.size());
    mMarker.resize(nMarkers);
    memcpy(mValuesX.data(), ValuesX.data(), ValuesX.size()*sizeof(double));
    memcpy(mValuesY.data(), ValuesY.data(), ValuesY.size()*sizeof(double));
    for(int i=0;i<nMarkers;++i)
        mMarker[i]=Marker[i];
    if(pMarker.size()!=nMarkers){
        while(pMarker.size()< nMarkers){
            QwtPlotMarker* pQwtPlotMarker= new QwtPlotMarker();
            pQwtPlotMarker->setLabelAlignment(Qt::AlignTop);//Qt::AlignLeft |
            pQwtPlotMarker->setLineStyle(QwtPlotMarker::NoLine);
            pQwtPlotMarker->attach(this);
            pMarker.append(pQwtPlotMarker);
        }
        while(pMarker.size()> nMarkers){
            delete pMarker.last();
            pMarker.removeLast();
        }
    }
    updateMarker();
}

void TVPlot::SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,std::vector<double>& Marker)
{
    if(ValuesX.size()!=ValuesY.size()||
            ValuesY.size()<Marker.size())
        return;
    int nMarkers=Marker.size();
    if(pMarker.size()!=nMarkers){
        while(pMarker.size()< nMarkers){
            pMarker.append(new QwtPlotMarker());
        }
        while(pMarker.size()> nMarkers){
            delete pMarker.last();
            pMarker.removeLast();
        }
    }
    for(int i=0;i<nMarkers;i++){
        if(Marker[i]<0.00001)
            continue;
        pMarker[i]->setLabel(QString::number(Marker[i]));
        pMarker[i]->setLabelAlignment(Qt::AlignTop);//Qt::AlignLeft |
        pMarker[i]->setLineStyle(QwtPlotMarker::NoLine);
        pMarker[i]->setXValue(ValuesX[i]);
        pMarker[i]->setYValue(ValuesY[i]);
        pMarker[i]->attach(this);
    }
    if(ValuesY.size()>nMarkers){
        pMarker[nMarkers-1]->setXValue(ValuesX[nMarkers]);
        pMarker[nMarkers-1]->setYValue(ValuesY[nMarkers]);
    }
    replot();
}

void TVPlot::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "tvplot.pdf" );
}

void TVPlot::setMode( int mode )
{
    QwtPlotItemList items = itemList( QwtPlotItem::Rtti_PlotHistogram );
    for ( int i = 0; i < items.size(); i++ ){
        QwtPlotHistogram *phistogram = static_cast<QwtPlotHistogram *>( items[i] );
        if ( mode < 3 ){
            phistogram->setStyle( static_cast<QwtPlotHistogram::HistogramStyle>( mode ) );
            phistogram->setSymbol( NULL );
        }else{
            phistogram->setStyle( QwtPlotHistogram::Columns );
            QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
            symbol->setFrameStyle( QwtColumnSymbol::Raised );
            symbol->setLineWidth( 2 );
            symbol->setPalette( QPalette( phistogram->brush().color() ) );
            phistogram->setSymbol( symbol );
        }
    }
}

void TVPlot::setZoomBase(bool setBase)
{
    m_zoomer->setZoomBase(setBase);
}

void TVPlot::onZoomed( const QRectF &rect )
{
        double x1 = rect.left();
                    double x2 = rect.right();
                    if ( !axisScaleDiv( QwtPlot::xBottom).isIncreasing() )
                        qSwap( x1, x2 );

                    setAxisScale(QwtPlot::xBottom, x1, x2 );
        double y1 = rect.top();
                    double y2 = rect.bottom();
                    if ( !axisScaleDiv( QwtPlot::yLeft).isIncreasing() )
                        qSwap( y1, y2 );

                    setAxisScale( QwtPlot::yLeft, y1, y2 );
        //this->replot();
    updateMarker();
}
