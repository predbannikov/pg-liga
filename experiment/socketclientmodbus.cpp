#include "socketclientmodbus.h"
#include "experiment.h"

#define DEBUG 1

ModbusClient::ModbusClient(Experiment *exp, QObject *parent) : QObject(parent), experiment(exp)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ModbusClient::readyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &ModbusClient::disconnected, Qt::DirectConnection);
    connect(socket, &QTcpSocket::connected, this, &ModbusClient::connected);
    connect(socket, &QTcpSocket::connected, this, &ModbusClient::connectedClient);
    connect(socket, &QTcpSocket::bytesWritten, this, &ModbusClient::bytesWritten);
    connect(this, &ModbusClient::onSendReqeust, this, &ModbusClient::procQueue);
    connectToHost();

//    connect(experiment, &InterfaceModbus::sendRequest, this, &ModbusClient::sendRequest, Qt::DirectConnection);

}

ModbusClient::~ModbusClient()
{
    qDebug() << Q_FUNC_INFO;
}

void ModbusClient::connectToHost()
{
    qDebug() << "Connecting,..";

    socket->connectToHost("127.0.0.1", 1234);

    if(socket->waitForDisconnected(1000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void ModbusClient::readyRead()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(socket->readAll());
    QJsonObject jrequest = jdoc.object();
//    if (DEBUG) {
//        qDebug() << "request" << jrequest["PDU_request"].toString();
//        qDebug() << "response" << jrequest["PDU_response"].toString();
//        qDebug() << "";
//    }
    experiment->put(jrequest);
}

void ModbusClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void ModbusClient::connected()
{
    procQueue();
}

void ModbusClient::disconnected()
{
    socket->deleteLater();
    socket = nullptr;
    emit disconnectClient();
}

void ModbusClient::sendRequest(const QJsonObject &jobj)
{
    queueRequests.push_back(jobj);
    emit onSendReqeust();

}

void ModbusClient::procQueue()
{
    while(!queueRequests.isEmpty()) {
        if (socket != nullptr && socket->isOpen()) {
            QJsonObject jobj(queueRequests.takeFirst());
            socket->write(QJsonDocument(jobj).toJson());
        } else {
            break;
        }
    }
}
