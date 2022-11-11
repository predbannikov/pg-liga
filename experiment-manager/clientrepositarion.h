#ifndef CLIENTREPOSITARION_H
#define CLIENTREPOSITARION_H

#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QTimer>
#include <QDate>

#include "network/abstractjsonclient.h"


class ClientRepositarion : public AbstractJSONClient
{
    Q_OBJECT
public:
    ClientRepositarion(QObject *parent = nullptr);
    ~ClientRepositarion();

signals:
    void sendReadyResponse(QJsonObject);

public slots:
    void readyRead(QJsonObject &jobj) override;
    void onSendReadyRequest(QJsonObject jobj);

};

#endif // CLIENTREPOSITARION_H
