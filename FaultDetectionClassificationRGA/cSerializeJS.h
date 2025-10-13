#pragma once

#include <QTreeWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class cSerializeJS
{
public:
    cSerializeJS();
    static bool saveTreeToFile(QTreeWidget *tree, const QString &filename);
    static QByteArray saveTreeToString(QTreeWidget *tree);
    static bool loadTreeFromFile(QTreeWidget *tree, const QString &filename);
    static bool loadTreeFromString(QTreeWidget *tree, const QByteArray& array);

private:
    static QJsonObject serializeItem(QTreeWidget *pTreeWidget, QTreeWidgetItem *item, int currentLevel);
    static QTreeWidgetItem* deserializeItem(QTreeWidget *pTreeWidget, QTreeWidgetItem *item, const QJsonObject &obj);
};
