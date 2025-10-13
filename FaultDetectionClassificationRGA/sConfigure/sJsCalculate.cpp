#include "sJsCalculate.h"
#include <cfloat>

sJsCalculate::sJsCalculate(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    ui.UI_PB_CANCLE_JSCALC->hide();
    connect(ui.UI_PB_OK_JSCALC, &QPushButton::click, [this](){
        hide();
    });

    //ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //QPair
//    QStringList strlist;
//    strlist<<"18"<<"28";
//    mFormula.insert("N2/H2O", QPair<QStringList, QString>(strlist,
//                                                          "calc0("));
//    strlist.clear();
//    strlist<<"18"<<"32";
//    mFormula.insert("O2/H2O", QPair<QStringList, QString>(strlist,
//                                                          "calc1("));
//    strlist.clear();
//    strlist<<"28"<<"40";
//    mFormula.insert("N2/Ar", QPair<QStringList, QString>(strlist,
//                                                         "calc2("));


//    mScriptEngine.evaluate("function calc0(M18,M28){return M28/M18;}\n"
//                           "function calc1(M18,M32){return M32/M18;}\n"
//                           "function calc2(M28,M40){return M28/M40;}\n");
//    if(mScriptEngine.hasUncaughtException())
//        return;
}

sJsCalculate::~sJsCalculate()
{

}

bool sJsCalculate::checkFormula(const QString& massStart, const QString& massEnd)
{
    bool ok;
    int start= massStart.toInt(&ok);
    if(!ok) return false;
    int end= massEnd.toInt(&ok);
    if(!ok) return false;
    if(start>end)
        std::swap(start, end);
    QStringList mass;
    for(;start<= end; ++start){
        mass<< QString::number(start);
    }
    return checkFormula(mass);
}

bool sJsCalculate::checkFormula(const QStringList& mass)
{
    bool ok;
    indexDurationDataS= 0;
    indexDurationDataE= 0;
    //mUseDurationEX= getUseDurationEX();
    mPointCounts= getPointCounts().toInt(&ok);
    if(!ok)
        return false;
    mDurationEX_S= getDurationEX().toInt(&ok);
    if(!ok)
        return false;
    mScriptEngine.clearExceptions();
    mFormula.clear();
    mThreshold.clear();
    mIndexAL.clear();
	mExceedingUpCntMap.clear();
    mExceedingDownCntMap.clear();
    mDurationData.clear();
    QStringList functions;//mFunction.clear();
    QString formula;
    int rowCount= ui.tableWidget->rowCount();
    for(int i= 0; i< rowCount; ++i){
        QTableWidgetItem *itemJS= ui.tableWidget->item(i, 1);
        if(!itemJS)
            continue;
        formula= itemJS->text();
        if(!formula.contains("M"))
            continue;

        QStringList dstList, masses;
        QMap<int,QString> tmpMap;
        QStringList tmpList= formula.split("M");
        for(const auto& pStr: tmpList){
            int size= 0;
            const QChar* ptr = pStr.constData();
            QChar tmpChar;
            for (; size < formula.size(); ++size) {
                tmpChar= ptr[size].toLatin1();
                if(tmpChar>57|| tmpChar<48)
                    break;
            }
            QString tmpStr= pStr.left(size);

            int tmpInt= tmpStr.toInt(&ok);
            if(!ok)
                continue;
            tmpMap[tmpInt]= tmpStr;
        }
        if(!ok)
            continue;
        for(auto& key: tmpMap.keys()){
            masses<< tmpMap[key];
            dstList<< "M"+ tmpMap[key];
        }
        ok= true;
        for(const auto& pStr: masses){
            ok= mass.contains(pStr);
            if(!ok)
                break;
        }
        if(!ok)
            continue;
        QTableWidgetItem *itemThreD= ui.tableWidget->item(i, 2);
        if(!itemThreD)
            continue;
        QString tmpStr= itemThreD->text();
        double thresholdD= tmpStr.toDouble(&ok);
        if(!ok){
            if(tmpStr=="N")
                thresholdD= DBL_MIN;
            else
                continue;
        }

        QTableWidgetItem *itemThreU= ui.tableWidget->item(i, 3);
        if(!itemThreU)
            continue;
        tmpStr= itemThreU->text();
        double thresholdU= tmpStr.toDouble(&ok);
        if(!ok){
            if(tmpStr=="N")
                thresholdU= DBL_MAX;
            else
                continue;
        }


        QTableWidgetItem *itemName= ui.tableWidget->item(i, 0);
        if(itemName!= nullptr){

            mThreshold.insert(itemName->text(), QPair<double,double>(thresholdD, thresholdU));
            mIndexAL.insert(itemName->text(), i);
			mExceedingUpCntMap.insert(itemName->text(), 0);
            mExceedingDownCntMap.insert(itemName->text(), 0);
            mDurationData.insert(itemName->text(), 0);
//            dstList.clear();
//            for (int jj= 0; jj< masses.size(); ++jj) {
//                dstList[jj].append( "M"+ masses[jj]);
//            }
//            mFunction<< "function calc"+ QString::number(i)+"("+
//                        dstList.join(",")+"){return "+ formula+ ";}";

            //            mFormula.insert(itemName->text(),
            //                            QPair<QStringList, QString>(dstList,
            //                                                        "calc"+ QString::number(i)+"("));
            QString funcName = QString("calc%1").arg(i);
            QString params= dstList.join(",");
            /*functions<< "function calc"+ QString::number(i)+"("+
                        dstList.join(",")+"){return "+ formula+ ";}"*/;
            functions<< QString("function %1(%2){return %3;}").arg(funcName).arg(params).arg(formula);
            mFormula[itemName->text()] = QPair<QStringList, QScriptValue>
                    (masses, mScriptEngine.globalObject().property(funcName));


        }
    }
    mScriptEngine.evaluate(functions.join("\n"));
    if(mScriptEngine.hasUncaughtException())
        return false;
    return true;
}

bool sJsCalculate::calculate(int indexDataXIC_src,
                             int sizeDataXIC_src,
               QMAP_PARAM_XIC& pMapXIC,
               QMAP_PARAM_XIC& pMapOther)
{
    if(pMapOther.isEmpty()||pMapXIC.isEmpty())
        return true;
    //if(mCurretnIndex> sizeDataXIC_src)//if(sizeDataXIC_dst> sizeDataXIC_src)
    //    mCurretnIndex= sizeDataXIC_src;//    sizeDataXIC_dst= sizeDataXIC_src;
    if(!DurationTimer.isValid())
        DurationTimer.start();

    bool elapsedS= false;
    if(DurationTimer.elapsed()/1000> mDurationEX_S){
        elapsedS= true;
        DurationTimer.restart();
    }
    QString fun;
    foreach (auto& key, pMapOther.keys()) {
        /*调用calculate之前必须checkFormula*/
        auto& formula = mFormula[key];
        pMapOther[key]->yListXIC.resize(sizeDataXIC_src);
        auto* other = pMapOther[key];
        for(int i= indexDataXIC_src; i< sizeDataXIC_src; ++i){
            QScriptValueList args;
            foreach (const auto& mass, formula.first) {
                args << pMapXIC[mass]->yListXIC[i];
            }
            double result = formula.second.call(QScriptValue(), args).toNumber();
            other->yListXIC[i] = result;
            if(i> 0)
                mDurationData[key]= mDurationData[key]*(i- 1)/i+ result/i;
            /* Modify by WQH 20250617
                 * PXW现场模拟测试，因腔室跑片时会有气体进入，导致分压瞬间增大
                 * 避免正常跑片时误报警，增加Retry，当连续EXCEEDING_COUNT次的数据大于/小于阈值时，发送报警
                 */
            if(result< mThreshold[key].first){
                if(mExceedingDownCntMap[key] > mPointCounts) {
                    emit sExceedingDown(key, mIndexAL[key], mThreshold[key].first, result);//emit sExceedingDown();
                } else {
                    mExceedingDownCntMap[key]++;
                }
            }else if(result> mThreshold[key].second){
                if(mExceedingUpCntMap[key] > mPointCounts){
                    emit sExceedingUp(key, mIndexAL[key], mThreshold[key].second, result);//emit sExceedingUp();
                } else {
                    mExceedingUpCntMap[key]++;
                }
            }else{
                mExceedingUpCntMap[key] = 0;
                mExceedingDownCntMap[key] = 0;
            }
        }
    }
    //if(mUseDurationEX){
        if(elapsedS){
            indexDurationDataE= sizeDataXIC_src- 1;
            if(indexDurationDataS> indexDurationDataE)
                indexDurationDataS= 0;
            if(indexDurationDataS== indexDurationDataE)
                return true;
            QStringList tmpKey, tmpValue;
            foreach (auto& key, pMapOther.keys()) {
                double sum= 0;
                for(int i= indexDurationDataS; i< indexDurationDataE; ++i)
                    sum+= pMapOther[key]->yListXIC[i];
                tmpKey<< key;
                tmpValue<< QString::number(sum/(indexDurationDataE- indexDurationDataS), 'e', 2);
            }
            indexDurationDataS= sizeDataXIC_src;
            emit sDurationTimer(tmpKey.join(','), tmpValue.join(','));
        }
    //}
    return true;
}

