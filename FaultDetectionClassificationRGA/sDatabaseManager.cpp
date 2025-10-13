#include "sDatabaseManager.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <qthread.h>

sDatabaseManager& sDatabaseManager::instance()
{
    static sDatabaseManager instance;
    return instance;
}

sDatabaseManager::sDatabaseManager(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    // 设置数据库路径
    m_databasePath = QDir::currentPath() + "/data.db";

    // 初始化数据库
        QSqlDatabase mainConnection = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
        mainConnection.setDatabaseName(m_databasePath);
        if (!mainConnection.open()) {
            //qCritical() << "Cannot open database:" << mainConnection.lastError().text();
            return;
        }

        // 启用WAL模式以提高并发性能
        QSqlQuery query(mainConnection);
        if (!query.exec("PRAGMA journal_mode=WAL;")) {
            //qWarning() << "Failed to enable WAL mode:" << query.lastError().text();
        }

        // 创建示例表（如果不存在）
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "email TEXT UNIQUE NOT NULL)");

        mainConnection.close();
}

sDatabaseManager::~sDatabaseManager()
{
    // 清理所有数据库连接
    QStringList connectionNames = QSqlDatabase::connectionNames();
    for (const QString &name : connectionNames) {
        QSqlDatabase::database(name).close();
        QSqlDatabase::removeDatabase(name);
    }
}

QSqlDatabase sDatabaseManager::getConnection()
{
    // 每个线程使用独立的数据库连接
    if (!m_connectionPool.hasLocalData()) {
        QString connectionName = QString("connection_%1").arg((quintptr)QThread::currentThread());
        QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        connection.setDatabaseName(m_databasePath);

        if (!connection.open()) {
            //qCritical() << "Cannot open database connection:" << connection.lastError().text();
            return QSqlDatabase();
        }

        m_connectionPool.setLocalData(connection);
    }

    return m_connectionPool.localData();
}

bool sDatabaseManager::executeQuery(const QString &queryStr, const QVariantList &params, QJsonArray &result)
{
    QWriteLocker locker(&m_lock);
    QSqlDatabase db = getConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare(queryStr);

    for (int i = 0; i < params.size(); ++i) {
        query.bindValue(i, params[i]);
    }

    if (!query.exec()) {
        //Warning() << "Query failed:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        QJsonObject recordObject;
        for (int i = 0; i < query.record().count(); ++i) {
            QString fieldName = query.record().fieldName(i);
            QVariant value = query.value(i);

            // 将QVariant转换为合适的JSON值
            if (value.isNull()) {
                recordObject[fieldName] = QJsonValue::Null;
            } else {
                switch (value.typeId()) {
                case QMetaType::Int:
                case QMetaType::LongLong:
                case QMetaType::Double:
                    recordObject[fieldName] = value.toDouble();
                    break;
                case QMetaType::Bool:
                    recordObject[fieldName] = value.toBool();
                    break;
                default:
                    recordObject[fieldName] = value.toString();
                }
            }
        }
        result.append(recordObject);
    }

    return true;
}

bool sDatabaseManager::executeCommand(const QString &commandStr, const QVariantList &params, int &rowsAffected)
{
    QWriteLocker locker(&m_lock);
    QSqlDatabase db = getConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare(commandStr);

    for (int i = 0; i < params.size(); ++i) {
        query.bindValue(i, params[i]);
    }

    if (!query.exec()) {
        qWarning() << "Command failed:" << query.lastError().text();
        return false;
    }

    rowsAffected = query.numRowsAffected();
    return true;
}
