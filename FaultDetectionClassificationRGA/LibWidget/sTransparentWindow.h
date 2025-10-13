#ifndef STRANSPARENTWINDOW_H
#define STRANSPARENTWINDOW_H

#include <QWidget>

namespace Ui {
class sTransparentWindow;
}

class sTransparentWindow : public QWidget
{
    Q_OBJECT

public:
    explicit sTransparentWindow(int highTitle, QWidget* pWidget= nullptr, QWidget *parent = nullptr);
    ~sTransparentWindow();
    void addCenterWidget(QWidget* pWidget);
    void addTopWidget(QWidget* pWidget);
    void addButtomWidget(QWidget* pWidget);
    void addLeftWidget(QWidget* pWidget);
    void addRightWidget(QWidget* pWidget);

private:
    Ui::sTransparentWindow *ui;
    QWidget* mParent;
    int mhighTitle= 0;
    void paintEvent(QPaintEvent*event);

};

#endif // STRANSPARENTWINDOW_H
