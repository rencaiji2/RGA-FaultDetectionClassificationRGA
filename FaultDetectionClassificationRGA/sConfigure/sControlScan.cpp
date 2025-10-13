#include "sConfigure.h"

//void sConfigure::getCleanliness(cConfigRGA::_struct_cleanliness& p_struct_cleanliness)
//{
//    p_struct_cleanliness.startMassNumerator= ui.UI_LE_STARTMASS_CLEAN_DT->text().toInt();
//    p_struct_cleanliness.endMassNumerator= ui.UI_LE_ENDMASS_CLEAN_DT->text().toInt();
//    p_struct_cleanliness.startMassDenominator= ui.UI_LE_STARTMASS_CLEAN_D_DT->text().toInt();
//    p_struct_cleanliness.endMassDenominator= ui.UI_LE_ENDMASS_CLEAN_D_DT->text().toInt();
//}

bool sConfigure::setParamScan(QString& massStart,
              QString& massEnd,
              QString& pointMass,
              QString& holdTimeMs)
{
    ui.UI_LE_STARTMASS_DT->setValue(massStart.toInt());
    ui.UI_LE_ENDMASS_DT->setValue(massEnd.toInt());
    ui.UI_CB_POINTMASS_DT->setCurrentText(pointMass);
    ui.UI_CB_HOLDTIME_DT->setCurrentText(holdTimeMs);
    return true;
}

bool sConfigure::getParamScan(QString& massStart,
              QString& massEnd,
              QString& pointMass,
              QString& holdTimeMs)
{
    if(!mJsCalculate)
        return false;
    massStart= ui.UI_LE_STARTMASS_DT->text();
    massEnd= ui.UI_LE_ENDMASS_DT->text();
    holdTimeMs= ui.UI_CB_HOLDTIME_DT->currentText();
    pointMass= ui.UI_CB_POINTMASS_DT->currentText();
    return mJsCalculate->checkFormula(massStart, massEnd);
}
