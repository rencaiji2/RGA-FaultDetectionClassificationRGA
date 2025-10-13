#pragma once

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlError>
#include <QPixmap>
#include <QSqlDatabase>
#include <QMutex>
#include <QReadWriteLock>

class cDatabaseFDC{
private:
    QSqlDatabase mDatabase;
    QString mIp;
    QString mDeviceName;
    QString mPath;
    QReadWriteLock lock;

public:
//    static cDatabaseFDC& instance(){
//        static cDatabaseFDC instance;
//        return instance;
//    }
    cDatabaseFDC(){}
    ~cDatabaseFDC(){}
    void update(QString ip,//56456
                QString deviceName,
                QString path){
        mIp= ip;
        mDeviceName= deviceName;
        mPath= path;
    }
    bool createConnection(QString nameDatabase){
        QWriteLocker writer(&lock);//QMutexLocker locker(&mutex);
        mDatabase =QSqlDatabase::addDatabase("QSQLITE", "Data");//以“QSQLITE”为数据库类型，在本进程地址空间内创建一个SQLite数据库。
        //mDatabase =QSqlDatabase::database("Trail");      //以“QSQLITE”为数据库类型，在本进程地址空间内创建一个SQLite数据库。

        QSqlQuery query(mDatabase);
        if (!query.exec("PRAGMA journal_mode=WAL;")) {// 启用WAL模式以提高并发性能
            qWarning() << "Failed to enable WAL mode:" << query.lastError().text();
        }

        mDatabase.setHostName("easybook-3313b1");                          //设置数据库主机名
        mDatabase.setDatabaseName(nameDatabase);//以上创建的数据库以“qtDB.db”为数据库名。它是SQLite在建立内存数据库时唯一可用的名字。
        //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
        mDatabase.setUserName("zhouxu");                                     //设置数据库用户名
        mDatabase.setPassword("123456");                                   //设置数据库密码
        if (!mDatabase.open()) {//打开连接
            //            QMessageBox::warning(0, QObject::tr("Database Error"),
            //                                 mDatabase.lastError().text());
            return false;
        }
        return true;
    }
    bool createTable(const QString& tableName){
        QWriteLocker writer(&lock);//QMutexLocker locker(&mutex);
        QSqlQuery query(mDatabase);
        QString tempStr= QString("CREATE TABLE IF NOT EXISTS %1 ("
                "userName TEXT, date REAL, time TEXT,"
                "customName TEXT, customData TEXT, totalPressure TEXT,"
                "RecipeNameEQ TEXT, RecipeNameRGA TEXT, Step TEXT,"
                "LotID TEXT, SlotID TEXT, WaferID TEXT ,ChamberID TEXT ,"
                "SlitValve TEXT)").arg(tableName);
        if(!query.exec(tempStr)){
            qDebug()<<query.lastError().driverText();
            query.finish();
            return false;
        }
        query.finish();
        return true;
    }
    bool addData(/*const QString& ip,//5
                 const QString& deviceName,//6
                 const QString path,//7*/
                 const QString& customName,//5 N2/O2,N2/H2O
                 const QString& customData,//6 0.54,0.52
                 const QString& totalPressure,//7
                 const QString& RecipeNameEQ,//8
                 const QString& RecipeNameRGA,//9
                 const QString& Step,//10
                 const QString& LotID,//11
                 const QString& SlotID,//12
                 const QString& WaferID,//13
                 const QString& ChamberID,//14
                 const QString& SlitValve,//15
                 QString tableName/*= "dataFDC"*/,//1
                 QString userName= "admin"){//2
        QWriteLocker writer(&lock);//QMutexLocker locker(&mutex);
        QSqlQuery query(mDatabase);
        QDate currentDate= QDate::currentDate();
        QString tempStr= QString("insert into %1 VALUES ('%2', %3, '%4', '%5', '%6', '%7', '%8', "
                                 "'%9', '%10', '%11', '%12', '%13', '%14', '%15')")
                .arg(tableName)
                .arg(userName)
                .arg(/*static_cast<qreal>*/(currentDate.toJulianDay()))
                .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
//                .arg(ip)//5
//                .arg(deviceName)//6
//                .arg(path)//7
                .arg(customName)//5
                .arg(customData)//6
                .arg(totalPressure)//7
                .arg(RecipeNameEQ)//8
                .arg(RecipeNameRGA)//9
                .arg(Step)//10
                .arg(LotID)//11
                .arg(SlotID)//12
                .arg(WaferID)//13
                .arg(ChamberID)//14
                .arg(SlitValve)//15
                ;
        if(!query.exec(tempStr)){
            qDebug()<<query.lastError().driverText();
            query.finish();
            return false;
        }
        query.finish();
        return true;
    }
//    bool addDataError(//const QString& userName,
//                 const QString& comment,
//                 const QString& ip,
//                 const QString& deviceName){
//        return addData(comment, ip, deviceName, QStringLiteral("ERROR"));
//    }
//    bool addDataWarning(//const QString& userName,
//                 const QString& comment,
//                 const QString& ip,
//                 const QString& deviceName){
//        return addData(comment, ip, deviceName, QStringLiteral("WARNING"));
//    }
//    bool addDataInformation(const QString& comment,
//                     const QString& ip,
//                     const QString& deviceName){
//            return addData(comment, ip, deviceName, QStringLiteral("INFO"));
//        }
    bool getData(QList<qreal>/*QStringList*/& dateList,
                 QStringList& timeList,
                 QStringList& customName,//5 N2/O2,N2/H2O
                 QStringList& customData,//6 0.54,0.52
                 QStringList& totalPressure,//7
                 QStringList& RecipeNameEQ,//8
                 QStringList& RecipeNameRGA,//9
                 QStringList& Step,//10
                 QStringList& LotID,//11
                 QStringList& SlotID,//12
                 QStringList& WaferID,//13
                 QStringList& ChamberID,//14
                 QStringList& SlitValve,//15
                 QDate& date,
                 QString tableName,
                 QString userName= "admin"){
        QReadLocker reader(&lock);//QMutexLocker locker(&mutex);
        QSqlQuery query(mDatabase);
        if(tableName.isEmpty())
            return false;
        tableName= tableName+ QString("%1%2")
                .arg(date.year(), 4, 10, QChar('0'))
                .arg(date.month(), 2, 10, QChar('0'));

        //QDate tempDate= QDate::fromString(date, "yyyy/ MM/ dd");
        //        if(!query.exec(QString("select * from %1 where userName='%2' and date=%3").
        //                       arg(tableName).arg(userName).arg(tempDate.toJulianDay()))){
        if(!query.exec(QString("select * from %1 where userName='%2' ").
                       arg(tableName).arg(userName))){
            qDebug()<<query.lastError().driverText();
            query.finish();
            return false;
        }
        dateList.clear();
        timeList.clear();
        customName.clear();
        customData.clear();
        totalPressure.clear();
        RecipeNameEQ.clear();
        RecipeNameRGA.clear();
        Step.clear();
        LotID.clear();
        SlotID.clear();
        WaferID.clear();
        ChamberID.clear();
        SlitValve.clear();
        while(query.next()){
            dateList<< /*QDate::fromJulianDay(*/query.record().value("date").toReal()/*).toString("dd/MM/yyyy")*/;
            timeList<< query.record().value("time").toString();
            customName<< query.record().value("customName").toString();
            customData<< query.record().value("customData").toString();
            totalPressure<< query.record().value("totalPressure").toString();
            RecipeNameEQ<< query.record().value("RecipeNameEQ").toString();
            RecipeNameRGA<< query.record().value("RecipeNameRGA").toString();
            Step<< query.record().value("Step").toString();
            LotID<< query.record().value("LotID").toString();
            SlotID<< query.record().value("SlotID").toString();
            WaferID<< query.record().value("WaferID").toString();
            ChamberID<< query.record().value("ChamberID").toString();
            SlitValve<< query.record().value("SlitValve").toString();
        }
        query.finish();
        return true;
    }
    bool getFromName(//const QString& usrName,
                     QStringList& dateTime,
                     QStringList& comment,
                     QStringList& ip,
                     QStringList& deviceName,
                     QStringList& levelMSG,
                     QString userName= "admin",
                     QString tableName= "dataFDC"){
        QReadLocker reader(&lock);//QMutexLocker locker(&mutex);
        QSqlQuery query(mDatabase);
        if(!query.exec(QString("select * from %1 where userName='%2'").arg(tableName).arg(userName))){
            qDebug()<<query.lastError().driverText();
            query.finish();
            return false;
        }
        dateTime.clear();
        comment.clear();
        while(query.next()){
            dateTime<< QDate::fromJulianDay(query.record().value("date").toReal()).toString("dd/MM/yyyy,")
                       + query.record().value("time").toString();
            comment<< query.record().value("comment").toString();
            ip<< query.record().value("deviceIP").toString();
            deviceName<< query.record().value("deviceName").toString();
            levelMSG<< query.record().value("levelMSG").toString();
        }
        query.finish();
        return true;
    }
    bool getFromDate(const QString& date,
                     QStringList& userName,
                     QStringList& time,
                     QStringList& comment,
                     QStringList& ip,
                     QStringList& deviceName,
                     QStringList& levelMSG,
                     //QString userName= "admin",
                     QString tableName= "dataFDC"){
        QReadLocker reader(&lock);//QMutexLocker locker(&mutex);
        QSqlQuery query(mDatabase);
        QDate tempDate= QDate::fromString(date, "yyyy/ MM/ dd");
        if(!query.exec(QString("select * from %1 where date=%2").arg(tableName).arg(tempDate.toJulianDay()))){
            qDebug()<<query.lastError().driverText();
            query.finish();
            return false;
        }
        userName.clear();
        comment.clear();
        while(query.next()){
            userName<< query.record().value("userName").toString();
            time<< query.record().value("time").toString();
            comment<< query.record().value("comment").toString();
            ip<< query.record().value("deviceIP").toString();
            deviceName<< query.record().value("deviceName").toString();
            levelMSG<< query.record().value("levelMSG").toString();
        }
        query.finish();
        return true;
    }

    bool getDatabyTimeArea(QList<uint>& dateList,
                           QStringList& timeList,
                           QStringList& customName,
                           QStringList& customData,
                           QStringList& totalPressure,
                           QStringList& RecipeNameEQ,
                           QStringList& RecipeNameRGA,
                           QStringList& Step,
                           QStringList& LotID,
                           QStringList& SlotID,
                           QStringList& WaferID,
                           QStringList& ChamberID,
                           QStringList& SlitValve,
                           const QString& recipeEQ,
                           const QDate& startDate,
                           const QDate& endDate,
                           const QString& lotID,
                           const QString& waferID,
                           const QString& chamberID,
                           QString baseTableName,
                           QString userName = "admin")
    {
        QReadLocker reader(&lock);
        if (!mDatabase.isOpen()) {
            qDebug() << "数据库未打开";
            return false;
        }

        if (startDate > endDate) {
            qDebug() << "开始日期不能晚于结束日期";
            return false;
        }

        // 清空输出参数
        dateList.clear();
        timeList.clear();
        customName.clear();
        customData.clear();
        totalPressure.clear();
        RecipeNameEQ.clear();
        RecipeNameRGA.clear();
        Step.clear();
        LotID.clear();
        SlotID.clear();
        WaferID.clear();
        ChamberID.clear();
        SlitValve.clear();

        // 1. 生成时间区间内涉及的所有月份表名
        QSet<QString> tableNamesSet; // 使用Set避免重复
        QDate currentDate = startDate;

        while (currentDate <= endDate) {
            QString fullTableName = baseTableName + QString("dataFDC%1%2")
                .arg(currentDate.year(), 4, 10, QChar('0'))
                .arg(currentDate.month(), 2, 10, QChar('0'));
            tableNamesSet.insert(fullTableName);
            currentDate = currentDate.addMonths(1);
        }

        // 2. 获取数据库中实际存在的表
        QStringList dbTables = mDatabase.tables();
        QList<QString> existingTables;

        for (const QString& tableName : tableNamesSet) {
            if (dbTables.contains(tableName)) {
                existingTables.append(tableName);
            } else {
                qDebug() << "警告: 表不存在:" << tableName;
            }
        }

        if (existingTables.isEmpty()) {
            qDebug() << "指定时间区间内没有找到任何表";
            return false;
        }

        // 3. 按表查询数据并合并结果
        bool hasSuccess = false;

        for (const QString& tableName : existingTables) {
            // 构建查询语句 - 添加日期范围条件
            QString sql = QString("SELECT date, time, customName, customData, "
                                 "totalPressure, RecipeNameEQ, RecipeNameRGA, Step, "
                                 "LotID, SlotID, WaferID, ChamberID, SlitValve "
                                 "FROM %1 WHERE date BETWEEN '%2' AND %3%4%5%6%7")
                    .arg(tableName)
                    .arg(startDate.toJulianDay()/*startDate.toString("yyyy-MM-dd")*/)
                    .arg(endDate.toJulianDay()/*endDate.toString("yyyy-MM-dd")*/)
                    .arg((lotID.isEmpty())||(lotID=="ALL")?"":" AND LotID='"+lotID+"'")
                    .arg((waferID.isEmpty())||(waferID=="ALL")?"":" AND WaferID='"+waferID+"'")
                    .arg((chamberID.isEmpty())||(chamberID=="ALL")?"":" AND ChamberID='"+chamberID+"'")
                    .arg((recipeEQ.isEmpty())||(recipeEQ=="ALL")?"":" AND RecipeNameEQ='"+recipeEQ+"'");

            qDebug().noquote() << "getDatabyTimeArea-sqlStr:    " << sql;

            if (!userName.isEmpty() && userName != "admin") {
                sql += QString(" AND userName = '%1'").arg(userName);
            }

            sql += " ORDER BY date, time";

            QSqlQuery query(mDatabase);
            if (!query.exec(sql)) {
                qDebug() << "查询表失败:" << tableName << "错误:" << query.lastError().text();
                continue;
            }

            // 处理查询结果
            int recordCount = 0;
            while (query.next()) {
                dateList.append(query.value(0).toUInt()/*.toReal()*/);
                timeList.append(query.value(1).toString());
                customName.append(query.value(2).toString());
                customData.append(query.value(3).toString());
                totalPressure.append(query.value(4).toString());
                RecipeNameEQ.append(query.value(5).toString());
                RecipeNameRGA.append(query.value(6).toString());
                Step.append(query.value(7).toString());
                LotID.append(query.value(8).toString());
                SlotID.append(query.value(9).toString());
                WaferID.append(query.value(10).toString());
                ChamberID.append(query.value(11).toString());
                SlitValve.append(query.value(12).toString());
                recordCount++;
            }

            qDebug() << "表" << tableName << "查询到" << recordCount << "条记录";
            if (recordCount > 0) {
                hasSuccess = true;
            }
        }

        if (!hasSuccess) {
            qDebug() << "所有表查询都失败了";
            return false;
        }

        qDebug() << "总共查询到" << dateList.size() << "条记录";
        return true;
    }

    //获取全部的表名
    QStringList getTables()
    {
        QStringList tables;
        if (mDatabase.isOpen()) {
            tables = mDatabase.tables(QSql::Tables);
        }
        return tables;
    }
};

class sDatabaseFDC{
public:
    static cDatabaseFDC* getWidgetFDC(){
        static sDatabaseFDC insDatabaseFDC;
        return &(insDatabaseFDC.mDatabaseFDC);
    }
private:
    cDatabaseFDC mDatabaseFDC;
    sDatabaseFDC(){}
    virtual ~sDatabaseFDC(){}
    sDatabaseFDC(const sDatabaseFDC&){}
    sDatabaseFDC& operator=(const sDatabaseFDC&){
        static sDatabaseFDC insDatabaseFDC;
        return insDatabaseFDC;
    }
};

