#include "ListItem.h"
#include "ui_ListItem.h"
#include <QDebug>
#include <QPalette>

ListItem::ListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->checkBox->setChecked(true);
}

ListItem::~ListItem()
{
    delete ui;
}

void ListItem::setColor(QColor color)
{
    myColor = color;
    QPalette colorSign;
    colorSign.setColor(QPalette::Text,color);
    ui->label->setPalette(colorSign);
}

void ListItem::setLineEdit(QString text)
{
    ui->label->setText(text);
    massNum = text;
}

void ListItem::on_pushButton_clicked()
{
    emit itemRemove(this);
}

void ListItem::on_checkBox_stateChanged(int arg1)
{
    if(2 == arg1)
        emit itemChecked(this,true);
    else
        emit itemChecked(this,false);
}
