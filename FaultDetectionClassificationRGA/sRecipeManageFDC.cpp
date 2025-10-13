#include "sRecipeManage.h"
#include "sRecipeManageFDC.h"
#include <QDomDocument>
#include <QFile>
#include <QTreeWidget>
#include <QDebug>
#include <QGroupBox>
#include <QSettings>

sRecipeManageFDC::sRecipeManageFDC(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    ui.UI_PB_SAVE_RGA_RECIPE->hide();
    ui.UI_W_RECIPE->hide();
    ui.splitter->setSizes({100, 100});
    ui.splitter_2->setSizes({100, 500});
    ui.UI_CB_RECIP_NAME->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    mConfigure= new sConfigure(cConfigRGA::_UNIT_GAUGE_TORR,cConfigRGA::_UNIT_GAUGE_TORR,this);
    ui.UI_LAYOUT_RGA_RECIPE->addWidget(mConfigure);
}

sRecipeManageFDC::~sRecipeManageFDC()
{

}

sRecipeManage* sRecipeManageFDC::getRecipeManage(const QString& EquipmentGroupIP, const QList<QGroupBox*>& pGroupBoxList,
                               QString& EquipmentGroupName){
    sRecipeManage* pRecipeManage= nullptr;
    for(auto& GroupBox: pGroupBoxList){
        if(!GroupBox)
            continue;
        pRecipeManage= (sRecipeManage*)(GroupBox->property("pRecipeManage").toInt());
        if(!pRecipeManage)
            continue;
        if(pRecipeManage->mIP==EquipmentGroupIP){
            EquipmentGroupName= GroupBox->title();
            return pRecipeManage;
        }
    }
    return nullptr;
}

bool sRecipeManageFDC::loadChartFDC()
{
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

    for(auto& GroupBox:mGroupBoxList){
        if(GroupBox){
            ui.UI_LAUOUT_DEVICE_LIST->removeWidget(GroupBox);
            delete GroupBox;
        }
        GroupBox= nullptr;
    }
    mGroupBoxList.clear();

    QDomNodeList groups = doc.elementsByTagName("EquipmentGroup");
    for (int i = 0; i < groups.count(); i++) {
        QDomElement groupElem = groups.at(i).toElement();
        if (groupElem.isNull())
            continue;
        QString EquipmentGroupIP= groupElem.attribute("EquipmentGroupIP");
        QString EquipmentGroupRecipe= groupElem.attribute("EquipmentGroupRecipe");
        QByteArray array= EquipmentGroupRecipe.toUtf8();
//        if("127.0.0.1"== ip){
//            QString path = QCoreApplication::applicationDirPath()+"/system.ini";
//            QSettings configIniRead(path, QSettings::IniFormat);
//            configIniRead.setIniCodec("utf-8");
//            mSTATE_RGA.LevelSoftware= configIniRead.value("/Config/LevelSoftware", 0).toUInt();
//            mSTATE_RGA.usingSEMI_XY= configIniRead.value("/Communication/SEMI_XY", false).toBool();
//            mSTATE_RGA.EQ= static_cast<_STATE_RGA::TYPE_EQ>(
//                        configIniRead.value("/Communication/EQ", _STATE_RGA::NULL_EQ).toUInt());
//            mSTATE_RGA.AD_SVID= configIniRead.value("/Communication/AD_SVID", 0).toUInt();
//            mSTATE_RGA.AD_CEID= configIniRead.value("/Communication/AD_CEID", 0).toUInt();
//            mSTATE_RGA.AD_ALID= configIniRead.value("/Communication/AD_ALID", 0).toUInt();
//        }
        QGroupBox *groupBox = new QGroupBox(groupElem.attribute("EquipmentGroupName"), this);
        mGroupBoxList<< groupBox;
        groupBox->setFlat(true); // 设置为非扁平样式，显示完整边框（可选）

        sRecipeManage* pRecipeManage= new sRecipeManage(EquipmentGroupIP, groupBox);
        groupBox->setProperty("pRecipeManage", (int)pRecipeManage);
        groupBox->setProperty("EquipmentGroupIP", EquipmentGroupIP);
        pRecipeManage->setToString(array);

        connect(pRecipeManage, &sRecipeManage::sGetRecipeListEQ, [this](QString ip){
            QString EquipmentGroupName;
            sRecipeManage* pRecipeManage= getRecipeManage(ip, mGroupBoxList, EquipmentGroupName);
            if(!pRecipeManage)
                return;
            if(ip== LOCAL_IP){
                QStringList ListEQ;
                pRecipeManage->getLocalRecipeListEQ(ListEQ);
                //pRecipeManage->updateRecipeListEQ(ListEQ);
                updateRecipeListEQ(ListEQ, pRecipeManage);
            }else{
                emit sGetRecipeListEQ(EquipmentGroupName, ip);
            }
        });
        connect(pRecipeManage, &sRecipeManage::sGetRecipeListRGA, [this](QString ip){
            QString EquipmentGroupName;
            sRecipeManage* pRecipeManage= getRecipeManage(LOCAL_IP, mGroupBoxList, EquipmentGroupName);
            if(!pRecipeManage)
                return;
            if(ip== LOCAL_IP){
                QStringList ListRGA;
                pRecipeManage->getLocalRecipeListRGA(ListRGA);
                //pRecipeManage->updateRecipeListRGA(ListRGA);
                updateRecipeListRGA(ListRGA, pRecipeManage);
            }else{
                emit sGetRecipeListRGA(EquipmentGroupName, ip);
            }
        });
        connect(pRecipeManage, &sRecipeManage::sSetRecipeMapping, [](QString ip, QByteArray str){

        });
        connect(pRecipeManage, &sRecipeManage::sGetRecipe, [this](QString ip, bool show){
            if(ip== LOCAL_IP){
                for(auto& GroupBox: mGroupBoxList){
                    if(!GroupBox)
                        continue;
                    if(GroupBox->property("EquipmentGroupIP").toString()== ip){
                        ui.UI_GB_GROUP_NAME->setTitle(GroupBox->title());
                        if(show){
                            ui.UI_W_RECIPE->show();
                        }else
                            ui.UI_W_RECIPE->hide();
                        break;
                    }
                }
            }else{

            }
        });

        QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);
        groupBoxLayout->addWidget(pRecipeManage);
        groupBox->setLayout(groupBoxLayout);
        ui.UI_LAUOUT_DEVICE_LIST->addWidget(groupBox);
    }

//    ui.UI_TW_DEVICE_CEQ->expandAll();
//    emit sUpdateDeviceList();
    return true;
}
void sRecipeManageFDC::onGetRecipeListEQ(QString& ip, QByteArray& array)
{
    QString EquipmentGroupName;
    sRecipeManage* pRecipeManage= getRecipeManage(ip, mGroupBoxList, EquipmentGroupName);
    if(!pRecipeManage)
        return;
    QStringList ListEQ;
    pRecipeManage->getLocalRecipeListEQ(ListEQ);
    //pRecipeManage->updateRecipeListEQ(ListEQ);
    updateRecipeListEQ(ListEQ, pRecipeManage);
}

void sRecipeManageFDC::on_UI_PB_SAVE_RM_FDC_clicked()
{
    QFile file(QCoreApplication::applicationDirPath() + "/ChartFDC");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading";
        return ;
    }
    QDomDocument doc;
    QString errorMsg;
        int errorLine, errorColumn;
    if (!doc.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << "XML parsing error at line" << errorLine << "column" << errorColumn;
                qDebug() << "Error message:" << errorMsg;
        file.close();
        return ;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        qDebug() << "XML document is empty";
        return ;
    }
    QDomNodeList groups = doc.elementsByTagName("EquipmentGroup");
    for (int i = 0; i < groups.count(); i++) {
        QDomElement groupElem = groups.at(i).toElement();
        if (groupElem.isNull())
            continue;
        QString EquipmentGroupIP= groupElem.attribute("EquipmentGroupIP");
        //QString EquipmentGroupRecipe= groupElem.attribute("EquipmentGroupRecipe");
        //array;//= EquipmentGroupRecipe.toUtf8();
        QString EquipmentGroupName;
        sRecipeManage* pRecipeManage= getRecipeManage(EquipmentGroupIP, mGroupBoxList, EquipmentGroupName);
        if(!pRecipeManage)
            continue;
        QByteArray array= pRecipeManage->getFromString();
        groupElem.setAttribute("EquipmentGroupRecipe", QString::fromUtf8(array));
    }
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        doc.save(out, 4);
        file.close();
    } else {
        qDebug() << "Cannot open file for writing";
    }
}

void sRecipeManageFDC::on_UI_PB_EQ_RECIPE_LIST_clicked()
{

}

void sRecipeManageFDC::on_UI_PB_RGA_RECIPE_LIST_clicked()
{

}

void sRecipeManageFDC::on_UI_PB_LOAD_RGA_RECIPE_clicked()
{
    for(auto& GroupBox: mGroupBoxList){
        if(!GroupBox)
            continue;
        if(GroupBox->property("EquipmentGroupIP").toString()== LOCAL_IP){
            ui.UI_GB_GROUP_NAME->setTitle(GroupBox->title());
            QString fileName= ui.UI_CB_RECIP_NAME->currentText();
            QDir dir(QCoreApplication::applicationDirPath()+"/Recipe");
            QString filePath = dir.filePath(fileName);
            if(!QFile::exists(filePath)){
                qWarning() << "文件不存在："<< filePath;
                return;
            }
            mConfigure->loadTuneFile(filePath);
            //mConfigure->loadTuneFile(bool ifUpdateUI=false, QString qPath=nullptr);
        }else{

        }
    }
//mConfigure->loadTuneFile(bool ifUpdateUI=false, QString qPath=nullptr);
}

void sRecipeManageFDC::on_UI_PB_SAVE_RGA_RECIPE_clicked()
{

}
