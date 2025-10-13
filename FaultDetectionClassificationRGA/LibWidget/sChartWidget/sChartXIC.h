#ifndef SCHARTXIC_H
#define SCHARTXIC_H

#include <QWidget>
#include "sManagerXIC.h"
#include <cPublicCCS.h>
#include <qwt_plot_curve.h>

namespace Ui {
class sChartXIC;
}

class sChartXIC : public QWidget
{
    Q_OBJECT

public:
    enum _XIC_PARAM{_XIC_STEP,_XIC_STRUCT};
//    struct _PARAM_XIC{
//        double Gain=1;
//        double Offset=0;
//        QColor color; /**< TODO */
//    };
    struct _TEMP_XIC:_PARAM_XIC{
        int nEvent;
        QString mass;
    };
    explicit sChartXIC(QWidget *parent = nullptr);
    ~sChartXIC();
    bool setParam(QString str, bool update=false);//to change _XIC_STEP and _XIC_STRUCT
    QString getParam(){
        return mXIC;
    }
private slots:
    void on_B_Add_clicked();
    void on_B_Modify_clicked();
    void on_B_Remove_clicked();
    void on_B_OK_clicked();
    void on_Manager_clicked(QListWidgetItem* pQListWidgetItem, bool isChanged);
    void on_B_Cancel_clicked();

private:
    Ui::sChartXIC *ui;
    sManagerXIC* mManagerXIC= nullptr;
    QString mXIC;

    void setParam(_XIC_PARAM name,QString str);//choose _XIC_STEP or _XIC_STRUCT
    void addXic(sManagerXIC* pManagerXIC);
    void ModifyXic(sManagerXIC* pManagerXIC);
    void removeXic();
    void ParamShow(QListWidgetItem* pItem);

signals:
    void ChangedManager(QString, bool);//-schartwidget
};

#endif // SCHARTXIC_H
