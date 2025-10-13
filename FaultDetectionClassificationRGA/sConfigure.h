#pragma once

#include <QSettings>
#include <QWidget>
//#include <LibWidget/sChartWidget.h>
//#include <sMethod/cCommandRGA.h>
//#include <sMethod/cConfigOMS.h>
#include <sConfigure/sJsCalculate.h>
#include "cStructRGA.h"
#include "ui_sConfigure.h"
#include "sConfigure/cConfigRGA.h"
#include "sConfigure/sEditSIM.h"
#include "sConfigure/cParamCPM.h"
#include <QFile>
#include <QFileDialog>
#include <cPublicDefine.h>

#define MIN_SIGNAL 5e-15

class sConfigure : public QWidget
{
    Q_OBJECT
private:
    void initProperty(/*QString& DeviceTuneFilePath*/){
        setProperty("PumpGaugeUnit", cConfigRGA::_UNIT_GAUGE_MTORR);
        setProperty("ExternalGaugeUnit", cConfigRGA::_UNIT_GAUGE_MBAR);
    }
public:
    explicit sConfigure(//QString& DeviceTuneFilePath,
                        cConfigRGA::_UNIT_GAUGE ExternalGaugeUnit,
                        cConfigRGA::_UNIT_GAUGE PumpGaugeUnit,
                        //_STATE_RGA& pSTATE_RGA,
                        QWidget *parent = nullptr);
    ~sConfigure();
    sJsCalculate* getJsCalculate(){
        return mJsCalculate;
    }
    void showSpectraConfig(){
        ui.tabWidget->setCurrentIndex(0);
        QWidget::show();
    }
    void showMethod(){
        ui.tabWidget->setCurrentIndex(1);
        QWidget::show();
    }
    void show(int tabIndex){
        ui.tabWidget->setCurrentIndex(tabIndex);
        QWidget::show();
    }
    inline quint16 getParamScanDelay(){
        return static_cast<quint16>(ui.lineEdit_scanDelay->text().toUInt());
    }
    inline quint16 getCurrentLimit(){
        return static_cast<quint16>(ui.UI_LE_CURR_LIMIT_DT->text().toUInt());
    }
    inline bool isFila2(){
        return ui.UI_RB_Fila2_DT->isChecked();
    }
    inline quint16 get_gauge_volt_prot_mv(){
        return static_cast<quint16>(ui.UI_LE_gauge_volt_prot_mv_DT->text().toUInt());
    }
    inline quint8 getDetector(){
        return static_cast<quint8>(ui.UI_RB_Multiplier_DT->isChecked());
    }
    inline bool isBaselineEnable(){
        return ui.UI_CB_BASELINE_ENABLE_CFIG->isChecked();
    }
    inline int isExternalGauge(){
        return ui.UI_RB_GAUGE_OUT_DT->isChecked();
    }
    inline double getTotalSettings(){
        return ui.UI_LE_GAUGE_NULL_DT->text().toDouble();
    }
    /***********************************************************/
    cConfigRGA::_UNIT_GAUGE getGaugeUint(){
        if(ui.UI_RB_PPM_CAL_CFIG->isChecked())
            return cConfigRGA::_UNIT_GAUGE_PPM_CAL;
        if(ui.UI_RB_A_CFIG->isChecked())
            return cConfigRGA::_UNIT_CURRENT_A;
        if(ui.UI_RB_MBAR_CFIG->isChecked())
            return cConfigRGA::_UNIT_GAUGE_MBAR;
        if(ui.UI_RB_PA_CFIG->isChecked())
            return cConfigRGA::_UNIT_GAUGE_PA;
        if(ui.UI_RB_TORR_CFIG->isChecked())
            return cConfigRGA::_UNIT_GAUGE_TORR;
        if(ui.UI_RB_PPM_CFIG->isChecked())
            return cConfigRGA::_UNIT_GAUGE_PPM;
        if(ui.UI_RB_PPN2_CFIG->isChecked())
            return cConfigRGA::_UNIT_PP_N2;
        if(ui.UI_RB_PPMN2_CFIG->isChecked())
            return cConfigRGA::_UNIT_PPM_N2;
        return cConfigRGA::_UNIT_CURRENT_A;
    }
    bool isRunTimeUnit_S(){
        return ui.UI_RB_RUN_TIME_S_DT->isChecked();
    }
    inline void disableDeviceConfig(bool disable){
        ui.UI_W_SCAN_CONFIG_CFIG->setDisabled(disable);
        //ui.UI_GB_UNIT_DT->setDisabled(disable);
        //ui.UI_GB_CLEAN_DT->setDisabled(disable);
    }
    bool isScanMode(){
        if(ui.UI_TB_MODE_CFIG->currentIndex())
            return false;
        return true;
    }
    void setScanMode(bool ifScan){
        if(ifScan)
            ui.UI_TB_MODE_CFIG->setCurrentIndex(1);
        else
            ui.UI_TB_MODE_CFIG->setCurrentIndex(0);
    }
    /**********************sConfigure/sControlScan***********************/
    //void getCleanliness(cConfigRGA::_struct_cleanliness& p_struct_cleanliness);
    bool setParamScan(QString& massStart,
                  QString& massEnd,
                  QString& pointMass,
                  QString& holdTimeMs);
    bool getParamScan(QString& massStart,
                  QString& massEnd,
                  QString& pointMass,
                  QString& holdTimeMs);
    /**********************sConfigure/sControlSIM***********************/
    bool getParamSIM(QStringList& mass,
                     QStringList& name,
                     QStringList& holdTimeMs,
                     QString& pointMass);
    QString  getStrParamSIM();
    bool setStrParamSIM(QString str);
    /**********************sConfigure/sControlCPM***********************/
    bool getCPM(_PARAM_CPM& p_PARAM_CPM);
    void setCPM(const _PARAM_CPM& p_PARAM_CPM);
    //QString mTuneFilePath, mTuneFile;

//    volatile double sysVacuumAcq = 1e-4;//采集阶段校正使用-总气压
//    volatile double totalCurrentAcq= 1; //采集阶段校正使用-总电流
//    volatile double N2CurrentAcq= 1;    //采集阶段校正使用-氮气电流
    //bool baseLineEnable=false;          //configIniRead.value("/Calibrate/BaseLineEnable", true).toBool();
    //double baseLine= 1e-15;             //configIniRead.value("/Calibrate/BaseLine", 1e-15).toDouble();
    //bool isRawData= true;               //configIniRead.value("/Data/rawData", true).toBool();
//    double mSEM= 1;//为兼容旧版本保留
//    double mSFC= 1;
//    double mGEM= 1;
    //double mResistance= 20000000000.0;

signals:
    void sApplyConfig();//显示配置的设置
    void sApplyMethod();
    void sUpdateCPM();

private slots:
    void onHP(){
        m_IO_EXT.HP_ON= true;
        emit sUpdateCPM();
    }
    void offBYPASS(){
        m_IO_EXT.BYPASS_ON= false;
        emit sUpdateCPM();
    }
    //    void on_UI_PB_APPLY_METHOD_CFIG_clicked();
    //    void on_UI_PB_APPLY_CONFIG_CFIG_clicked();
    void on_UI_RB_PPM_CAL_CFIG_stateChanged(int arg1);
    void on_UI_RB_Multiplier_DT_clicked();
    void on_UI_RB_FARADAY_DT_clicked();
    void on_UI_CB_ENABLE_CPM_CFIG_clicked(bool checked);
    void on_UI_PB_customize_DT_clicked();
//    void on_UI_PB_SCAN_SAVE_CFIG_clicked(){
//        saveTuneFile(mTuneFile);
//    }
//    void on_UI_PB_SCAN_LOAD_CFIG_clicked(){
//        QString fileName = QFileDialog::getOpenFileName(this,
//                                                        tr("Open File"),
//                                                        mTuneFilePath,//QDir::currentPath()
//                                                        "Recipe Files(*.recipe)",
//                                                        0);
//        if (fileName.isNull())
//            return;
//        loadTuneFile(true, fileName);
//    }
//    void on_UI_PB_SCAN_SAVEAS_CFIG_clicked(){
//        QString fileName= QFileDialog::getSaveFileName(this, tr("Save As File"),
//                                                       mTuneFilePath,
//                                                       "Recipe Files(*.recipe)");
//        if (fileName.isNull())
//            return;
//        if (QFile::exists(fileName)) {
//            QMessageBox messageBox(this);
//            messageBox.setWindowTitle(tr("警告"));
//            messageBox.setText("<font color='black'>文件已存在，是否覆盖？</font>");
//            //messageBox.setIconPixmap(QPixmap(":/images/warn.png").scaled(80, 80));
//            messageBox.addButton(tr("覆盖"), QMessageBox::AcceptRole);
//            messageBox.addButton("退出", QMessageBox::RejectRole);

//            // 文本居中显示
//            QLabel* textLabel = messageBox.findChild<QLabel*>("qt_msgbox_label");
//            if (textLabel) {
//                textLabel->setAlignment(Qt::AlignCenter);
//            }

//            int result = messageBox.exec();  // 模态显示
//            if (result == QMessageBox::AcceptRole) {
//                QFile::remove(fileName);
//            }else if(result == QMessageBox::RejectRole){
//                return;
//            }
//        }
//        if (!QFile::copy(mTuneFile, fileName))
//            QMessageBox::warning( this, tr("警告"), tr("写入文件失败!"), QMessageBox::Ok );
//    }

private:
    Ui::sConfigure ui;
    sEditSIM* mEditSIM=nullptr;
    sJsCalculate* mJsCalculate=nullptr;
    _IO_EXT m_IO_EXT;
    _PARAM_CPM mPARAM_CPM;
    bool initCPM(cConfigRGA::_UNIT_GAUGE ExternalGaugeUnit,
                 cConfigRGA::_UNIT_GAUGE PumpGaugeUnit);

public:
    bool setParam(QByteArray& param);
    bool loadTuneFile(QString qPath=nullptr);

private:
    bool loadTuneFileCPM(QSettings& configIniRead);
    void saveTuneFile(QString qPath=nullptr);
    void saveTuneFileCPM(QString qPath=nullptr);
    void showMultiplierOption(bool show){
        if(show){
            ui.UI_W_Multiplier_START_DT->show();
            ui.UI_W_Multiplier_STOP_DT->show();
        }else{
            ui.UI_W_Multiplier_START_DT->hide();
            ui.UI_W_Multiplier_STOP_DT->hide();
        }
    }
public:
    _PARAM_CPM& getPARAM_CPM(){
        return mPARAM_CPM;
    }
    _IO_EXT& getIO_EXT(){
        return m_IO_EXT;
    }
};
