#pragma once

#include "sChartWidget.h"


class sChartWidgetTouchScreen:public sChartWidget
{
    Q_OBJECT
public:
    explicit sChartWidgetTouchScreen(_STYLE_CHART _STYLE_CHART1, QWidget *parent= nullptr,
                          QWidget* pMenu= nullptr);
    ~sChartWidgetTouchScreen();
    void setStyleM(TVPlot::MPlotStyle type, QString title, double xStart,double xEnd);

private://触摸屏
    bool m_bGesture=false;
    bool bMouseM=false;
    //QPoint mStartPos;
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
};
