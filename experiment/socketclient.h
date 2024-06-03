#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMutex>
#include <mutex>

#include "serialport.h"
#include "network/abstractjsonclient.h"


class ClientSocket : public AbstractJSONClient
{
    Q_OBJECT

    QJsonObject jrequest;

public:
    ClientSocket(QTcpSocket *sock, SerialPort *serial_port, QObject *parent = nullptr);
    ~ClientSocket();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void sendReqeust(QJsonObject);

public slots:
    void readyRead(QJsonObject &jobj) override;
    void sendReadyRequest(QJsonObject &jobj);


private:
    SerialPort *serialport;
};

#endif // SOCKETCLIENT_H
