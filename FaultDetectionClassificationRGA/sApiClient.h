#pragma once

#include "ui_sApiClient.h"
#include <QWidget>
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>
#include <QMutex>
#include <sThread.h>


class sApiClient : public QWidget
{
    Q_OBJECT

public:
    explicit sApiClient(QWidget *parent = nullptr);
    ~sApiClient();
    void connectToServer(const QString &host, quint16 port= 8085);
    void disconnectFromServer();
    bool isConnected(){
        return m_connected;
    }
    void sendData(QByteArray& data){
        QMutexLocker locker(&sendBuffMutex);
        sendBuff.append(data);
    }
//    void executeQuery(const QJsonObject &params);
//    void executeRecipe(const QJsonObject &params);
    //void executeCommand(const QString &sql, const QVariantList &params = QVariantList());
//    void getDataOnce(QJsonObject& response){//待数据传输完毕再提取，清理缓存
//        if(receiveBuff.size()< 1)
//            return;
//        if(!processResponse(receiveBuff[0], response))
//            return;
//        receiveBuff.removeFirst();
//    }
    void getDataOnce(QByteArray& data){//待数据传输完毕再提取，清理缓存
        if(receiveBuff.size()< 1)
            return;
        data= receiveBuff[0];
//        if(!processResponse(receiveBuff[0], response))
//            return;
        receiveBuff.removeFirst();
    }
    //void batchExecute(const QStringList &statements);
    //void ping();
    //void getTableSchema(const QString &tableName);
    //void listTables();
//    bool getJS2Data(const QJsonObject &response,
//                    QList<qreal>& dateList,
//                    QStringList& timeList,
//                    QStringList& customName,
//                    QStringList& customData,
//                    QStringList& totalPressure,
//                    QStringList& RecipeNameEQ,
//                    QStringList& RecipeNameRGA,
//                    QStringList& Step,
//                    QStringList& LotID,
//                    QStringList& SlotID,
//                    QStringList& WaferID,
//                    QStringList& ChamberID,
//                    QStringList& SlitValve){

//        QJsonArray hobbiesArray = response["data"].toArray();
//        for (const QJsonValue &value : hobbiesArray) {
//            if(!value.isObject())
//                continue;
//            QJsonObject recordObject= value.toObject();
//            dateList<< recordObject["date"].toDouble();
//            timeList<< recordObject["time"].toString();
//            customName<< recordObject["customName"].toString();
//            customData<< recordObject["customData"].toString();
//            totalPressure<< recordObject["totalPressure"].toString();
//            RecipeNameEQ<< recordObject["RecipeNameEQ"].toString();
//            RecipeNameRGA<< recordObject["RecipeNameRGA"].toString();
//            Step<< recordObject["Step"].toString();
//            LotID<< recordObject["LotID"].toString();
//            SlotID<< recordObject["SlotID"].toString();
//            WaferID<< recordObject["WaferID"].toString();
//            ChamberID<< recordObject["ChamberID"].toString();
//            SlitValve<< recordObject["SlitValve"].toString();
//        }
//        return true;
//    }
//    bool processResponse(const QByteArray &data, QJsonObject& response){
//        QJsonParseError parseError;
//        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
//        if (parseError.error != QJsonParseError::NoError)
//            return false;
//        if (!doc.isObject())
//            return false;
//        response= doc.object();
//        return true;
//    }
private:
    Ui::sApiClient ui;
    QByteArray m_buffer;
    int packetSize= 0;

protected:
    QString mHostAddress;
    quint16 mPort;
    volatile bool m_connected = false;

    SThread* mTcpThread= nullptr;
    static int tcpThread(void *pParam, const bool &bRunning);
    QMutex sendBuffMutex;
    QList<QByteArray> sendBuff;
    QList<QByteArray> receiveBuff;

signals:
    void dataReceived(QByteArray data);
    void isReceived();

};

