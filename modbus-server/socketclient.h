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


class ClientSocket : public QThread
{
    Q_OBJECT

    QJsonObject jrequest;

public:
    ClientSocket(QTcpSocket *sock, SerialPort *serial_port, QObject *parent = nullptr);
    ~ClientSocket();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void disconnectClient(quint64);
    void sendReqeust(QJsonObject);

public slots:
    void readyRead();
    void disconnected();
    void sendReadyRequest(QJsonObject jobj);


private:
    QTcpSocket *socket;
    SerialPort *serialport;
};

#endif // SOCKETCLIENT_H
