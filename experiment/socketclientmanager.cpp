#include "socketclientmanager.h"

SocketClientManager::SocketClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent) : QObject(parent), socket(sock), experiment(exp)
{
    connect(socket, &QTcpSocket::readyRead, this, &SocketClientManager::readyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &SocketClientManager::disconnected, Qt::DirectConnection);
    connect(experiment, &Experiment::sendRequestToClient, this, &SocketClientManager::procQueue);
}

SocketClientManager::~SocketClientManager()
{
    qDebug() << Q_FUNC_INFO;
}

void SocketClientManager::readyRead()
{
//    qDebug() << Q_FUNC_INFO;
    QJsonDocument jdoc = QJsonDocument::fromJson(socket->readAll());
    QJsonObject jrequest = jdoc.object();
    experiment->put(jrequest);

}

void SocketClientManager::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void SocketClientManager::connected()
{
    qDebug() << Q_FUNC_INFO;
//    QJsonObject jObj;
//    jObj["hi"];
//    socket->write(QJsonDocument(jObj).toJson());
}

void SocketClientManager::disconnected()
{
    quint64 isock = reinterpret_cast<quint64>(socket);
    socket->deleteLater();
    socket = nullptr;
    emit disconnectClient(isock);
}

void SocketClientManager::procQueue(const QJsonObject &jResponse)
{
    queueResponse.push_back(jResponse);
    while(!queueResponse.isEmpty()) {
        if (socket != nullptr && socket->isOpen()) {
            QJsonObject jobj(queueResponse.takeFirst());
            socket->write(QJsonDocument(jobj).toJson());
        } else {
            break;
        }
    }
}

