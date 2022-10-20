#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QMutex>
#include "socketclient.h"


class Server : public QTcpServer
{
    Q_OBJECT

    QMap <quint64, ClientSocket* > clients;
    QThread *serialPortThread;
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:



public slots:
    void parserReadyRequest(QJsonObject jobj);
    void removeClient(quint64 isock);

signals:
    void sendRequest(QJsonObject);

};

#endif // SERVER_H
