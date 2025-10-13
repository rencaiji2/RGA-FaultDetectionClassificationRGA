#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>
#include <QSharedPointer>
#include <QThreadPool>
#include "cDatabaseFDC.h"

//class cApiServer;
//// 请求处理任务类
//class cRequestTask : public QRunnable
//{
//public:
//    cRequestTask(cApiServer* server, QSharedPointer<QTcpSocket> socket, const QJsonObject& request)
//        : m_server(server), m_socket(socket), m_request(request)
//    {
//        setAutoDelete(true);
//    }

//    void run() override {
//        if (m_server && !m_socket.isNull()) {
//            m_server->processRequest(m_socket, m_request);
//        }
//    }

//private:
//    cApiServer* m_server;
//    QSharedPointer<QTcpSocket> m_socket;
//    QJsonObject m_request;
//};

// 主服务器类
class cApiServer : public QObject
{
    Q_OBJECT
public:
    explicit cApiServer(QObject *parent = nullptr);
    ~cApiServer();

    bool start(quint16 port = 8085);
    void stop();
    void processRequest(QTcpSocket* clientSocket, const QByteArray dataR/*const QJsonObject &request*/);

signals:
    void responseReady(QTcpSocket* socket, QByteArray dataR/*QJsonObject response*/);
    void sendDataToSocket(QTcpSocket* socket, QByteArray data);
    void serverStarted(bool success, quint16 port);
    void serverStopped();
    void clientConnected(QString address);
    void clientDisconnected(QString address);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void sendResponse(QTcpSocket* socket, QByteArray data);
    void onSendDataToSocket(QTcpSocket* socket, QByteArray data);

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QMutex m_clientsMutex;
    cDatabaseFDC* mDatabaseFDC;
    quint16 m_port;
    bool m_running;

    bool loadLocalData(const QString& chamberName, const QString& recipeEQ, const QDate& startDate, const QDate& endDate,
                       const QString& lotID,
                                              const QString& waferID,
                                              const QString& chamberID, QJsonArray& result);
    bool isSocketValid(QTcpSocket* socket);
    void cleanupSocket(QTcpSocket* socket);
};
