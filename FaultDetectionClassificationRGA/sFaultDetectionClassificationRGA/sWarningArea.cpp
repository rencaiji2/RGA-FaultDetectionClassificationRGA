#include "sFaultDetectionClassificationRGA.h"

void sFaultDetectionClassificationRGA::drawAlarmArea()
{
    PublicDef* pDef = sPublicDefSingleton::GetInstance();
    //放大衍生区域
    double maxY = pDef->m_max_y;
    double minY = pDef->m_min_y;

    //告警区域绘制
    m_chartWgt->initializeAreaSeries();
    // 设置区域值
    double upBase = m_alarmUpValue;    // 上部区域基线
    double upLine = maxY;   // 上部区域线
    double downBase = m_alarmDownValue; // 下部区域基线
    double downLine = minY;  // 下部区域线

    qDebug().noquote() << QString("upBase:%1,upLine:%2,downBase:%3,downLine:%4")
                          .arg(upBase).arg(upLine).arg(downBase).arg(downLine);

    m_chartWgt->updateAreaValues(upBase, upLine, downBase, downLine);
    m_chartWgt->setShowAreas(false,true);
}
