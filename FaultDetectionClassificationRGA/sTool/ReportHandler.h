#ifndef REPORTHANDLER_H
#define REPORTHANDLER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "AlarmDbOperate.h"
#include "LibAlgorithm/PublicAlgorithm.h"

class ReportHandler : public QObject
{
    Q_OBJECT
public:
    explicit ReportHandler(QObject *parent = nullptr);
    ~ReportHandler();

signals:
    void sAlarmData(QVariant);
    void sLogMsg(QString);

public slots:
    void stopTimer()
    {
        if(m_timer->isActive()){
            m_timer->stop();
        }
    }

    void startTimer()
    {
        if(!m_timer->isActive()){
            m_timer->start(10000);
        }
    }

    void updateAlarmFlag(QVariantList i_alramInfo);
private slots:
    void onAlarmInfoProcessing();

private:
    AlarmDbOperate* m_dbOperate = nullptr;
    QTimer *m_timer = nullptr;

private:
    void init();
};

#endif // REPORTHANDLER_H
