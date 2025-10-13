#include "sConfirmWidget.h"
#include "ui_sConfirmWidget.h"

sConfirmWidget::sConfirmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sConfirmWidget)
{
    ui->setupUi(this);
}

sConfirmWidget::~sConfirmWidget()
{
    delete ui;
}
