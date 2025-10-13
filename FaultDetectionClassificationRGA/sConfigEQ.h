#ifndef SCONFIGEQ_H
#define SCONFIGEQ_H

#include <QWidget>
#include "ui_sConfigEQ.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>

class sConfigEQ : public QWidget
{
    Q_OBJECT

public:
    explicit sConfigEQ(QWidget *parent = nullptr);
    ~sConfigEQ();
    bool getParam(QStringList& ip, QStringList& name, QMap<QString, QStringList>& MapSV){
        ip.clear();
        name.clear();
        MapSV.clear();
//        int rowCount= ui.UI_TW_DEVICE_CEQ->rowCount();
//        for (int row= 0; row< rowCount; ++row) {
//            QString strIP= ui.UI_TW_DEVICE_CEQ->item(row, 0)->text();
//            ip<< strIP;
//            name<< ui.UI_TW_DEVICE_CEQ->item(row, 1)->text();
//            //ui.UI_TW_SVID1
//            QStringList listSV;
//            listSV<< "N2/H2O, 123456";
//            listSV<< "O2/H2O, 789101";
//            MapSV[strIP]= listSV;
//        }
        return true;
    }
bool getParam(QTreeWidget* treeWidget){
    treeWidget->clear();
    // 设置树控件的列数和标题
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderLabels(QStringList() << " ");

    int EquipmentGroupNum= ui.UI_TW_DEVICE_CEQ->topLevelItemCount();
    for(int indexEquipmentGroup= 0; indexEquipmentGroup< EquipmentGroupNum; ++indexEquipmentGroup){
        QTreeWidgetItem* pEquipmentGroup= ui.UI_TW_DEVICE_CEQ->topLevelItem(indexEquipmentGroup);
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(treeWidget);
        groupItem->setText(0, pEquipmentGroup->text(0));
        groupItem->setData(0, Qt::UserRole, pEquipmentGroup->text(1));
        treeWidget->addTopLevelItem(groupItem);

        int ChamberNum= pEquipmentGroup->childCount();
        for(int indexChamber= 0; indexChamber< ChamberNum; ++indexChamber){
            QTreeWidgetItem* pChamber= pEquipmentGroup->child(indexChamber);
            QTreeWidgetItem* chamberItem = new QTreeWidgetItem(groupItem);
            chamberItem->setText(0, pChamber->text(0));

            int ChannelNum= pChamber->childCount();
            for(int indexChannel= 0; indexChannel< ChannelNum; ++indexChannel){
                QTreeWidgetItem* pChannel= pChamber->child(indexChannel);
                QTreeWidgetItem* channelItem = new QTreeWidgetItem(chamberItem);
                channelItem->setText(0, pChannel->text(0));
                channelItem->setCheckState(0, Qt::Unchecked);
                channelItem->setData(0, Qt::UserRole, pChannel->text(1));
            }
        }
    }
    return true;
}

static void traverseItemCheckboxes(QTreeWidget* treeWidget, QMap<QString, QMap<QString, QStringList>>& pCurve, QMap<QString,QString>& pIP)
{
    int EquipmentGroupNum= treeWidget->topLevelItemCount();
    for(int indexEquipmentGroup= 0; indexEquipmentGroup< EquipmentGroupNum; ++indexEquipmentGroup){
        QTreeWidgetItem* pEquipmentGroup= treeWidget->topLevelItem(indexEquipmentGroup);

        int ChamberNum= pEquipmentGroup->childCount();
        for(int indexChamber= 0; indexChamber< ChamberNum; ++indexChamber){
            QTreeWidgetItem* pChamber= pEquipmentGroup->child(indexChamber);
            QStringList tmpList;
            int ChannelNum= pChamber->childCount();
            for(int indexChannel= 0; indexChannel< ChannelNum; ++indexChannel){
                QTreeWidgetItem* pChannel= pChamber->child(indexChannel);
                if (pChannel->flags() & Qt::ItemIsUserCheckable) {
                    if(pChannel->checkState(0)== Qt::Checked){
                        tmpList<< pChannel->data(0, Qt::UserRole).toString();
                    }
                }
            }
            if(!tmpList.isEmpty()){
            pCurve[pEquipmentGroup->text(0)][pChamber->text(0)]= tmpList;
            pIP[pEquipmentGroup->text(0)]= pEquipmentGroup->data(0, Qt::UserRole).toString();
            }
        }
    }
}

bool loadChartFDC();
signals:
    void sUpdateDeviceList();

private slots:
    void on_UI_PB_ADD_DEVICE_clicked();
    void on_UI_PB_LOCAL_ADDRESS_clicked();
    void on_UI_PB_OK_clicked();

private:
    Ui::sConfigEQ ui;
    QString getIPAddress() const {
        QStringList parts;
        parts << ui.UI_LE_IP0->text();
        parts << ui.UI_LE_IP1->text();
        parts << ui.UI_LE_IP2->text();
        parts << ui.UI_LE_IP3->text();
        return parts.join(".");
    }
    bool saveChartFDC();

};

#endif // SCONFIGEQ_H
