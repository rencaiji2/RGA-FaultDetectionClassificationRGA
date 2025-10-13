#include "cSerializeJS.h"

#include <QComboBox>
#include <QLineEdit>


cSerializeJS::cSerializeJS()
{

}

QJsonObject cSerializeJS::serializeItem(QTreeWidget *pTreeWidget, QTreeWidgetItem *item, int currentLevel)
{
    QJsonObject obj;
    obj["level"] = currentLevel;

    QJsonObject widgets;
    for (int col = 0; col < pTreeWidget->columnCount(); ++col) {
        QWidget *widget = pTreeWidget->itemWidget(item, col);
        if (QComboBox *combo = qobject_cast<QComboBox*>(widget)) {
            widgets[QString::number(col)] = combo->currentText();
        } else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget)) {
            widgets[QString::number(col)] = lineEdit->text();
        } else {
            widgets[QString::number(col)] = item->text(col);
        }
    }
    if (!widgets.isEmpty()) {
        obj["widgets"] = widgets;
    }

    QJsonArray children;
    for (int i = 0; i < item->childCount(); ++i) {
        children.append(serializeItem(pTreeWidget, item->child(i), currentLevel + 1));
    }
    if (!children.isEmpty()) {
        obj["children"] = children;
    }

    return obj;
}

QByteArray cSerializeJS::saveTreeToString(QTreeWidget *tree)
{
    QJsonArray rootArray;
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        rootArray.append(serializeItem(tree, tree->topLevelItem(i), 0));
    }
    QJsonDocument doc(rootArray);
    return doc.toJson(QJsonDocument::Compact);
}

bool cSerializeJS::saveTreeToFile(QTreeWidget *tree, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(saveTreeToString(tree));
    return true;
}

QTreeWidgetItem* cSerializeJS::deserializeItem(QTreeWidget *pTreeWidget,
                                               QTreeWidgetItem *item,
                                               const QJsonObject &obj)
{
    int level = obj["level"].toInt();
    if (obj.contains("widgets")) {
        QJsonObject widgets = obj["widgets"].toObject();
        for (auto key : widgets.keys()) {
            int col = key.toInt();
            QString value = widgets[key].toString();
            if (level == 0) {
                item->setText(col, value);
                //pTreeWidget->addTopLevelItem(item);
            }else if (level == 1) {  // 配方节点
                if (col == 0) {  // EQ Recipe 列
                    item->setText(col, value);
                } else if (col == 1) {  // EQ Recipe 列
                    QComboBox *combo = new QComboBox();
                    combo->addItem(value);//combo->addItems(mRecipeListEQ);
                    combo->setCurrentText(value);
                    pTreeWidget->setItemWidget(item, col, combo);
                } else if (col == 2) {  // RGA 列
                    QComboBox *combo = new QComboBox();
                    combo->addItem(value);//combo->addItems(mRecipeListRGA);
                    combo->setCurrentText(value);
                    pTreeWidget->setItemWidget(item, col, combo);
                }
            } else if (level == 2) {  // step
                if (col == 0) {  // EQ Recipe 列
                    item->setText(col, value);
                } else if (col == 1) {  // 步骤序号列
                    QLineEdit *lineEdit = new QLineEdit(value);
                    QRegularExpression regExp("^[0-9]\\d*$");
                    lineEdit->setValidator(new QRegularExpressionValidator(regExp, lineEdit));
                    pTreeWidget->setItemWidget(item, col, lineEdit);
                } else if (col == 2) {  // RGA 列
                    QComboBox *combo = new QComboBox();
                    combo->addItem(value);//
                    combo->setCurrentText(value);
                    pTreeWidget->setItemWidget(item, col, combo);
                    //item->setText(col, value);


                }
            }
        }
    }

    if (obj.contains("children")) {
        QJsonArray children = obj["children"].toArray();
        for (const QJsonValue &child : children) {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(item);
            item->addChild(childitem);
            deserializeItem(pTreeWidget, childitem, child.toObject());
        }
    }

    return item;
}

bool cSerializeJS::loadTreeFromString(QTreeWidget *tree, const QByteArray& array)
{
    if((!tree)||(array.isEmpty()))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(array);
    if (doc.isNull())
        return false;

    tree->clear();
    tree->setColumnCount(3);
    QJsonArray rootArray = doc.array();
    for (const QJsonValue &rootVal : rootArray) {
        QTreeWidgetItem *rootItem = new QTreeWidgetItem(tree);
        deserializeItem(tree, rootItem, rootVal.toObject());
//        tree->addTopLevelItem(rootItem);
    }
    tree->expandAll();
    return true;
}

bool cSerializeJS::loadTreeFromFile(QTreeWidget *tree, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray array= file.readAll();
    return loadTreeFromString(tree, array);
}
