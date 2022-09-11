#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{

//    connect(modbusClient, &ModbusClient::readyReadPDU, this, &Experiment::readyReadPDU);
//    connect(modbusClient, &ModbusClient::connectedClient, this, &Experiment::resume);

    experiment = new Experiment(address);
    modbusClient = new ModbusClient(experiment, this);
    threadExperiment = new QThread;
    experiment->moveToThread(threadExperiment);
    QObject::connect(threadExperiment, &QThread::started, experiment, &Experiment::doWork, Qt::QueuedConnection);
    QObject::connect(experiment, &Experiment::sendRequest, modbusClient, &ModbusClient::sendRequest);
    threadExperiment->start();


}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}

void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,50000 | address))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening...";
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
    SocketClientManager *sockClientManager = new SocketClientManager(experiment, socket, this);
    connect(sockClientManager, &SocketClientManager::disconnectClient, this, &Server::removeClient);
//    connect()sendRequestToClient
    clients.insert(reinterpret_cast<quint64>(socket), sockClientManager);

    qDebug() << socketDescriptor << " Connecting... clients.size =" << clients.size();

}

void Server::removeClient(quint64 isock)
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

