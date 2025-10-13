#include "sConfigEQ.h"

#include <QDomDocument>
#include <QFile>
#include <QDebug>

sConfigEQ::sConfigEQ(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    //saveChartFDC();

}

sConfigEQ::~sConfigEQ()
{

}

void sConfigEQ::on_UI_PB_ADD_DEVICE_clicked()
{
    //QString deviceName= ui.UI_LE_DEVICE_NAME->text();
    //QString Address= getIPAddress();
    //int rowCount= ui.UI_TW_DEVICE_CEQ->rowCount();
    //ui.UI_TW_DEVICE_CEQ->setRowCount(rowCount+ 1);
    //ui.UI_TW_DEVICE_CEQ->setItem(rowCount, 0, new QTableWidgetItem(Address));
    //ui.UI_TW_DEVICE_CEQ->setItem(rowCount, 1, new QTableWidgetItem(deviceName));
}

void sConfigEQ::on_UI_PB_LOCAL_ADDRESS_clicked()
{
    ui.UI_LE_IP0->setText("127");
    ui.UI_LE_IP1->setText("0");
    ui.UI_LE_IP2->setText("0");
    ui.UI_LE_IP3->setText("1");
}

void sConfigEQ::on_UI_PB_OK_clicked()
{
    emit sUpdateDeviceList();
}

bool sConfigEQ::saveChartFDC()
{
    QDomDocument doc;
    for(int i= 0; i< 1; ++i){
        QDomElement EquipmentGroup = doc.createElement("EquipmentGroup");
        EquipmentGroup.setAttribute("EquipmentGroupNo", QString::number(i));
        EquipmentGroup.setAttribute("EquipmentGroupIP", "127.0.0.1");
        EquipmentGroup.setAttribute("EquipmentGroupName", "");
        doc.appendChild(EquipmentGroup);
        for(int indexChamber= 0; indexChamber< 2; ++indexChamber){
            QDomElement Chamber = doc.createElement("Chamber"/*QString("Chamber%1").arg(indexChamber)*/);
            Chamber.setAttribute("ChamberIP",         "");
            if(indexChamber== 1)
                Chamber.setAttribute("ChamberName",       "TM1");
            else
                Chamber.setAttribute("ChamberName",       "CHE");
            QDomElement Channel0 = doc.createElement(QString("Channel"));
            Channel0.setAttribute("Name",  "N2/H2O");
            Channel0.setAttribute("Value",  "N2/H2O,M28/M18,N,N");
            Chamber.appendChild(Channel0);
            QDomElement Channel1 = doc.createElement(QString("Channel"));
            Channel1.setAttribute("Name",  "O2/H2O");
            Channel1.setAttribute("Value",   "O2/H2O,M32/M18,1,2");
            Chamber.appendChild(Channel1);
            QDomElement Channel2 = doc.createElement(QString("Channel"));
            Channel2.setAttribute("Name",  "N2/Ar");
            Channel2.setAttribute("Value",  "N2/Ar,M28/M40,N,N");
            Chamber.appendChild(Channel2);
            EquipmentGroup.appendChild(Chamber);
        }
    }
    QString filePath= QCoreApplication::applicationDirPath()+"/ChartFDC";
//    if (QFile::exists(filePath)) {
//        QFile::remove(filePath);
//    }
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        doc.save(out, 4);
        file.close();
    } else {
        qDebug() << "Cannot open file for writing";
    }
    return true;
}

bool sConfigEQ::loadChartFDC()
{
    ui.UI_TW_DEVICE_CEQ->clear();
    // 设置树控件的列数和标题
    ui.UI_TW_DEVICE_CEQ->setColumnCount(2);
    ui.UI_TW_DEVICE_CEQ->setHeaderLabels(QStringList() << " " << " ");

    QFile file(QCoreApplication::applicationDirPath() + "/ChartFDC");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading";
        return false;
    }
    QDomDocument doc;
    QString errorMsg;
        int errorLine, errorColumn;
    if (!doc.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << "XML parsing error at line" << errorLine << "column" << errorColumn;
                qDebug() << "Error message:" << errorMsg;
        file.close();
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        qDebug() << "XML document is empty";
        return false;
    }

    QDomNodeList groups = doc.elementsByTagName("EquipmentGroup");
    for (int i = 0; i < groups.count(); i++) {
        QDomElement groupElem = groups.at(i).toElement();
        if (groupElem.isNull()) continue;

        QTreeWidgetItem* groupItem = new QTreeWidgetItem(ui.UI_TW_DEVICE_CEQ);
        groupItem->setText(0, groupElem.attribute("EquipmentGroupName"));
        groupItem->setText(1, groupElem.attribute("EquipmentGroupIP"));
        ui.UI_TW_DEVICE_CEQ->addTopLevelItem(groupItem);

        QDomNodeList chambers = groupElem.childNodes();
        for (int j = 0; j < chambers.count(); j++) {
            QDomElement chamberElem = chambers.at(j).toElement();
            if (chamberElem.isNull() || (chamberElem.tagName()!="Chamber")/*!chamberElem.tagName().startsWith("Chamber")*/)
                continue;

            QTreeWidgetItem* chamberItem = new QTreeWidgetItem(groupItem);
            chamberItem->setText(0, chamberElem.attribute("ChamberName"));
            chamberItem->setText(1, chamberElem.attribute("ChamberIP"));
            //groupItem->addChild(chamberItem);

            QDomNodeList channels = chamberElem.childNodes();
            for (int k = 0; k < channels.count(); k++) {
                QDomElement channelElem = channels.at(k).toElement();
                if (channelElem.isNull()|| (channelElem.tagName()!="Channel"))
                    continue;

                QTreeWidgetItem* channelItem = new QTreeWidgetItem(chamberItem);
                channelItem->setText(0, channelElem.attribute("Name"));
                channelItem->setText(1, channelElem.attribute("Value"));
                //chamberItem->addChild(channelItem);
            }
        }
    }

    ui.UI_TW_DEVICE_CEQ->expandAll();
    emit sUpdateDeviceList();
    return true;
}
