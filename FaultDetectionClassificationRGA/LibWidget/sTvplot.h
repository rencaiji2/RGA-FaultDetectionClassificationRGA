#pragma once
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
//#include <qwt_plot_glcanvas.h>

class Histogram;
class TVPlot: public QwtPlot
{
    Q_OBJECT

public:
    enum MPlotStyle
    {
        /*!
           Don't draw a curve. Note: This doesn't affect the symbols.
        */
        NoCurve = -1,

        /*!
           Connect the points with straight lines. The lines might
           be interpolated depending on the 'Fitted' attribute. Curve
           fitting can be configured using setCurveFitter().
        */
        Lines,

        /*!
           Draw vertical or horizontal sticks ( depending on the
           orientation() ) from a baseline which is defined by setBaseline().
        */
        Sticks,

        /*!
           Connect the points with a step function. The step function
           is drawn from the left to the right or vice versa,
           depending on the QwtPlotCurve::Inverted attribute.
        */
        Steps,

        /*!
           Draw dots at the locations of the data points. Note:
           This is different from a dotted line (see setPen()), and faster
           as a curve in QwtPlotCurve::NoStyle style and a symbol
           painting a point.
        */
        Dots,

        /*!
           Styles >= QwtPlotCurve::UserCurve are reserved for derived
           classes of QwtPlotCurve that overload drawCurve() with
           additional application specific curve types.
        */
        Histograms,
        UserCurve = 100
    };
    QList<QwtPlotCurve*> mQwtPlotCurves;//=nullptr;
    //QList<QwtPlotGLCanvas*> mQwtPlotCurves;//=nullptr;
    int getPlotSize(){
        return mQwtPlotCurves.size();
    }
    void setPlotSize(int size){
        if(mQwtPlotCurves.size()== size)
            return;
        while(mQwtPlotCurves.size()> size){
            auto* pPlotCurve= mQwtPlotCurves.last();
            if(pPlotCurve)
                delete pPlotCurve;
            mQwtPlotCurves.removeLast();
        }
        while(mQwtPlotCurves.size()< size){
            QwtPlotCurve* pQwtPlotCurve= new QwtPlotCurve("");
            pQwtPlotCurve->setRenderHint(QwtPlotItem::RenderAntialiased, false);
            pQwtPlotCurve->attach(this);
            pQwtPlotCurve->setStyle((QwtPlotCurve::CurveStyle)mStyle);
            mQwtPlotCurves<< pQwtPlotCurve;
        }
    }
    void removePlot(int index){
        if(index< mQwtPlotCurves.size()){
            mQwtPlotCurves.removeAt(index);
        }
    }
    TVPlot( QWidget* = nullptr, MPlotStyle type=Lines);
    ~TVPlot();
    void setStyle(MPlotStyle type){
        if(mQwtPlotCurves.isEmpty())
            return;
        foreach (auto* pPlotCurve, mQwtPlotCurves) {
            if(pPlotCurve)
                pPlotCurve->setStyle((QwtPlotCurve::CurveStyle)type);
        }
        mStyle=(int)type;
    }
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,QVector<QString>& Marker);
    void SetMarker(std::vector<double>const& ValuesX,std::vector<double>& ValuesY,std::vector<double>& Marker);
    //replot();
    bool mRestore=false;
    void updateMarker();
public Q_SLOTS:
    void setMode( int );
    void exportPlot();
    void populate(std::vector<double>const& ValuesX,std::vector<double>& ValuesY, int index, double baseLine=0.0);
    void populate( const double *xData, const double *yData, int size, int index, double baseLine=0.0){
        Q_UNUSED(baseLine)
        int lengthList= mQwtPlotCurves.size();
        for(int i= 0; i< lengthList; ++i){
            if(index!= i)
                continue;
            QwtPlotCurve* pPlotCurve= mQwtPlotCurves[i];
            if(pPlotCurve){
                pPlotCurve->setSamples(xData, yData, size);
            }
            break;
        }
        QwtPlot::updateAxes();//replot();20250526
    }
//    void setSamples( const double *xData, const double *yData, int size, double baseLine=0.0){
//        pQwtPlotCurve->setSamples(xData, yData, size);
//        replot();
//    }
    void setZoomBase(bool setBase);
    void onZoomed( const QRectF &rect );
    void setQwtPlotCurveVisible(bool flag=true){
        if(mQwtPlotCurves.isEmpty())
            return;
        foreach (QwtPlotCurve* pPlotCurve, mQwtPlotCurves) {
            if(pPlotCurve)
                pPlotCurve->setVisible(flag);
        }
    }
private:
    int mStyle;
    QList<QwtPlotMarker*> pMarker;
    //QwtPlotCurve* pQwtPlotCurve=nullptr;
    Histogram* phistogram=nullptr;
    QwtPlotZoomer* m_zoomer=nullptr;
    struct _AXIS_GRAPH{
        double xmin= 0;
        double xmax= 0;
        double ymin= 0;
        double ymax= 0;
    };
    bool showMarker(double ValuesX, double ValuesY, QSize& size, QVector<QVector<quint8> >& bitMap,
                    QwtPlotMarker* tmpMarker, _AXIS_GRAPH& pAXIS_GRAPH);
    std::vector<double> mValuesX;
    std::vector<double> mValuesY;
    QVector<QString> mMarker;

    //std::vector<double> tmpValuesY;
    //std::vector<double> tmpValuesX;
    //std::vector<int> marker;
};

