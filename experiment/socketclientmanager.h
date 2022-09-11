#ifndef SOCKETCLIENTMANAGER_H
#define SOCKETCLIENTMANAGER_H

#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "experiment.h"

class SocketClientManager : public QObject
{
    Q_OBJECT
    QQueue <QJsonObject> queueResponse;
public:
    SocketClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent = nullptr);
    ~SocketClientManager();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void disconnectClient(quint64);
    void connectedClient();
    void sendReqeust(QJsonObject);
    void readyReadPDU(const QByteArray &pdu);

public slots:
    void readyRead();
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void procQueue(const QJsonObject &jResponse);

private:

    QTcpSocket *socket;
    Experiment *experiment;
//    void connectToHost();
};

#endif // SOCKETCLIENTMANAGER_H
