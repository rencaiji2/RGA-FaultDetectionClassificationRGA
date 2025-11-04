#include "AlarmDbOperate.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

// 静态成员初始化
AlarmDbOperate* AlarmDbOperate::s_instance = nullptr;
QMutex AlarmDbOperate::s_mutex;

AlarmDbOperate::AlarmDbOperate(QObject *parent)
    : QObject(parent)
{
    // 创建默认数据库连接
    QString connectionName = QString("alarm_db_connection_%1").arg((quintptr)QThread::currentThread());
    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
}

AlarmDbOperate::~AlarmDbOperate()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

AlarmDbOperate* AlarmDbOperate::getInstance()
{
    if (s_instance == nullptr) {
        s_mutex.lock();
        if (s_instance == nullptr) {
            s_instance = new AlarmDbOperate();
        }
        s_mutex.unlock();
    }
    return s_instance;
}

void AlarmDbOperate::destroyInstance()
{
    s_mutex.lock();
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
    s_mutex.unlock();
}

bool AlarmDbOperate::connectDatabase(const QString& databasePath)
{
    QString path = databasePath;
    
    // 如果没有指定路径，使用默认路径
    if (path.isEmpty()) {
        path = QCoreApplication::applicationDirPath() + "/../keyTrailRGA";
    }
    
    m_databasePath = path;
    m_database.setDatabaseName(path);
    
    if (!m_database.open()) {
        setLastError(m_database.lastError().text());
        return false;
    }
    
    return true;
}

bool AlarmDbOperate::isConnected() const
{
    return m_database.isOpen();
}

QString AlarmDbOperate::lastError() const
{
    return m_lastError;
}

bool AlarmDbOperate::initializeDatabase()
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    QSqlQuery query(m_database);
    
    // 创建报警记录表
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS alarm_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            alarm_time DATETIME NOT NULL,
            alarm_type TEXT,
            equipment_name TEXT,
            chamber_id TEXT,
            alarm_level INTEGER,
            alarm_message TEXT,
            status INTEGER DEFAULT 0,
            handled_by TEXT,
            handled_time DATETIME,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createTableSql)) {
        setLastError(query.lastError().text());
        return false;
    }
    
    // 创建索引
    QStringList indexes = {
        "CREATE INDEX IF NOT EXISTS idx_alarm_time ON alarm_records(alarm_time)",
        "CREATE INDEX IF NOT EXISTS idx_equipment_name ON alarm_records(equipment_name)",
        "CREATE INDEX IF NOT EXISTS idx_chamber_id ON alarm_records(chamber_id)",
        "CREATE INDEX IF NOT EXISTS idx_alarm_level ON alarm_records(alarm_level)",
        "CREATE INDEX IF NOT EXISTS idx_status ON alarm_records(status)"
    };
    
    for (const QString& indexSql : indexes) {
        if (!query.exec(indexSql)) {
            setLastError(query.lastError().text());
            return false;
        }
    }
    
    return true;
}

bool AlarmDbOperate::insertAlarmRecord(const QVariantMap& recordData)
{
    return insertAlarmRecordAndGetId(recordData) > 0;
}

int AlarmDbOperate::insertAlarmRecordAndGetId(const QVariantMap& recordData)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return -1;
    }
    
    QSqlQuery query(m_database);
    
    // 构建字段和值
    QStringList fields;
    QStringList placeholders;
    QVariantMap bindValues;
    
    // 添加默认字段
    QVariantMap data = recordData;
    if (!data.contains("alarm_time")) {
        data["alarm_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if (!data.contains("created_at")) {
        data["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if (!data.contains("updated_at")) {
        data["updated_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        fields.append(it.key());
        placeholders.append(":" + it.key());
        bindValues[":" + it.key()] = it.value();
    }
    
    QString sql = QString("INSERT INTO alarm_records (%1) VALUES (%2)")
                  .arg(fields.join(", "))
                  .arg(placeholders.join(", "));
    
    if (executeQuery(query, sql, bindValues)) {
        return query.lastInsertId().toInt();
    }
    
    return -1;
}

bool AlarmDbOperate::updateAlarmRecord(int id, const QVariantMap& recordData)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    // 添加更新时间
    QVariantMap data = recordData;
    data["updated_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    
    QSqlQuery query(m_database);
    QStringList setClauses;
    QVariantMap bindValues;
    
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        setClauses.append(it.key() + " = :" + it.key());
        bindValues[":" + it.key()] = it.value();
    }
    
    bindValues[":id"] = id;
    
    QString sql = QString("UPDATE alarm_records SET %1 WHERE id = :id")
                  .arg(setClauses.join(", "));
    
    return executeQuery(query, sql, bindValues);
}

bool AlarmDbOperate::updateAlarmRecordByCondition(QString &o_sqlStr, const QString& condition, const QVariantMap& recordData)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    if (condition.isEmpty()) {
        setLastError("更新条件不能为空");
        return false;
    }
    
    QSqlQuery query(m_database);
    QStringList setClauses;
    QVariantMap bindValues;
    
    for (auto it = recordData.constBegin(); it != recordData.constEnd(); ++it) {
        setClauses.append(it.key() + " = :" + it.key());
        bindValues[":" + it.key()] = it.value();
    }
    
    QString sql = QString("UPDATE AlarmRecords SET %1 WHERE %2")
                  .arg(setClauses.join(", "))
                  .arg(condition);

    o_sqlStr = sql;
    
    return executeQuery(query, sql, bindValues);
}

bool AlarmDbOperate::deleteAlarmRecord(int id)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    QSqlQuery query(m_database);
    QString sql = "DELETE FROM alarm_records WHERE id = :id";
    QVariantMap bindValues;
    bindValues[":id"] = id;
    
    return executeQuery(query, sql, bindValues);
}

bool AlarmDbOperate::deleteAlarmRecords(const QString& condition)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    if (condition.isEmpty()) {
        setLastError("删除条件不能为空");
        return false;
    }
    
    QSqlQuery query(m_database);
    QString sql = QString("DELETE FROM alarm_records WHERE %1").arg(condition);
    
    return executeQuery(query, sql);
}

QVariantList AlarmDbOperate::selectAlarmRecords(QString &o_sqlStr, const QString& condition,
                                               const QStringList& fields,
                                               const QString& orderBy, int limit, bool desc)
{
    QVariantList result;
    
    if (!isConnected()) {
        setLastError("数据库未连接");
        return result;
    }
    
    QSqlQuery query(m_database);
    QStringList fieldList = fields;
    
    if (fieldList.isEmpty()) {
        fieldList << "*";
    }
    
    QString sql = QString("SELECT %1 FROM AlarmRecords")
                  .arg(fieldList.join(", "));
    
    if (!condition.isEmpty()) {
        sql += " WHERE " + condition;
    }
    
    if (!orderBy.isEmpty()) {
        sql += " ORDER BY " + orderBy;
    }

    if(desc){
        sql += " DESC ";
    }
    else
    {
        sql += " ASC ";
    }

    if(limit > 0){
        sql += QString(" LIMIT %1 ").arg(limit);
    }

    o_sqlStr = sql;
    qDebug().noquote() << "sqlStr:  " << sql;
    
    if (!executeQuery(query, sql)) {
        return result;
    }
    
    while (query.next()) {
        QVariantMap record;
        for (int i = 0; i < query.record().count(); ++i) {
            record[query.record().fieldName(i)] = query.value(i);
        }
        result.append(record);
    }
    
    return result;
}

QVariantMap AlarmDbOperate::selectAlarmRecordById(int id)
{
    QVariantMap result;
    
    if (!isConnected()) {
        setLastError("数据库未连接");
        return result;
    }
    
    QSqlQuery query(m_database);
    QString sql = "SELECT * FROM alarm_records WHERE id = :id";
    QVariantMap bindValues;
    bindValues[":id"] = id;
    
    if (executeQuery(query, sql, bindValues) && query.next()) {
        for (int i = 0; i < query.record().count(); ++i) {
            result[query.record().fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

bool AlarmDbOperate::batchInsertRecords(const QList<QVariantMap>& records)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    if (records.isEmpty()) {
        return true;
    }
    
    // 开始事务
    if (!beginTransaction()) {
        return false;
    }
    
    bool success = true;
    for (const QVariantMap& record : records) {
        if (!insertAlarmRecord(record)) {
            success = false;
            break;
        }
    }
    
    if (success) {
        success = commitTransaction();
    } else {
        rollbackTransaction();
    }
    
    return success;
}

int AlarmDbOperate::getRecordCount(const QString& condition)
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return -1;
    }
    
    QSqlQuery query(m_database);
    QString sql = "SELECT COUNT(*) FROM alarm_records";
    
    if (!condition.isEmpty()) {
        sql += " WHERE " + condition;
    }
    
    if (executeQuery(query, sql) && query.next()) {
        return query.value(0).toInt();
    }
    
    return -1;
}

bool AlarmDbOperate::beginTransaction()
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    return m_database.transaction();
}

bool AlarmDbOperate::commitTransaction()
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    return m_database.commit();
}

bool AlarmDbOperate::rollbackTransaction()
{
    if (!isConnected()) {
        setLastError("数据库未连接");
        return false;
    }
    
    return m_database.rollback();
}

QString AlarmDbOperate::getDatabasePath() const
{
    return m_databasePath;
}

QStringList AlarmDbOperate::getTableNames()
{
    QStringList tables;
    
    if (!isConnected()) {
        setLastError("数据库未连接");
        return tables;
    }
    
    return m_database.tables();
}

QString AlarmDbOperate::buildInsertQuery(const QVariantMap& data) const
{
    Q_UNUSED(data)
    return QString();
}

QString AlarmDbOperate::buildUpdateQuery(const QVariantMap& data) const
{
    Q_UNUSED(data)
    return QString();
}

QString AlarmDbOperate::buildSelectQuery(const QStringList& fields, 
                                        const QString& condition, 
                                        const QString& orderBy) const
{
    Q_UNUSED(fields)
    Q_UNUSED(condition)
    Q_UNUSED(orderBy)
    return QString();
}

void AlarmDbOperate::setLastError(const QString &error)
{
    m_lastError = error;
    qDebug() << "AlarmDbOperate Error:" << error;
}

bool AlarmDbOperate::executeQuery(QSqlQuery& query, const QString& sql, const QVariantMap& bindValues)
{
    query.prepare(sql);
    
    // 绑定参数
    for (auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }
    
    if (!query.exec()) {
        setLastError(query.lastError().text());
        return false;
    }
    
    return true;
}
