#pragma once

#include "ui_sJsCalculate.h"
#include <QScriptEngine>
#include <QWidget>
#include <cPublicCCS.h>
#include <QDebug>
#include <cPublicStruct.h>
#include <QElapsedTimer>
//#include <cDatabaseFDC.h>

//#define EXCEEDING_COUNT 8

class sJsCalculate : public QWidget
{
    Q_OBJECT

public:
    explicit sJsCalculate(QWidget *parent = nullptr);
    ~sJsCalculate();
    QMutex mutex;
    bool setParame(const QStringList& strList){
    Q_UNUSED(strList)
        return true;
    }
    bool setParame(const QString& str){
        QStringList strList= str.split(";");
        int rowCount= strList.size();
        ui.tableWidget->setRowCount(10);//rowCount
        ui.tableWidget->setColumnCount(4);

        for(int i= 0; i< rowCount; ++i){
            QStringList tmpList= strList[i].split(",");
            if(tmpList.size()!=4)
                continue;
            QTableWidgetItem *itemName= ui.tableWidget->item(i, 0);
            QTableWidgetItem *itemJS= ui.tableWidget->item(i, 1);
            QTableWidgetItem *itemThreD= ui.tableWidget->item(i, 2);
            QTableWidgetItem *itemThreU= ui.tableWidget->item(i, 3);
            if(!itemName){
                ui.tableWidget->setItem(i, 0, new QTableWidgetItem(tmpList[0]));
            }else
                itemName->setText(tmpList[0]);
            if(!itemJS){
                ui.tableWidget->setItem(i, 1, new QTableWidgetItem(tmpList[1]));
            }else
                itemJS->setText(tmpList[1]);
            if(!itemThreD){
                ui.tableWidget->setItem(i, 2, new QTableWidgetItem(tmpList[2]));
            }else
                itemThreD->setText(tmpList[2]);
            if(!itemThreU){
                ui.tableWidget->setItem(i, 3, new QTableWidgetItem(tmpList[3]));
            }else
                itemThreU->setText(tmpList[3]);
        }
        return true;
    }
    QString getParame(){
        int rowCount= ui.tableWidget->rowCount();
        QStringList strList;

        for(int i= 0; i< rowCount; ++i){
            QStringList tmpList;
            QTableWidgetItem *itemName= ui.tableWidget->item(i, 0);
            QTableWidgetItem *itemJS= ui.tableWidget->item(i, 1);
            QTableWidgetItem *itemThreD= ui.tableWidget->item(i, 2);
            QTableWidgetItem *itemThreU= ui.tableWidget->item(i, 3);
            if((!itemName)||(!itemJS)||(!itemThreD)||(!itemThreU))
                continue;
            tmpList<< itemName->text();
            tmpList<< itemJS->text();
            tmpList<< itemThreD->text();
            tmpList<< itemThreU->text();
            strList<< tmpList.join(",");
        }
        return strList.join(";");
    }
    bool getParame(QMap<QString, QString>& pMap){
        Q_UNUSED(pMap)
        return true;
    }
    bool getUseDurationEX(){
        return ui.UI_RB_DurationEX_JSCALC->isChecked();
    }
    void setUseDurationEX(bool isChecked){
        ui.UI_RB_DurationEX_JSCALC->setChecked(isChecked);
    }
    QString getDurationEX(){
        return ui.UI_LE_DurationEX_JSCALC->text();
    }
    void setDurationEX(QString str){
        ui.UI_LE_DurationEX_JSCALC->setText(str);
    }
    QString getPointCounts(){
        return ui.UI_LE_COUNTS_JSCALC->text();
    }
    void setPointCounts(QString str){
        ui.UI_LE_COUNTS_JSCALC->setText(str);
    }
    bool getNameList(QStringList& strList){
        strList.clear();
        for(auto& key: mFormula.keys()){
            strList<< key;
        }
        return true;
    }
    int getIndexName(QString& name){
        for(auto& key: mIndexAL.keys()){
            if(key== name)
                return mIndexAL[key];
        }
        return -1;
    }
    bool checkFormula(const QString& massStart, const QString& massEnd);
    bool checkFormula(const QStringList& mass);
    bool calculate(int indexDataXIC_src, int sizeDataXIC_src, QMAP_PARAM_XIC& pMapXIC,
                   QMAP_PARAM_XIC& pMapOther);//后续改成数组处理
    bool getCustomData(QString& customName, QString& customData){//注意避免多线程使用
        if(mDurationData.size()< 1)
            return false;
        QStringList nameList, dataList;
        foreach (auto& key, mDurationData.keys()){
            nameList<< key;
            dataList<< QString::number(mDurationData[key], 'e', 2);
        }
        customName= nameList.join(",");
        customData= dataList.join(",");
        return true;
    }

private:
    Ui::sJsCalculate ui;
    QMap<QString/*名字*/, QPair<QStringList/*质量数*/, QScriptValue/*公式*/>> mFormula;
    QMap<QString, QPair<double/*down*/,double/*up*/>> mThreshold;
    QMap<QString, int> mIndexAL;
    QScriptEngine mScriptEngine;

    //bool mUseDurationEX= false;
    int mDurationEX_S= 0;
    int mPointCounts= 0;
    QElapsedTimer DurationTimer;
    QMap<QString, int> mExceedingDownCntMap;
    QMap<QString, int> mExceedingUpCntMap;
    int indexDurationDataS= 0,
        indexDurationDataE= 0;
    QMap<QString, double> mDurationData;

signals:
    void sDurationTimer(QString customName, QString customData);
    void sExceedingUp(QString name, int indexAL, double threshold, double current);
    void sExceedingDown(QString name, int indexAL, double threshold, double current);
};

