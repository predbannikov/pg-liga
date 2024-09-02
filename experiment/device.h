#ifndef DEVICE_H
#define DEVICE_H

#include <QDataStream>
#include <QJsonDocument>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QtEndian>
#include <QElapsedTimer>
#include <QThread>
#include <QDir>
#include "measurements.h"
#include "global.h"
#include "sensors.h"
#include "stepper.h"
#include "controller.h"
#include "storedata.h"
//#include "DataStore/datastore.h"


class LoadFrame
{
    enum STATE_PROCESS {PROCESS_IDLE, PROCESS_READ_NEXT_STEP, PROCESS_EXPERIMENT, PROCESS_FINISH} stateProc = PROCESS_FINISH;

    enum STATE_PRESURE {STATE_PRESURE_1, STATE_PRESURE_2, STATE_PRESURE_3} statePresure = STATE_PRESURE_1;

    enum STATE {STATE_IDLE, STATE_START, STATE_INIT, STATE_READ_NEXT_STEP, STATE_EXPERIMENT, STATE_COMPLATION,
                STATE_MOVE_FRAME, STATE_STOP_FRAME,
                STATE_UNLOCK_PID,
                STATE_FINISH,
                STATE_SERVICE_MODE} state = STATE_IDLE;

    enum READ_SENS {READ_SENS_1, READ_SENS_2, READ_SENS_3, READ_SENS_4, READ_SENS_5, READ_SENS_6} readSensState = READ_SENS_1;

    enum STATE_SET_STATE_PID_CONTROLLER {STATE_SET_STATE_PID_CONTROLLER_DEFAULT, STATE_P_CORRECTION} state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;

    Sensor &getSensorFromStr(QString strSensorName) {
        if (strSensorName == "SensLoad0") {
            return forceSens;
        } else if (strSensorName == "SensDef0") {
            return deformSens;
        } else if (strSensorName == "SensPrs0") {

        } else if (strSensorName == "SensPrs1") {

        } else if (strSensorName == "SensPrs2") {

        }
        return forceSens;
    }


public:
    LoadFrame();
    ~LoadFrame();

    bool init();
    bool deleteData();

    bool readNextStep();

    RETCODE next(QJsonObject &jOperation);

    RETCODE setTarget(QJsonObject &jOperation);

    RETCODE setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd);
//    RETCODE setPID_D(QJsonObject &jOperation);
//    RETCODE setUpPID_P(QJsonObject &jOperation);
//    RETCODE setUpPID_D(QJsonObject &jOperation);


    RETCODE setHz(QJsonObject &jOperation);

    RETCODE setStatePid(QJsonObject &jOperation);

    RETCODE statusSensors(QJsonObject &jOperation);

    RETCODE compression(QJsonObject &jOperation);

    RETCODE criterionTime(QJsonObject &jOperation);

    RETCODE criterionStabilization(QJsonObject &jOperation);

    RETCODE criterionManual(QJsonObject &jOperation);

    void resetStateModeBusCommunication();


    void readConfig();
    RETCODE writeConfig(QJsonObject &jobj);
    void startProcess();
    RETCODE moveFrame(QJsonObject &jobj);
    RETCODE unlockPID(QJsonObject &jobj);
    RETCODE stopFrame(QJsonObject &jobj);
//    RETCODE hardReset(QJsonObject &jobj);
    RETCODE sensorSetZero(QJsonObject &jobj);
    RETCODE resetSensorOffset(QJsonObject &jobj);

    void manualNextStep();
    void readSensors(QJsonObject &jobj);
    void sendProtocol(QJsonObject &jobj);
    void sendStoreData(QJsonObject &jobj);
    void switchToServiceMode(QJsonObject &jobj);


    Measurements::Force targetNewtones;
    Measurements::Force targetMinNewtones;
    Measurements::Area area;

    QJsonArray jsteps;
    QJsonObject jcurStep;
    QJsonObject jconfig;
    quint8 address = 0;

    Sensor forceSens;
    Sensor deformSens;

    Stepper stepper;
    Controller controller;


    StoreData *store = nullptr;

    QElapsedTimer timeElapse;
    QElapsedTimer criterionElapseTime;


    bool compressionManual = false;
    qint32 cur_speed = 0;
    bool togle = false;
    quint64 counter = 0;

private:
};


#endif // DEVICE_H
