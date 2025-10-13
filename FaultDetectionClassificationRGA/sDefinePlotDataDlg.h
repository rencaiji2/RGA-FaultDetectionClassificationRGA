#ifndef SDEFINEPLOTDATADLG_H
#define SDEFINEPLOTDATADLG_H

#include <QDialog>
#include <QTreeWidgetItem>

#include "publicdef.h"

namespace Ui {
class sDefinePlotDataDlg;
}

class sDefinePlotDataDlg : public QDialog
{
    Q_OBJECT

public:
    explicit sDefinePlotDataDlg(const QMap<QString, EquipmentGroupInfo>& i_optionMap,
                                const QVariantMap& i_chamerIDConf,QWidget *parent = nullptr);
    ~sDefinePlotDataDlg();

    QVariantMap getData();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    void initUI();

    void initTreeWgt();

private:
    Ui::sDefinePlotDataDlg *ui;

    QMap<QString, EquipmentGroupInfo> m_optionMap;
    QVariantMap m_chamberIDMap;

    QTreeWidgetItem* m_rootNode = nullptr;
};

#endif // SDEFINEPLOTDATADLG_H
