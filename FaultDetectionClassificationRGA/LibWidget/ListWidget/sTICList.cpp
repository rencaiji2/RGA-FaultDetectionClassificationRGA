#include "sTICList.h"
#include "ui_sTICList.h"
#include <QDebug>
#include <QStringList>


sTICList::sTICList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sTICList)
{
    ui->setupUi(this);
    QList<QColor> list;
    list << QColor("#993333") << QColor("#CC9966") << QColor("#0066CC") << QColor("#003300") << QColor("#FF0033") << QColor("#99CC00") << QColor("#333399") << QColor("#666666");

    for(int i = 0; i < list.size(); i++ ){
        QPair<QColor,bool> temp;
        temp.first = list[i];
        temp.second = false;
        colorList.append(temp);
    }

}

sTICList::~sTICList()
{
    delete ui;
}

QColor sTICList::getColor()
{
    for(QList<QPair<QColor,bool>>::iterator it = colorList.begin();it != colorList.end() ; it++){
        if((*it).second == false){
            (*it).second = true;
            return (*it).first;
        }
    }
}

void sTICList::modifyList()
{
    QString strTemp,strParam="0.5/";
    if(ItemList.size() != 0){
        int nRow = 0;
        strParam += "0:"+ItemList[nRow].first->getLineEdit()+strTemp.sprintf(":%X:1:0",
                                    ItemList[nRow].first->getColor().rgba());
        for(++nRow; nRow < ui->listWidget->count(); ++nRow){
            strParam += "@0:"+ItemList[nRow].first->getLineEdit()+ strTemp.sprintf(":%X:1:0",
                                        ItemList[nRow].first->getColor().rgba());
        }
    }
    emit ChangedManager(strParam, true);
}

void sTICList::on_BtnAdd_clicked()
{
    if(ItemList.size() > 8)
        return;
    QString text = ui->lineEdit->text();
    ui->lineEdit->clear();
    ListItem* widget = new ListItem();
    widget->setLineEdit(text);
    QColor color = getColor();
    widget->setColor(color);

    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    QSize size = item->sizeHint();
    item->setSizeHint(QSize(size.width(), 35));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,widget);
    QPair<ListItem*,QListWidgetItem*> temp;
    temp.first = widget;
    temp.second = item;
    ItemList.append(temp);

    connect(widget,SIGNAL(itemRemove(ListItem*)),this,SLOT(on_ItemRemove(ListItem*)));
    connect(widget,SIGNAL(itemChecked(ListItem*,bool)),this,SLOT(on_ItemChecked(ListItem*,bool)));

    modifyList();
}


void sTICList::on_ItemRemove(ListItem *item)
{
    for(QList<QPair<ListItem*,QListWidgetItem*>>::iterator it = ItemList.begin(); it != ItemList.end();it++){
        if((*it).first == item){
            ui->listWidget->removeItemWidget((*it).second);

            QString massNum = (*it).first->getLineEdit();
            emit itemRemove(massNum);

            qDebug() << "删除质量数为：" << massNum;

            delete (*it).second;
            delete (*it).first;
            ItemList.removeOne((*it));
            break;
        }
    }
    modifyList();
}

void sTICList::on_ItemChecked(ListItem *item, bool state)
{
    for(QList<QPair<ListItem*,QListWidgetItem*>>::iterator it = ItemList.begin(); it != ItemList.end();it++){
        if((*it).first == item){

            QString massNum = (*it).first->getLineEdit();
            //emit itemChecked(massNum,state);
            emit HideXIC(0, massNum, state);
            qDebug() << "质量数为： " << massNum << "状态为：" << state;

            return;
        }
    }
}
