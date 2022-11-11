#include "socketclient.h"

ClientSocket::ClientSocket(QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::readyRead(QJsonObject &jobj)
{
    if (jobj["CMD"].toString() == "check")  {
        qDebug() << "client version" << jobj;
    } else {
        qDebug() << "readyRead " << jobj;
    }
}

void ClientSocket::sendReadyRequest(QJsonObject &jobj)
{
    write(jobj);
}
