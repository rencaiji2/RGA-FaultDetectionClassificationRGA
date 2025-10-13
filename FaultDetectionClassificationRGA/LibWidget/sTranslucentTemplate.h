#ifndef STRANSLUCENTTEMPLATE_H
#define STRANSLUCENTTEMPLATE_H

#include <QWidget>
#include "ui_sTranslucentTemplate.h"

class sTranslucentTemplate : public QWidget
{
    Q_OBJECT

public:
    explicit sTranslucentTemplate(/*QWidget *pWidget, */QWidget *parent = nullptr);
    ~sTranslucentTemplate(){}
    virtual void initUI(QWidget *pWidget= nullptr);
    void show();
    void resize();
    void setParent(QWidget *parent){
        mParent=parent;
        return QWidget::setParent(parent);
    }
private:
    Ui::sTranslucentTemplate sUi;
    QWidget* mParent= nullptr;
};

#endif // STRANSLUCENTTEMPLATE_H
