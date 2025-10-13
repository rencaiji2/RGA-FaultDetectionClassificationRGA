#include "sFileDir.h"
#include "ui_sFileDir.h"

sFileDir::sFileDir(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sFileDir)
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

sFileDir::~sFileDir()
{
    delete ui;
}

QStringList sFileDir::getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.tuning";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}

void sFileDir::showFileList(int nColumn, const QStringList list)
{
    if(nColumn<1)
        nColumn=1;
    if(list.isEmpty())
        return;
    int tmpRow=0,tmpColumn=0;
    ui->tableWidget->setColumnCount(nColumn);
    ui->tableWidget->setRowCount((list.size()+nColumn-1)/nColumn);
    foreach (QString fileName, list) {
        if(fileName.isEmpty())
            continue;
        ui->tableWidget->setItem(tmpRow,tmpColumn,new QTableWidgetItem(fileName.left(fileName.indexOf(".tuning"))));
        tmpColumn++;
        if(tmpColumn> nColumn-1){
            tmpColumn=0;
            tmpRow++;
        }
    }
}

void sFileDir::on_tableWidget_clicked(const QModelIndex &index)
{
    emit selectFile(false,index.data().toString());
}
