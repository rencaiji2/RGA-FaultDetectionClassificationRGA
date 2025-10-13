#ifndef SCOORDINATEMASK_H
#define SCOORDINATEMASK_H

#include <QMainWindow>
#include <QWidget>

namespace Ui {
class sCoordinateMask;
}

class sCoordinateMask : public QWidget
{
    Q_OBJECT

public:
    explicit sCoordinateMask(QWidget* pWidget, QWidget *parent = nullptr);
    ~sCoordinateMask();
    void _show();
    void _hide();
    void resize();
    bool _isHidden();

    void change_UI_PB_OK_Text(QString str);//ly

private slots:
    void on_UI_PB_OK_clicked();

    void on_UI_PB_CANCEL_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_UI_PB_CLEAR_clicked();

private:
    Ui::sCoordinateMask *ui;
    QWidget* mParent;
    QWidget* pWidget;
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent*event);

signals:
    void sCalculate();
    void sClearMark();
    void sCancel();

    void sConfirm_BG_Range();
    void sConfirm_SG_Range();
    void sConfirm_TIC_Range();
};

#endif // SCOORDINATEMASK_H
