#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "experiment.h"
#include "network/abstractjsonclient.h"

class ClientManager : public AbstractJSONClient
{
    Q_OBJECT

    QQueue <QJsonObject> queueResponse;
public:
    ClientManager(Experiment *exp, QTcpSocket *sock, QObject *parent = nullptr);
    ~ClientManager();

    void readyRead(QJsonObject &jobj) override;

signals:
//    void error(QTcpSocket::SocketError socketerror);
//    void disconnectClient(quint64);
//    void connectedClient();
//    void sendReqeust(QJsonObject);
//    void readyReadPDU(const QByteArray &pdu);

public slots:
    void procQueue(const QJsonObject &jResponse);

//    void readyRead();
//    void connected();
//    void disconnected();
//    void bytesWritten(qint64 bytes);

private:
    Experiment *experiment;
};

#endif // CLIENTMANAGER_H
