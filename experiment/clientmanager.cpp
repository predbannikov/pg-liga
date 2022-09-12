#include "clientmanager.h"

ClientManager::ClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent), experiment(exp)
{}

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
