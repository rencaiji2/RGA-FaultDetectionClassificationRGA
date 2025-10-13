#include "sDefinePlotDataDlg.h"
#include "ui_sDefinePlotDataDlg.h"
#include "XMLReadHelp.h"

#include <QFont>
#include <QIcon>
#include <QMessageBox>
#include <QFile>

sDefinePlotDataDlg::sDefinePlotDataDlg(const QMap<QString, EquipmentGroupInfo> &i_optionMap,
                                       const QVariantMap &i_chamerIDConf,const QVariantMap& i_parentSysConf,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sDefinePlotDataDlg),m_optionMap(i_optionMap),
    m_chamberIDMap(i_chamerIDConf),m_parentSystemConFMap(i_parentSysConf)
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

    /*
    QList<QTreeWidgetItem*> selectItems = ui->treeWidget->selectedItems();
    if(selectItems.count() > 0){
        QTreeWidgetItem* item = selectItems.value(0);

        dataMap = item->data(0,Qt::UserRole + 1).toMap();

        dataMap["com_box"] = ui->comChamberID->currentText();
    }
    */

    QList<QTreeWidgetItem*> allItems = PublicDef::traverseTreeBFS(m_rootNode);
    foreach (QTreeWidgetItem* item, allItems) {
        QVariantMap infoMap = item->data(0,Qt::UserRole + 1).toMap();
        QString jsonStr = cJsonFileOperate::variant2Json(infoMap);

        qDebug().noquote() << "jsonStr: " << jsonStr;

        QString type = infoMap["type"].toString();
        bool chk = infoMap["chk"].toBool();
        if(type == "item" && chk){
            dataMap = item->data(0,Qt::UserRole + 1).toMap();
            //时间参数
            QDate startDate = ui->dateEditStart->date();
            QDate endDate = ui->dateEditEnd->date();
            dataMap["start_date"] = startDate;
            dataMap["end_date"] = endDate;
            dataMap["enable_date"] = ui->chk_enabledDate->isChecked();
            break;
        }
    }

    return dataMap;
}

void sDefinePlotDataDlg::initUI()
{
    //combobox
    QStringList chamberIDs = m_chamberIDMap.keys();
    ui->comChamberID->clear();
    ui->comChamberID->addItems(chamberIDs);
    ui->comChamberID->hide();//2025-1011 不显示，但是作为配置管理的一个输出保留这边，用于数据合理性判断
    ui->lb_chamberID->hide();

    //时间 - 默认最近3个月
    ui->dateEditStart->setDate(QDate::currentDate().addDays(-90));
    ui->dateEditEnd->setDate(QDate::currentDate());

    initTreeWgtFromReportConf();
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
    m_rootNode->setIcon(0,QIcon(":/icon/picture/database_40.png"));
    QVariantMap infoMap;
    infoMap["type"] = "root";
    infoMap["chk"] = false;
    m_rootNode->setData(0,Qt::UserRole + 1,infoMap);
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
        topLevelItem->setIcon(0,QIcon(":/icon/picture/folder_48.png"));
        infoMap.clear();
        infoMap["type"] = "top_level";
        infoMap["chk"] = false;
        topLevelItem->setData(0,Qt::UserRole + 1,infoMap);

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

            //补充2个信息
            channelMap["type"] = "item";
            channelMap["chk"] = false;

            cItem->setData(0,Qt::UserRole + 1,channelMap);
            cItem->setIcon(0,QIcon(":/icon/picture/doc_48.png"));
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

void sDefinePlotDataDlg::initTreeWgtFromReportConf()
{
    QString path = QCoreApplication::applicationDirPath() + "\\reportTreeConf.json";
    if(QFile::exists(path))
    {
        //独立配置 - 先满足北京逻辑
        QVariantMap confMap = cJsonFileOperate::jsonFile2QVarint(path).toMap();
        QStringList keysGrpName = confMap.keys();

        //创建节点
        ui->treeWidget->setColumnCount(2);
        ui->treeWidget->setHeaderLabels(QStringList() << " " << " ");

        QFont font;
        font.setFamily("Times");
        font.setPointSize(14);
        ui->treeWidget->setFont(font);

        //根节点
        m_rootNode = new QTreeWidgetItem(ui->treeWidget);
        m_rootNode->setText(0,"databaseRGA");
        m_rootNode->setText(1," ");
        m_rootNode->setIcon(0,QIcon(":/icon/picture/database_40.png"));
        QVariantMap infoMap;
        infoMap["type"] = "root";
        infoMap["chk"] = false;
        m_rootNode->setData(0,Qt::UserRole + 1,infoMap);
        ui->treeWidget->addTopLevelItem(m_rootNode);

        foreach (QString groupName, keysGrpName)
        {
            QVariantMap grpMap = confMap.value(groupName).toMap();
            QString groupIP = grpMap["ip"].toString();
            QStringList ChamberNameKeys = grpMap.keys();//有几个chamberName[需要去掉ip这个key]
            ChamberNameKeys.removeOne("ip");

            //即：local  testServer 这个等级节点
            QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(m_rootNode);
            topLevelItem->setText(0,groupName);
            topLevelItem->setText(1,groupIP);
            topLevelItem->setIcon(0,QIcon(":/icon/picture/folder_48.png"));
            infoMap.clear();
            infoMap["type"] = "top_level";
            infoMap["chk"] = false;
            topLevelItem->setData(0,Qt::UserRole + 1,infoMap);

            foreach (QString ChamberName , ChamberNameKeys)
            {
                //创建ChamberName节点【也是文件夹节点-会有子项】
                QTreeWidgetItem* chamberNameItem = new QTreeWidgetItem(topLevelItem);
                chamberNameItem->setText(0,ChamberName);
                chamberNameItem->setText(1," ");
                chamberNameItem->setIcon(0,QIcon(":/icon/picture/folder1_48.png"));
                infoMap.clear();
                infoMap["type"] = "chamber_name";
                infoMap["chk"] = false;
                chamberNameItem->setData(0,Qt::UserRole + 1,infoMap);

                //创建各自子项 - 具体可以被chk的条目
                QVariantList itemList = grpMap.value(ChamberName).toList();
                foreach (QVariant val, itemList)
                {
                    QVariantMap channelMap;
                    QVariantMap rowMap = val.toMap();
                    QString chamberName = rowMap["chamber_name"].toString();//chamberName
                    QString channelName = rowMap["channel_name"].toString();//name
                    QString chamberID = rowMap["chamber_id"].toString();

                    QTreeWidgetItem* cItem = new QTreeWidgetItem(chamberNameItem);
                    //cItem->setText(0,chamberName);
                    cItem->setText(0,"");

                    //显示前缀 - system.conf中配置
                    QString preStr = m_parentSystemConFMap.value("define_pre","From Defined mode").toString();

                    QString str = QString("%1 :%2,ChambersID: %3").arg(preStr)
                                    .arg(channelName).arg(chamberID);
                    cItem->setText(1,str);

                    //补充信息
                    channelMap["chamberID"] = chamberID;
                    channelMap["chamberName"] = chamberName;
                    channelMap["name"] = channelName;
                    channelMap["type"] = "item";
                    channelMap["chk"] = false;
                    channelMap["equipmentGroupName"] = groupName;
                    channelMap["equipmentGroupIP"] = groupIP;

                    cItem->setData(0,Qt::UserRole + 1,channelMap);
                    cItem->setIcon(0,QIcon(":/icon/picture/doc_48.png"));
                }
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
    else
    {
        //原始逻辑，基于主界面的chartFDC与配置管理chamberID那边
        initTreeWgt();
    }
}

void sDefinePlotDataDlg::on_btnOk_clicked()
{
    accept();
}

void sDefinePlotDataDlg::on_btnCancel_clicked()
{
    close();
}

void sDefinePlotDataDlg::on_btnChk_clicked()
{
    QList<QTreeWidgetItem*> selectedList = ui->treeWidget->selectedItems();
    if(selectedList.count() < 0){
        QMessageBox::warning(nullptr,tr("提示"),tr("请选择一个数据！"));
        return;
    }
    QList<QTreeWidgetItem*> allItems = PublicDef::traverseTreeBFS(m_rootNode);
    //取消全部的选择标志 - 实际就是换个icon与自定义值
    foreach (QTreeWidgetItem* item, allItems) {
        QVariantMap infoMap = item->data(0,Qt::UserRole + 1).toMap();
        QString type = infoMap["type"].toString();
        if(type == "item"){
            item->setIcon(0,QIcon(":/icon/picture/doc_48.png"));

            infoMap["chk"] = false;
            item->setData(0,Qt::UserRole + 1,infoMap);
        }
    }

    //当前的选中
    QTreeWidgetItem* curItem = selectedList.value(0);
    QVariantMap infoMap = curItem->data(0,Qt::UserRole + 1).toMap();
    QString type = infoMap["type"].toString();
    if(type == "item"){
        infoMap["chk"] = true;
        curItem->setIcon(0,QIcon(":/icon/picture/chkeck_48.png"));
        curItem->setData(0,Qt::UserRole + 1,infoMap);
    }
    else
    {
        QMessageBox::warning(nullptr,tr("警告"),tr("只能对具体的报告项进行选择！"));
    }
}

void sDefinePlotDataDlg::on_chk_enabledDate_clicked(bool checked)
{
    if(checked){
        ui->dateEditStart->setEnabled(true);
        ui->dateEditEnd->setEnabled(true);
    }
    else
    {
        ui->dateEditStart->setEnabled(false);
        ui->dateEditEnd->setEnabled(false);
    }
}
