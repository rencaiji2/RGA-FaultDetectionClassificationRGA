#include "sTranslucentTemplate.h"
#include "ui_sTranslucentTemplate.h"

sTranslucentTemplate::sTranslucentTemplate(/*QWidget *pWidget, */QWidget *parent) :
    QWidget(parent),
    mParent(parent)
{

}

void sTranslucentTemplate::initUI(QWidget *pWidget)
{
    sUi.setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    if(pWidget){
        sUi.UI_LAYOUT_TTEMPLATE->addWidget(pWidget, 1, 1);
    }
    QPalette palette;//= this->palette();
    palette.setColor(QPalette::Background, QColor(0, 0, 0, 64));
    setPalette(palette);
    setAutoFillBackground(true);
}

void sTranslucentTemplate::show()
{
    resize();
    return QWidget::show();
}

void sTranslucentTemplate::resize()
{
//    QDesktopWidget *deskWgt = QApplication::desktop();
//    if(!deskWgt)
//        return;
//    QRect rWindow = deskWgt->screenGeometry();
    QRect rWindow= mParent->geometry();
    setFixedSize(rWindow.width(), rWindow.height());//- 32
}
