#ifndef SOCKETCLIENTMODBUS_H
#define SOCKETCLIENTMODBUS_H

#include <QQueue>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "network/abstractjsonclient.h"

class Experiment;


class ModbusClient : public AbstractJSONClient
{
    Q_OBJECT

    QQueue <QJsonObject> queueRequests;
public:
    ModbusClient(Experiment *exp, QObject *parent = nullptr);
    ~ModbusClient();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void onSendReqeust();

public slots:
    void readyRead(QJsonObject &jobj) override;
    void sendRequest(const QJsonObject &jobj);
    void procQueue();


private:
    Experiment *experiment;
};

#endif // SOCKETCLIENTMODBUS_H
