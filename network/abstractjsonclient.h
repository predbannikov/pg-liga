#ifndef ABSTRACTJSONCLIENT_H
#define ABSTRACTJSONCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QTimer>

class AbstractJSONClient : public QObject
{
    Q_OBJECT

    enum TYPE {CLIENT, SERVER} type;

    QTcpSocket *socket;
    QTimer timerReconnect;
    QHostAddress host;
    int port;
    int timeout = 1000;
    bool transact = false;
public:
    explicit AbstractJSONClient(QString endPoint_, QObject *parent = nullptr);
    AbstractJSONClient(QTcpSocket *socket_, QObject *parent = nullptr);
    virtual ~AbstractJSONClient();
    void setTimeoutConnection(int ms = 1000);
    void setTimerReconnect(int ms = 1000);
    bool write(QJsonObject jobj);
    virtual void readyRead(QJsonObject &jobj) = 0;

private:

    void init();

public slots:
    void connectToHost();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();
    void disconnected();
    void connected();
    void bytesWritten(qint64 bytes);

signals:
    void connectedClient();
    void disconnectClient();
    void finished();
};

#endif // ABSTRACTJSONCLIENT_H
