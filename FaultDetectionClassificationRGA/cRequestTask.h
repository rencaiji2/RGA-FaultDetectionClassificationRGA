#pragma once

#include <QRunnable>
#include <QTcpSocket>
#include <QJsonObject>
#include <QSharedPointer>

class cApiServer;

class cRequestTask : public QRunnable
{
    //Q_OBJECT
public:
    cRequestTask(cApiServer *server, QTcpSocket* socket, const QByteArray& data/*const QJsonObject &request*/);
    void run() override;

private:
    cApiServer *m_server;
    QTcpSocket* m_socket;
    //QJsonObject m_request;
    QByteArray mData;
};

