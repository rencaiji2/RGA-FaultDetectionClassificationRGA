#include "sManagerXIC.h"
#include "ui_sManagerXIC.h"
#include <QColorDialog>
sManagerXIC::sManagerXIC(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sManagerXIC)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    ui->B_Color->setFlat(true);
    ui->B_Color->setAutoFillBackground(true);
}

sManagerXIC::~sManagerXIC()
{
    delete ui;
}

void sManagerXIC::setXic(QListWidgetItem* pQListWidgetItem)
{
    mQListWidgetItem= pQListWidgetItem;
    if(pQListWidgetItem){
        ui->E_Mass->    setText(QString::number(pQListWidgetItem->data(_MANAGE_Mass).toDouble()));
        ui->E_Event->   setText(QString::number(pQListWidgetItem->data(_MANAGE_Event).toInt()));
        ui->E_Gain->    setText(QString::number(pQListWidgetItem->data(_MANAGE_Gain).toDouble()));
        ui->E_Offset->  setText(QString::number(pQListWidgetItem->data(_MANAGE_Offset).toDouble()));
        ui->B_Color->   setProperty("color", pQListWidgetItem->data(_MANAGE_Color).toString()/*toUInt()*/);
        QPalette pal = ui->B_Color->palette();
        pal.setColor(QPalette::Button, QColor(pQListWidgetItem->data(Qt::UserRole + 2).toString().toUInt(nullptr, 16)));
        ui->B_Color->setPalette(pal);
    }
}
uint sManagerXIC::getXic(QListWidgetItem* pQListWidgetItem)
 {
    if(pQListWidgetItem){
        pQListWidgetItem->setData(_MANAGE_Event,   ui->E_Event->text().toInt());
        pQListWidgetItem->setData(_MANAGE_Mass,    ui->E_Mass->text().toDouble());
        pQListWidgetItem->setData(_MANAGE_Color,   ui->B_Color->property("color").toString()/*ui->B_Color->property("color").toUInt()*/);
        pQListWidgetItem->setData(_MANAGE_Gain,    ui->E_Gain->text().toDouble());
        pQListWidgetItem->setData(_MANAGE_Offset,  ui->E_Offset->text().toDouble());
    }
    return ui->B_Color->property("color").toUInt();
 }

void sManagerXIC::on_B_Color_clicked()
{
    QColor clr(QRgb(ui->B_Color->property("color").toUInt()));
    clr = QColorDialog::getColor(clr, this);
    if(!clr.isValid())
        return;
    QPalette pal = ui->B_Color->palette();
    pal.setColor(QPalette::Button, clr);
    ui->B_Color->setPalette(pal);
    QRgb rgb = qRgb(clr.red(), clr.green(), clr.blue());
    QString rgbStr = QString::number(rgb, 16);
    ui->B_Color->setProperty("color", rgbStr/* clr.rgb()*/);
}
