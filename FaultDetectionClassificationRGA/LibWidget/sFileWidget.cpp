#include "sFileWidget.h"
#include "ui_sFileWidget.h"
#include <QDir>

sFileWidget::sFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sFileWidget)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setStyleSheet("QTableWidget {border:none;background: #E4DAD0;gridline-color: #3c3c3c;}\
                                   QTableWidget::item {padding-left: 5px;padding-right: 5px;border: none;\
                                                       background: #E4DAD0;border:none;}\
                                   QTableWidget::item:selected {background: #3c3c3c;color:#E4DAD0);}\
                                   QTableWidget::item:selected:!active {color: white;}");
}

sFileWidget::~sFileWidget()
{
    delete ui;
}

QFileInfoList sFileWidget::GetFileList(QString path)
{
    QDir dir(path);
    QStringList filters;
          filters << "*.txt" << "*.jpg";
          dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();
    return list;
}

void sFileWidget::showFileList(QFileInfoList list)
{
    if(list.isEmpty())
        return;
    int tmpRow=0,tmpColumn=0;
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(list.size()/2-2);
    QString fileName;
    foreach (auto listValue, list) {
        fileName= listValue.fileName();
        if(fileName.isEmpty())
            continue;
        if((fileName== "tmpfile.jpg") || (fileName== "tmpfile.txt"))
            continue;
        if(ui->tableWidget->item(tmpRow,tmpColumn))
            ui->tableWidget->item(tmpRow,tmpColumn)->setText(fileName);
        else
            ui->tableWidget->setItem(tmpRow,tmpColumn,new QTableWidgetItem(fileName));

        tmpColumn++;
        if(tmpColumn> 1){
            tmpColumn=0;
            tmpRow++;
        }
    }
}

bool sFileWidget::creatNewFile(QString path,QString name)
{
    ui->tableWidget->clear();
    showFileList(GetFileList(path));
    mfileName=name;
    ui->E_FILENAME->setText(name);
    return true;
}

uint sFileWidget::getFrameCount()
{
    uint nFrameCount= ui->lineEdit->text().toUInt();
    if(nFrameCount< 1)
        nFrameCount= 1;
    return nFrameCount;
}
