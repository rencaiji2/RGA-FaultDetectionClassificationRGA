#ifndef SRECIPEMANAGE_H
#define SRECIPEMANAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QWidget>
#include "cSerializeJS.h"
#include "ui_sRecipeManage.h"
#include "IPInputDialog.h"

class sRecipeManage : public QWidget
{
    Q_OBJECT

public:
    explicit sRecipeManage(/*cTcpClient* pTcpClient, */QString IP, QWidget *parent = nullptr);
    ~sRecipeManage();
    QString mIP;
    static bool getLocalRecipeListEQ(QStringList& pListEQ);
    void updateRecipeListEQ(const QStringList& pRecipeEQ);
    static bool getLocalRecipeListRGA(QStringList& pListRGA);
    void updateRecipeListRGA(const QStringList& pListRGA);
    bool setToString(const QByteArray& array){
        if(array.isEmpty())
            return false;
        return cSerializeJS::loadTreeFromString(ui.UI_TW_RECIPE_TREE_RM, array);
    }
    QByteArray getFromString(){
        return cSerializeJS::saveTreeToString(ui.UI_TW_RECIPE_TREE_RM);
    }

private slots:
    void on_UI_TW_RECIPE_TREE_RM_customContextMenuRequested(const QPoint &pos);
    void on_UI_PB_SAVE_RM_clicked(){
        if(mIP== LOCAL_IP){
            QString folderPath= QCoreApplication::applicationDirPath()+"/Recipe/132";
            cSerializeJS::saveTreeToFile(ui.UI_TW_RECIPE_TREE_RM, folderPath);
        }
        emit sSetRecipeMapping(mIP, cSerializeJS::saveTreeToString(ui.UI_TW_RECIPE_TREE_RM));
    }
    void UI_PB_RGA_RECIPE_UPDATE_RM_clicked(){
        emit sGetRecipeListRGA(mIP);
    }
    void on_UI_PB_EQ_RECIPE_UPDATE_RM_clicked(){
        emit sGetRecipeListEQ(mIP);
    }
    void on_UI_PB_LOAD_RM_clicked(){
        if(mIP== LOCAL_IP){
            QString folderPath= QCoreApplication::applicationDirPath()+"/Recipe/132";
            cSerializeJS::loadTreeFromFile(ui.UI_TW_RECIPE_TREE_RM, folderPath);
        }else{
            emit sGetRecipeMapping(mIP);
        }
    }
    void on_UI_PB_RECIPE_CHECK_clicked();

private:
    Ui::sRecipeManage ui;
    QStringList mRecipeListEQ;
    QStringList mRecipeListRGA;

signals:
    void sGetRecipeListEQ(QString ip);
    void sGetRecipeListRGA(QString ip);
    void sGetRecipeMapping(QString ip);
    void sSetRecipeMapping(QString, QByteArray);
    void sGetRecipe(QString ip, bool show);
};

#endif // SRECIPEMANAGE_H
