#include "sConfigure.h"

bool sConfigure::getParamSIM(QStringList& mass,
                 QStringList& name,
                 QStringList& holdTimeMs,
                 QString& pointMass)
{
    if(!mEditSIM||!mJsCalculate)
        return false;
    pointMass= ui.UI_CB_POINTMASS_DT->currentText();
    if(!mEditSIM->getParam(mass, name, holdTimeMs))
        return false;
    return mJsCalculate->checkFormula(mass);
}

QString  sConfigure::getStrParamSIM()
{
    QStringList mass, name, holdTimeMs;
    QString pointMass;
    if(!getParamSIM(mass, name, holdTimeMs, pointMass))
        return QString();
    QStringList tmpList;
    tmpList<< mass.join("@");
    tmpList<< name.join("@");
    tmpList<< holdTimeMs.join("@");
    return pointMass+ ";"+ tmpList.join("&");
}

bool sConfigure::setStrParamSIM(QString str)
{
    QStringList tmpList= str.split(";");
    if(tmpList.size()!= 2)
        return false;
    ui.UI_CB_POINTMASS_DT->setCurrentText(tmpList[0]);
    tmpList= tmpList[1].split("&");
    if(tmpList.size()!=3)
        return false;
    QStringList mass= tmpList[0].split("@"),
            name= tmpList[1].split("@"),
            holdTimeMs= tmpList[2].split("@");
    mEditSIM->setParam(mass, name, holdTimeMs);
    return true;
}
