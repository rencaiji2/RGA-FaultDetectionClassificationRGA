#include "sConfigure.h"

#include <QBuffer>
#include <QFile>
#include <QIntValidator>
#include <QSettings>


sConfigure::sConfigure(//QString& DeviceTuneFilePath,
                       cConfigRGA::_UNIT_GAUGE ExternalGaugeUnit,
                       cConfigRGA::_UNIT_GAUGE PumpGaugeUnit,
                       //_STATE_RGA& pSTATE_RGA,
                       QWidget *parent) :
    QWidget(parent)/*,
    mTuneFilePath(DeviceTuneFilePath)*/
{
    ui.setupUi(this);
    initProperty(/*DeviceConfigFilePath*/);
    setProperty("ExternalGaugeUnit", ExternalGaugeUnit);
    setProperty("PumpGaugeUnit", PumpGaugeUnit);

    //mTuneFile= mTuneFilePath+ "/TuneFile.tune";
    ui.lineEdit_scanDelay->setValidator(new QIntValidator(1, 6000, this));
    ui.UI_LE_VALVE4_DELAYTIME_CFIG->hide();
    ui.UI_L_VALVE4_DELAYTIME_CFIG->hide();

//    int limitMass= 50;
//    switch (pSTATE_RGA.LevelSoftware) {
//    case 1:limitMass= 100; break;
//    case 2:limitMass= 200; break;
//    case 3:limitMass= 300; break;
//    }

//    ui.UI_LE_STARTMASS_DT->setRange(0,limitMass);
//    ui.UI_LE_ENDMASS_DT->setRange(0,limitMass);

//    ui.UI_LE_STARTMASS_DT->setValue(0);
//    ui.UI_LE_ENDMASS_DT->setValue(limitMass);

    QStringList str_list;
    str_list<<"1"<<"2"<<"4"<<"5"<<"10"<<"20"<<"25"<<"50"<<"100";
    ui.UI_CB_POINTMASS_DT->addItems(str_list);
    str_list.clear();
    str_list<<"2"<<"4"<<"8"<<"16"<<"32"<<"64"
           <<"128"<<"256"<<"512"<<"1024"
          <<"2048" <<"4096"<<"8192"<<"16384";
    ui.UI_CB_HOLDTIME_DT->addItems(str_list);
    mEditSIM = new sEditSIM(this);
    ui.UI_LAYOUT_EDITSIM_CFIG->addWidget(mEditSIM);
    mJsCalculate= new sJsCalculate();
    initCPM(ExternalGaugeUnit, PumpGaugeUnit);
    //loadTuneFile(true, mTuneFile);
}

sConfigure::~sConfigure()
{
//    if(mTimerCPM_ID!= -1)
//        killTimer(mTimerCPM_ID);
//    mTimerCPM_ID= -1;
}

bool sConfigure::setParam( QByteArray& param)
{
   QString  qPath = QCoreApplication::applicationDirPath()+"/tmpRecipeBackup";
   QFile file(qPath);
   if (file.open(QIODevice::WriteOnly)) {
       file.write(param);
       file.close();
   }
   return loadTuneFile(qPath);
}

bool sConfigure::loadTuneFile(QString qPath)
{
    if(qPath.isEmpty())
        qPath = QCoreApplication::applicationDirPath()+"/default.tune";

    bool ok= false;
    double tmpD= 0;
    QSettings configIniRead(qPath, QSettings::IniFormat);
    configIniRead.setIniCodec("utf-8");

    if(configIniRead.value("Debug/SwitchN2", false).toBool()){
        ui.UI_RB_PPN2_CFIG->show();
        ui.UI_RB_PPMN2_CFIG->show();
    }else{
        ui.UI_RB_PPN2_CFIG->hide();
        ui.UI_RB_PPMN2_CFIG->hide();
    }

    QString tmpStr= configIniRead.value("/Spectrum/SignalUnit", "A").toString();
    if(tmpStr.contains("Torr")){
        ui.UI_RB_TORR_CFIG->setChecked(true);
    }else if(tmpStr.contains("Pa")){
        ui.UI_RB_PA_CFIG->setChecked(true);
    }else if(tmpStr.contains("mbar")){
        ui.UI_RB_MBAR_CFIG->setChecked(true);
    }else if(tmpStr.contains("A")){
        ui.UI_RB_A_CFIG->setChecked(true);
    }else if(tmpStr.contains("PPM")){//Spectrum/SignalUnit", "PPM"
        ui.UI_RB_PPM_CFIG->setChecked(true);
    }else{
        ui.UI_RB_A_CFIG->setChecked(true);
    }
    tmpStr= configIniRead.value("/Spectrum/runTimeUnit", "S").toString();//"Date"
    if(tmpStr.contains("S")){
        ui.UI_RB_RUN_TIME_S_DT->setChecked(true);
    }else{
        ui.UI_RB_RUN_TIME_DATE_DT->setChecked(true);
    }
//    tmpStr= configIniRead.value("/Cleanliness/startMassNumerator", "45").toString();//"startMass"
//    ui.UI_LE_STARTMASS_CLEAN_DT->setText(tmpStr);
//    tmpStr= configIniRead.value("/Cleanliness/endMassNumerator", "100").toString();//"endMass"
//    ui.UI_LE_ENDMASS_CLEAN_DT->setText(tmpStr);
//    tmpStr= configIniRead.value("/Cleanliness/startMassDenominator", "0").toString();//"startMass"
//    ui.UI_LE_STARTMASS_CLEAN_D_DT->setText(tmpStr);
//    tmpStr= configIniRead.value("/Cleanliness/endMassDenominator", "100").toString();//"endMass"
//    ui.UI_LE_ENDMASS_CLEAN_D_DT->setText(tmpStr);
    tmpStr= configIniRead.value("/ConfigScan/timeIntervals", "1000").toString();
    ui.lineEdit_scanDelay->setText(tmpStr);
    tmpStr= configIniRead.value("/ConfigScan/Fila", "1").toString();//"2"
    if(tmpStr.contains("1")){
        ui.UI_RB_Fila1_DT->setChecked(true);
    }else{
        ui.UI_RB_Fila2_DT->setChecked(true);
    }

    tmpStr= configIniRead.value("/ConfigScan/detector", "Multiplier").toString();//"Faraday"
    if(tmpStr.contains("Multiplier")){
        ui.UI_RB_Multiplier_DT->setChecked(true);
        showMultiplierOption(true);
    }else{
        ui.UI_RB_FARADAY_DT->setChecked(true);
        showMultiplierOption(false);
    }

    tmpStr= configIniRead.value("/ConfigScan/currentLimit", "0").toString();
    ui.UI_LE_CURR_LIMIT_DT->setText(tmpStr);

    tmpStr= configIniRead.value("/ConfigScan/gaugeVoltProt", "0").toString();
    ui.UI_LE_gauge_volt_prot_mv_DT->setText(tmpStr);

    ui.UI_RB_GAUGE_OUT_DT->setChecked(configIniRead.value("/ConfigScan/isExternalGauge", true).toBool());
    tmpStr= configIniRead.value("/ConfigScan/VacuumGauge", "0").toString();
    ui.UI_LE_GAUGE_NULL_DT->setText(tmpStr);

    ui.UI_RB_PPM_CAL_CFIG->setChecked(configIniRead.value("/ConfigScan/UsingStandard", false).toBool());

    int tmpInt= configIniRead.value("/Method/startMass").toInt();
    ui.UI_LE_STARTMASS_DT->setValue(tmpInt);
    tmpInt= configIniRead.value("/Method/endMass").toInt();
    ui.UI_LE_ENDMASS_DT->setValue(tmpInt);
    tmpStr= configIniRead.value("/Method/pointsPerAmu").toString();
    ui.UI_CB_POINTMASS_DT->setCurrentText(tmpStr);
    tmpStr= configIniRead.value("/Method/dwellTime").toString();
    ui.UI_CB_HOLDTIME_DT->setCurrentText(tmpStr);

    tmpStr= configIniRead.value("/Method/SIM", "").toString();
    setStrParamSIM(tmpStr);

    tmpStr= configIniRead.value("/Method/JsCalculate", "").toString();
    mJsCalculate->setParame(tmpStr);
    mJsCalculate->setUseDurationEX(configIniRead.value("/Method/UseDuration​​EX", false).toBool());
    mJsCalculate->setDurationEX(configIniRead.value("/Method/​​AlarmDuration​​EX", "600").toString());
    mJsCalculate->setPointCounts(configIniRead.value("/Method/​​AlarmPointCounts​​EX", "8").toString());
    ui.UI_CB_BASELINE_ENABLE_CFIG->setChecked(configIniRead.value("/Calibrate/BaseLineEnable", true).toBool());

    return loadTuneFileCPM(configIniRead);
}

void sConfigure::saveTuneFile(QString qPath)
{
    QString path;
    if(qPath.isEmpty())
        path = QCoreApplication::applicationDirPath()+"/default.tune";
    else
        path = qPath;
    QSettings configIniRead(path, QSettings::IniFormat);
    configIniRead.setIniCodec("utf-8");
    if(ui.UI_RB_A_CFIG->isChecked()){
        configIniRead.setValue("/Spectrum/SignalUnit", "A");
    }else if(ui.UI_RB_PA_CFIG->isChecked()){
        configIniRead.setValue("/Spectrum/SignalUnit", "Pa");
    }else if(ui.UI_RB_MBAR_CFIG->isChecked()){
        configIniRead.setValue("/Spectrum/SignalUnit", "mbar");
    }else if(ui.UI_RB_TORR_CFIG->isChecked()){
        configIniRead.setValue("/Spectrum/SignalUnit", "Torr");
    }else if(ui.UI_RB_PPM_CFIG->isChecked()){
        configIniRead.setValue("/Spectrum/SignalUnit", "PPM");
    }else{
        configIniRead.setValue("/Spectrum/SignalUnit", "A");
    }

    if(ui.UI_RB_RUN_TIME_S_DT->isChecked()){
        configIniRead.setValue("/Spectrum/runTimeUnit", "S");
    }else{
        configIniRead.setValue("/Spectrum/runTimeUnit", "Date");
    }
//    configIniRead.setValue("/Cleanliness/startMassNumerator", ui.UI_LE_STARTMASS_CLEAN_DT->text());//"startMass"

//    configIniRead.setValue("/Cleanliness/endMassNumerator", ui.UI_LE_ENDMASS_CLEAN_DT->text());//"endMass"

//    configIniRead.setValue("/Cleanliness/startMassDenominator", ui.UI_LE_STARTMASS_CLEAN_D_DT->text());//"startMass"

//    configIniRead.setValue("/Cleanliness/endMassDenominator", ui.UI_LE_ENDMASS_CLEAN_D_DT->text());//"endMass"

    configIniRead.setValue("/ConfigScan/timeIntervals", ui.lineEdit_scanDelay->text());


    if(ui.UI_RB_Fila1_DT->isChecked()){
        configIniRead.setValue("/ConfigScan/Fila", "1");//"2"
    }else{
        configIniRead.setValue("/ConfigScan/Fila", "2");//"2"
    }

    if(ui.UI_RB_Multiplier_DT->isChecked()){
        configIniRead.setValue("/ConfigScan/detector", "Multiplier");//"Faraday"
    }else{
        configIniRead.setValue("/ConfigScan/detector", "Faraday");//"Faraday"
    }

    configIniRead.setValue("/ConfigScan/currentLimit", ui.UI_LE_CURR_LIMIT_DT->text());

    configIniRead.setValue("/ConfigScan/gaugeVoltProt", ui.UI_LE_gauge_volt_prot_mv_DT->text());

    configIniRead.setValue("/ConfigScan/isExternalGauge", ui.UI_RB_GAUGE_OUT_DT->isChecked());
    configIniRead.setValue("/ConfigScan/VacuumGauge", ui.UI_LE_GAUGE_NULL_DT->text());

    configIniRead.setValue("/ConfigScan/UsingStandard", ui.UI_RB_PPM_CAL_CFIG->isChecked());

    configIniRead.setValue("/Method/startMass", ui.UI_LE_STARTMASS_DT->text());
    configIniRead.setValue("/Method/endMass", ui.UI_LE_ENDMASS_DT->text());
    configIniRead.setValue("/Method/pointsPerAmu", ui.UI_CB_POINTMASS_DT->currentText());
    configIniRead.setValue("/Method/dwellTime", ui.UI_CB_HOLDTIME_DT->currentText());

    configIniRead.setValue("/Method/SIM", getStrParamSIM());

    configIniRead.setValue("/Method/JsCalculate", mJsCalculate->getParame());
    configIniRead.setValue("/Method/UseDuration​​EX", mJsCalculate->getUseDurationEX());
    configIniRead.setValue("/Method/​​AlarmDuration​​EX", mJsCalculate->getDurationEX());
    configIniRead.setValue("/Method/​​AlarmPointCounts​​EX", mJsCalculate->getPointCounts());

    configIniRead.setValue("/Calibrate/BaseLineEnable", ui.UI_CB_BASELINE_ENABLE_CFIG->isChecked());
}

//void sConfigure::on_UI_PB_APPLY_METHOD_CFIG_clicked()
//{
//    saveTuneFile(mTuneFile);
//    emit sApplyMethod();
//    hide();
//}

//void sConfigure::on_UI_PB_APPLY_CONFIG_CFIG_clicked()
//{
//    saveTuneFile(mTuneFile);
//    emit sApplyConfig();
//}

void sConfigure::on_UI_RB_PPM_CAL_CFIG_stateChanged(int arg1)
{
    if (arg1 == 2) {
        ui.UI_GB_UNIT_DT->setEnabled(false);
        ui.UI_CB_BASELINE_ENABLE_CFIG->setChecked(false);
        ui.UI_CB_BASELINE_ENABLE_CFIG->setEnabled(false);
    } else {
        ui.UI_GB_UNIT_DT->setEnabled(true);
        ui.UI_CB_BASELINE_ENABLE_CFIG->setEnabled(true);
    }
}

void sConfigure::on_UI_RB_Multiplier_DT_clicked()
{
    showMultiplierOption(ui.UI_RB_Multiplier_DT->isChecked());
}

void sConfigure::on_UI_RB_FARADAY_DT_clicked()
{
    showMultiplierOption(!ui.UI_RB_FARADAY_DT->isChecked());
}

void sConfigure::on_UI_CB_ENABLE_CPM_CFIG_clicked(bool checked)
{
    ui.UI_CB_LP_CFIG->setDisabled(checked);
    ui.UI_CB_HP_CFIG->setDisabled(checked);
    ui.UI_CB_HC_CFIG->setDisabled(checked);
    ui.UI_CB_BYPASS_CFIG->setDisabled(checked);
    ui.UI_CB_PUMP_CFIG->setDisabled(checked);
}

void sConfigure::on_UI_PB_customize_DT_clicked()
{
    if(mJsCalculate)
        mJsCalculate->show();
}


