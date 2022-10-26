#include <QCoreApplication>

#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
//    QDir cur_dir = QDir::current();
//    QStringList list = cur_dir.entryList(QDir::Files);

    if (!QFile::exists("./config.json")) {
        qDebug() << "file config.json not exists";
    } else {
        readConfig();
        checkAndPrepFoldersPrograms();
        runPrograms();
    }
}

Server::~Server()
{
    qDebug() << Q_FUNC_INFO;
}


void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,55123))
    {
        qDebug() << Q_FUNC_INFO << "Could not start server" << this->serverAddress() << this->serverPort();
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Listening..." << this->serverAddress() << this->serverPort();
    }
}

QByteArray Server::getConfigBase64()
{
    QByteArray configBase64 = QJsonDocument(jconfig).toJson().toBase64();
    return configBase64;
}

QJsonObject Server::readConfig()
{
    QFile file_settings("./config.json");
    if (!file_settings.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << Q_FUNC_INFO << file_settings.fileName() << "file no open";
        return QJsonObject();
    }
    jconfig = QJsonDocument::fromJson(file_settings.readAll()).object();
    file_settings.close();
    if (!jconfig.contains("programs")) {
        qDebug() << "no settings path programs";
    } else {
        jprograms = jconfig["programs"].toArray();
    }
    return jconfig;
}

void Server::startModbusProcess(QString fileName)
{
    modbus = new QProcess(this);
    modbus->setProgram(fileName);
    connect(modbus, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Server::handlePsCodeModbus, Qt::QueuedConnection);
//    connect(modbus, &QProcess::readyReadStandardError, [this]() {
//        qDebug() << modbus->program() << qPrintable(modbus->readAllStandardError());
//    });
//    connect(modbus, &QProcess::readyReadStandardOutput, [this]() {
//        qDebug() << modbus->program() << qPrintable(modbus->readAllStandardOutput());
//    });
    qDebug() << "start" << modbus->program();
    modbus->start();
    if (!modbus->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << fileName << "not started";
    }
}

void Server::startExperimentProccess(QString fileName)
{
    QString name = fileName.split('/').last();
    QString address = name.split('-')[1];
    QString host = QString("127.0.0.1:%1").arg(50000 | address.toUInt());
    ClientExperiment *clientExp = new ClientExperiment(host, this);
    connect(clientExp, &ClientExperiment::sendReadyResponse, this, &Server::resolvingResponsExperiment, Qt::QueuedConnection);
    experiments.insert(clientExp);

    QProcess *procExp = new QProcess(this);
    qDebug() << "start" << procExp->program();
    procExp->start(fileName, QStringList(address));
//    procExp->setProcessChannelMode(QProcess::MergedChannels);
    connect(procExp, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Server::handlePsCodeExperiment, Qt::QueuedConnection);
    connect(procExp, &QProcess::readyReadStandardError, [procExp]() {
        qDebug() << procExp->program() << qPrintable(procExp->readAllStandardError());
    });
    connect(procExp, &QProcess::readyReadStandardOutput, [procExp]() {
        qDebug() << procExp->program() << qPrintable(procExp->readAllStandardOutput());
    });
    if (!procExp->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << procExp->program() << "not started";
    }
    procExperiments.insert(procExp);
}

void Server::stopExperiementProccesses()
{
    for (auto it = experiments.begin(); it != experiments.end();) {
        if (!configContainsInstr((*it)->address)) {
            (*it)->deleteLater();
            it = experiments.erase(it);
        } else {
            it++;
        }
    }
    for (auto it = procExperiments.begin(); it != procExperiments.end();) {
        if (!configContainsInstr((*it)->program().split('/').last().split('-')[1].toUInt())) {
            (*it)->kill();
            (*it)->deleteLater();
            it = procExperiments.erase(it);
        } else {
            it++;
        }
    }
}

void Server::startProccess(QProcess *procExp)
{
    qDebug() << "start" << procExp->program();
    procExp->start();
    if (!procExp->waitForStarted(1000)) {
        qDebug() << Q_FUNC_INFO << procExp->program() << "not started";
    }
}

int Server::countClients()
{
    return clients.size();
}

void Server::resolvingResponsExperiment(QJsonObject jobj)
{
    auto it = clients.begin();
    for (; it != clients.end(); it++) {
        if ((*it)->socketID() == jobj["client_liga"].toString().toLongLong()) {
            (*it)->procQueue(jobj);
            return;
        }
    }
    qDebug() << Q_FUNC_INFO << "not found client for response experiemnt";
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
        QDir app_path(QDir(jobj["app_folder"].toString()).absolutePath());
        if (!cur_path.mkpath(app_path.path())) {
            qDebug() << app_path.path() << "folder not created";
        }
        QFile app_file(app_path.filePath(jobj["app_name"].toString()));
        if (!app_file.exists()) {
            qDebug() << app_file.fileName() << "copy";
            QFile srcFile(cur_path.filePath(jobj["app_src_name"].toString()));
            srcFile.copy(app_file.fileName());
        } else {
            qDebug() << app_file.fileName() << "file exist";
        }
    }
}

bool Server::isRuningExperiment(quint8 address)
{
    auto it = experiments.begin();
    for (; it != experiments.end(); it++) {
        if ((*it)->address == address)
            return true;
    }
    return false;
}

void Server::runPrograms()
{
    for (const auto &jobj: qAsConst(jprograms)) {
        QDir app_path(QDir(jobj["app_folder"].toString()).absolutePath());
        QFile app_file(app_path.filePath(jobj["app_name"].toString()));
        if (jobj["app_src_name"].toString() == "experiment") {
            if (!isRuningExperiment(app_file.fileName().split('/').last().split('-')[1].toUInt()))
                startExperimentProccess(app_file.fileName());
        } else if (jobj["app_src_name"].toString() == NAME_PROGRAM_MODBUS_SERVER) {
            startModbusProcess(app_file.fileName());
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

bool Server::configContainsInstr(quint8 addr)
{
    for (const auto &jobj: qAsConst(jprograms))
        if (jobj["app_src_name"].toString() == "experiment")
            if (jobj["app_name"].toString().split('-')[1].toUInt() == addr)
                return true;
    return false;
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
            p.start(QString("pkill"), QStringList(NAME_PROGRAM_MODBUS_SERVER));
            p.waitForFinished();
            QThread::msleep(1000);
            auto proc = qobject_cast<QProcess*>(sender());
            startProccess(proc);
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
            startProccess(proc);
        }
        break;
    }
}

void Server::readCMD(QJsonObject jobj)
{
    if (jobj["CMD"].toString() == "stop_manager") {
        qDebug() << jobj["CMD"].toString();
        qApp->exit();
    } else if (jobj["CMD"].toString() == "close_instr") {

        qDebug() << jobj["CMD"].toString();
        modbus->kill();
    } else if (jobj["CMD"].toString() == "stop_modbus") {
        qDebug() << jobj["CMD"].toString();
        modbus->kill();
    } else if (jobj["CMD"].toString() == "start_modbus") {
        for (const auto &jobj: qAsConst(jprograms)) {
            QDir app_path(QDir(jobj["app_folder"].toString()).absolutePath());
            QFile app_file(app_path.filePath(jobj["app_name"].toString()));
            if (jobj["app_src_name"].toString() == NAME_PROGRAM_MODBUS_SERVER) {
                startModbusProcess(app_file.fileName());
            }
        }
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
        QByteArray jconfByteArray = QByteArray::fromBase64(jobj["settings"].toString().toUtf8());
        file_settings.write(jconfByteArray);
        file_settings.close();
        readConfig();


        jconfig = QJsonDocument::fromJson(jconfByteArray).object();
        checkAndPrepFoldersPrograms();
        runPrograms();
        stopExperiementProccesses();


    }
}
