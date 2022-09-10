#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QJsonObject>

#include "socketthread.h"

class ClientManager : public QObject
{
    Q_OBJECT

    QMutex mutex;
    QWaitCondition notEmpty;
    bool abort;
    QQueue<QJsonObject> responses;
    QMap<qintptr, SocketThread*> clients;

public:
    explicit ClientManager(QObject *parent = nullptr);
    ~ClientManager();

    void addClient(qintptr id, SocketThread*);

    void run();


signals:
    void sendResponse(QJsonObject jobj);

public slots:
    void put(QJsonObject jobj);

private:

};

#endif // CLIENTMANAGER_H
