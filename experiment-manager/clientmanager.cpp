#include "clientmanager.h"
#include "server.h"

ClientManager::ClientManager(QSet<ClientExperiment *> *experiments_, QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent)
{
    experiments = experiments_;
    for(auto &exp: qAsConst(*experiments)) {
        connect(exp, &ClientExperiment::sendReadyResponse, this, &ClientManager::procQueue);
    }

//    const int address = 2;
//    QString host = QString("127.0.0.1:%1").arg(50000 | address);
//    ClientExperiment *clientExp = new ClientExperiment(host, this);
//    experiments.insert(clientExp);
//    connect(clientExp, &ClientExperiment::sendReadyResponse, this, &ClientManager::procQueue);
//    connect(clientExp, &ClientExperiment::finished, this, &ClientManager::onRemoveClientExperiment);
}

ClientManager::~ClientManager()
{
    qDebug() << Q_FUNC_INFO;
}

void ClientManager::readyRead(QJsonObject &jobj)
{
    parserJSON(jobj);
}

void ClientManager::procQueue(const QJsonObject &jResponse)
{
    queueResponse.push_back(jResponse);
    while(!queueResponse.isEmpty()) {
        QJsonObject jobj(queueResponse.takeFirst());
        write(jobj);
    }
}

void ClientManager::parserJSON(QJsonObject &jobj)
{
    if (jobj["type"].toString() == "manager") {
        if (jobj.contains("get")) {
            if (jobj["get"].toString() == "clients") {
                jobj["clients_size"] = QString::number(qobject_cast<Server*>(this->parent())->countClients());
                jobj["clients_exp_size"] = QString::number(experiments->size());
                QJsonArray jaddr;
                for(auto &item: *experiments)
                    jaddr.append(QString::number(item->address));
                jobj["clients_address"] = jaddr;
                write(jobj);
            }
        } else if (jobj.contains("CMD")) {
            emit sendServerCMD(jobj);
        }
    } else if (jobj["type"].toString() == "client") {
        if (jobj["CMD"].toString() == "settings") {
            qDebug() << "settings";
        }
        if (jobj.contains("CMD")) {
            for(auto &exp: *experiments)
                if (jobj["address"].toString().toInt() == exp->address) {
                    exp->onSendReadyRequest(jobj);
                }
        }
    }
}
