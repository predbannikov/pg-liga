#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QQueue>
#include <QTcpSocket>
#include <QSet>
#include <QDebug>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include "clientexperiment.h"
#include "network/abstractjsonclient.h"

class ClientManager : public AbstractJSONClient
{
    Q_OBJECT

    QQueue <QJsonObject> queueResponse;
public:
    ClientManager(QSet<ClientExperiment*> *experiments_, QTcpSocket *sock, QObject *parent = nullptr);
    ~ClientManager();

    void readyRead(QJsonObject &jobj) override;

signals:
    void sendServerCMD(QJsonObject &);

public slots:

    /**
     * @brief procQueue     Принимает данные по сигналу из ExperimentClient::sendReadyResponse
     * @param jResponse
     */
    void procQueue(const QJsonObject &jResponse);

private:
    void parserJSON(QJsonObject &jobj);
    QSet<ClientExperiment*> *experiments;
};

#endif // CLIENTMANAGER_H
