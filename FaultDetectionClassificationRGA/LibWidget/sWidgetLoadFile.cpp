#include "sWidgetLoadFile.h"
#include "ui_sWidgetLoadFile.h"

sWidgetLoadFile::sWidgetLoadFile(QWidget *parent) :
    QWidget(parent),
    mParent(parent),
    ui(new Ui::sWidgetLoadFile)
{
    ui->setupUi(this);
    mDestTable= new sFileManager(this);
    ui->verticalLayout->insertWidget(0, mDestTable);
    connect(mDestTable, &sFileManager::sCurrentFile, this, &sWidgetLoadFile::onCurrentFile);
    //mDestTable->hide();
}

sWidgetLoadFile::~sWidgetLoadFile()
{
    if(!mDestTable)
        delete mDestTable;
    mDestTable= nullptr;
    delete ui;
}

void sWidgetLoadFile::show(QString& filePath)
{
    if(filePath.isEmpty())
        return;
    if(!mDestTable)
        return;
    resize();
    mDestTable->updata(filePath);
    return QWidget::show();
}

void sWidgetLoadFile::show(QString& filePath, QStringList& filters)
{
    if(filePath.isEmpty())
        return;
    if(!mDestTable)
        return;
    resize();
    mDestTable->updata(filePath, filters);
    return QWidget::show();
}

void sWidgetLoadFile::resize()
{
    //    QDesktopWidget *deskWgt = QApplication::desktop();
    //    if(!deskWgt)
    //        return;
    //    QRect rWindow = deskWgt->screenGeometry();
    QRect rWindow= mParent->geometry();
    setFixedSize(rWindow.width(), rWindow.height());//- 32
}

void sWidgetLoadFile::on_UI_PB_OK_WLF_clicked()
{
    if(!mDestTable)
        return;
    QString strPath;
    if(!mDestTable->getDirPath(strPath))
        return;
    emit sOpenFile(strPath);
    hide();
}

void sWidgetLoadFile::on_UI_PB_CANCEL_WLF_clicked()
{
    hide();
}

void sWidgetLoadFile::onCurrentFile(QString strPath)
{
    emit sOpenFile(strPath);
    hide();
}
