#ifndef SDEFINEPLOTDATADLG_H
#define SDEFINEPLOTDATADLG_H

#include <QDialog>
#include <QTreeWidgetItem>

#include "publicdef.h"
#include "LibAlgorithm/PublicAlgorithm.h"

namespace Ui {
class sDefinePlotDataDlg;
}

class sDefinePlotDataDlg : public QDialog
{
    Q_OBJECT

public:
    explicit sDefinePlotDataDlg(const QMap<QString, EquipmentGroupInfo>& i_optionMap,
                                const QVariantMap& i_chamerIDConf,const QVariantMap& i_parentSysConf,
                                QWidget *parent = nullptr);
    ~sDefinePlotDataDlg();

    QVariantMap getData();

    void setParentSystemConf(const QVariantMap& i_parentSysConf)
    {
        m_parentSystemConFMap = i_parentSysConf;
    }

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnChk_clicked();

    void on_chk_enabledDate_clicked(bool checked);

private:
    void initUI();

    void initTreeWgt();
    void initTreeWgtFromReportConf();

private:
    Ui::sDefinePlotDataDlg *ui;

    QMap<QString, EquipmentGroupInfo> m_optionMap;
    QVariantMap m_chamberIDMap;

    QTreeWidgetItem* m_rootNode = nullptr;

    QVariantMap m_parentSystemConFMap;
};

#endif // SDEFINEPLOTDATADLG_H
