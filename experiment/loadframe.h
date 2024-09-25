#ifndef LOADFRAME_H
#define LOADFRAME_H

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "global.h"
#include "sensors.h"
#include "stepper.h"
#include "controller.h"
#include "data.h"

class LoadFrame
{
    enum READ_SENS {READ_SENS_1, READ_SENS_2, READ_SENS_3, READ_SENS_4, READ_SENS_5, READ_CONTROLLER_STATUS, READ_SENS_6} readSensState = READ_SENS_1;

    // enum STATE_SET_STATE_PID_CONTROLLER {STATE_SET_STATE_PID_CONTROLLER_DEFAULT, STATE_P_CORRECTION} state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;

    Sensor *getSensorFromStr(QString strSensorName) {
        if (strSensorName == "SensLoad0") {
            return forceSens;
        } else if (strSensorName == "SensDef0") {
            return deformSens;
        }
        return forceSens;
    }

public:
    LoadFrame();
    ~LoadFrame();

    void setStoreData(Data *storeData);
    bool init();
    bool deleteData();
    void resetStateModeBusCommunication();

    RETCODE readSensorForce(QJsonObject &jOperation);
    RETCODE readRawSensorForce(QJsonObject &jOperation);
    RETCODE readSensorDeform(QJsonObject &jOperation);
    RETCODE readRawSensorDeform(QJsonObject &jOperation);
    RETCODE readHolStatus(QJsonObject &jOperation);
    RETCODE readPosition(QJsonObject &jOperation);
    RETCODE readControllerStatus(QJsonObject &jOperation);

    RETCODE setHz(QJsonObject &jOperation);
    // RETCODE setStatePid(QJsonObject &jOperation);
    RETCODE statusSensors(QJsonObject &jOperation);
    RETCODE moveFrame(QJsonObject &jobj);
    RETCODE unlockPID(QJsonObject &jobj);
    RETCODE setTarget(QJsonObject &jOperation);
    RETCODE setTargetVibro(QJsonObject &jOperation);
    RETCODE setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd);
    RETCODE stopFrame(QJsonObject &jobj);
    RETCODE sensorSetZero(QJsonObject &jobj);
    RETCODE resetSensorOffset(QJsonObject &jobj);
    RETCODE stepperSetNull(QJsonObject &jobj);

    void readSensors(QJsonObject &jobj);
    void sendProtocol(QJsonObject &jobj);
    void sendStoreData(QJsonObject &jobj);

    Measurements::Force targetNewtones;
    Measurements::Force targetMinNewtones;

    QJsonObject jconfig;
    quint8 address = 0;

    Sensor *forceSens = nullptr;
    Sensor *deformSens = nullptr;
    Stepper *stepper = nullptr;
    Controller *controller = nullptr;

    Data *store = nullptr;

    void setStore(Data *pStore);

    QString unitName;

private:
};


#endif // LOADFRAME_H
