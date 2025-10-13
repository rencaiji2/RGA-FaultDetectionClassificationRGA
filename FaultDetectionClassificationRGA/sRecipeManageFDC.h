#ifndef SRECIPEMANAGEFDC_H
#define SRECIPEMANAGEFDC_H

#include "cExecuteStruct.h"
#include "ui_sRecipeManageFDC.h"
#include "sConfigure.h"
#include "sRecipeManage.h"
#include <QGroupBox>
#include <QWidget>


class sRecipeManageFDC : public QWidget
{
    Q_OBJECT

public:
    explicit sRecipeManageFDC(QWidget *parent = nullptr);
    ~sRecipeManageFDC();
    bool loadChartFDC();
    void updateRecipeListEQ(const QStringList& pRecipeEQ, const QString& ip){
        QString EquipmentGroupName;
        sRecipeManage* pRecipeManage= getRecipeManage(ip, mGroupBoxList, EquipmentGroupName);
        if(!pRecipeManage)
            return;
        updateRecipeListEQ(pRecipeEQ, pRecipeManage);
    }
    void updateRecipeListRGA(const QStringList& pListRGA, const QString& ip){
        QString EquipmentGroupName;
        sRecipeManage* pRecipeManage= getRecipeManage(ip, mGroupBoxList, EquipmentGroupName);
        if(!pRecipeManage)
            return;
        updateRecipeListRGA(pListRGA, pRecipeManage);
    }
private slots:
    void on_UI_PB_SAVE_RM_FDC_clicked();

    void on_UI_PB_EQ_RECIPE_LIST_clicked();

    void on_UI_PB_RGA_RECIPE_LIST_clicked();

    void on_UI_PB_LOAD_RGA_RECIPE_clicked();

    void on_UI_PB_SAVE_RGA_RECIPE_clicked();
    void onGetRecipeListEQ(QString& ip, QByteArray& array);

private:
    Ui::sRecipeManageFDC ui;
    QList<QGroupBox*> mGroupBoxList;
    sConfigure* mConfigure= nullptr;
    sRecipeManage* getRecipeManage(const QString& EquipmentGroupIP, const QList<QGroupBox*>& pGroupBoxList,
                                   QString& EquipmentGroupName);
    void updateRecipeListEQ(const QStringList& pRecipeEQ, sRecipeManage* pRecipeManage){
        if(!pRecipeManage)
            return;
        pRecipeManage->updateRecipeListEQ(pRecipeEQ);
        ui.UI_LW_EQ_RECIPE_LIST->clear();
        ui.UI_LW_EQ_RECIPE_LIST->addItems(pRecipeEQ);
    }
    void updateRecipeListRGA(const QStringList& pListRGA, sRecipeManage* pRecipeManage){
        if(!pRecipeManage)
            return;
        pRecipeManage->updateRecipeListRGA(pListRGA);
        ui.UI_CB_RECIP_NAME->clear();
        ui.UI_CB_RECIP_NAME->addItems(pListRGA);
    }
signals:
    void sGetRecipeListEQ(QString& ,QString& ip);
    void sGetRecipeListRGA(QString& ,QString& ip);
    void sGetRecipeMapping(QString ip);
    void sSetRecipeMapping(QString, QByteArray);
    void sGetRecipe(QString ip, bool show);
};

#endif // SRECIPEMANAGEFDC_H
