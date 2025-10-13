#include "sFaultDetectionClassificationRGA.h"

#include <LibAlgorithm/PublicAlgorithm.h>

#include <publicdef.h>

bool sFaultDetectionClassificationRGA::createNewSeries(
        QMap<QString, QList<QPair<QDateTime, double>>>& pMap)
{
    chart->removeAllSeries();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    QDateTime minTime = QDateTime::currentDateTime();
    QDateTime maxTime = QDateTime::fromSecsSinceEpoch(0);

    for(auto& key: pMap.keys()){
        QScatterSeries *series = new QScatterSeries();
        series->setName(key);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle); // 圆形点
        series->setMarkerSize(10.0); // 点的大小
        // 显示坐标提示框
        series->setPointsVisible(true);
        connect(static_cast<QXYSeries*>(series), &QXYSeries::clicked, [=]{
            series->setVisible(!series->isVisible());
        });
        //        connect(series, &QScatterSeries::hovered, [=](const QPointF& point, bool state){
        //            if(state) {
        //                QList<QPointF> points = series->points();
        //                int indexSeries= -1;
        //                for (int i = 0; i < points.size(); ++i) {
        //                    const QPointF& pt = points.at(i);
        //                    if (std::abs(pt.x() - point.x()) < 1e-6 &&
        //                            std::abs(pt.y() - point.y()) < 1e-6) {
        //                        indexSeries= i;
        //                    }
        //                }
        //                if(indexSeries== -1)
        //                    return;
        //                QToolTip::showText(QCursor::pos(), formatTooltip(point, key, indexSeries));
        //            } else {
        //                QToolTip::hideText();
        //            }
        //        });

        QList<QPair<QDateTime, double>>& dataPoints= pMap[key];
        for (const auto &point : dataPoints) {
            qint64 MSecsSinceEpoch= point.first.toMSecsSinceEpoch();
            *series << QPointF(point.first.toMSecsSinceEpoch(), point.second);
            minY = qMin(minY, point.second);
            maxY = qMax(maxY, point.second);
            if(point.first < minTime)
                minTime = point.first;
            if(point.first > maxTime)
                maxTime = point.first;
        }
        chart->addSeries(series);
        if(minTime.isValid() && maxTime.isValid()) {
            double yRange = qMax(maxY - minY, 0.1);  // 避免0范围
            axisY->setRange(minY - yRange * 0.05, maxY + yRange * 0.05);
            qint64 timeRangeSecs = minTime.secsTo(maxTime);// 增加时间边界范围
            if(timeRangeSecs < 60) {// 如果时间范围小于1分钟，增加足够的时间显示宽度
                minTime = minTime.addSecs(-60 * 10);
                maxTime = maxTime.addSecs(60 * 10);
            } else {// 按时间范围的10%增加边界
                minTime = minTime.addSecs(-timeRangeSecs / 10);
                maxTime = maxTime.addSecs(timeRangeSecs / 10);
            }
            qDebug() << "设置X轴范围:" << minTime.toString() << "到" << maxTime.toString();
            axisX->setRange(minTime, maxTime);
        } else {
            qWarning() << "无效的坐标轴范围，使用默认范围";
            QDateTime now = QDateTime::currentDateTime();// 默认范围：今天0点到24点
            axisX->setRange(now.addDays(-1), now.addDays(1));
            axisY->setRange(0, 10);
        }
        series->attachAxis(axisX);
        series->attachAxis(axisY);  // 附加Y轴
    }
    return true;
}

void sFaultDetectionClassificationRGA::addChartSeries(const QString& keyEquipmentGroup, const QString& keyChamber, const QString& keyChannel,
                                                      const QStringList& RecipeNameEQ, const QStringList& RecipeNameRGA, const QStringList& Step,
                                                      const QStringList& LotID, const QStringList& SlotID, const QStringList& WaferID,const QStringList& ChamberID,
                                                      const QList<uint>& dateList, const QStringList& timeList, int length, const QVector<double>& pData)
{
    if(!m_chartWgt)
        return;
    QString SeriesName= keyEquipmentGroup+" "+ keyChamber+" "+ keyChannel;
    //mDataMap[SeriesName]= QList<QPair<QDateTime, double>>();
    m_chartWgt->mOptionMap[SeriesName]= dateList;
    m_chartWgt->mRecipeNameEQ[SeriesName]= RecipeNameEQ;
    m_chartWgt->mRecipeNameRGA[SeriesName]= RecipeNameRGA;
    m_chartWgt->mStep[SeriesName]= Step;
    m_chartWgt->mLotID[SeriesName]= LotID;
    m_chartWgt->mSlotID[SeriesName]= SlotID;
    m_chartWgt->mWaferID[SeriesName]= WaferID;
    m_chartWgt->mChamberID[SeriesName] = ChamberID;
    QList<QPair<QDateTime, double>>& pList= mDataMap[SeriesName];
    for(int i= 0; i< length; ++i){
        QDateTime dt;
        dt.setDate(QDate::fromJulianDay(dateList[i]));
        dt.setTime(QTime::fromString(timeList[i], "hh:mm:ss.zzz"));
        pList.append(qMakePair(dt, pData[i]));
    }

    //2025-1001 新增
    QList<QPair<QDateTime, QString>>& pChamberIDList= mChamberIDDataMap[SeriesName];
    for(int i= 0; i< length; ++i){
        QDateTime dt;
        dt.setDate(QDate::fromJulianDay(dateList[i]));
        dt.setTime(QTime::fromString(timeList[i], "hh:mm:ss.zzz"));
        pChamberIDList.append(qMakePair(dt, ChamberID[i]));
    }
}

void sFaultDetectionClassificationRGA::onUpdateCL_Manual()
{

}
/*!
 * \brief sFaultDetectionClassificationRGA::onPostHandleProcessStart 带参数启动后处理软件
 * \param i_option
 */
void sFaultDetectionClassificationRGA::onPostHandleProcessStart(QStringList i_option)
{
    qDebug().noquote() << "onPostHandleProcessStart:    " << i_option;
    cJsonFileOperate::jsonStr2File("onPostHandleProcessStart.txt",i_option.join("\n"));
    if(i_option.count() > 0)
    {
        //后处理软件路径
        QString appPath = "E:\\workspaces_wqh\\RGA_DataAnalysis\\DataAnalysis.exe";//临时需要删除
        //QString appPath = QCoreApplication::applicationDirPath()+"/DataAnalysis.exe";

        // 创建进程对象
        QProcess* process = new QProcess(this);

        // 设置程序和参数
        process->setProgram(appPath);
        process->setArguments(i_option);

        // 连接输出信号
        connect(process, &QProcess::readyReadStandardOutput, [process]() {
            QByteArray output = process->readAllStandardOutput();
            if (!output.isEmpty()) {
                qDebug() << "后处理输出:" << output.trimmed();
            }
        });

        connect(process, &QProcess::readyReadStandardError, [process]() {
            QByteArray error = process->readAllStandardError();
            if (!error.isEmpty()) {
                qWarning() << "后处理错误:" << error.trimmed();
            }
        });

        // 进程结束后自动清理
        connect(process,SIGNAL(finished(int , QProcess::ExitStatus)),
                this,SLOT(onPostSoftwareProcessFinished(int , QProcess::ExitStatus)));
        connect(process, &QProcess::errorOccurred, [process]() {
            qWarning() << "后处理进程错误:" << process->errorString();
            process->deleteLater();
        });

        // 启动进程（不等待）
        process->start(QProcess::ReadOnly);

        if (!process->waitForStarted(5000)) {
            qWarning() << "启动失败:" << process->errorString();
            QMessageBox::warning(nullptr, tr("错误"), tr("后处理软件启动失败！"));
            delete process;
            return;
        }

        qDebug() << "后处理软件已启动";
    }
    else
    {
        QMessageBox::warning(nullptr, tr("警告"), tr("无法自动启动后处理软件，请手动启动，或者反馈技术人员！"));
    }
}

/*!
 * \brief sFaultDetectionClassificationRGA::onPointSelected 散点选中的combobox数据处理
 * \param seriesName
 * \param point
 * \param selected
 */
void sFaultDetectionClassificationRGA::onPointSelected(const QString &seriesName, const QPointF &point, bool selected)
{
    qDebug().noquote() << "onPointSelected: " << seriesName << " - " << point << " - " << selected;
    //主要处理选择取消的情况
    if(selected){
        return;
    }

    QString itemStr = createComboboxItemText(seriesName,point);

    qDebug().noquote() << "itemStr: " <<itemStr;
    QStringList exitedTexts = PublicDef::getAllItems(ui.UI_PNTS_COMBOX);
    if(exitedTexts.contains(itemStr)){
        ui.UI_PNTS_COMBOX->removeItem(exitedTexts.indexOf(itemStr));
    }
}
