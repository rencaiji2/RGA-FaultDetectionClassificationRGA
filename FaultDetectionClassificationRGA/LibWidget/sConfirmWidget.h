#ifndef SCONFIRMWIDGET_H
#define SCONFIRMWIDGET_H

#include <QWidget>

namespace Ui {
class sConfirmWidget;
}

class sConfirmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit sConfirmWidget(QWidget *parent = nullptr);
    ~sConfirmWidget();

private:
    Ui::sConfirmWidget *ui;
};

#endif // SCONFIRMWIDGET_H
