#include "sFileManager.h"
#include "ui_sFileManager.h"
#include <QDebug>
#include <QCheckBox>
#include <QDir>
#include "sTool/cFileOperate.h"

sFileManager::sFileManager(QWidget *parent, _STYLE_WIDGET style) :
    QWidget(parent),
    mSTYLE_WIDGET(style),
    ui(new Ui::sFileManager)
{
    ui->setupUi(this);
    switch (mSTYLE_WIDGET) {
        case(_STYLE_CHECKBOX): ui->UI_PB_DELETE_TW->show(); break;
        default: ui->UI_PB_DELETE_TW->hide(); break;
    }


    ui->UI_PB_OPEN_TW->setEnabled(false);
    ui->UI_PB_RETURN_TW->hide();
    //ui->scrollArea->hide();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setShowGrid(false);
//    ui->tableWidget->setStyleSheet("QTableWidget {border:none;background: #E4DAD0;gridline-color: #3c3c3c;}\
//                                   QTableWidget::item {padding-left: 5px;padding-right: 5px;border: none;\
//                                                       background: #E4DAD0;border:none;}\
//                                   QTableWidget::item:selected {background: #3c3c3c;color:#E4DAD0);}\
//                                   QTableWidget::item:selected:!active {color: white;}");
}

sFileManager::~sFileManager()
{
    delete ui;
}

void sFileManager::updata(const QString& dirPath, bool updataRoot)
{
    mCurrentDir= dirPath.split("/");
    for(int i= 0; i< mCurrentDir.size(); ++i){
        if(mCurrentDir[i].isEmpty()){
            mCurrentDir.removeAt(i);
            --i;
        }
    }
    if(updataRoot)
        mRootDir= mCurrentDir;
    QStringList filters;
    //filters << "*.txt" << "*.jpg";
    GetFileInfoList(dirPath, mFilters);
    showFileList();
    ui->UI_PB_OPEN_TW->setEnabled(false);
    if(mRootDir.join("/")== dirPath)
        ui->UI_PB_RETURN_TW->hide();
}

void sFileManager::updata(const QString& dirPath, QStringList& filters, bool updataRoot)
{
    mCurrentDir= dirPath.split("/");
    for(int i= 0; i< mCurrentDir.size(); ++i){
        if(mCurrentDir[i].isEmpty()){
            mCurrentDir.removeAt(i);
            --i;
        }
    }
    if(updataRoot)
        mRootDir= mCurrentDir;
    mFilters= filters;
    //filters << "*.txt" << "*.jpg";
    GetFileInfoList(dirPath, filters);
    showFileList();
    ui->UI_PB_OPEN_TW->setEnabled(false);
    if(mRootDir.join("/")== dirPath)//if(mRootDir== dirPath)
        ui->UI_PB_RETURN_TW->hide();
}

void sFileManager::GetFileInfoList(const QString& path, QStringList& filters)
{
    mCurrentFileList.clear();
    if(!filters.isEmpty()){
        QDir dir(path);
        mCurrentFileList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    }
    QDir dir1(path);
    if(!filters.isEmpty())
        dir1.setNameFilters(filters);
    dir1.setSorting(QDir::Time|QDir::DirsFirst);
    mCurrentFileList << dir1.entryInfoList();
    return ;
}

void sFileManager::showFileList()
{
    if(mCurrentFileList.isEmpty())
        return;
    mSize= ui->tableWidget->size();
    int Column= mSize.width()/minWidthTableCell;
    if(!Column)
        Column=1;
    int Row= (mCurrentFileList.size())/ Column+ 1;
    ui->tableWidget->setColumnCount(Column);
    ui->tableWidget->setRowCount(Row);
    ui->tableWidget->clear();

    int tmpRow=0,tmpColumn=0;
    QString fileName;
    foreach (auto listValue, mCurrentFileList) {
        fileName= listValue.fileName();
        if(fileName.isEmpty())
            continue;
        if(fileName=="." || fileName=="..")
            continue;
        if((fileName== "tmpfile.jpg") || (fileName== "tmpfile.txt"))
            continue;

        switch (mSTYLE_WIDGET) {
            case(_STYLE_CHECKBOX):{
                QCheckBox *checkBox = new QCheckBox(fileName);
                connect(checkBox, SIGNAL(clicked()),
                        this, SLOT(on_tableWidget_cellClicked()));
                ui->tableWidget->setCellWidget(tmpRow,tmpColumn, checkBox);
            }
            break;
        default:
            if(ui->tableWidget->item(tmpRow,tmpColumn)){
                ui->tableWidget->item(tmpRow,tmpColumn)->setText(fileName);
            }else{
                QTableWidgetItem* TableWidgetItem= new QTableWidgetItem(fileName);
                //if(listValue.isDir())
                //TableWidgetItem->setIcon();
                //else
                //TableWidgetItem->setIcon();
                ui->tableWidget->setItem(tmpRow,tmpColumn, TableWidgetItem);
            }
            break;
        }
        tmpColumn++;
        if(tmpColumn>= Column){
            tmpColumn=0;
            tmpRow++;
        }
    }
}

bool sFileManager::getFilePath(QStringList& pStringList)
{
    pStringList.clear();
    int rowCount= ui->tableWidget->rowCount();
    int columnCount= ui->tableWidget->columnCount();
    int countsWidget= rowCount* columnCount;
    if(!countsWidget)
        return true;
    switch (mSTYLE_WIDGET) {
    case(_STYLE_CHECKBOX):{
        QCheckBox *pCheckBox= nullptr;
        for(int tmpRow=0; tmpRow< rowCount; ++tmpRow){
            for(int tmpColumn=0; tmpColumn< columnCount; ++tmpColumn){
                pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(tmpRow,tmpColumn)));
                if(!pCheckBox)
                    continue;
                if(pCheckBox->isChecked())
                    pStringList.append(mCurrentDir.join("/")+ "/"+ pCheckBox->text());
            }
        }
        return true;
    }
    default:{
        QTableWidgetItem* pTableWidgetItem= ui->tableWidget->currentItem();
        if(!pTableWidgetItem)
            return false;
        pStringList.append(mCurrentDir.join("/")+ "/"+ pTableWidgetItem->text());
        return true;
    }
    }
    return false;
}

bool sFileManager::getDirPath(QString& pString)
{
    pString= mCurrentDir.join("/");
    return true;
}

void sFileManager::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(ui->tableWidget->size()== mSize)
        return;
    mSize= ui->tableWidget->size();
    int Column= mSize.width()/minWidthTableCell;
    if(!Column)
        Column=1;
    int Row= mCurrentFileList.size()/ Column+ 1;
    ui->tableWidget->clear();

    ui->tableWidget->setColumnCount(Column);
    ui->tableWidget->setRowCount(Row);
    int tmpRow=0,tmpColumn=0;
    QString fileName;
    foreach (auto listValue, mCurrentFileList) {
        fileName= listValue.fileName();
        if(fileName.isEmpty())
            continue;
        if(fileName=="." || fileName=="..")
            continue;
        if((fileName== "tmpfile.jpg") || (fileName== "tmpfile.txt"))
            continue;
        switch (mSTYLE_WIDGET) {
            case(_STYLE_CHECKBOX):
            /*if(listValue.isDir()){
                QTableWidgetItem* TableWidgetItem= new QTableWidgetItem(fileName);
                ui->tableWidget->setItem(tmpRow,tmpColumn, TableWidgetItem);
            }else*/{
                QCheckBox *checkBox = new QCheckBox(fileName);
                //if(listValue.isDir())
                //checkBox->setIcon();
                connect(checkBox, SIGNAL(clicked()),
                        this, SLOT(on_tableWidget_cellClicked()));
                ui->tableWidget->setCellWidget(tmpRow,tmpColumn, checkBox);
            }
            break;
        default:
            if(ui->tableWidget->item(tmpRow,tmpColumn)){
                ui->tableWidget->item(tmpRow,tmpColumn)->setText(fileName);
            }else{
                QTableWidgetItem* TableWidgetItem= new QTableWidgetItem(fileName);
                //if(listValue.isDir())
                //TableWidgetItem->setIcon(QIcon(":/button/picture/canshushezhi_32.png"));
//                else
//                TableWidgetItem->setIcon();
                ui->tableWidget->setItem(tmpRow,tmpColumn, TableWidgetItem);
            }
            break;
        }
        tmpColumn++;
        if(tmpColumn>= Column){
            tmpColumn=0;
            tmpRow++;
        }
    }
}

bool sFileManager::openDir(const QString &pathName)
{
    if(pathName.isEmpty())
        return false;
    for(int i= 0; i< mCurrentDir.size(); ++i){
        if(mCurrentDir[i].isEmpty()){
            mCurrentDir.removeAt(i);
            --i;
        }
    }
    mCurrentDir<< pathName;//mCurrentDir+= "/"+ pathName;
    QString str= mCurrentDir.join("/");
    updata(str, false);
    return true;
}

void sFileManager::on_tableWidget_cellClicked()
{
    int row=ui->tableWidget->currentRow();
    int column=ui->tableWidget->currentColumn();
    QCheckBox* pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(row,column)));
    if(!pCheckBox)
        return ;
    QString tempPath= mCurrentDir.join("/")+ "/"+ pCheckBox->text();
    if(cFileOperate::isDir(tempPath)){
        if(!mCellDoubleClickTimer){
            mCellDoubleClickTimer= new QElapsedTimer();
            mCellDoubleClickTimer->start();
        }else{
            if(mCellDoubleClickTimer->elapsed()<500){
                tempPath= pCheckBox->text();
                if(openDir(tempPath)){
                    ui->UI_PB_RETURN_TW->show();
                }else{
                    emit sCurrentFile(tempPath);
                }
            }
            delete mCellDoubleClickTimer;
            mCellDoubleClickTimer= nullptr;
        }
    }
}

void sFileManager::on_tableWidget_cellClicked(int row, int column)
{
    QString tempPath;
    QCheckBox* pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(row,column)));
    if(pCheckBox){
        tempPath= mCurrentDir.join("/")+ "/"+ pCheckBox->text();
    }else{
        QTableWidgetItem* TableWidgetItem= ui->tableWidget->item(row,column);
        if(!TableWidgetItem)
            return;
        tempPath= mCurrentDir.join("/")+ "/"+ TableWidgetItem->text();
    }
    if(cFileOperate::isDir(tempPath))
        return ui->UI_PB_OPEN_TW->setEnabled(true);

    ui->UI_PB_OPEN_TW->setEnabled(false);
}

void sFileManager::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QString tempPath;
    QCheckBox* pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(row,column)));
    if(pCheckBox){
        tempPath= mCurrentDir.join("/")+ "/"+ pCheckBox->text();
        if(!cFileOperate::isDir(tempPath))
            return;
        tempPath= pCheckBox->text();
    }else{
        QTableWidgetItem* TableWidgetItem= ui->tableWidget->item(row,column);
        if(!TableWidgetItem)
            return;
        tempPath= mCurrentDir.join("/")+ "/"+ TableWidgetItem->text();
        if(!cFileOperate::isDir(tempPath)){
            emit sCurrentFile(tempPath);
            return;
        }
        tempPath= TableWidgetItem->text();
    }
    if(!openDir(tempPath))
        return;
    ui->UI_PB_RETURN_TW->show();
}

void sFileManager::on_UI_PB_OPEN_TW_clicked()
{
    QString tempPath;
    switch (mSTYLE_WIDGET) {
    case(_STYLE_CHECKBOX):{
        int row=ui->tableWidget->currentRow();
        int column=ui->tableWidget->currentColumn();
        QCheckBox* pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(row,column)));
        if(!pCheckBox)
            return;
        tempPath= pCheckBox->text();
        break;
    }default:
        QTableWidgetItem* pTableWidgetItem= ui->tableWidget->currentItem();
        if(!pTableWidgetItem)
            return;
        tempPath= pTableWidgetItem->text();
        break;
    }
    if(!openDir(tempPath))
        return;
    ui->UI_PB_RETURN_TW->show();
}

void sFileManager::on_UI_PB_RETURN_TW_clicked()
{
    if(mCurrentDir== mRootDir)
        return;
    for(int i= 0; i< mCurrentDir.size(); ++i){
        if(mCurrentDir[i].isEmpty()){
            mCurrentDir.removeAt(i);
            --i;
        }
    }
    mCurrentDir.removeLast();
    QString str= mCurrentDir.join("/");
    updata(str, false);
}

void sFileManager::on_UI_PB_DELETE_TW_clicked()
{
    QStringList pStringList;
    int rowCount= ui->tableWidget->rowCount();
    int columnCount= ui->tableWidget->columnCount();
    switch (mSTYLE_WIDGET) {
    case(_STYLE_CHECKBOX):{
        QCheckBox *pCheckBox= nullptr;
        for(int tmpRow=0; tmpRow< rowCount; ++tmpRow){
            for(int tmpColumn=0; tmpColumn< columnCount; ++tmpColumn){
                pCheckBox= ((QCheckBox *)(ui->tableWidget->cellWidget(tmpRow,tmpColumn)));
                if(!pCheckBox)
                    continue;
                if(pCheckBox->isChecked())
                    pStringList.append(mCurrentDir.join("/")+ "/"+ pCheckBox->text());
            }
        }
        break;
    }default:
        break;
    }
    deleteDir(pStringList);
    QString str= mCurrentDir.join("/");
    updata(str,false);
}

bool sFileManager::deleteDir(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()){
        QFile file(path);
        if(! file.remove())
            qDebug() << "remove error:" << file.errorString();
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo file, fileList){
        if (file.isFile()){
            file.dir().remove(file.fileName());
        }else{
            deleteDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

bool sFileManager::deleteDir(const QStringList &pathList)
{
    if (pathList.isEmpty())
        return false;
    foreach(QString path, pathList)
        deleteDir(path);
    return true;
}

