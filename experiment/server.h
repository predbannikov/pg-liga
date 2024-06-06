#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QThread>
#include "experiment.h"
//#include "socketclientmodbus.h"
#include "clientmanager.h"
//#include "serialport.h"

class Server : public QTcpServer
{
    Q_OBJECT

    QSet <ClientManager*> clients;

    QThread *threadExperiment = nullptr;
    Experiment *experiment = nullptr;

    const quint8 address;

public:
    explicit Server(quint8 addr_, QObject *parent = nullptr);
    ~Server();

    void startServer();
protected:
    void incomingConnection(qintptr socketDescriptor);

private:



public slots:
    void onRemoveClientManager();

signals:
    void sendRequest(QJsonObject);


};

#endif // SERVER_H
