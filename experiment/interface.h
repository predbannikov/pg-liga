#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QJsonObject>
#include <QQueue>
#include <QMutexLocker>
#include <QWaitCondition>

//#include "socketclientmodbus.h"
#include "global.h"
#include "serialport.h"

class Interface : public QObject
{
    Q_OBJECT

    QMutex mtx;
//    QMutex mtxcmd;
    QWaitCondition notEmpty;
    /// json объект из очереди комманд
    QJsonObject jobjTaked;
    QQueue <QJsonObject> queueRequest;
    bool abort = false;




public:
    explicit Interface(quint8 addr, QObject *parent = nullptr);
    ~Interface();

    void write(QJsonObject &jOperation);
    void read(QJsonObject &jOperation);
    void put(QJsonObject &jobj);
    void stop();

//    virtual void execCMD(QJsonObject &jobj) = 0;

    SerialPort *modbus = nullptr;


signals:
    /// send to modbus
    void sendRequestToModbus(QJsonObject &);
    void sendRequestToClient(QJsonObject);

private:
//    ModbusClient *modbusClient;
    quint8 address = 0;
};

#endif // INTERFACE_H
