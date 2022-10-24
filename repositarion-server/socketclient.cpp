#include "socketclient.h"

ClientSocket::ClientSocket(QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::readyRead(QJsonObject &jobj)
{
    jobj["id_experiment"] = QString::number(socketID());
}

void ClientSocket::sendReadyRequest(QJsonObject &jobj)
{
    write(jobj);
}
