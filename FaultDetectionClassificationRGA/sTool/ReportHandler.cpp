#include "ReportHandler.h"

ReportHandler::ReportHandler(QObject *parent) : QObject(parent)
{
    init();
}

ReportHandler::~ReportHandler()
{
    m_timer->deleteLater();
    m_timer = nullptr;
}

void ReportHandler::updateAlarmFlag(QVariantList i_alramInfo)
{
    QStringList ids;
    foreach (QVariant val, i_alramInfo) {
        QVariantMap map = val.toMap();
        QString id = map["id"].toString();
        ids.append(id);
    }

    //执行更新
    QString o_sql = "";
    QString where = QString("id IN (%1)").arg(ids.join(","));
    QVariantMap dataMap;
    dataMap["mailSendFlag"] = "1";
    m_dbOperate->updateAlarmRecordByCondition(o_sql,where,dataMap);

    emit sLogMsg(o_sql);
}

void ReportHandler::onAlarmInfoProcessing()
{
    QString where = QString(" mailSendFlag is null ");
    QString o_sqlStr = "";
    QStringList fields;
    QVariantList dataList = m_dbOperate->selectAlarmRecords(o_sqlStr,where,fields,"id");
    emit sLogMsg(o_sqlStr);

    int count = dataList.count();
    if(count > 0){
        QString jsonStr = cJsonFileOperate::variant2Json(dataList);
        emit sLogMsg(QString("dataList-count:%1").arg(count));
        //emit sLogMsg(jsonStr);

        stopTimer();//有告警就停止，检测【主界面发送完邮件后会再次唤醒】
        emit sAlarmData(QVariant(dataList));
    }
}

void ReportHandler::init()
{
    // 获取数据库操作实例
    m_dbOperate = AlarmDbOperate::getInstance();

    // 1. 连接数据库
    qDebug() << "=== 连接数据库 ===";
    if (!m_dbOperate->connectDatabase()) {
        qDebug() << "连接数据库失败:" << m_dbOperate->lastError();
        return;
    }
    qDebug() << "数据库连接成功";

    // 2. 初始化数据库表
    qDebug() << "=== 初始化数据库 ===";
    if (!m_dbOperate->initializeDatabase()) {
        qDebug() << "初始化数据库失败:" << m_dbOperate->lastError();
        return;
    }
    qDebug() << "数据库初始化成功";

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onAlarmInfoProcessing()));
    m_timer->start(10000);
}
