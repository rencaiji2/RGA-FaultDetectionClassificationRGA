#include "sApiClient.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QDateTime>

sApiClient::sApiClient(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
//    m_socket = new QTcpSocket(this);
//    connect(m_socket, &QTcpSocket::connected, this, &sApiClient::onConnected);
//    connect(m_socket, &QTcpSocket::disconnected, this, &sApiClient::onDisconnected);
//    connect(m_socket, &QTcpSocket::readyRead, this, &sApiClient::onReadyRead);
//    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
//            this, &sApiClient::onError);
    mTcpThread= new SThread(this);
}

sApiClient::~sApiClient()
{
    disconnectFromServer();
    if(mTcpThread)
        delete mTcpThread;
    mTcpThread= nullptr;
}

void sApiClient::connectToServer(const QString &host, quint16 port)
{
    if((mHostAddress== host)&&(mPort== port)){
        if (isConnected())
            return;
    }else{
        if (isConnected())
            disconnectFromServer();
    }
    mHostAddress= host;
    mPort= port;
    mTcpThread->setUserFunction(tcpThread);
    mTcpThread->setUserParam(this);
    mTcpThread->start();
}

void sApiClient::disconnectFromServer()
{
//    if (m_socket->state() == QAbstractSocket::ConnectedState) {
//        m_socket->disconnectFromHost();
//    }
    if(!mTcpThread)
        return;
    if(mTcpThread->isRunning()){
        mTcpThread->stop();
        m_connected = false;
        if(!mTcpThread->wait(1000)){
                mTcpThread->terminate();
                mTcpThread->wait();
        }
        qDebug()<<"~cTcpClient()->stop()";
    }
}

int sApiClient::tcpThread(void *pParam, const bool &bRunning)
{
    sApiClient* pTcpClient = (sApiClient*)pParam;
    if(!pTcpClient)
        return 0;
    QTime timer;
    timer.start();
    QTcpSocket socket;
    while(bRunning){
        if(socket.state()!= QAbstractSocket::ConnectedState){
            pTcpClient->m_connected = false;
            if(socket.state() == QAbstractSocket::UnconnectedState) {
                qDebug() << "尝试连接到服务器..." << pTcpClient->mHostAddress << ":" << pTcpClient->mPort;
                socket.connectToHost(pTcpClient->mHostAddress, pTcpClient->mPort);
            }
            if(socket.waitForConnected(100)) {
                qDebug() << "连接成功!";
                pTcpClient->m_connected = true;
            }else{
                socket.abort();
                if(timer.elapsed()> 300){
                    qDebug() << "连接到服务器超时" << pTcpClient->mHostAddress << ":" << pTcpClient->mPort;
                    return 0;
                }
            }

            QThread::msleep(1);
            continue;
        }
        pTcpClient->m_connected = true;
        QByteArray wArray;
        pTcpClient->sendBuffMutex.lock();
        if(pTcpClient->sendBuff.size()> 0){
            wArray= pTcpClient->sendBuff[0];
            pTcpClient->sendBuff.pop_front();
        }else{
            pTcpClient->sendBuffMutex.unlock();
            break;
        }
        pTcpClient->sendBuffMutex.unlock();
        if(!wArray.isEmpty()){
            qint64 bytesWritten = socket.write(wArray);
            if (bytesWritten == -1) {
                qDebug() << "Write failed:" << socket.errorString();
                QThread::msleep(1);
                continue;
            } else if (bytesWritten != wArray.size()) {
                qDebug() << "Partial write:" << bytesWritten << "/" << wArray.size();
                QThread::msleep(1);
                continue;
            }
        }
        QByteArray rArray;
        int packetSize= 0;
        //bool isReceived= false;
        while(1){
            if(!socket.waitForReadyRead(100)){
                QThread::msleep(1);
                continue;
            }
            rArray.append(socket.readAll());
            //QByteArray buffer= m_socket->readAll();
            if(rArray.size() < sizeof(qint32))
                continue;
            if(packetSize< 1){
                packetSize= *((qint32*)rArray.data());
                rArray= rArray.right(rArray.size()- sizeof(qint32));
            }
            if(packetSize== rArray.size()){
                pTcpClient->receiveBuff<<rArray;
                emit pTcpClient->isReceived();
                break;
            }
        }
        QThread::msleep(1);
    }
    socket.abort();
    pTcpClient->m_connected = false;
    return 0;
}

/*void sApiClient::executeQuery(const QJsonObject &params)
{
    QJsonObject request;
    request["action"] = "query";
    request["params"] = params;
    QJsonDocument doc(request);
    QByteArray dataArray = doc.toJson(QJsonDocument::Compact) + "\n";
    sendData(dataArray);
}

void sApiClient::executeRecipe(const QJsonObject &params)
{
    QJsonObject request;
    request["action"] = "recipe";
    request["params"] = params;
    QJsonDocument doc(request);
    QByteArray dataArray = doc.toJson(QJsonDocument::Compact) + "\n";
    sendData(dataArray);
}*/

//void sApiClient::executeCommand(const QString &sql, const QVariantList &params)
//{
//    QJsonObject request;
//    request["action"] = "execute";
//    QJsonObject data;
//    //data["sql"] = sql;
//    QJsonArray paramsArray;
//    for (const QVariant &param : params) {
//        paramsArray.append(QJsonValue::fromVariant(param));
//    }
//    data["params"] = paramsArray;
//    request["data"] = data;

//    QJsonDocument doc(request);
//    QByteArray dataArray = doc.toJson(QJsonDocument::Compact) + "\n";
//    sendData(dataArray);
//}

