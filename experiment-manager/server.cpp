#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{

    QFile file("config.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "file not open";
    } else {
        QJsonObject jprog, jobj;
        QJsonArray jprograms;
        jprog["path"] = PATH_MODBUS_SERVER;
        jprog["name"] = "ModbusServer";
        jprog["path_program"] = QDir(PATH_MODBUS_SERVER).filePath("ModbusServer");
        jprograms.append(jprog);
        jobj["programs"] = jprograms;
        file.write(QJsonDocument(jobj).toJson());
//        jconfig = QJsonDocument::fromJson(file.readAll()).object();
    }
    startModbus();
    file.close();
}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}


void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,55123))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening...";
    }
}

void Server::startModbus()
{
    QString program_path = QString(PATH_MODBUS_SERVER) + "/ModbusServer";
    modbus = new QProcess(this);
    modbus->setProgram(program_path);
    modbus->start();
}


int Server::countClients()
{
    return clientManagers.size();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *sockClient = new QTcpSocket(this);
    if (!sockClient->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << sockClient->error();
        return;
    }
    ClientManager *client = new ClientManager(sockClient, this);
    connect(client, &ClientManager::finished, this, &Server::onRemoveClientManager);
    connect(client, &ClientManager::sendServerCMD, this, &Server::readCMD);
    clientManagers.insert(client);

    qDebug() << socketDescriptor << " Connecting... clients.size =" << clientManagers.size();

}

void Server::onRemoveClientManager()
{
    auto client = qobject_cast<ClientManager*>(sender());
    auto it = clientManagers.find(client);

    if (it == clientManagers.end()){
        qDebug() << Q_FUNC_INFO << "Error clients not contains this set. set.size =" << clientManagers.size();
        return;
    }
    (*it)->deleteLater();
    clientManagers.erase(it);
    qDebug() << " #####################################  clientsManager.size =" << clientManagers.size();

}

qint64 Server::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void Server::readCMD(QJsonObject &jobj)
{
    modbus->pid();
    modbus->deleteLater();
}
