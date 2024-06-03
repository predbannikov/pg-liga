#include "clientmanager.h"

ClientManager::ClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent), experiment(exp)
{
    connect(exp, &Experiment::sendRequestToClient, this, &ClientManager::procQueue);
}

ClientManager::~ClientManager()
{
    qDebug() << Q_FUNC_INFO;
}

void ClientManager::readyRead(QJsonObject &jobj)
{
    qDebug() << Q_FUNC_INFO << jobj;
    experiment->put(jobj);
}

void ClientManager::procQueue(const QJsonObject &jResponse)
{
    queueResponse.push_back(jResponse);
    while(!queueResponse.isEmpty()) {
        QJsonObject jobj(queueResponse.takeFirst());
        jobj["responseComplated"] = "responseComplated";
        write(jobj);
    }
}
