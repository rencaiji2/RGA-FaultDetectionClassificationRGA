#ifndef SEDITSIM_H
#define SEDITSIM_H

#include <QWidget>
//#include <sMethod/cCommandRGA.h>
#include "ui_sEditSIM.h"
#include <QComboBox>
#include <QMessageBox>
//namespace Ui {
//class sEditSIM;
//}

class sEditSIM : public QWidget
{
    Q_OBJECT

public:
    explicit sEditSIM(QWidget *parent = nullptr);
    ~sEditSIM();
    bool getParam(QStringList& mass,
                  QStringList& name,
                  QStringList& holdTimeMs){
        int rowCount = ui.UI_TW_EDITSIM->rowCount();
        for (int i=0; i<rowCount; ++i) {
            mass<< ui.UI_TW_EDITSIM->item(i, 0)->text();
            name<< ui.UI_TW_EDITSIM->item(i, 1)->text();
            holdTimeMs<< ((QComboBox*)(ui.UI_TW_EDITSIM->cellWidget(i, 2)))->currentText()/*  ->item(i, 2)->text()*/;
        }
        return true;
    }
    void setParam(const QStringList& mass,
                  const QStringList& name,
                  const QStringList& holdTimeMs){
        int size= mass.size();
        if((name.size()!=size)||(holdTimeMs.size()!=size))
            return ;
        for(int i=0; i<size; ++i){
            addRow(mass[i], name[i], holdTimeMs[i]);
        }
    }

private slots:
    void on_U_PB_ADD_EDITSIM_clicked();

    void on_UI_PB_CHEMISTRY_EDITSIM_clicked();

    void on_UI_PB_DELETE_EDITSIM_clicked();

    void on_UI_PB_CLEAR_EDITSIM_clicked();

private:
    Ui::sEditSIM ui;
    void addRow(QString mass, QString name, QString holdTimeMs);
};

#endif // SEDITSIM_H
