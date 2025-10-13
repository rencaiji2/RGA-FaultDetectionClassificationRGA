#include "cApiServer.h"
#include "cRequestTask.h"
#include "cDatabaseFDC.h"
#include <QThreadPool>
#include <QRunnable>
#include <QTimer>
#include <QMessageBox>

cApiServer::cApiServer(QObject *parent) : QObject(parent)
{
    mDatabaseFDC= sDatabaseFDC::getWidgetFDC();
    mDatabaseFDC->createConnection("databaseRGA");
    m_server = new QTcpServer(this);
    connect(this, &cApiServer::responseReady, this, &cApiServer::sendResponse);
    connect(this, &cApiServer::sendDataToSocket, this, &cApiServer::onSendDataToSocket, Qt::QueuedConnection);
}

cApiServer::~cApiServer()
{
    for (auto &client : m_clients) {// 清理所有客户端连接
        if (!client.isNull() && client->state() == QAbstractSocket::ConnectedState) {
            client->disconnectFromHost();
            if (client->state() == QAbstractSocket::ConnectedState) {
                client->waitForDisconnected(1000);
            }
        }
    }
    m_clients.clear();
}

bool cApiServer::start(quint16 port)
{
    if (!m_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Server could not start:" << m_server->errorString();
        return false;
    }

    connect(m_server, &QTcpServer::newConnection, this, &cApiServer::onNewConnection);
    qInfo() << "API server listening on port" << port;
    return true;
}

void cApiServer::onNewConnection()
{
    QTcpSocket *clientSocket = m_server->nextPendingConnection();
    if (!clientSocket) return;

        // 移除与QTcpServer的父子关系，避免双重删除
        clientSocket->setParent(nullptr);
    QSharedPointer<QTcpSocket> sharedSocket(clientSocket);
    m_clients.append(sharedSocket);

    connect(clientSocket, &QTcpSocket::readyRead, this, &cApiServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &cApiServer::onDisconnected);

    qInfo() << "New client connected:" << clientSocket->peerAddress().toString();
}

void cApiServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;
    QSharedPointer<QTcpSocket> sharedSocket;
    for (auto &socket : m_clients) {
        if (socket.data() == clientSocket) {
            sharedSocket = socket;
            break;
        }
    }

    if (sharedSocket.isNull()) {
        qWarning() << "Cannot find shared pointer for socket";
        return;
    }

    QByteArray data = clientSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isObject()) {
        cRequestTask *task = new cRequestTask(this, sharedSocket, doc.object());
        QThreadPool::globalInstance()->start(task);
    } else {
        QJsonObject errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Invalid JSON format";
        emit responseReady(sharedSocket, errorResponse);
    }
}

void cApiServer::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        for (int i = 0; i < m_clients.size(); ++i) {
            if (m_clients[i].data() == clientSocket) {
                m_clients.removeAt(i);
                break;
            }
        }

        qInfo() << "Client disconnected:" << clientSocket->peerAddress().toString();
        clientSocket->deleteLater();
    }
}

void cApiServer::processRequest(QSharedPointer<QTcpSocket> clientSocket, const QJsonObject &request)
{
    QString action = request.value("action").toString();
    if (action == "query") {
        QMap<QString, /*QStringList*/QString> chamber;
        QJsonObject EquipmentGroup= request.value("params").toObject();
        QString EquipmentGroupName= EquipmentGroup.value("EquipmentGroupName").toString();
        QDate startDate= QDate::fromString(EquipmentGroup.value("startDate").toString(), "yyyy-MM-dd");
        QDate endDate= QDate::fromString(EquipmentGroup.value("endDate").toString(), "yyyy-MM-dd");
        for (auto it = EquipmentGroup.begin(); it != EquipmentGroup.end(); ++it) {
            if (!it.value().isObject())
                continue;
            QJsonObject Chamber=it.value().toObject();
            chamber[it.key()]= Chamber.value("Channel").toString()/*.split(";")*/;
        }
        QJsonObject response;
        response["EquipmentGroupName"]= EquipmentGroupName;
        for(auto& keyChamber: chamber.keys()){
            QJsonObject Chamber;
            QJsonArray result;
            if(loadLocalData(keyChamber, startDate, endDate, result)){
                Chamber["status"] = "success";
                Chamber["channel"] = chamber[keyChamber];
                Chamber["data"] = result+ '\n';
            } else {
                Chamber["status"] = "error";
                Chamber["message"] = "Query execution failed";
            }
            response[keyChamber]= Chamber;
        }
        sendResponse(clientSocket, response);
    }
}

void cApiServer::sendResponse(QSharedPointer<QTcpSocket> socket, QJsonObject response)
{
    if (socket.isNull())
        return;

    QJsonDocument doc(response);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    qint32 packetSize= data.size();
    QByteArray strPacketSize;
    strPacketSize.resize(sizeof(qint32));
    memcpy(strPacketSize.data(), &packetSize, sizeof(qint32));
    data= strPacketSize+ data;

    emit sendDataToSocket(socket, data);
}

bool cApiServer::loadLocalData(const QString& chamberName, /*const QStringList& listChannel,
                                                     const QString& keyEquipmentGroup,*/
                                                     const QDate& startDate, const QDate& endDate,
                               QJsonArray& result)
{
    cDatabaseFDC* mDatabaseFDC= sDatabaseFDC::getWidgetFDC();
    if(!mDatabaseFDC)
        return false;
    QList<uint> dateList;
    QStringList timeList, customName, customData,//6 0.54,0.52
            totalPressure, RecipeNameEQ, RecipeNameRGA, Step,//10
            LotID, SlotID, WaferID, ChamberID, SlitValve;//15
    bool ok = mDatabaseFDC->getDatabyTimeArea(dateList,
                                              timeList,
                                              customName,
                                              customData,
                                              totalPressure,
                                              RecipeNameEQ,
                                              RecipeNameRGA,
                                              Step,//10
                                              LotID,//11
                                              SlotID,//12
                                              WaferID,//13
                                              ChamberID,//14
                                              SlitValve,//15
                                              startDate,
                                              endDate,
                                              chamberName);
    if(!ok){
        return false;//continue;
    }
    qDebug().noquote() << tr("总数据长度:%1").arg(dateList.count());
    for(int i= 0; i< dateList.size(); ++i){
        QJsonObject recordObject;
        recordObject["date"] = QString::number(dateList[i]);
        recordObject["time"] = timeList[i];
        recordObject["customName"] = customName[i];
        recordObject["customData"] = customData[i];
        recordObject["totalPressure"] = totalPressure[i];
        recordObject["RecipeNameEQ"] = RecipeNameEQ[i];
        recordObject["RecipeNameRGA"] = RecipeNameRGA[i];
        recordObject["Step"] = Step[i];
        recordObject["LotID"] = LotID[i];
        recordObject["SlotID"] = SlotID[i];
        recordObject["WaferID"] = WaferID[i];
        recordObject["ChamberID"] = ChamberID[i];
        recordObject["SlitValve"] = SlitValve[i];
        result.append(recordObject);
    }
    return true;
//    parsedData(listChannel, customName, customData,
//               keyEquipmentGroup, chamberName,
//               RecipeNameEQ, RecipeNameRGA, Step,
//               LotID, SlotID, WaferID,
//               dateList, timeList);
}

