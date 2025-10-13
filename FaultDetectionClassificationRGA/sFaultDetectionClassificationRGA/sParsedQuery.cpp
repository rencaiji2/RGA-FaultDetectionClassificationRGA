#include "sFaultDetectionClassificationRGA.h"

bool sFaultDetectionClassificationRGA::parsedData(const QStringList& listChannel, QStringList& customName, QStringList& customData,
                                                  const QString& keyEquipmentGroup, const QString& keyChamber, //const QString& keyChannel,
                                                  const QStringList& RecipeNameEQ, const QStringList& RecipeNameRGA, const QStringList& Step,
                                                  const QStringList& LotID, const QStringList& SlotID, const QStringList& WaferID, const QStringList& ChamberID,
                                                  const QList<uint>& dateList, const QStringList& timeList/*, int length, const QVector<double>& pData*/)
{
    int length= customName.size();
    if(customData.size()!= length)
        return false;//continue;

    //2025-1007
    if(length <= 0){
        return false;
    }

    mRecipeEQ.clear();
    mRecipeEQ<<"ALL";
    mLotID.clear();
    mLotID<<"ALL";
    mWaferID.clear();
    mWaferID<<"ALL";
    mChamberID.clear();
    mChamberID<<"ALL";

    QHash<QString, QPair<double,double>> tmpChannel;//"N2/H2O"
    for(auto& keyChannel: listChannel/*chamber[keyChamber]*/){
        QStringList tmpList= keyChannel.split(',');
        if(tmpList.size()!= 4)
            continue;
        tmpChannel[tmpList[0]]= QPair<double,double>(tmpList[2].toDouble(), tmpList[3].toDouble());
    }

    QMap <QString, QVector<double>> tmpMap;//key:"N2/H2O"
    QStringList tmpCustomName;
    QStringList tmpCustomData;
    for(int i= 0; i< length; ++i){
        tmpCustomName= customName[i].split(',');//"N2/AR""N2/H2O""O2/H2O"
        tmpCustomData= customData[i].split(',');
        int sizeData= tmpCustomData.size();
        for(int indexCustom=0; indexCustom< sizeData; ++indexCustom){
            auto& tmpKey= tmpCustomName[indexCustom];
            if(!tmpChannel.contains(tmpKey))
                continue;
            if (tmpMap.contains(tmpKey)) {
                tmpMap[tmpKey][i]= tmpCustomData[indexCustom].toDouble();
            }else{
                tmpMap[tmpKey]= QVector<double>(length, 0);
                tmpMap[tmpKey][i]= tmpCustomData[indexCustom].toDouble();
            }
        }
        if(!mLotID.contains(LotID[i]))
            mLotID<< LotID[i];
        if(!mWaferID.contains(WaferID[i]))
            mWaferID<< WaferID[i];
        if(!mChamberID.contains(ChamberID[i]))
            mChamberID<< ChamberID[i];
        if(!mRecipeEQ.contains(RecipeNameEQ[i]))
            mRecipeEQ<< RecipeNameEQ[i];
    }
    if(m_version == 0)
    {
        QString tmpStr= ui.UI_CB_CHAMBERID->currentText();
        ui.UI_CB_CHAMBERID->clear();
        ui.UI_CB_CHAMBERID->addItems(mChamberID);
        ui.UI_CB_CHAMBERID->setCurrentText(tmpStr);
    }
    else if(m_version == 1)
    {
        //啥都不做，标识一下
    }

    QString tmpStr= ui.UI_CB_LOTID->currentText();
    ui.UI_CB_LOTID->clear();
    ui.UI_CB_LOTID->addItems(mLotID);
    ui.UI_CB_LOTID->setCurrentText(tmpStr);

    tmpStr= ui.UI_CB_WAFERID->currentText();
    ui.UI_CB_WAFERID->clear();
    ui.UI_CB_WAFERID->addItems(mWaferID);
    ui.UI_CB_WAFERID->setCurrentText(tmpStr);

    tmpStr= ui.UI_CB_EQ_RECIPE->currentText();
    ui.UI_CB_EQ_RECIPE->clear();
    ui.UI_CB_EQ_RECIPE->addItems(mRecipeEQ);
    ui.UI_CB_EQ_RECIPE->setCurrentText(tmpStr);

    for(auto& keyChannel: tmpMap.keys()){//key:"N2/H2O"
        addChartSeries(keyEquipmentGroup, keyChamber, keyChannel,
                       RecipeNameEQ, RecipeNameRGA, Step,
                       LotID, SlotID, WaferID,ChamberID, dateList, timeList,
                       length, tmpMap[keyChannel]);
    }
    return true;
}

void sFaultDetectionClassificationRGA::parsedQuery(const QJsonObject& request, const QString& EquipmentGroupName)
{
    QList<uint> dateList;
    QStringList timeList, customName, customData,//6 0.54,0.52
            totalPressure, RecipeNameEQ, RecipeNameRGA, Step,//10
            LotID, SlotID, WaferID, ChamberID, SlitValve;//15
    for (auto it = request.begin(); it != request.end(); ++it) {
        if (!it.value().isObject())
            continue;
        QJsonObject Chamber=it.value().toObject();
        QString ChamberName= it.key();
        QString status= Chamber["status"].toString();
        if("success"!= status){
            //            QMessageBox::warning(nullptr, tr("提示"), EquipmentGroupName+ " "+
            //                                 ChamberName+ " "+ Chamber["message"].toString());
            qDebug()<< EquipmentGroupName+ " "+
                       ChamberName+ " "+ Chamber["message"].toString();
            continue;
        }
        QStringList listChannel= Chamber["channel"].toString().split(";");
        QJsonArray hobbiesArray = Chamber["data"].toArray();
        for (const QJsonValue& value : hobbiesArray) {
            if(!value.isObject())
                continue;
            QJsonObject recordObject= value.toObject();
            dateList<< /*static_cast<uint>*/(recordObject["date"].toString().toUInt());
            timeList<< recordObject["time"].toString();
            customName<< recordObject["customName"].toString();
            customData<< recordObject["customData"].toString();
            totalPressure<< recordObject["totalPressure"].toString();
            RecipeNameEQ<< recordObject["RecipeNameEQ"].toString();
            RecipeNameRGA<< recordObject["RecipeNameRGA"].toString();
            Step<< recordObject["Step"].toString();
            LotID<< recordObject["LotID"].toString();
            SlotID<< recordObject["SlotID"].toString();
            WaferID<< recordObject["WaferID"].toString();
            ChamberID<< recordObject["ChamberID"].toString();
            SlitValve<< recordObject["SlitValve"].toString();
        }
        parsedData(listChannel, customName, customData,
                   EquipmentGroupName, ChamberName,
                   RecipeNameEQ, RecipeNameRGA, Step,
                   LotID, SlotID, WaferID, ChamberID,
                   dateList, timeList);
    }

}

void sFaultDetectionClassificationRGA::onPointDoubleClicked(const QString &seriesName, const QPointF &point)
{
    qDebug() << "onPointDoubleClicked:" << seriesName << point;
    QString itemStr = createComboboxItemText(seriesName,point);
    //qDebug().noquote() << "itemStr: " <<itemStr;
    QStringList exitedTexts = PublicDef::getAllItems(ui.UI_PNTS_COMBOX);
    if(!exitedTexts.contains(itemStr)){
        ui.UI_PNTS_COMBOX->addItem(itemStr);
    }

    /*
    //输出【可以与单击点的QToolTip对比，index是否正确】
    QString formatStr = QDateTime::fromMSecsSinceEpoch(point.x()).toString("yyyy-MM-dd hh:mm:ss")
            + "\n值: " + QString::number(point.y())
             + "\nRecipeNameEQ: " + recipeEQ
             + "\nRecipeNameRGA: " + recipeRGA
             + "\nStep: " + step
             + "\nLotID: " + lotID
             + "\nSlotID: " + slotID
             + "\nWaferID: " + waferID;

    qDebug().noquote() << "onPointDoubleClicked-formatStr:\n  " << formatStr;
    */

}

void sFaultDetectionClassificationRGA::onExecuteSelectionClicked()
{
    if (m_chartWgt == nullptr) {
        return;
    }
    if(m_chartWgt == nullptr){
        QMessageBox::warning(nullptr,tr("警告"),tr("无有效的点选数据！"));
        return;
    }

    //启动参数
    QStringList processOptions;
    QStringList itemList = PublicDef::getAllItems(ui.UI_PNTS_COMBOX);
    foreach (QString curPnt, itemList)
    {
        //1.取格式化参数
        QStringList tmpList = PublicDef::extractParametersList(curPnt);
        if(tmpList.count() >= 6){
            QString timestamp = tmpList[0];
            //QString eq = tmpList[1];
            QString rga = tmpList[2];
            QString waferId = tmpList[3];
            QString lotId = tmpList[4];
            QString slotId = tmpList[5];

            //ini默认配置规则:MASS_DATA_%1(RecipeNameRGA@%2)(WaferID@%3)(LotID@%4)(SlotID@%5)
            //这边传5哥关键值关键值，而不传format，具体由后处理那边的自定义规则再去检索文件
            QString param = QString("%1,%2,%3,%4,%5")
                            .arg(timestamp).arg(rga).arg(waferId).arg(lotId).arg(slotId);

            processOptions.append(param);

        }
    }

    //启动后处理
    if(processOptions.count() > 0){
        onPostHandleProcessStart(processOptions);//处理软件启动
    }else{
        QMessageBox::warning(nullptr,tr("警告"),tr("无有效的数据供后处理软件使用！"));
    }



    //以下为作废的旧的，参考
    bool isOld = false;
    if(isOld)
    {
        //选中的点 - 时间格式是yyyyMMddhhmmsszzz精确到毫秒的，传给后处理的是到秒。这样就会有重复数据
        QList<QPair<QString, QPointF>> selectedPnts = m_chartWgt->getSelectedPoints();
        //匹配好的EQ RGA对应数据
        QMap<QDateTime,QStringList>& optionMap = m_chartWgt->getOptionMap();//20250925添加了mOptionMap

        if (selectedPnts.isEmpty()) {
            QMessageBox::information(this, "提示", "没有选中任何点");
            return;
        }

        //点数据处理(去毫秒,转格式化字符串) - 需去重【但从数据库存储看重读的概率不大】
        QStringList pntsList_s;
        for (int i =0;i < selectedPnts.count();i++) {
            QPair<QString, QPointF> tmp = selectedPnts.value(i);//QString是轴的名字，这边暂时用不到
            qreal x= tmp.second.x();
            //转到秒
            QString secs = QDateTime::fromMSecsSinceEpoch(x).toString("yyyyMMddhhmmss");
            if(!pntsList_s.contains(secs)){
                pntsList_s.append(secs);
            }
        }

        //EQ RGA匹配关系处理(去毫秒，转用格式化字符串组建map) - 需去重【但从数据库存储看重读的概率不大】
        QMap<QString,QStringList> optionMap_s;
        foreach (QDateTime key, optionMap.keys()) {
            QStringList EQ_RGA = optionMap.value(key);
            //同名key直接覆盖
            optionMap_s[key.toString("yyyyMMddhhmmss")] = EQ_RGA;
        }

        // 执行处理
        QStringList processOptions;
        foreach (QString pntX, pntsList_s) {
            if(optionMap_s.contains(pntX)){
                QStringList EQ_RGA = optionMap_s[pntX];
                if(EQ_RGA.count() >= 2){
                    QString recipeNameEQ = EQ_RGA.value(0,"NULL");
                    QString recipeNameRGA = EQ_RGA.value(1,"NULL");

                    //文件名格式 ：MASS_DATA_20250904172706(RecipeNameEQ@NULL)(RecipeNameRGA@ChC-Idle.tuneFile)
                    QString formarStr = QString("%1,%2,%3")
                            .arg(pntX)
                            .arg(recipeNameEQ)
                            .arg(recipeNameRGA);
                    //实际是把组建文件名的关键参数弄出来【时间，EQ,RGA】，不创建完整的文件名，由后处理自己去创建
                    processOptions.append(formarStr);
                }
            }
        }

        //启动后处理
        if(processOptions.count() > 0){
            onPostHandleProcessStart(processOptions);//处理软件启动
        }else{
            QMessageBox::warning(nullptr,tr("警告"),tr("无有效的数据供后处理软件使用！"));
        }

        // 处理完成后清除选中点
        m_chartWgt->clearSelection();
        qDebug() << "批量处理完成，共处理" << selectedPnts.size() << "个点";
    }

}

void sFaultDetectionClassificationRGA::onClearSelectionClicked()
{
    if (m_chartWgt != nullptr) {
        m_chartWgt->clearSelection();
    }
}
