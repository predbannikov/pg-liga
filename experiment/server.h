#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QThread>
#include "experiment.h"
#include "socketclientmodbus.h"
#include "clientmanager.h"

class Server : public QTcpServer
{
    Q_OBJECT

    QSet <ClientManager*> clients;

    QThread *threadExperiment;
    ModbusClient *modbusClient;

    const quint8 address;

public:
    explicit Server(quint8 addr_, QObject *parent = nullptr);
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
