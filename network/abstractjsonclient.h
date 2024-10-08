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
#include <QHostInfo>
#include <QTimer>

#define TIMEOUT_CONNECTION  1000
#define TIMER_RECONNECTION  1000
#define TIMER_PING          3000

class AbstractJSONClient : public QObject
{
    Q_OBJECT

    enum TYPE {CLIENT, SERVER} type;

    QTcpSocket *socket = nullptr;
    QTimer timerReconnect;
    QTimer timerPing;
    QHostAddress host;
    int port;
    int timeout = TIMEOUT_CONNECTION;
    bool transact = false;
    /**
     * @brief statusConnection  устанавливается в true после каждого приёма пакета
     *                          устанавливается в false после отправки пакета ping
     *                          перед отправкой ping проверяется этот статус, если он в false закрывается соединение
     */
    bool statusConnection = false;
public:
    explicit AbstractJSONClient(QString endPoint, QObject *parent = nullptr);
    AbstractJSONClient(QTcpSocket *socket_, QObject *parent = nullptr);
    virtual ~AbstractJSONClient();
    void setTimeoutConnection(int ms = TIMEOUT_CONNECTION);
    void setTimerReconnect(int ms = TIMER_RECONNECTION);
    void setTimerPing(int ms = TIMER_PING);
    bool getStatusConnection() { return statusConnection; }
    qint64 socketID();
    bool write(QJsonObject &jobj);
    virtual void readyRead(QJsonObject &jobj) = 0;

    QString clientLastError;
private:
    QString typeToStr(TYPE tp);

public slots:
    void init();
    void connectToHost();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();
    void disconnected();
    void connected();
    void bytesWritten(qint64 bytes);
    void ping();

signals:
    void connectedClient();

    /**
     * @brief disconnectClient      client закрыл соединени
     */
    void disconnectClient(qint64);

    /**
     * @brief finished              server закрыл соединение
     */
    void finished(qint64);
};

#endif // ABSTRACTJSONCLIENT_H
