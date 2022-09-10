#include "clientmanager.h"
#include "server.h"
#include <QCoreApplication>

ClientManager::ClientManager(QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent)
{
    ClientExperiment *clientExp = new ClientExperiment(2, this);
    experiments.insert(2, clientExp);
    connect(clientExp, &ClientExperiment::deleteClient, this, &ClientManager::onRemoveClientExperiment);
    connect(clientExp, &ClientExperiment::sendReadyResponse, this, &ClientManager::procQueue);
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

void ClientManager::onRemoveClientExperiment(qint8 addr)
{
    if (!experiments.contains(addr)) {
        qDebug() << Q_FUNC_INFO << "Error clients not contains " << addr << "address";
        return;
    }
    experiments.value(addr)->deleteLater();
    experiments.remove(addr);
    qDebug() << " #####################################" << Q_FUNC_INFO << "clientsExperiment.size =" << experiments.size();
}

void ClientManager::parserJSON(QJsonObject &jobj)
{
    if (jobj["type"].toString() == "manager") {
        if (jobj.contains("get")) {
            if (jobj["get"].toString() == "clients") {
                jobj["clients_size"] = QString(qobject_cast<Server*>(this->parent())->countClients());
                jobj["clients_exp_size"] = QString::number(experiments.size());
                QJsonArray jaddr;
                for(auto &item: experiments)
                    jaddr.append(QString::number(item->address));
                jobj["clients_address"] = jaddr;
                write(jobj);
            }
        } else if (jobj.contains("stop")) {
            if (jobj["stop"].toString() == "manager") {
                qApp->exit();
            } else if (jobj["stop"].toString() == "modbus") {
                emit sendServerCMD(jobj);
            }
        }
    } else if (jobj["type"].toString() == "client") {
        if (jobj["CMD"].toString() == "settings") {
            qDebug() << "settings";
        }
        if (jobj.contains("CMD")) {
            for(auto &exp: experiments)
                if (jobj["address"].toString().toInt() == exp->address) {
                    exp->onSendReadyRequest(jobj);
                }
        }
    }
}
