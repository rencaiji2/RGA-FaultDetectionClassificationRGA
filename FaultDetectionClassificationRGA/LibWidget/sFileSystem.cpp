#include "sFileSystem.h"
#include "ui_sFileSystem.h"
#include <QDebug>
#include <QDir>
#include "sTool/cFileOperate.h"

const uchar strDirPath[]={'e','f','g',//'c','d',
                          'h','i','j','k','l',
                          'm','n','o','p','q',
                          'r','s','t','u','v',
                          'w','x','y','z'};

sFileSystem::sFileSystem(QWidget *parent)
    : QWidget(parent),
    mParent(parent),
    ui(new Ui::sFileSystem)
{
    ui->setupUi(this);
    QPalette palette;//= this->palette();
    palette.setColor(QPalette::Background, QColor(233, 234, 239, 255));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    mTableWidget= new sFileManager(this, sFileManager::_STYLE_CHECKBOX);
    ui->horizontalLayout_2->insertWidget(0, mTableWidget);
    mDestTable= new sFileManager(this, sFileManager::_STYLE_CHECKBOX);
    ui->horizontalLayout_2->addWidget(mDestTable);
}

sFileSystem::~sFileSystem()
{
    if(mTableWidget)
        delete mTableWidget;
    mTableWidget=nullptr;
    delete ui;
}

void sFileSystem::resize()
{
//    QDesktopWidget *deskWgt = QApplication::desktop();
//    if(!deskWgt)
//        return;
//    QRect rWindow = deskWgt->screenGeometry();
    QRect rWindow= mParent->geometry();
    setFixedSize(rWindow.width(), rWindow.height());//- 32
}

void sFileSystem::show(QString& dir)
{
    resize();
    mTableWidget->updata(dir);
    return QWidget::show();
}

void sFileSystem::on_UI_PB_SAVEAS_FS_clicked()
{
    if((!mTableWidget)||(!mDestTable))
        return;
    QStringList tempList;
    if(!mTableWidget->getFilePath(tempList))
        return;
    if(tempList.isEmpty())
        return;

    QString DestDir;
    if(!mDestTable->getDirPath(DestDir))
        return;
    if(DestDir.isEmpty())
        return;
    if(cFileOperate::isFile(DestDir))
        DestDir= DestDir.left((DestDir.lastIndexOf("/")));

    cFileOperate::copyFileToPath(tempList, DestDir);
    mDestTable->updata(DestDir, false);
}

void sFileSystem::on_UI_PB_LOAD_FS_clicked()
{
    if((!mTableWidget)||(!mDestTable))
        return;
    QStringList tempList;
    if(!mDestTable->getFilePath(tempList))
        return;
    if(tempList.isEmpty())
        return;

    QString DestDir;
    if(!mTableWidget->getDirPath(DestDir))
        return;
    if(DestDir.isEmpty())
        return;

    if(cFileOperate::isFile(DestDir))
        DestDir= DestDir.left((DestDir.lastIndexOf("/")));

    cFileOperate::copyFileToPath(tempList, DestDir);
    mTableWidget->updata(DestDir, false);
}

void sFileSystem::slot_action_triggered(QAction *action)
{
    if(action->text().isEmpty())
        return;
    QString DestDir= action->text()+":";
    mDestTable->updata(DestDir);
}

void sFileSystem::on_UI_PB_DIR_FS_clicked()
{
    if(mymenu)
        delete mymenu;
    mymenu= new QMenu();
    connect(mymenu, &QMenu::triggered, this, &sFileSystem::slot_action_triggered);
    QFileInfo fileInfo;
    for(int i=0;i<22;++i){
        fileInfo.setFile(QString(strDirPath[i])+":");
        if(fileInfo.isDir()){
            QAction *click=new QAction(QString(strDirPath[i]),this);
            mymenu->addAction(click);
        }
    }
    ui->UI_PB_OPEN_FS->setMenu(mymenu);
}

void sFileSystem::on_UI_PB_CLOSE_FS_clicked()
{
    hide();
}
