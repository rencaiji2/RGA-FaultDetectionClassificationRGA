#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QReadWriteLock>
#include <QThreadStorage>
#include "ui_sDatabaseManager.h"

class sDatabaseManager : public QWidget
{
    Q_OBJECT
public:
    static sDatabaseManager& instance();
    bool executeQuery(const QString &queryStr, const QVariantList &params, QJsonArray &result);
    bool executeCommand(const QString &commandStr, const QVariantList &params, int &rowsAffected);

public:
    explicit sDatabaseManager(QWidget *parent = nullptr);
    ~sDatabaseManager();

private:
    Ui::sDatabaseManager ui;
    QSqlDatabase getConnection();
    void releaseConnection(QSqlDatabase &connection);

    QString m_databasePath;
    QReadWriteLock m_lock;
    QThreadStorage<QSqlDatabase> m_connectionPool;
};

