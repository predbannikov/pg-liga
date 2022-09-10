#ifndef CLIENTEXPERIMENT_H
#define CLIENTEXPERIMENT_H

#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QTimer>




class ClientExperiment : public QObject
{
    Q_OBJECT

    QJsonObject jresponse;
public:
    const quint8 address;

    ClientExperiment(quint8 addr, QObject *parent = nullptr);
    ~ClientExperiment();

signals:
//    void error(QTcpSocket::SocketError socketerror);
    void deleteClient(qint8);
    void sendReadyResponse(QJsonObject);
    void connectClient();

public slots:
//    void onSendJson(const QJsonObject &jobj);
    void connectToHost();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void readyRead();
    void disconnected();
    void onSendReadyRequest(QJsonObject jobj);
    void connected();
    void bytesWritten(qint64 bytes);


private:
    QTcpSocket *socket;

    qint64 secs;
    qint64 timeInterval(const QString &date, const QString &format);
    QTimer timerReconnect;
};

#endif // CLIENTEXPERIMENT_H
