#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
    QDir cur_path = QDir::currentPath();
    cur_path.mkpath("testing");
    cur_path.mkpath("release");
}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}

void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,51234))
    {
        qDebug() << Q_FUNC_INFO << "Could not start server" << this->serverAddress() << this->serverPort();
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Listening..." << this->serverAddress() << this->serverPort();
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket;
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << socket->error();
        return;
    }
    ClientSocket *clientSock = new ClientSocket(socket, this);
    connect(clientSock, &AbstractJSONClient::disconnectClient, this, &Server::removeClient);
    connect(clientSock, &AbstractJSONClient::finished, this, &Server::removeClient);

    clients.insert(reinterpret_cast<qint64>(socket), clientSock);

    qDebug() << socketDescriptor << " Connecting... clients.size =" << clients.size();
}

void Server::parserReadyRequest(QJsonObject jobj)
{
    quint64 tcpsock = jobj["id_experiment"].toString().toULongLong();

    if (clients.contains(tcpsock)) {
        clients.value(tcpsock)->sendReadyRequest(jobj);
    }
}

void Server::removeClient(qint64 isock)
{
    if (!clients.contains(isock)) {
        qDebug() << "Error clients not contains this socket" << isock;
        return;
    }
    clients.value(isock)->deleteLater();
    clients.remove(isock);
    if (clients.size() == 0)
        qDebug() << " #####################################  clients.size =" << clients.size();
}

