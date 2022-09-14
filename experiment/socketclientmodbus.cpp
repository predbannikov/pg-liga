#include "socketclientmodbus.h"
#include "experiment.h"

#define DEBUG 1

ModbusClient::ModbusClient(Experiment *exp, QObject *parent) : AbstractJSONClient("127.0.0.1:1234", parent), experiment(exp)
{
//    connect(this,  &AbstractJSONClient::connectedClient, this, &ModbusClient::procQueue, Qt::QueuedConnection);
    connect(this,  &AbstractJSONClient::connectedClient, this, &ModbusClient::notifyExperiment, Qt::QueuedConnection);
    connect(this, &ModbusClient::onSendReqeust, this, &ModbusClient::procQueue, Qt::QueuedConnection);
}

ModbusClient::~ModbusClient()
{
    qDebug() << Q_FUNC_INFO;
}

void ModbusClient::readyRead(QJsonObject &jobj)
{
    experiment->put(jobj);
}

void ModbusClient::sendRequest(const QJsonObject &jobj)
{
    queueRequests.push_back(jobj);
    emit onSendReqeust();

}

void ModbusClient::procQueue()
{
    while(!queueRequests.isEmpty()) {
        QJsonObject jobj(queueRequests.takeFirst());
        if (!write(jobj)) {
            experiment->put(jobj);
        }
    }
}

void ModbusClient::notifyExperiment()
{
    QJsonObject jobj;
    jobj["type"] = "modbus";
    jobj["connection"] = "connected";
    experiment->put(jobj);
}
