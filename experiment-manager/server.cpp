#include <QCoreApplication>

#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
    QDir cur_dir = QDir::current();
    QStringList list = cur_dir.entryList(QDir::Files);

    if (!list.contains("modbusserver")) {
        qDebug() << "qtcreator version start";
    } else {

        if (!QFile::exists("./config.json")) {
            qDebug() << "file config.json not exists";
        } else {

            QFile file("config.json");
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "file not open";
            } else {

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
            file.close();
        }
    }
    checkAndPrepFoldersPrograms();
    runPrograms();
//    startExperiment(address);
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
    connect(modbus, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Server::handlePsCodeModbus, Qt::QueuedConnection);
    modbus->start();
    if (!modbus->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << jmodbusconfig["app_path"].toString() << "not started";
    }
}

void Server::startExperiment(QString fileName)
{
    QString name = fileName.split('/').last();
    QString address = name.split('-')[1];
    QString host = QString("127.0.0.1:%1").arg(50000 | address.toUInt());
    ClientExperiment *clientExp = new ClientExperiment(host, this);
    experiments.insert(clientExp);

    QProcess *procExp = new QProcess(this);
//    procExp->setProcessChannelMode(QProcess::MergedChannels);
    connect(procExp, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Server::handlePsCodeExperiment, Qt::QueuedConnection);
    procExp->start(fileName, QStringList(address));
    if (!procExp->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << procExp->program() << "not started";
    }
    procExperiments.insert(procExp);
}

void Server::startExperiment(QProcess *procExp)
{
    procExp->start();
    if (!procExp->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << procExp->program() << "not started";
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
        qDebug() << Q_FUNC_INFO << "Error clients not contains this set experiments. size =" << experiments.size();
        return;
    }
    (*it)->deleteLater();
    experiments.erase(it);
    qDebug() << " #####################################  clientsExperiment.size =" << experiments.size();
}

void Server::checkAndPrepFoldersPrograms()
{
    QDir cur_path = QDir::currentPath();
    for (const auto &jobj: qAsConst(jprograms)) {
        QDir app_path(QDir(jobj["path"].toString()).absolutePath());
        if (!cur_path.mkpath(app_path.path())) {
            qDebug() << app_path.path() << "folder not created";
        }
        QFile app_file(app_path.filePath(jobj["name"].toString()));
        if (!app_file.exists()) {
            qDebug() << app_file.fileName() << "copy";
            QFile srcFile(cur_path.filePath(jobj["program"].toString()));
            srcFile.copy(app_file.fileName());
        } else {
            qDebug() << app_file.fileName() << "file exist";
        }
    }
}

void Server::runPrograms()
{
    for (const auto &jobj: qAsConst(jprograms)) {
        if (jobj["program"].toString() == "experiment"){
            QDir app_path(QDir(jobj["path"].toString()).absolutePath());
            QFile app_file(app_path.filePath(jobj["name"].toString()));
            startExperiment(app_file.fileName());
        }
    }
}

qint64 Server::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void Server::handlePsCodeModbus(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch (exitStatus) {
    case QProcess::CrashExit:
        qDebug() << Q_FUNC_INFO << "Warning: modbus crash exit";
        break;
    case QProcess::NormalExit:
        if (exitCode == 1) {
            qDebug() << Q_FUNC_INFO<< "handle when modbus runing";
            QProcess p;
            p.start(QString("pkill"), QStringList("modbusserver"));
            p.waitForFinished();
            QThread::msleep(1000);
            startModbus();
        }
        break;
    }
}

void Server::handlePsCodeExperiment(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch (exitStatus) {
    case QProcess::CrashExit:
        qDebug() << Q_FUNC_INFO << "Warning: experiment crash exit";
        break;
    case QProcess::NormalExit:
        if (exitCode == 1) {
            qDebug() << Q_FUNC_INFO<< "handle when experiment runing";
            auto proc = qobject_cast<QProcess*>(sender());
            auto it = procExperiments.find(proc);
            if (it == procExperiments.end()) {
                qDebug() << Q_FUNC_INFO << "Error proccess not contains this set procExperiments. size =" << procExperiments.size();
                return;
            }
            QProcess p;
            QStringList args;
            QString name = proc->program().split('/').last();
            args.append(name);
            p.start(QString("pkill"), args);
            p.waitForFinished();
            QThread::msleep(1000);
            startExperiment(proc);
        }
        break;
    }
}

void Server::readCMD(QJsonObject jobj)
{
    if (jobj["CMD"].toString() == "stop_manager") {
        qDebug() << jobj["CMD"].toString();
        qApp->exit();
    } else if (jobj["CMD"].toString() == "stop_modbus") {
        qDebug() << jobj["CMD"].toString();
        modbus->kill();
    } else if (jobj["CMD"].toString() == "stop_experiemnt") {
        qDebug() << jobj["CMD"].toString();
        QString addr = jobj["addr_exp"].toString();
        for (auto proc: qAsConst(procExperiments)) {
            QString name = proc->program().split('/').last();
            if (name.split('-')[1] == addr) {
                proc->kill();
                break;
            }
        }
    } else if (jobj["CMD"].toString() == "update_config") {
        QFile file_settings("./config.json");
        if (!file_settings.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << Q_FUNC_INFO << file_settings.fileName() << "file no open";
            return;
        }
        file_settings.write(QByteArray::fromBase64(jobj["settings"].toString().toUtf8()));
        file_settings.close();
        checkAndPrepFoldersPrograms();
        runPrograms();
    }
}
