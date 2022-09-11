#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{

    QFile file("config.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "file not open";
    } else {
        QJsonArray jprograms;
        jconfig = QJsonDocument::fromJson(file.readAll()).object();
        if (!jconfig.contains("programs")) {
            qDebug() << "no settings path programs";
            return;
        }
        jprograms = jconfig["programs"].toArray();
        for (auto jobj: qAsConst(jprograms)) {
            if (jobj.toObject()["name"].toString() == "modbusserver") {
                jmodbusconfig = jobj.toObject();
            }
        }
    }
    startModbus();
    file.close();


    const int address = 2;
    QString host = QString("127.0.0.1:%1").arg(50000 | address);
    ClientExperiment *clientExp = new ClientExperiment(host, this);
    experiments.insert(clientExp);


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
    modbus = new QProcess(this);
    modbus->setProgram(jmodbusconfig["app_path"].toString());
    connect(modbus, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Server::handlePsCode);
    modbus->start();
    if (!modbus->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << jmodbusconfig["app_path"].toString() << "not started";
    }
}


int Server::countClients()
{
    return clients.size();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << socket->error();
        return;
    }
    ClientManager *client = new ClientManager(&experiments, socket, this);
    connect(client, &ClientManager::finished, this, &Server::onRemoveClientManager, Qt::QueuedConnection);
    connect(client, &ClientManager::sendServerCMD, this, &Server::readCMD, Qt::QueuedConnection);
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

void Server::onRemoveClientExperiment()
{
    auto client = qobject_cast<ClientExperiment*>(sender());
    auto it = experiments.find(client);

    if (it == experiments.end()){
        qDebug() << Q_FUNC_INFO << "Error clients not contains this set. set.size =" << experiments.size();
        return;
    }
    (*it)->deleteLater();
    experiments.erase(it);
    qDebug() << " #####################################  clientsExperiment.size =" << experiments.size();
}

qint64 Server::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void Server::handlePsCode(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch (exitStatus) {
    case QProcess::CrashExit:
        qDebug() << Q_FUNC_INFO << "Warning: modbus crash exit";
        break;
    case QProcess::NormalExit:
        if (exitCode == 1) {
            qDebug() << Q_FUNC_INFO<< "handle when modbus run";
            QProcess p;
            p.start(QString("pkill"), QStringList("modbusserver"));
            p.waitForFinished();
            QThread::msleep(300);
            startModbus();
        }
        break;
    }
}

void Server::readCMD(QJsonObject &jobj)
{
//    modbus->pid();
    modbus->deleteLater();
}
