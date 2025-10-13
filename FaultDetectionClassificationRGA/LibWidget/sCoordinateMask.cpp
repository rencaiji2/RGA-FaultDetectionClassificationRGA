#include "sCoordinateMask.h"
#include "ui_sCoordinateMask.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

sCoordinateMask::sCoordinateMask(QWidget* pWidget, QWidget *parent) :
    QWidget(parent),//QMainWindow(parent, Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint),//
    pWidget(pWidget),
    mParent(parent),
    ui(new Ui::sCoordinateMask)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //shadow->setOffset(0, 0);
    //shadow->setColor(QColor("#444444"));
    //shadow->setBlurRadius(900);
    //ui->widget->setGraphicsEffect(shadow);
    ui->UI_W_LEFT->setParent(parent);
    ui->UI_W_RIGHT->setParent(parent);
    ui->UI_W_TOP->setParent(parent);
    ui->UI_W_BOTTOM->setParent(parent);
    ui->UI_W_LEFTW->setParent(parent);
    //ui->UI_W_TOPW->setParent(parent);
    //ui->UI_W_BOTTOMW->setParent(parent);
    _hide();
    this->hide();
}

sCoordinateMask::~sCoordinateMask()
{
    delete ui;
}

void sCoordinateMask::_show()
{
    resize();
    ui->UI_W_LEFT->show();
    ui->UI_W_RIGHT->show();
    ui->UI_W_TOP->show();
    ui->UI_W_BOTTOM->show();
    ui->UI_W_LEFTW->show();
    //ui->UI_W_TOPW->show();
    //ui->UI_W_BOTTOMW->show();
}

void sCoordinateMask::_hide()
{
    ui->UI_W_LEFT->hide();
    ui->UI_W_RIGHT->hide();
    ui->UI_W_TOP->hide();
    ui->UI_W_BOTTOM->hide();
    ui->UI_W_LEFTW->hide();//this->hide();
    //ui->UI_W_TOPW->hide();
    //ui->UI_W_BOTTOMW->hide();
}

void sCoordinateMask::resize()
{
    QPoint pointWidget= pWidget->mapToGlobal(pWidget->pos());
    QRect rWindow= mParent->geometry();
    QRect rWidget= pWidget->geometry();
    ui->UI_W_TOP->move(0, 38);
    ui->UI_W_TOP->setFixedSize(rWindow.width(), pointWidget.y()- rWindow.y()- 38);//- 32
    ui->UI_W_BOTTOM->move(0, pointWidget.y()- rWindow.y()+ rWidget.height());//+ 32
    ui->UI_W_BOTTOM->setFixedSize(rWindow.width(), rWindow.height()- pointWidget.y()+ rWindow.y()- rWidget.height());//- 32

    ui->UI_W_LEFT->move(0, ui->UI_W_TOP->y()+ ui->UI_W_TOP->height());
    ui->UI_W_LEFT->setFixedSize(pointWidget.x()- rWindow.x()- 72- 8, rWidget.height());//+ 64
    ui->UI_W_LEFTW->move(ui->UI_W_LEFT->width(), ui->UI_W_TOP->height()+  38);//+ 32
    ui->UI_W_LEFTW->setFixedSize(72, rWidget.height());
    ui->UI_W_RIGHT->move(ui->UI_W_LEFT->width()+ ui->UI_W_LEFTW->width()+ rWidget.width()+ 8, ui->UI_W_LEFT->y());
    ui->UI_W_RIGHT->setFixedSize(rWindow.width()-ui->UI_W_RIGHT->x(), ui->UI_W_LEFT->height());

//    ui->UI_W_TOPW->move(ui->UI_W_LEFT->width(), ui->UI_W_LEFT->y());
//    ui->UI_W_TOPW->setFixedSize(rWidget.width()+ ui->UI_W_LEFTW->width()+ 8, 32);
//    ui->UI_W_BOTTOMW->move(ui->UI_W_LEFT->width(), ui->UI_W_LEFT->y()+ ui->UI_W_LEFT->height());
//    ui->UI_W_BOTTOMW->setFixedSize(ui->UI_W_TOPW->width(), 32);
}

bool sCoordinateMask::_isHidden()
{
    return ui->UI_W_TOP->isHidden();
}

void sCoordinateMask::on_UI_PB_OK_clicked()
{
    if(ui->UI_PB_OK->text() == "确认背景")
        emit sConfirm_BG_Range();
    else if(ui->UI_PB_OK->text() == "确认信号")
        emit sConfirm_SG_Range();
    else if(ui->UI_PB_OK->text() == "确认TIC")
        emit sConfirm_TIC_Range();
    else
        emit sCalculate();
    hide();
}

void sCoordinateMask::on_UI_PB_CLEAR_clicked()
{
    emit sClearMark();
}

void sCoordinateMask::on_UI_PB_CANCEL_clicked()
{
    emit sCancel();
    hide();
}

void sCoordinateMask::on_horizontalSlider_sliderMoved(int position)
{

}

void sCoordinateMask::on_horizontalSlider_2_sliderMoved(int position)
{

}

void sCoordinateMask::resizeEvent(QResizeEvent *event)
{
    resize();
    this->update();								//更新
}

void sCoordinateMask::paintEvent(QPaintEvent*event)
{
    //    show();
    //    if(mParent){
    //        //this->move(0, mhighTitle);
    //        this->resize(mParent->width(), mParent->height());//- mhighTitle
    //    }
    //    QPainter p(this);
    //    QRect tempRect=this->rect();
    //    p.setPen(QColor(0, 0, 0, 0));
    //    p.setBrush(QColor(0, 0, 0, 0));
    //    //p.setCompositionMode(QPainter::CompositionMode_Clear);
    //    //p.fillRect(10, 10, 300, 300, Qt::SolidPattern);
    //    p.drawRect(tempRect);
    //    //p.setPen(QColor(0, 0, 0, 255));
    //    //p.setBrush(QColor(0, 0, 0, 255));
    //    //ui->UI_W_RIGHTW->setStyleSheet("background-color:transparent;");
    QWidget::paintEvent(event);
}

void sCoordinateMask::change_UI_PB_OK_Text(QString str)
{
    ui->UI_PB_OK->setText(str);
}
