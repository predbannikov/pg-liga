#include "clientmanager.h"

ClientManager::ClientManager(QString host_, QObject *parent) : AbstractJSONClient(host_, parent)
{

}

ClientManager::~ClientManager()
{
    qDebug() << Q_FUNC_INFO ;
}

void ClientManager::readyRead(QJsonObject &jobj) {
    emit readyReadResponse(jobj);
}

void ClientManager::sendReadyRequest(QJsonObject jobj)
{
    write(jobj);
}
