#ifndef ALARMDBOPERATE_H
#define ALARMDBOPERATE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariantMap>
#include <QDateTime>
#include <QMutex>

class AlarmDbOperate : public QObject
{
    Q_OBJECT

public:
    // 单例模式
    static AlarmDbOperate* getInstance();
    static void destroyInstance();
    
    // 数据库连接管理
    bool connectDatabase(const QString& databasePath = QString());
    bool isConnected() const;
    QString lastError() const;
    
    // 数据库初始化
    bool initializeDatabase();
    
    // 插入操作
    bool insertAlarmRecord(const QVariantMap& recordData);
    int insertAlarmRecordAndGetId(const QVariantMap& recordData);
    
    // 更新操作
    bool updateAlarmRecord(int id, const QVariantMap& recordData);
    bool updateAlarmRecordByCondition(QString& o_sqlStr,const QString& condition, const QVariantMap& recordData);
    
    // 删除操作
    bool deleteAlarmRecord(int id);
    bool deleteAlarmRecords(const QString& condition);
    
    // 查询操作
    QVariantList selectAlarmRecords(QString& o_sqlStr,const QString& condition = QString(),
                                   const QStringList& fields = QStringList(),
                                   const QString& orderBy = QString(),int limit = 5,bool desc = true);
    
    QVariantMap selectAlarmRecordById(int id);
    
    // 批量操作
    bool batchInsertRecords(const QList<QVariantMap>& records);
    
    // 获取记录数量
    int getRecordCount(const QString& condition = QString());
    
    // 事务操作
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // 数据库信息
    QString getDatabasePath() const;
    QStringList getTableNames();

private:
    explicit AlarmDbOperate(QObject *parent = nullptr);
    ~AlarmDbOperate();
    
    // 私有成员变量
    QSqlDatabase m_database;
    QString m_lastError;
    QString m_databasePath;
    static AlarmDbOperate* s_instance;
    static QMutex s_mutex;
    
    // 内部辅助方法
    QString buildInsertQuery(const QVariantMap& data) const;
    QString buildUpdateQuery(const QVariantMap& data) const;
    QString buildSelectQuery(const QStringList& fields, const QString& condition, const QString& orderBy) const;
    
    void setLastError(const QString& error);
    bool executeQuery(QSqlQuery& query, const QString& sql, const QVariantMap& bindValues = QVariantMap());
};

#endif // ALARMDBOPERATE_H
