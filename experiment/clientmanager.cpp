#include "clientmanager.h"

ClientManager::ClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent), experiment(exp)
{
//    connect(socket, &QTcpSocket::readyRead, this, &ClientManager::readyRead, Qt::DirectConnection);
//    connect(socket, &QTcpSocket::disconnected, this, &ClientManager::disconnected, Qt::DirectConnection);
//    connect(experiment, &Experiment::sendRequestToClient, this, &ClientManager::procQueue);
}

ClientManager::~ClientManager()
{
    qDebug() << Q_FUNC_INFO;
}

void ClientManager::readyRead(QJsonObject &jobj)
{
    experiment->put(jobj);
}

void ClientManager::procQueue(const QJsonObject &jResponse)
{
    queueResponse.push_back(jResponse);
    while(!queueResponse.isEmpty()) {
        QJsonObject jobj(queueResponse.takeFirst());
        write(jobj);
    }
}
