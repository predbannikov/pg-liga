#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "socketclient.h"

class ClientManager : public QObject
{
    Q_OBJECT
public:
    ClientManager(QObject *parent = nullptr);
    void startManager();


};

#endif // CLIENTMANAGER_H
