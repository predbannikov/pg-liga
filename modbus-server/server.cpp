#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
    serialPortThread = new QThread;
    serialPort = new SerialPort;
    serialPort->moveToThread(serialPortThread);
    connect(serialPortThread, &QThread::started, serialPort, &SerialPort::process, Qt::QueuedConnection);
    connect(serialPort, &SerialPort::readyRequest, this, &Server::parserReadyRequest, Qt::QueuedConnection);
    serialPortThread->start();
    qDebug() << "Server::Server()       threadID =" << QThread::currentThreadId();
}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}

void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,1234))
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
    ClientSocket *clientSock = new ClientSocket(socket, serialPort, this);
    connect(clientSock, &ClientSocket::disconnectClient, this, &Server::removeClient);

    clients.insert(reinterpret_cast<quint64>(socket), clientSock);

    qDebug() << socketDescriptor << " Connecting... clients.size =" << clients.size();

}

void Server::parserReadyRequest(QJsonObject jobj)
{
//    QMutexLocker lock(&mutex);
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
//    QMutexLocker lock(&mutex);
    clients.value(isock)->deleteLater();
    clients.remove(isock);
    if (clients.size() == 0)
        qDebug() << " #####################################  clients.size =" << clients.size();

}

