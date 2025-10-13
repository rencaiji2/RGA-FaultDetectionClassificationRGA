#include "cRequestTask.h"
#include "cApiServer.h"

cRequestTask::cRequestTask(cApiServer *server, QTcpSocket* socket, const QByteArray& data)
    : m_server(server), m_socket(socket), mData(data)
{
    setAutoDelete(true); // 任务完成后自动删除
}

void cRequestTask::run()
{
    m_server->processRequest(m_socket, mData);
}
