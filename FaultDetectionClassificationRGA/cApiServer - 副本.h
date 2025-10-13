#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include "cDatabaseFDC.h"

class cApiServer : public QObject
{
    Q_OBJECT
public:
    explicit cApiServer(QObject *parent = nullptr);
    ~cApiServer();
    bool start(quint16 port = 8085);
    void processRequest(QSharedPointer<QTcpSocket> clientSocket, const QJsonObject &request);

signals:
    void responseReady(QSharedPointer<QTcpSocket> socket, QJsonObject response);
signals:
    void sendDataToSocket(QSharedPointer<QTcpSocket> socket, QByteArray data);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void sendResponse(QSharedPointer<QTcpSocket> socket, QJsonObject response);
    void onSendDataToSocket(QSharedPointer<QTcpSocket> socket, QByteArray data)
    {
        if (socket.isNull() || socket->state() != QAbstractSocket::ConnectedState) {
            return;
        }

        qint64 bytesWritten = 0;//socket->write(data);
while(1){
    bytesWritten = socket->write(data);
    if (bytesWritten == -1) { // 写入错误
        //queue.dequeue();
        //cleanupSocket(socket);
        return;
    } else if (bytesWritten < data.size()) { // 部分写入
        data = data.mid(bytesWritten); // 保留剩余数据
        return; // 等待下次bytesWritten信号
    } else { // 完整写入
        //queue.dequeue();
        return;
    }
}

//        auto& queue = sendQueues_[socket.data()];
//        queue.enqueue(data);

//        // 如果未连接readyWrite信号，则连接并尝试发送
//        if (!activeSockets_.contains(socket.data())) {
//            activeSockets_.insert(socket.data());
//            connect(socket.data(), &QTcpSocket::bytesWritten,
//                    this, [this, socket]() { sendNextChunk(socket); });
//            connect(socket.data(), &QTcpSocket::disconnected,
//                    this, [this, socket]() { cleanupSocket(socket); });
//            sendNextChunk(socket);
//        }
    }


private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    cDatabaseFDC* mDatabaseFDC= nullptr;
    bool loadLocalData(const QString& chamberName, /*const QStringList& listChannel, const QString& keyEquipmentGroup,*/
                       const QDate& startDate, const QDate& endDate, QJsonArray& result);
//    bool getData2JS(QJsonArray& result, QDate date){
//        QList<qreal> dateList;
//        QStringList timeList, customName, customData,//6 0.54,0.52
//                totalPressure, RecipeNameEQ, RecipeNameRGA, Step,//10
//                LotID, SlotID, WaferID, ChamberID, SlitValve;//15
//        //QDate date= QDate::fromString("20250830", "yyyyMMdd");//QDate date= QDate::currentDate();
//        QString mCurrentTable= "CHEdataFDC";
//        cDatabaseFDC* mDatabaseFDC= sDatabaseFDC::getWidgetFDC();
//        if(!mDatabaseFDC->getData(dateList,
//                                  timeList,
//                                  customName,
//                                  customData,
//                                  totalPressure,
//                                  RecipeNameEQ,
//                                  RecipeNameRGA,
//                                  Step,//10
//                                  LotID,//11
//                                  SlotID,//12
//                                  WaferID,//13
//                                  ChamberID,//14
//                                  SlitValve,//15
//                                  date,
//                                  mCurrentTable))
//            return false;

//        for(int i= 0; i< dateList.size(); ++i){
//            QJsonObject recordObject;
//            recordObject["date"] = dateList[i];
//            recordObject["time"] = timeList[i];
//            recordObject["customName"] = customName[i];
//            recordObject["customData"] = customData[i];
//            recordObject["totalPressure"] = totalPressure[i];
//            recordObject["RecipeNameEQ"] = RecipeNameEQ[i];
//            recordObject["RecipeNameRGA"] = RecipeNameRGA[i];
//            recordObject["Step"] = Step[i];
//            recordObject["LotID"] = LotID[i];
//            recordObject["SlotID"] = SlotID[i];
//            recordObject["WaferID"] = WaferID[i];
//            recordObject["ChamberID"] = ChamberID[i];
//            recordObject["SlitValve"] = SlitValve[i];
//            result.append(recordObject);
//        }
//        return true;
//    }
};

