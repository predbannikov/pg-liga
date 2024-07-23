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


class Plata {

public:
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE };
    quint16 address = 0x0;


    RETCODE ret = ERROR;
    STATE stateWrite = STATE_0x10_WRITE;
    FunctionCode funCode = ActBase;


    RETCODE write(QJsonObject &jOperation, OpCode cmd)
    {
        switch (stateWrite) {
        case STATE_0x03_WRITE:
            break;
        case STATE_0x03_CHECK:
            break;
        case STATE_COMPLATE:
            break;
        case STATE_0x10_WRITE:
            jOperation["PDU_request"] = QString(Requests::write(cmd, funCode, address));
            stateWrite = STATE_0x10_CHECK;
            break;
        case STATE_0x10_CHECK:
            stateWrite = STATE_0x10_WRITE;
            if (jOperation["modbus_error"].toString() == "no_error")
                return COMPLATE;
            else
                return ERROR;
            break;
        }
        return NOERROR;
    }
};

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
    RETCODE hardReset(QJsonObject &jobj);

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

    Plata plata;

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
