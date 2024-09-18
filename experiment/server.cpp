#include "server.h"

Server::Server(QString serial_port_name, quint8 addr_, QObject *parent) : QTcpServer(parent), address(addr_)
{

//    connect(modbusClient, &ModbusClient::readyReadPDU, this, &Experiment::readyReadPDU);
//    connect(modbusClient, &ModbusClient::connectedClient, this, &Experiment::resume);

//    modbus = new SerialPort(this);
    experiment = new Experiment(serial_port_name, address);
    threadExperiment = new QThread;
    experiment->moveToThread(threadExperiment);
    QObject::connect(threadExperiment, &QThread::started, experiment, &Experiment::doWork, Qt::QueuedConnection);
    threadExperiment->start();

//    experiment->doWork();

}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}

void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,50000))
    {
        qDebug() << Q_FUNC_INFO << "Could not start server" << this->serverAddress() << this->serverPort();;
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Listening..." << this->serverAddress() << this->serverPort();
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << socket->error();
        return;
    }
    ClientManager *client = new ClientManager(experiment, socket, this);
    connect(client, &ClientManager::finished, this, &Server::onRemoveClientManager, Qt::QueuedConnection);
    clients.insert(client);

    qDebug() << socketDescriptor << " Connecting... clients.size =" << clients.size();

}

void Server::onRemoveClientManager()
{
    auto client = qobject_cast<ClientManager*>(sender());
    auto it = clients.find(client);

    if (it == clients.end()){
        qDebug() << Q_FUNC_INFO << "Error clients not contains this set. set.size =" << clients.size();
        return;
    }
    (*it)->deleteLater();
    clients.erase(it);
    qDebug() << " #####################################  clientsManager.size =" << clients.size();
}

//void Server::removeClient(quint64 isock)
//{
//    if (!clients.contains(isock)) {
//        qDebug() << "Error clients not contains this socket" << isock;
//        return;
//    }
//    clients.value(isock)->deleteLater();
//    clients.remove(isock);
//    if (clients.size() == 0)
//        qDebug() << " #####################################  clients.size =" << clients.size();

//}

