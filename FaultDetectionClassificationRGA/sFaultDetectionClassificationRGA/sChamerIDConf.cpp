#include "sFaultDetectionClassificationRGA.h"

void sFaultDetectionClassificationRGA::loadChamerIDConf()
{
    QString path = QCoreApplication::applicationDirPath() + "\\chamerID_CL_Conf.json";
    m_chamerIDConfMap = cJsonFileOperate::jsonFile2QVarint(path).toMap();
    //转
    foreach (QString chamerID, m_chamerIDConfMap.keys()) {
        QVariantMap rowMap = m_chamerIDConfMap.value(chamerID).toMap();
        QMap<QString,QPair<QString,QString>> pairMap;
        foreach (QString item, rowMap.keys()) {
            QVariantMap valMap = rowMap.value(item).toMap();

            QPair<QString,QString> tmp = qMakePair(valMap["min"].toString(),valMap["max"].toString());
            pairMap.insert(item,tmp);

        }
        m_chamerIDConf.insert(chamerID,pairMap);
    }

    //界面赋值
    ui.tableWgt_ChmaerID->clearContents();

    int rowCount = m_chamerIDConf.keys().count();
    ui.tableWgt_ChmaerID->setRowCount(rowCount);

    int i = 0;
    foreach (QString chamerID, m_chamerIDConf.keys()) {
        QMap<QString,QPair<QString,QString>> pairMap = m_chamerIDConf.value(chamerID);

        QTableWidgetItem* idItem = new QTableWidgetItem();
        idItem->setText(chamerID);
        ui.tableWgt_ChmaerID->setItem(i,0,idItem);

        int col = 1;
        foreach (QString itemName, pairMap.keys())
        {
            QString min = pairMap.value(itemName).first;
            QString max = pairMap.value(itemName).second;

            QTableWidgetItem* nameItem = new QTableWidgetItem();
            QTableWidgetItem* minItem = new QTableWidgetItem();
            QTableWidgetItem* maxItem = new QTableWidgetItem();

            nameItem->setText(itemName);
            minItem->setText(min);
            maxItem->setText(max);

            ui.tableWgt_ChmaerID->setItem(i,col,nameItem);
            ui.tableWgt_ChmaerID->setItem(i,col + 1,minItem);
            ui.tableWgt_ChmaerID->setItem(i,col + 2,maxItem);
            col = col + 3;
        }
        i++;
    }

    //如果是北京版本就初始化这个
    if(m_version == 1){
        //初始化
        ui.UI_CB_CHAMBERID->clear();
        ui.UI_CB_CHAMBERID->addItem("ALL");
        ui.UI_CB_CHAMBERID->addItems(m_chamerIDConfMap.keys());
    }

}

void sFaultDetectionClassificationRGA::saveChamerIDConf()
{
    int rowCount = ui.tableWgt_ChmaerID->rowCount();
    if(rowCount <= 0) return;

    //检查是否为正确的格式
    //1.chamberID不得重复
    QStringList idList;
    for (int i =0;i < rowCount;i++) {
        QTableWidgetItem* item = ui.tableWgt_ChmaerID->item(i,0);
        if(item != nullptr){
            idList.append(item->text());
        }
    }

    bool hasDup = PublicDef::hasDuplicates(idList);
    if(hasDup){
        QMessageBox::warning(nullptr,tr("错误"),tr("存在ChamberID相同的行!请重新编辑!"));
        return;
    }

    //2.条目项必须符合实际

    QVariantMap saveMap;

    for (int i =0;i<rowCount;i++) {
        QVariantMap rowMap;
        QVariantMap tmpMap;
        QString chamerID = ui.tableWgt_ChmaerID->item(i,0)->text();

        QString O2_H2O = ui.tableWgt_ChmaerID->item(i,1)->text();
        QString lcl1 = ui.tableWgt_ChmaerID->item(i,2)->text();
        QString ucl1 = ui.tableWgt_ChmaerID->item(i,3)->text();

        tmpMap["min"] = lcl1;
        tmpMap["max"] = ucl1;
        rowMap.insert(O2_H2O,tmpMap);

        QString N2_H2O = ui.tableWgt_ChmaerID->item(i,4)->text();
        QString lcl2 = ui.tableWgt_ChmaerID->item(i,5)->text();
        QString ucl2 = ui.tableWgt_ChmaerID->item(i,6)->text();

        tmpMap["min"] = lcl2;
        tmpMap["max"] = ucl2;
        rowMap.insert(N2_H2O,tmpMap);

        QString N2_Ar = ui.tableWgt_ChmaerID->item(i,7)->text();
        QString lcl3 = ui.tableWgt_ChmaerID->item(i,8)->text();
        QString ucl3 = ui.tableWgt_ChmaerID->item(i,9)->text();

        tmpMap["min"] = lcl3;
        tmpMap["max"] = ucl3;
        rowMap.insert(N2_Ar,tmpMap);

        saveMap.insert(chamerID,rowMap);
    }

    updateChamerIDConf(saveMap);

    QString path = QCoreApplication::applicationDirPath() + "\\chamerID_CL_Conf.json";
    cJsonFileOperate::toJsonFile(saveMap,path);
}

void sFaultDetectionClassificationRGA::updateChamerIDConf(const QVariantMap &i_dataMap)
{
    m_chamerIDConfMap = i_dataMap;
    //转
    foreach (QString chamerID, m_chamerIDConfMap.keys()) {
        QVariantMap rowMap = m_chamerIDConfMap.value(chamerID).toMap();
        QMap<QString,QPair<QString,QString>> pairMap;
        foreach (QString item, rowMap.keys()) {
            QVariantMap valMap = rowMap.value(item).toMap();

            QPair<QString,QString> tmp = qMakePair(valMap["min"].toString(),valMap["max"].toString());
            pairMap.insert(item,tmp);

        }
        m_chamerIDConf.insert(chamerID,pairMap);
    }
}


void sFaultDetectionClassificationRGA::on_btnOpen_clicked()
{
    loadChamerIDConf();
}

void sFaultDetectionClassificationRGA::on_btnSave_clicked()
{
    saveChamerIDConf();
    QMessageBox::information(nullptr,tr("提示"),tr("保存成功！"));
}

void sFaultDetectionClassificationRGA::onConfTablecustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    QMenu intRstTMenu;

    QAction* copyAction = intRstTMenu.addAction(tr("追加一行"));
    connect(copyAction, &QAction::triggered, this, &sFaultDetectionClassificationRGA::onCopyAction);

    intRstTMenu.addSeparator();

    QAction* delAction = intRstTMenu.addAction(tr("删除"));
    connect(delAction, &QAction::triggered, this, &sFaultDetectionClassificationRGA::onDeleteAction);

    intRstTMenu.exec(QCursor::pos());
}

/*!
 * \brief sFaultDetectionClassificationRGA::onCopyAction 使用上一行的值，追加到下面，ID在后面加个copy
 */
void sFaultDetectionClassificationRGA::onCopyAction()
{
    int rowCount = ui.tableWgt_ChmaerID->rowCount();
    ui.tableWgt_ChmaerID->insertRow(rowCount);

    int lastIndex = rowCount - 1;
    //结合onDeleteAction ，肯定成立
    if(lastIndex >=0){
        QString chamerID = ui.tableWgt_ChmaerID->item(lastIndex,0)->text();
        QTableWidgetItem* idItem = new QTableWidgetItem();
        idItem->setText(chamerID + "_copy");
        ui.tableWgt_ChmaerID->setItem(rowCount,0,idItem);

        QString O2_H2O = ui.tableWgt_ChmaerID->item(lastIndex,1)->text();
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(O2_H2O);
        ui.tableWgt_ChmaerID->setItem(rowCount,1,item);

        QString lcl1 = ui.tableWgt_ChmaerID->item(lastIndex,2)->text();
        item = new QTableWidgetItem();
        item->setText(lcl1);
        ui.tableWgt_ChmaerID->setItem(rowCount,2,item);

        QString ucl1 = ui.tableWgt_ChmaerID->item(lastIndex,3)->text();
        item = new QTableWidgetItem();
        item->setText(ucl1);
        ui.tableWgt_ChmaerID->setItem(rowCount,3,item);

        QString N2_H2O = ui.tableWgt_ChmaerID->item(lastIndex,4)->text();
        item = new QTableWidgetItem();
        item->setText(N2_H2O);
        ui.tableWgt_ChmaerID->setItem(rowCount,4,item);

        QString lcl2 = ui.tableWgt_ChmaerID->item(lastIndex,5)->text();
        item = new QTableWidgetItem();
        item->setText(lcl2);
        ui.tableWgt_ChmaerID->setItem(rowCount,5,item);

        QString ucl2 = ui.tableWgt_ChmaerID->item(lastIndex,6)->text();
        item = new QTableWidgetItem();
        item->setText(ucl2);
        ui.tableWgt_ChmaerID->setItem(rowCount,6,item);

        QString N2_Ar = ui.tableWgt_ChmaerID->item(lastIndex,7)->text();
        item = new QTableWidgetItem();
        item->setText(N2_Ar);
        ui.tableWgt_ChmaerID->setItem(rowCount,7,item);

        QString lcl3 = ui.tableWgt_ChmaerID->item(lastIndex,8)->text();
        item = new QTableWidgetItem();
        item->setText(lcl3);
        ui.tableWgt_ChmaerID->setItem(rowCount,8,item);

        QString ucl3 = ui.tableWgt_ChmaerID->item(lastIndex,9)->text();
        item = new QTableWidgetItem();
        item->setText(ucl3);
        ui.tableWgt_ChmaerID->setItem(rowCount,9,item);
    }
}

void sFaultDetectionClassificationRGA::onDeleteAction()
{
    int rowCount = ui.tableWgt_ChmaerID->rowCount();
    if(rowCount == 1){
        QMessageBox::warning(nullptr,tr("错误"),tr("至少保留一行数据，无法删除！"));
        return;
    }
    int index = ui.tableWgt_ChmaerID->currentRow();
    if(index >= 0){
        ui.tableWgt_ChmaerID->removeRow(index);
    }
}

