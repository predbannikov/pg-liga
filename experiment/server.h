#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QThread>
#include "experiment.h"
#include "socketclientmodbus.h"
#include "clientmanager.h"

#define ADDRESS_MODBUS  2


class Server : public QTcpServer
{
    Q_OBJECT

    QSet <ClientManager*> clients;

    QThread *threadExperiment;
    ModbusClient *modbusClient;

    const quint8 address = ADDRESS_MODBUS;

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void startServer();
protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    Experiment *experiment;



public slots:
    void onRemoveClientManager();

signals:
    void sendRequest(QJsonObject);


};

#endif // SERVER_H
