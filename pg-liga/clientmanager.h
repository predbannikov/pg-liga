#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QTcpSocket>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "network/abstractjsonclient.h"


class ClientManager : public AbstractJSONClient
{
    Q_OBJECT

    QJsonObject jrequest;
public:
    ClientManager(QString host_, QObject *parent = nullptr);
    ~ClientManager();
    void sendReadyRequest(QJsonObject jobj);
    void readyRead(QJsonObject &jobj) override;

signals:
    void readyReadResponse(const QJsonObject &jobj);

private:
};

#endif // CLIENTMANAGER_H
