#include "sConfigure.h"
#include <QSettings>
#include <QTimer>
#include <sConfigure.h>
#include <sNumberValidator.h>

bool sConfigure::initCPM(cConfigRGA::_UNIT_GAUGE ExternalGaugeUnit,
                         cConfigRGA::_UNIT_GAUGE PumpGaugeUnit)
{

    QRegularExpression regExp("[+-]?((\\d+\\.?\\d*)|(\\.\\d+))([eE][+-]?\\d+)?");

    double min_Gauge= pow(10, 0* 1.667- 11.33);
    double max_Gauge= pow(10, 10000* 1.667- 11.33);
    if(ExternalGaugeUnit== cConfigRGA::_UNIT_GAUGE_MBAR){
        ui.UI_RB_GAUGE_NULL_DT->setText(tr("规读数输入(mbar)"));
        ui.UI_L_VacuumGauge_uint1_CFIG->setText("mbar");
        ui.UI_L_VacuumGauge_uint2_CFIG->setText("mbar");
    }else if(cConfigRGA::_UNIT_GAUGE_PA== ExternalGaugeUnit){
        min_Gauge= min_Gauge* 100;
        max_Gauge= max_Gauge* 100;
        ui.UI_RB_GAUGE_NULL_DT->setText(tr("规读数输入(Pa)"));
        ui.UI_L_VacuumGauge_uint1_CFIG->setText("Pa");
        ui.UI_L_VacuumGauge_uint2_CFIG->setText("Pa");
    }else if(cConfigRGA::_UNIT_GAUGE_TORR== ExternalGaugeUnit){
        min_Gauge= min_Gauge* 0.750062;
        max_Gauge= max_Gauge* 0.750062;
        ui.UI_RB_GAUGE_NULL_DT->setText(tr("规读数输入(Torr)"));
        ui.UI_L_VacuumGauge_uint1_CFIG->setText("Torr");
        ui.UI_L_VacuumGauge_uint2_CFIG->setText("Torr");
    }else if(cConfigRGA::_UNIT_GAUGE_MTORR== ExternalGaugeUnit){
        min_Gauge= min_Gauge* 750.062;
        max_Gauge= max_Gauge* 750.062;
        ui.UI_RB_GAUGE_NULL_DT->setText(tr("规读数输入(mTorr)"));
        ui.UI_L_VacuumGauge_uint1_CFIG->setText("mTorr");
        ui.UI_L_VacuumGauge_uint2_CFIG->setText("mTorr");
    }

    ui.UI_LE_thresholdPUMP_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorPUMP = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdPUMP_CFIG);
    ui.UI_LE_thresholdPUMP_CFIG->setValidator(validatorPUMP);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdPUMP_CFIG,
                                         min_Gauge, max_Gauge);

    min_Gauge= 0;
    max_Gauge= 10000;//mv =mtorr
    QString strPumpUint;
    switch (PumpGaugeUnit) {
    case cConfigRGA::_UNIT_GAUGE_MBAR:{
        strPumpUint= "mBar";
        min_Gauge*= 0.00133322;
        max_Gauge*= 0.00133322;
        break;}
    case cConfigRGA::_UNIT_GAUGE_PA:{
        strPumpUint= "Pa";
        min_Gauge*= 0.133322;
        max_Gauge*= 0.133322;
        break;}
    case cConfigRGA::_UNIT_GAUGE_TORR:{
        strPumpUint= "Torr";
        min_Gauge/= 1000;
        max_Gauge/= 1000;
        break;}
    case cConfigRGA::_UNIT_GAUGE_MTORR:{
        strPumpUint= "mTorr";
        break;}
    default:break;
    }
    ui.label_41->setText(strPumpUint);
    ui.label_42->setText(strPumpUint);
    //ui.label_43->setText(strPumpUint);
    //ui.label_44->setText(strPumpUint);
    ui.label_30->setText(strPumpUint);
    ui.label_31->setText(strPumpUint);
    ui.label_32->setText(strPumpUint);
    ui.label_37->setText(strPumpUint);
    //ui.label_38->setText(strPumpUint);
    ui.UI_LE_thresholdLP0_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorLP0 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdLP0_CFIG);
    ui.UI_LE_thresholdLP0_CFIG->setValidator(validatorLP0);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdLP0_CFIG,
                                         min_Gauge, max_Gauge);

    ui.UI_LE_thresholdHP0_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorHP0 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdHP0_CFIG);
    ui.UI_LE_thresholdHP0_CFIG->setValidator(validatorHP0);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdHP0_CFIG,
                                         min_Gauge, max_Gauge);

//    ui.UI_LE_thresholdHC1_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
//    QRegularExpressionValidator *validatorHC0 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdHC1_CFIG);
//    ui.UI_LE_thresholdHC1_CFIG->setValidator(validatorHC0);

    ui.UI_LE_thresholdLP1_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorLP1 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdLP1_CFIG);
    ui.UI_LE_thresholdLP1_CFIG->setValidator(validatorLP1);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdLP1_CFIG,
                                         min_Gauge, max_Gauge);

    ui.UI_LE_thresholdHP1_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorHP1 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdHP1_CFIG);
    ui.UI_LE_thresholdHP1_CFIG->setValidator(validatorHP1);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdHP1_CFIG,
                                         min_Gauge, max_Gauge);

    ui.UI_LE_thresholdHC1_CFIG->setPlaceholderText("输入科学计数法（如 1.23e5）");
    QRegularExpressionValidator *validatorHC1 = new QRegularExpressionValidator(regExp, ui.UI_LE_thresholdHC1_CFIG);
    ui.UI_LE_thresholdHC1_CFIG->setValidator(validatorHC1);
    sNumberValidator::setNumberValidator(ui.UI_LE_thresholdHC1_CFIG,
                                         min_Gauge, max_Gauge);

    return true;//getCPM(mPARAM_CPM);
}

double getUI_LE_DELAYTIME_CFIG_s(QString str)
{
    double delayTimeS= str.toDouble();
    if(delayTimeS< 0.02)
        delayTimeS= 0.02;
    return delayTimeS;
}

bool sConfigure::getCPM(_PARAM_CPM& p_PARAM_CPM)
{
    bool ok;

    p_PARAM_CPM.autoCPM= ui.UI_RB_AUTO_CPM_CFIG->isChecked();
    //p_PARAM_CPM.isGauge1= ui.UI_RB_GAUGE1_CFIG->isChecked();
    p_PARAM_CPM.delayTimeS= getUI_LE_DELAYTIME_CFIG_s(ui.UI_LE_DELAYTIME_CFIG->text());

    p_PARAM_CPM.CPM_GaugeUnit= static_cast<cConfigRGA::_UNIT_GAUGE>(
                property("PumpGaugeUnit").toUInt());

    p_PARAM_CPM.thresholdLP0= ui.UI_LE_thresholdLP0_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);
    p_PARAM_CPM.thresholdLP1= ui.UI_LE_thresholdLP1_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);

    p_PARAM_CPM.thresholdHP0= ui.UI_LE_thresholdHP0_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);
    p_PARAM_CPM.thresholdHP1= ui.UI_LE_thresholdHP1_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);

    p_PARAM_CPM.thresholdHC= ui.UI_LE_thresholdHC1_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);
//    p_PARAM_CPM.thresholdHC1= ui.UI_LE_thresholdHC1_CFIG->text().toDouble(&ok);
//    IF_RETURN_FALSE(ok);

//    p_PARAM_CPM.thresholdBYPASS0= ui.UI_LE_thresholdBYPASS0_CFIG->text().toDouble(&ok);
//    IF_RETURN_FALSE(ok);
//    p_PARAM_CPM.thresholdBYPASS1= ui.UI_LE_thresholdBYPASS1_CFIG->text().toDouble(&ok);
//    IF_RETURN_FALSE(ok);

    p_PARAM_CPM.ExternalGaugeUnit= static_cast<cConfigRGA::_UNIT_GAUGE>(
                property("ExternalGaugeUnit").toUInt());

    p_PARAM_CPM.thresholdPUMP= ui.UI_LE_thresholdPUMP_CFIG->text().toDouble(&ok);
    IF_RETURN_FALSE(ok);
    return true;
}

void sConfigure::setCPM(const _PARAM_CPM& p_PARAM_CPM)
{
    ui.UI_RB_AUTO_CPM_CFIG->setChecked(p_PARAM_CPM.autoCPM);
    //ui.UI_RB_GAUGE1_CFIG->setChecked(p_PARAM_CPM.isGauge1);
    ui.UI_LE_DELAYTIME_CFIG->setText(
                QString::number(p_PARAM_CPM.delayTimeS));

    ui.UI_LE_thresholdLP0_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdLP0, 'e', 2));
    ui.UI_LE_thresholdLP1_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdLP1, 'e', 2));

    ui.UI_LE_thresholdHP0_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdHP0, 'e', 2));
    ui.UI_LE_thresholdHP1_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdHP1, 'e', 2));

    //    ui.UI_LE_thresholdHC0_CFIG->setText(QString::number(p_PARAM_CPM.thresholdHC0, 'e', 2));
    ui.UI_LE_thresholdHC1_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdHC, 'e', 2));
    //    ui.UI_LE_thresholdBYPASS0_CFIG->setText(QString::number(p_PARAM_CPM.thresholdBYPASS0));
    //    ui.UI_LE_thresholdBYPASS1_CFIG->setText(QString::number(p_PARAM_CPM.thresholdBYPASS1));
    ui.UI_LE_thresholdPUMP_CFIG->setText(
                QString::number(p_PARAM_CPM.thresholdPUMP, 'e', 2));
}

bool sConfigure::loadTuneFileCPM(QSettings& configIniRead)
{

    //    if(qPath.isEmpty())
    //        qPath = QCoreApplication::applicationDirPath()+"/default.tune";
    //    QSettings configIniRead(qPath, QSettings::IniFormat);
    //    configIniRead.setIniCodec("utf-8");


    bool ok= false;
    double tmpD= 0;

    mPARAM_CPM.delayTimeS= configIniRead.value("/CPM/delayTimeS", 1).toBool();
    mPARAM_CPM.autoCPM= configIniRead.value("/CPM/enableCPM", false).toBool();

    tmpD= configIniRead.value("/CPM/thresholdHC", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdHC= tmpD;

    tmpD= configIniRead.value("/CPM/thresholdHP0", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdHP0= tmpD;
    tmpD= configIniRead.value("/CPM/thresholdHP1", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdHP1= tmpD;

    tmpD= configIniRead.value("/CPM/thresholdLP0", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdLP0= tmpD;
    tmpD= configIniRead.value("/CPM/thresholdLP1", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdLP1= tmpD;

    //    tmpD= configIniRead.value("/CPM/thresholdBYPASS0", 0).toDouble(&ok);
    //    IF_RETURN_FALSE(ok);
    //    mPARAM_CPM.thresholdBYPASS0= tmpD;
    //    tmpD= configIniRead.value("/CPM/thresholdBYPASS1", 0).toDouble(&ok);
    //    IF_RETURN_FALSE(ok);
    //    mPARAM_CPM.thresholdBYPASS1= tmpD;

    tmpD= configIniRead.value("/CPM/thresholdPUMP", 0).toDouble(&ok);
    IF_RETURN_FALSE(ok);
    mPARAM_CPM.thresholdPUMP= tmpD;

    cConfigRGA::_UNIT_GAUGE PumpGaugeUnit= static_cast<cConfigRGA::_UNIT_GAUGE>(
                property("PumpGaugeUnit").toInt());


    setCPM(mPARAM_CPM);
    QString strUint;
    switch (PumpGaugeUnit) {
    case cConfigRGA::_UNIT_GAUGE_MBAR:
        strUint= "mBar";break;
    case cConfigRGA::_UNIT_GAUGE_PA:
        strUint= "Pa";break;
    case cConfigRGA::_UNIT_GAUGE_TORR:
        strUint= "Torr";break;
    case cConfigRGA::_UNIT_GAUGE_MTORR:
        strUint= "mTorr";break;
    default:break;
    }
    ui.label_41->setText(strUint);
    ui.label_42->setText(strUint);
    //ui.label_43->setText(strUint);
    //ui.label_44->setText(strUint);

    ui.label_30->setText(strUint);

    ui.label_31->setText(strUint);
    ui.label_32->setText(strUint);
    ui.label_37->setText(strUint);
    //ui.label_38->setText(strUint);


    return true;
}

void sConfigure::saveTuneFileCPM(QString qPath)
{
    _PARAM_CPM tmpPARAM_CPM;
    if(!getCPM(tmpPARAM_CPM))
        return;

    if(qPath.isEmpty())
        qPath = QCoreApplication::applicationDirPath()+"/default.tune";
    QSettings pSettings(qPath, QSettings::IniFormat);
    pSettings.setIniCodec("utf-8");

    pSettings.setValue("/CPM/delayTimeS", tmpPARAM_CPM.delayTimeS);
    pSettings.setValue("/CPM/enableCPM", tmpPARAM_CPM.autoCPM);
    //pSettings.setValue("/CPM/isGauge1", tmpPARAM_CPM.isGauge1);

    pSettings.setValue("/CPM/thresholdHC", tmpPARAM_CPM.thresholdHC);
    //pSettings.setValue("/CPM/thresholdHC1", tmpPARAM_CPM.thresholdHC1);

    pSettings.setValue("/CPM/thresholdHP0", tmpPARAM_CPM.thresholdHP0);
    pSettings.setValue("/CPM/thresholdHP1", tmpPARAM_CPM.thresholdHP1);

    pSettings.setValue("/CPM/thresholdLP0", tmpPARAM_CPM.thresholdLP0);
    pSettings.setValue("/CPM/thresholdLP1", tmpPARAM_CPM.thresholdLP1);

//    pSettings.setValue("/CPM/thresholdBYPASS0", tmpPARAM_CPM.thresholdBYPASS0);
//    pSettings.setValue("/CPM/thresholdBYPASS1", tmpPARAM_CPM.thresholdBYPASS1);

    pSettings.setValue("/CPM/thresholdPUMP", tmpPARAM_CPM.thresholdPUMP);
}
