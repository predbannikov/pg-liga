#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QMap>
#include <QProcess>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QPointer>

#include "clientmanager.h"

#define FILE_CONFIG_JSON    "config.json"
#define PATH_MODBUS_SERVER  "/home/user/projects/control_app_liga/modbus_server/modbusserver"

class Server : public QTcpServer
{
    Q_OBJECT

    QSet <ClientManager*> clientManagers;
    QJsonObject jconfig;
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void startServer();

    void startModbus();
protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QProcess *modbus = nullptr;
    qint64 timeInterval(const QString &date, const QString &format);

public slots:
    void readCMD(QJsonObject &jobj);
    void onRemoveClientManager();
    int countClients();

signals:
    void sendRequest(QJsonObject);


};

#endif // SERVER_H
