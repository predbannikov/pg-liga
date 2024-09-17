#ifndef VOLUMETER2_H
#define VOLUMETER2_H

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "global.h"
#include "sensors.h"
#include "stepper.h"
#include "controller.h"
#include "storedata.h"

class Volumeter2
{
    enum READ_SENS {READ_SENS_1, READ_SENS_2, READ_SENS_3, READ_SENS_4, READ_CONTROLLER_STATUS, READ_SENS_6} readSensState = READ_SENS_1;

    Sensor *getSensorFromStr(QString strSensorName) {
        if (strSensorName == "SensPress0Addr") {
            return pressureSens;
        }
        return pressureSens;
    }
public:
    Volumeter2();
    ~Volumeter2();

    bool init();
    bool deleteData();
    void resetStateModeBusCommunication();

    // RETCODE setStatePid(QJsonObject &jOperation);
    RETCODE readSensorPressure(QJsonObject &jOperation);
    RETCODE readRawSensorPressure(QJsonObject &jOperation);
    RETCODE readStatusHol(QJsonObject &jOperation);
    RETCODE readPosition(QJsonObject &jOperation);
    RETCODE readControllerStatus(QJsonObject &jOperation);
    // RETCODE updateStoreData(QJsonObject &jOperation);

    RETCODE statusSensors(QJsonObject &jOperation);
    RETCODE movePiston(QJsonObject &jobj);
    RETCODE unlockPID(QJsonObject &jobj);
    RETCODE setTarget(QJsonObject &jOperation);
    RETCODE setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd);
    RETCODE stopPiston(QJsonObject &jobj);
    RETCODE sensorSetZero(QJsonObject &jobj);
    RETCODE resetSensorOffset(QJsonObject &jobj);
    RETCODE stepperSetNull(QJsonObject &jobj);

    void readSensors(QJsonObject &jobj);
    void sendProtocol(QJsonObject &jobj);
    void sendStoreData(QJsonObject &jobj);

    Measurements::Pressure targetPressure;

    QJsonObject jconfig;
    quint8 address = 0;

    Sensor *pressureSens = nullptr;
    Stepper *stepper = nullptr;
    Controller *controller = nullptr;

    StoreData *store = nullptr;
    void setStore(StoreData *pStore);
    QString unitName;
};



#endif // VOLUMETER2_H
