#include "sChartWidgetTouchScreen.h"

sChartWidgetTouchScreen::sChartWidgetTouchScreen(_STYLE_CHART _STYLE_CHART1, QWidget *parent, QWidget *pMenu):
    //m_STYLE_CHART(_STYLE_CHART1),
    sChartWidget(_STYLE_CHART1, parent, pMenu)
{
    //m_Histogram_plot->canvas()->installEventFilter(this);
}

sChartWidgetTouchScreen::~sChartWidgetTouchScreen()
{

}

void sChartWidgetTouchScreen::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
//    if (changeFlags & QPinchGesture::CenterPointChanged) {
//        if(isFirstCenter){
//            isFirstCenter=false;
//            if(mLastEndX==0){
//                mLastStartX=0;
//                mLastEndX=mValuesX.size()-1;
//            }
//            mScale=1;
//            QPoint startValue=gesture->property("lastCenterPoint").toPoint();
//            canvasW=(double)(m_Histogram_plot->canvas()->width());
//            mCenterX=(int)(double(startValue.x())*double(mLastEndX-mLastStartX)
//                           /canvasW+mLastStartX);
//        }
//    }
//    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
//        qreal lastValue=gesture->property("lastScaleFactor").toReal();
//        if(lastValue>0 || lastValue<0){
//            mScale*=lastValue;
//            QPoint startValue=gesture->property("lastCenterPoint").toPoint();
//            double dcanvasX=double(startValue.x())/mScale;
//            tmpStartX=mCenterX-double(mLastEndX-mLastStartX)*dcanvasX/canvasW;
//            tmpEndX=tmpStartX+double(mLastEndX-mLastStartX)/mScale;
//            if(tmpStartX<0)
//                tmpStartX=0;
//            if(tmpEndX>mValuesY.size()-1)
//                tmpEndX=mValuesY.size()-1;
//            m_Histogram_plot->setAxisScale(QwtPlot::xBottom, mValuesX[tmpStartX],
//                                           mValuesX[tmpEndX]);//20240407修改，mValuesX已作为存储数据
//        }
//    }
//    if (gesture->state() == Qt::GestureFinished) {
//        mLastStartX=tmpStartX;
//        mLastEndX=tmpEndX;
//        isFirstCenter=true;
//        bMouseM=false;
//    }
}

bool sChartWidgetTouchScreen::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

bool sChartWidgetTouchScreen::event(QEvent *event)
{
//    if (event->type() == QEvent::Gesture)
//        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool sChartWidgetTouchScreen::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)
    if (ev->type() == QEvent::MouseButtonDblClick)
        return true;
    if (ev->type() == QEvent::MouseButtonPress ||
            ev->type() == QEvent::MouseButtonRelease ){//||ev->type() == QEvent::MouseButtonDblClick
        QMouseEvent *mouseev = static_cast<QMouseEvent *>(ev);
        if(mouseev->button() == Qt::LeftButton) {
            if(m_bGesture){
                if(ev->type() == QEvent::MouseButtonPress){
                    mStartPos=mouseev->pos();
                    bMouseM=true;
                }else if(ev->type() == QEvent::MouseButtonRelease){
                    if(bMouseM){
                        int distance = (mouseev->pos() - mStartPos).manhattanLength();
                        if (distance >= 50){//QApplication::startDragDistance()
                            qDebug("Eat Left button!");
                        }
                    }
                }
                return true;
            }else{
                if(ev->type() == QEvent::MouseButtonPress){
                    mStartPos=mouseev->pos();
                    //bPickerM=false;
                }else if(ev->type() == QEvent::MouseButtonRelease){
                    //if(!bMouseM){
                    int distance = (mouseev->pos() - mStartPos).manhattanLength();
                    if (distance < 5){//QApplication::startDragDistance()
                        emit selected(mCurrentX);//bPickerM=true;
                        //return true;
                    }
                    //else
                    //                            bPickerM=false;
                    //}
                }
                return false;//false控制鼠标启用
            }
        }else if(mouseev->button() == Qt::RightButton){
#if 1
            if(m_Histogram_plot!=nullptr){
                if(mPlotProp["RangeX"] == "auto" ||
                        mPlotProp["RangeX"].isEmpty())
                    setScale(QwtPlot::xBottom, mPublicProperty["RangeX"]);
                else
                    setScale(QwtPlot::xBottom, mPlotProp["RangeX"]);
                setScale(QwtPlot::yLeft, mPlotProp["RangeY"]);
                mScale=1;
                m_Histogram_plot->updateMarker();
            }
            return true;
#else
            return false;
#endif
        }else
            return false;
    }
    return false;
}

void sChartWidgetTouchScreen::setStyleM(TVPlot::MPlotStyle type, QString title, double xStart, double xEnd)
{
    sChartWidget::setStyleM(type, title, xStart, xEnd);
    mCurrentStartX=xStart;
    mCurrentEndX=xEnd;
}
