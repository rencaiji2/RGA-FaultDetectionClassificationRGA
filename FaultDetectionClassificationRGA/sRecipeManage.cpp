
#include "sRecipeManage.h"

#include <QDebug>
#include <QComboBox>
#include <QMenu>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
//#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "cSerializeJS.h"

sRecipeManage::sRecipeManage(/*cTcpClient* pTcpClient, */QString IP, QWidget *parent) :
    //mTcpClient(pTcpClient),
    mIP(IP),
    QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Recipe Manage");
    ui.UI_TW_RECIPE_TREE_RM->setColumnCount(3);
    ui.UI_TW_RECIPE_TREE_RM->setContextMenuPolicy(Qt::CustomContextMenu); // 启用自定义菜单
    connect(ui.UI_PB_RGA_RECIPE_UPDATE_RM, &QPushButton::clicked,
            this, &sRecipeManage::UI_PB_RGA_RECIPE_UPDATE_RM_clicked);
    //on_UI_PB_LOAD_RM_clicked();
}

sRecipeManage::~sRecipeManage()
{
    //delete ui;
}

//void sRecipeManage::on_UI_PB_OK_RM_clicked()
//{
//    mRecipeMap.clear();
//    for (int indexRGA = 0; indexRGA < ui.UI_TW_RECIPE_TREE_RM->topLevelItemCount(); ++indexRGA) {
//        QTreeWidgetItem *itemRGA= ui.UI_TW_RECIPE_TREE_RM->topLevelItem(indexRGA);
//        QString strRGA= itemRGA->text(0);
//        mRecipeMap[strRGA]= QMap<QString, QMap<QString, QString>>();
//        for (int indexRecipe = 0; indexRecipe < itemRGA->childCount(); ++indexRecipe) {
//            QTreeWidgetItem *itemRecipe= itemRGA->child(indexRecipe);
//            QComboBox *combo1 = qobject_cast<QComboBox*>(
//                        ui.UI_TW_RECIPE_TREE_RM->itemWidget(itemRecipe, 1));
//            QComboBox *combo2 = qobject_cast<QComboBox*>(
//                        ui.UI_TW_RECIPE_TREE_RM->itemWidget(itemRecipe, 2));
//            QString strRecipe= combo1->currentText();
//            mRecipeMap[strRGA][strRecipe]= QMap<QString, QString>();
//            mRecipeMap[strRGA][strRecipe][""]= combo2->currentText();
//            for (int indexStep = 0; indexStep < itemRecipe->childCount(); ++indexStep) {
//                QTreeWidgetItem *itemStep= itemRecipe->child(indexStep);
//                //mRecipeMap[strRGA][itemStep->text(1)]= itemStep->text(2);
//                QComboBox *combo = qobject_cast<QComboBox*>(
//                            ui.UI_TW_RECIPE_TREE_RM->itemWidget(itemStep, 2));
//                QLineEdit *lineEdit = qobject_cast<QLineEdit*>(
//                            ui.UI_TW_RECIPE_TREE_RM->itemWidget(itemStep, 1));
//                mRecipeMap[strRGA][strRecipe][lineEdit->text()]=combo->currentText();
//            }
//        }
//    }
//}

int getItemLevel(QTreeWidgetItem *item) {
    int level = 0;
    while (item->parent()) { // 父节点存在时继续循环
        item = item->parent();
        level++;
    }
    return level;
}
QString getIPAddress(QWidget *parent = nullptr) {
    IPInputDialog dialog;

    // 显示对话框并等待用户输入
    if (dialog.exec() == QDialog::Accepted) {
        QString ip = dialog.getIPAddress();
        if (!ip.isEmpty()) {
            return ip;
        } else {
            QMessageBox::warning(parent, "错误", "IP地址格式无效！");
            return QString(); // 返回空字符串表示无效
        }
    }
    return QString(); // 用户取消输入
}
void sRecipeManage::on_UI_TW_RECIPE_TREE_RM_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = ui.UI_TW_RECIPE_TREE_RM->itemAt(pos);
    QMenu menu;
    if (!item){
        QAction *addAction = menu.addAction("Add RGA");
        QAction *selectedAction = menu.exec(ui.UI_TW_RECIPE_TREE_RM->viewport()->mapToGlobal(pos));
        if(selectedAction == addAction){
            QString ip = getIPAddress();
            if (ip.isEmpty())
                return;
            QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui.UI_TW_RECIPE_TREE_RM);
            rootItem->setText(0, ip);
        }
        return;
    }
    int level= getItemLevel(item);
    if(level==0){
        //QMenu menu;
        QAction *addAction = menu.addAction("Add EQ Recipe");
        QAction *selectedAction = menu.exec(ui.UI_TW_RECIPE_TREE_RM->viewport()->mapToGlobal(pos));

        if(selectedAction == addAction){
            QTreeWidgetItem *childItem = new QTreeWidgetItem(item);
            childItem->setText(0, "Recipe");
            //childItem->setText(1, "文件");
            //childItem->setCheckState(0, Qt::Unchecked); // 添加复选框

            QComboBox *comboBox = new QComboBox();
            comboBox->addItems(mRecipeListEQ);
            ui.UI_TW_RECIPE_TREE_RM->setItemWidget(childItem, 1, comboBox);
            comboBox = new QComboBox();
            comboBox->addItems(mRecipeListRGA);
            ui.UI_TW_RECIPE_TREE_RM->setItemWidget(childItem, 2, comboBox);
        }
    }else if(level==1){
        QMenu menu;
        QAction *addAction = menu.addAction("Add EQ Step");
        QAction *deleteAction = menu.addAction("Delete EQ Recipe");
        QAction *selectedAction = menu.exec(ui.UI_TW_RECIPE_TREE_RM->viewport()->mapToGlobal(pos));

        if (selectedAction == deleteAction) {
            delete item;
        } else if(selectedAction == addAction){
            QTreeWidgetItem *childItem = new QTreeWidgetItem(item);
            childItem->setText(0, "Step");

            QLineEdit *LineEdit= new QLineEdit("0");
            QRegularExpression regExp("^[0-9]\\d*$"); // 正则表达式规则
            QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, LineEdit);
            LineEdit->setValidator(validator);
            ui.UI_TW_RECIPE_TREE_RM->setItemWidget(childItem, 1, LineEdit);
            QComboBox* comboBox = new QComboBox();
            comboBox->addItems(mRecipeListRGA);
            ui.UI_TW_RECIPE_TREE_RM->setItemWidget(childItem, 2, comboBox);
        }
    }else if(level==2){
        QMenu menu;
        QAction *deleteAction = menu.addAction("Delete EQ Step");
        QAction *selectedAction = menu.exec(ui.UI_TW_RECIPE_TREE_RM->viewport()->mapToGlobal(pos));
        if (selectedAction == deleteAction) {
            delete item;
        }
    }
}

void sRecipeManage::updateRecipeListRGA(const QStringList& pListRGA)
{
    mRecipeListRGA.clear();
    mRecipeListRGA<< pListRGA;

    QTreeWidget *pQTreeWidget= ui.UI_TW_RECIPE_TREE_RM;
    for (int indexRGA = 0; indexRGA < pQTreeWidget->topLevelItemCount(); ++indexRGA) {
        QTreeWidgetItem* pItemRGA= pQTreeWidget->topLevelItem(indexRGA);
        if(!pItemRGA)
            continue;
        for (int indexRecipe = 0; indexRecipe < pItemRGA->childCount(); ++indexRecipe) {
            QTreeWidgetItem* pRecipe= pItemRGA->child(indexRecipe);
            if(!pRecipe)
                continue;
            QComboBox *combo = qobject_cast<QComboBox*>(pQTreeWidget->itemWidget(pRecipe, 2));
            if (combo) {
                QString str= combo->currentText();
                combo->clear();
                combo->addItems(mRecipeListRGA);
                combo->setCurrentText(str);
            }
            for (int indexStep = 0; indexStep < pRecipe->childCount(); ++indexStep) {
                QTreeWidgetItem* pStep= pRecipe->child(indexStep);
                if(!pStep)
                    continue;
                QComboBox *combo1 = qobject_cast<QComboBox*>(pQTreeWidget->itemWidget(pStep, 2));
                if (combo1) {
                    QString str= combo->currentText();
                    combo1->clear();
                    combo1->addItems(mRecipeListRGA);
                    combo->setCurrentText(str);
                }
            }
        }
    }
}

bool sRecipeManage::getLocalRecipeListRGA(QStringList& pListRGA)
{
    pListRGA.clear();
    QString folderPath= QCoreApplication::applicationDirPath()+"/Recipe";
    QDir dir(folderPath);
    dir.setNameFilters(QStringList() << "*.tuneFile");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QFileInfoList files = dir.entryInfoList();
    if(files.isEmpty())
        return false;
    //QStringList tmpListRGA;
    foreach (const QFileInfo &fileInfo, files) {
        pListRGA<< fileInfo.fileName();
    }
    //updateRecipeRGA(ListRGA);
    return true;
}

void sRecipeManage::updateRecipeListEQ(const QStringList& pRecipeEQ)
{
    mRecipeListEQ.clear();
    mRecipeListEQ<< "NULL";
    mRecipeListEQ<< pRecipeEQ;

    QTreeWidget *pQTreeWidget= ui.UI_TW_RECIPE_TREE_RM;
    for (int indexRGA = 0; indexRGA < pQTreeWidget->topLevelItemCount(); ++indexRGA) {
        QTreeWidgetItem* pItemRGA= pQTreeWidget->topLevelItem(indexRGA);
        if(!pItemRGA)
            continue;
        for (int indexRecipe = 0; indexRecipe < pItemRGA->childCount(); ++indexRecipe) {
            QTreeWidgetItem* pRecipe= pItemRGA->child(indexRecipe);
            if(!pRecipe)
                continue;
            QComboBox *combo = qobject_cast<QComboBox*>(pQTreeWidget->itemWidget(pRecipe, 1));
            if (combo) {
                QString str= combo->currentText();
                combo->clear();
                combo->addItems(mRecipeListEQ);
                combo->setCurrentText(str);
            }
        }
    }
}

bool readFileByLine(const QString &filePath, QStringList& lineData)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "文件不存在："<< filePath;
        return false;
    }
    lineData.clear();
    int lineNumber = 0;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        lineNumber++;
        if (line.endsWith('\n')) {
            line.chop(1);
        }
        if (line.endsWith('\r')) {
            line.chop(1);
        }
        lineData<< QString::fromUtf8(line);
        //qDebug() << "Line" << lineNumber << ":" << lineStr;
    }
    file.close();
    return true;
}

bool sRecipeManage::getLocalRecipeListEQ(QStringList& pListEQ)
{
    pListEQ.clear();
    QDir dir(QCoreApplication::applicationDirPath()+"/Recipe");
    QString filePath = dir.filePath("RecipeListEQ");
    return readFileByLine(filePath, pListEQ);
}

void sRecipeManage::on_UI_PB_RECIPE_CHECK_clicked()
{
    if(ui.UI_PB_RECIPE_CHECK->text()== "RGA Recipe >>>"){
        ui.UI_PB_RECIPE_CHECK->setText("RGA Recipe <<<");
        emit sGetRecipe(mIP, true);
    }else{
        ui.UI_PB_RECIPE_CHECK->setText("RGA Recipe >>>");
        emit sGetRecipe(mIP, false);
    }
}
