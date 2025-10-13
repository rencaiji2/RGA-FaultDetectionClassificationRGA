#include "cApiServer.h"
#include "cExecuteStruct.h"
#include "cRequestTask.h"
#include "sRecipeManage.h"
#include <QThreadPool>
#include <QDateTime>
#include <QHostAddress>
#include <QDebug>

cApiServer::cApiServer(QObject *parent)
    : QObject(parent),
      m_server(nullptr),
      mDatabaseFDC(nullptr),
      m_port(0),
      m_running(false)
{
    mDatabaseFDC = sDatabaseFDC::getWidgetFDC();
    if (mDatabaseFDC) {
        mDatabaseFDC->createConnection("databaseRGA");
    }

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &cApiServer::onNewConnection);
    connect(this, &cApiServer::responseReady, this, &cApiServer::sendResponse);
    connect(this, &cApiServer::sendDataToSocket, this, &cApiServer::onSendDataToSocket, Qt::QueuedConnection);
}

cApiServer::~cApiServer()
{
    stop();
}

bool cApiServer::start(quint16 port)
{
    if (m_running) {
        qWarning() << "Server is already running";
        return true;
    }

    if (!m_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Server could not start:" << m_server->errorString();
        emit serverStarted(false, port);
        return false;
    }

    m_port = port;
    m_running = true;
    qInfo() << "API server listening on port" << port;
    emit serverStarted(true, port);
    return true;
}

void cApiServer::stop()
{
    if (!m_running) return;

    m_running = false;

    // 停止接受新连接
    m_server->close();

    // 断开所有客户端连接
    QMutexLocker locker(&m_clientsMutex);
    for (auto &client : m_clients) {
        if (client) {
            client->disconnectFromHost();
            if (client->state() != QAbstractSocket::UnconnectedState) {
                client->waitForDisconnected(1000);
            }
        }
    }
    m_clients.clear();

    qInfo() << "Server stopped";
    emit serverStopped();
}

void cApiServer::onNewConnection()
{
    QTcpSocket *clientSocket = m_server->nextPendingConnection();
    if (!clientSocket)
        return;
    clientSocket->setParent(nullptr);// 移除与QTcpServer的父子关系，避免双重删除
    {
        QMutexLocker locker(&m_clientsMutex);
        m_clients.append(clientSocket);
    }

    connect(clientSocket, &QTcpSocket::readyRead, this, &cApiServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &cApiServer::onDisconnected);

    QString clientAddress = clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort());
    qInfo() << "New client connected:" << clientAddress;
    emit clientConnected(clientAddress);
}

void cApiServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    cRequestTask *task = new cRequestTask(this, clientSocket/*sharedSocket*/, clientSocket->readAll());
    QThreadPool::globalInstance()->start(task);

//        QJsonObject errorResponse;
//        errorResponse["status"] = "error";
//        errorResponse["message"] = "Invalid JSON format";
//        errorResponse["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

//        if (isSocketValid(/*sharedSocket*/clientSocket)) {
//            emit responseReady(/*sharedSocket*/clientSocket, errorResponse);
//        }
}

void cApiServer::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;
    QString clientAddress = clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort());
    {
        QMutexLocker locker(&m_clientsMutex);
        for (int i = 0; i < m_clients.size(); ++i) {
            if (m_clients[i]/*.data() */== clientSocket) {
                m_clients.removeAt(i);
                break;
            }
        }
    }

    qInfo() << "Client disconnected:" << clientAddress;
    emit clientDisconnected(clientAddress);
    clientSocket->deleteLater();
}

void cApiServer::processRequest(QTcpSocket* clientSocket, const QByteArray dataR/*const QJsonObject &request*/)
{
    QByteArray dataW;
    QString action;
    QJsonObject request;
    QString EquipmentGroupName;

    cExecuteStruct::_TYPE_ACTION TYPE_ACTION= cExecuteStruct::getAction(dataR, request, action);
    switch (TYPE_ACTION) {
    case cExecuteStruct::_query_ACTION:{
        QString RecipeEQ;
        QDate startDate;
        QDate endDate;
        QString lotID;
        QString waferID;
        QString chamberID;
        QMap<QString, QString> chamber;
        if(!cExecuteStruct::parsedQueryServer(request, EquipmentGroupName, RecipeEQ,
                                             startDate, endDate, lotID, waferID, chamberID, chamber)){
            dataW= cExecuteStruct::createErrorServer(action);
            break;
        }
        QJsonObject response;
        for(auto& keyChamber : chamber.keys()) {
            QJsonObject Chamber;
            QJsonArray result;
            if(loadLocalData(keyChamber, RecipeEQ, startDate, endDate, lotID, waferID, chamberID, result)) {
                Chamber["status"] = "success";
                Chamber["channel"] = chamber[keyChamber];
                Chamber["data"] = result;
            } else {
                Chamber["status"] = "error";
                Chamber["message"] = "Query execution failed";
            }
            response[keyChamber] = Chamber;
        }
        dataW= cExecuteStruct::createQueryServer(response, EquipmentGroupName);
        break;
    }
    case cExecuteStruct::_recipeListEQ_ACTION:{
        QString ip;
        if(!cExecuteStruct::parsedRecipeListEQ_Server(request, EquipmentGroupName, ip)){
            dataW= cExecuteStruct::createErrorServer(action);
            break;
        }
        QStringList RecipeListEQ;
        if(sRecipeManage::getLocalRecipeListEQ(RecipeListEQ))
            dataW= cExecuteStruct::createRecipeListEQ_Server(EquipmentGroupName, ip, RecipeListEQ);
        else
            dataW= cExecuteStruct::createErrorServer(action);
        break;
    }
    case cExecuteStruct::_recipeListRGA_ACTION:{
        QString ip;
        if(!cExecuteStruct::parsedRecipeListRGA_Server(request, EquipmentGroupName, ip)){
            dataW= cExecuteStruct::createErrorServer(action);
            break;
        }
        QStringList RecipeListRGA;
        if(sRecipeManage::getLocalRecipeListRGA(RecipeListRGA))
            dataW= cExecuteStruct::createRecipeListRGA_Server(EquipmentGroupName, ip, RecipeListRGA);
        else
            dataW= cExecuteStruct::createErrorServer(action);
        break;
    }
    default:{
        dataW= cExecuteStruct::createErrorServer(action);
        break;
    }
    }
    emit responseReady(clientSocket, dataW);
}

void cApiServer::sendResponse(QTcpSocket* socket, QByteArray data)
{
    if (!isSocketValid(socket))
        return;
    qint32 packetSize = data.size();
    QByteArray sizeData;
    sizeData.resize(sizeof(qint32));
    memcpy(sizeData.data(), &packetSize, sizeof(qint32));// 添加消息长度前缀

    data.prepend(sizeData);
    emit sendDataToSocket(socket, data);
}

void cApiServer::onSendDataToSocket(QTcpSocket* socket, QByteArray data)
{
    if (!isSocketValid(socket)) {
        return;
    }

    qint64 bytesWritten = 0;
    qint64 totalBytes = data.size();

    while (bytesWritten < totalBytes) {
        if (!isSocketValid(socket)) {
            return; // Socket became invalid during writing
        }

        qint64 written = socket->write(data.constData() + bytesWritten, totalBytes - bytesWritten);
        if (written == -1) {
            // 写入错误，可能是连接已断开
            qWarning() << "Error writing to socket:" << socket->errorString();
            cleanupSocket(socket);
            return;
        } else if (written == 0) {
            // 没有数据被写入，可能是缓冲区已满
            if (!socket->waitForBytesWritten(5000)) {
                qWarning() << "Timeout waiting for bytes to be written";
                cleanupSocket(socket);
                return;
            }
        } else {
            bytesWritten += written;
        }
    }

    // 确保所有数据被刷新
    socket->flush();
}

bool cApiServer::loadLocalData(const QString& chamberName, const QString& recipeEQ, const QDate& startDate, const QDate& endDate,
                               const QString& lotID,
                                                      const QString& waferID,
                                                      const QString& chamberID, QJsonArray& result)
{
    if (!mDatabaseFDC) {
        qWarning() << "Database connection not available";
        return false;
    }

    QList<uint> dateList;
    QStringList timeList, customName, customData,
                totalPressure, RecipeNameEQ, RecipeNameRGA, Step,
                LotID, SlotID, WaferID, ChamberID, SlitValve;

    bool ok = mDatabaseFDC->getDatabyTimeArea(dateList,
                                            timeList,
                                            customName,
                                            customData,
                                            totalPressure,
                                            RecipeNameEQ,
                                            RecipeNameRGA,
                                            Step,
                                            LotID,
                                            SlotID,
                                            WaferID,
                                            ChamberID,
                                            SlitValve,
                                            recipeEQ,startDate,
                                            endDate,
                                              lotID,
                                              waferID,
                                              chamberID,
                                            chamberName);
    if (!ok) {
        qWarning() << "Failed to get data from database for chamber:" << chamberName;
        return false;
    }

    qDebug().noquote() << tr("Total data length for chamber %1: %2").arg(chamberName).arg(dateList.count());

    for(int i = 0; i < dateList.size(); ++i) {
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
}

bool cApiServer::isSocketValid(QTcpSocket* socket)
{
    if (!socket/*.isNull()*/) {
        return false;
    }

    QMutexLocker locker(&m_clientsMutex);
    // 检查socket是否仍在客户端列表中且处于连接状态
    for (auto &client : m_clients) {
        if (client == socket/*.data()*/) {
            return socket->state() == QAbstractSocket::ConnectedState;
        }
    }

    return false;
}

void cApiServer::cleanupSocket(QTcpSocket* socket)
{
    if (!socket/*.isNull()*/) return;

    QMutexLocker locker(&m_clientsMutex);
    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]/*.data()*/ == socket/*.data()ss*/) {
            m_clients.removeAt(i);
            break;
        }
    }

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}
