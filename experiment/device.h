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

class LoadFrame
{
    enum STATE_PROCESS {PROCESS_IDLE, PROCESS_READ_NEXT_STEP, PROCESS_EXPERIMENT, PROCESS_FINISH} stateProc = PROCESS_FINISH;

    enum STATE_PRESURE {STATE_PRESURE_1, STATE_PRESURE_2, STATE_PRESURE_3} statePresure = STATE_PRESURE_1;

    enum STATE {STATE_IDLE, STATE_START, STATE_INIT, STATE_READ_NEXT_STEP, STATE_EXPERIMENT, STATE_COMPLATION,
                STATE_MOVE_FRAME, STATE_STOP_FRAME,
                STATE_UNLOCK_PID,
                STATE_FINISH} state = STATE_IDLE;

    enum READ_SENS {READ_SENS_1, READ_SENS_2, READ_SENS_3, READ_SENS_4, READ_SENS_5, READ_SENS_6} readSensState;


    void init();


public:
    LoadFrame();
    ~LoadFrame();

    bool readNextStep();

    RETCODE next(StatusOperation &operation);

    RETCODE setPresure(StatusOperation &operation);

    RETCODE statusSensors(StatusOperation &operation);

    RETCODE compression(StatusOperation &operation);

    RETCODE criterionTime(StatusOperation &operation);

    RETCODE criterionStabilization(StatusOperation &operation);

    RETCODE criterionManual(StatusOperation &operation);


    void readConfig();
    void startProcess();
    void moveFrame(QJsonObject &jobj);
    void unlockPID();
    void stopFrame();
    void manualNextStep();
    void readSensors(QJsonObject &jobj);
    void sendProtocol(QJsonObject &jobj);


    Measurements::Pressure targetPresure;
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
