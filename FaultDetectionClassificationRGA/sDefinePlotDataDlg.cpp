#include "sDefinePlotDataDlg.h"
#include "ui_sDefinePlotDataDlg.h"
#include "XMLReadHelp.h"

#include <QFont>

sDefinePlotDataDlg::sDefinePlotDataDlg(const QMap<QString, EquipmentGroupInfo> &i_optionMap, const QVariantMap &i_chamerIDConf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sDefinePlotDataDlg),m_optionMap(i_optionMap),m_chamberIDMap(i_chamerIDConf)
{
    ui->setupUi(this);

    initUI();

    setWindowTitle(tr("导航"));
}

sDefinePlotDataDlg::~sDefinePlotDataDlg()
{
    delete ui;
}

QVariantMap sDefinePlotDataDlg::getData()
{
    QVariantMap dataMap;

    QList<QTreeWidgetItem*> selectItems = ui->treeWidget->selectedItems();
    if(selectItems.count() > 0){
        QTreeWidgetItem* item = selectItems.value(0);

        dataMap = item->data(0,Qt::UserRole + 1).toMap();

        dataMap["com_box"] = ui->comChamberID->currentText();
    }

    return dataMap;
}

void sDefinePlotDataDlg::initUI()
{
    //combobox
    QStringList chamberIDs = m_chamberIDMap.keys();
    ui->comChamberID->clear();
    ui->comChamberID->addItems(chamberIDs);

    initTreeWgt();
}

void sDefinePlotDataDlg::initTreeWgt()
{
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << " " << " ");

    QFont font;
    font.setFamily("Times");
    font.setPointSize(14);
    ui->treeWidget->setFont(font);

    m_rootNode = new QTreeWidgetItem(ui->treeWidget);
    m_rootNode->setText(0,"databaseRGA");
    m_rootNode->setText(1," ");
    ui->treeWidget->addTopLevelItem(m_rootNode);
    foreach (EquipmentGroupInfo grpInfo, m_optionMap.values())
    {
        QString groupName = grpInfo.groupName;
        QString groupIP = grpInfo.groupIP;
        QString groupNo = grpInfo.groupNo;
        QString groupRecipe = grpInfo.groupRecipe;
        QList<ChannelInfo> channels = grpInfo.channels;

        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(m_rootNode);
        topLevelItem->setText(0,groupName);
        topLevelItem->setText(1,groupIP);

        //子项
        foreach (ChannelInfo channel, channels) {

            ChannelValueInfo chValInfo = XMLReadHelp::parseChannelValue(channel.value);
            QVariantMap channelMap = channel.toVariantMap();
            channelMap["lcl"] = chValInfo.param1;
            channelMap["ucl"] = chValInfo.param2;

            QTreeWidgetItem* cItem = new QTreeWidgetItem(topLevelItem);
            cItem->setText(0,channel.chamberName);
            QString str = QString("From Defined mode:%1,Chambers Auto")
                            .arg(channel.name);
            cItem->setText(1,str);
            cItem->setData(0,Qt::UserRole + 1,channelMap);
        }
    }

    ui->treeWidget->expandAll();

    // 设置交替行颜色和网格
    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setStyleSheet(
        "QTreeWidget { alternate-background-color: #F5F5F5; }"
        "QTreeWidget::item { border-bottom: 1px solid #E0E0E0;height: 30px; }"
        "QTreeWidget::item:selected{background-color: #FFE4E1;color: white;}"
    );
    ui->treeWidget->setColumnWidth(0,150);
}

void sDefinePlotDataDlg::on_btnOk_clicked()
{
    accept();
}

void sDefinePlotDataDlg::on_btnCancel_clicked()
{
    close();
}
