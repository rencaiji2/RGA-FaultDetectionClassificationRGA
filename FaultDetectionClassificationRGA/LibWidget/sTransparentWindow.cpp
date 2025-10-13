#include "sTransparentWindow.h"
#include "ui_sTransparentWindow.h"

#include <QPainter>

sTransparentWindow::sTransparentWindow(int highTitle, QWidget* pWidget, QWidget *parent) :
    QWidget(parent),
    mParent(parent),
    mhighTitle(highTitle),
    ui(new Ui::sTransparentWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    if(pWidget)
        ui->UI_LAYOUT_CENTER->addWidget(pWidget);
}

sTransparentWindow::~sTransparentWindow()
{
    delete ui;
}

void sTransparentWindow::paintEvent(QPaintEvent*event)
{
    if(mParent){
        this->move(0, mhighTitle);
        this->resize(mParent->width(), mParent->height()- mhighTitle);
    }
    QPainter p(this);
    p.setPen(QColor(0, 0, 0, 150));
    p.setBrush(QColor(0, 0, 0, 150));
    //p.setCompositionMode(QPainter::CompositionMode_Clear);
    //p.fillRect(10, 10, 300, 300, Qt::SolidPattern);
    p.drawRect(this->rect());
    QWidget::paintEvent(event);
}

void sTransparentWindow::addCenterWidget(QWidget* pWidget)
{
    if(pWidget)
        ui->UI_LAYOUT_CENTER->addWidget(pWidget);
}

void sTransparentWindow::addTopWidget(QWidget* pWidget)
{
    if(pWidget)
        ui->UI_LAYOUT_TOP->addWidget(pWidget);
}

void sTransparentWindow::addButtomWidget(QWidget* pWidget)
{
    if(pWidget)
        ui->UI_LAYOUT_BUTTOM->addWidget(pWidget);
}

void sTransparentWindow::addLeftWidget(QWidget* pWidget)
{
    if(pWidget)
        ui->UI_LAYOUT_LEFT->addWidget(pWidget);
}

void sTransparentWindow::addRightWidget(QWidget* pWidget)
{
    if(pWidget)
        ui->UI_LAYOUT_RIGHT->addWidget(pWidget);
}
