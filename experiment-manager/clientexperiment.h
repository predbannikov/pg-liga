#ifndef CLIENTEXPERIMENT_H
#define CLIENTEXPERIMENT_H

#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QTimer>
#include <QDate>

#include "network/abstractjsonclient.h"




class ClientExperiment : public AbstractJSONClient
{
    Q_OBJECT

public:
    quint8 address;

    ClientExperiment(QString host_, QObject *parent = nullptr);
    ~ClientExperiment();

signals:
//    void error(QTcpSocket::SocketError socketerror);
    void sendReadyResponse(QJsonObject);

public slots:
//    void onSendJson(const QJsonObject &jobj);
    void readyRead(QJsonObject &jobj) override;
    void onSendReadyRequest(QJsonObject jobj);

private:
    qint64 timeInterval(const QString &date, const QString &format);
};

#endif // CLIENTEXPERIMENT_H
