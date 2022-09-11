#ifndef SOCKETCLIENTMODBUS_H
#define SOCKETCLIENTMODBUS_H

#include <QQueue>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class Experiment;


class ModbusClient : public QObject
{
    Q_OBJECT

    QQueue <QJsonObject> queueRequests;
public:
    ModbusClient(Experiment *exp, QObject *parent = nullptr);
    ~ModbusClient();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void disconnectClient();
    void connectedClient();
    void onSendReqeust();

public slots:
    void connectToHost();
    void readyRead();
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void sendRequest(const QJsonObject &jobj);
    void procQueue();


private:
    QTcpSocket *socket;
    Experiment *experiment;
};

#endif // SOCKETCLIENTMODBUS_H
