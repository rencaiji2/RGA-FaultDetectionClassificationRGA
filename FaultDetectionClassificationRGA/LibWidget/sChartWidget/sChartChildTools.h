#ifndef SCHARTCHILDTOOLS_H
#define SCHARTCHILDTOOLS_H
#include <QDate>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_grid.h>
#include <cPublicCCS.h>

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
    _GRAPH_XIC& operator=(_GRAPH_XIC& pGRAPH_XIC){
        if (this != &pGRAPH_XIC){
            massRange= pGRAPH_XIC.massRange;
            color= pGRAPH_XIC.color;
            Gain= pGRAPH_XIC.Gain;
            Offset= pGRAPH_XIC.Offset;
            curve= pGRAPH_XIC.curve;
            //_GRAPH_XIC* p= const_cast<_GRAPH_XIC*>(&pGRAPH_XIC);
            yListXIC.swap(pGRAPH_XIC.yListXIC);
        }
        return *this;
    }
    _GRAPH_XIC& operator=(_PARAM_XIC& pPARAM_XIC){
        if (this != &pPARAM_XIC){
            massRange= pPARAM_XIC.massRange;
            color= pPARAM_XIC.color;
            Gain= pPARAM_XIC.Gain;
            Offset= pPARAM_XIC.Offset;
            yListXIC.swap(pPARAM_XIC.yListXIC);
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
    DistroScaleDraw(bool isLogarithmic = false):QwtScaleDraw(),mColor(QColor(0,0,0)){
        //默认为笛卡尔坐标
        m_isLogarithmic = isLogarithmic;
    }
    virtual ~DistroScaleDraw();
    void setLogarithmic(bool logarithmic){
        m_isLogarithmic = logarithmic;
    }
    void setColor(QColor pColor){
        mColor= pColor;
    }
    virtual QwtText label(double value) const{
        QwtText text(QString::number(value));
        text.setColor(mColor);
        if (m_isLogarithmic){
        //对数坐标下，将刻度值转换为指数形式
            text.setText(QString::number(value, 'e')) ;//, 1
        }else {
        // 笛卡尔坐标下，使用默认的刻度标签
            text = QwtScaleDraw::label(value);
        }
        return text;
    }
private:
    QColor mColor;
    bool m_isLogarithmic;
};

class DistroQwtLegend:public QwtLegend
{
public:
    DistroQwtLegend( QWidget *parent = nullptr ):QwtLegend(parent){}
    virtual ~DistroQwtLegend();
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
    TimeScaleDraw(const QDateTime& base={}):
        baseDateTime( base ){}
    virtual ~TimeScaleDraw();
    void setBaseTime(const QDateTime &base){
                baseDateTime= base;
            }
    virtual QwtText label( double v ) const{
        QDateTime upTime = baseDateTime.addSecs( static_cast<int>( v ) );
        return upTime.toString("   MM/dd\nhh:mm:ss");
    }
private:
    QDateTime baseDateTime;
};

class ScientificNotationScaleDraw : public QwtScaleDraw
{
public:
    explicit ScientificNotationScaleDraw(bool enableScientificNotation = false)
    : mScientificNotationEnabled(enableScientificNotation){}
    virtual ~ScientificNotationScaleDraw() override;
    virtual QwtText label(double value) const override{
        if (mScientificNotationEnabled){
            return QwtText(QString::number(value, 'e', 2));
        }else{
            return QwtScaleDraw::label(value);//return QwtText(QString::number(value, 'f', 0));
        }
    }
private:
    bool mScientificNotationEnabled= false;
};

//class sMapString: public QString
//{
//public:
//    explicit sMapString(const QChar *unicode, int size = -1):QString(unicode, size){}
//    sMapString(QChar c):QString(c){}
//    sMapString(int size, QChar c):QString(size, c){}
//    inline sMapString(QLatin1String latin1):QString(latin1){}
//    inline sMapString(const QString & other) Q_DECL_NOTHROW:QString(other){}
//    //inline ~sMapString();

//    inline bool operator < (const sMapString& other) const{
//        return this->toDouble()< other.toDouble();//return str > other.str;
//    }
//};
#endif // SCHARTCHILDTOOLS_H
