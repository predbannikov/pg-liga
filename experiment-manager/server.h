#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QMap>
#include <QProcess>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QThread>

#include "clientmanager.h"

class Server : public QTcpServer
{
    Q_OBJECT

    QSet <ClientManager*> clients;
    QSet <ClientExperiment*> experiments;
    QSet <QProcess*> procExperiments;
    QJsonObject jconfig;
    QJsonArray jprograms;
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void startServer();
    QByteArray getConfigBase64();
    QJsonObject readConfig();
protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    void startExperimentProccess(QString fileName);
    void startProccess(QProcess *procExp);
    void startModbus(QString fileName);
    void stopExperiementProccess(QString addr);
//    void stopExperiemnt
    QProcess *modbus = nullptr;
    qint64 timeInterval(const QString &date, const QString &format);

public slots:
    void handlePsCodeModbus(int exitCode, QProcess::ExitStatus exitStatus);
    void handlePsCodeExperiment(int exitCode, QProcess::ExitStatus exitStatus);
    void readCMD(QJsonObject jobj);
    void onRemoveClientManager();
    int countClients();

    void onRemoveClientExperiment();

    void checkAndPrepFoldersPrograms();
    void runPrograms();
signals:
    void sendRequest(QJsonObject);


};

#endif // SERVER_H
