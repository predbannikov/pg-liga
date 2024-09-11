#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "global.h"

#include "interface.h"
#include <QJsonObject>
#include <QFile>
#include "operations.h"
#include "global.h"
#include "loadframe.h"
#include "volumeter1.h"
#include "volumeter2.h"


class Plata : public AbstractUnit {

public:
    Plata(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_){}
    ~Plata(){}

    void setValue(quint16 *data, AbstractUnit::CMD cmd) override {
    Q_UNUSED(data);
    Q_UNUSED(cmd);
    }
};


class Operations : public Interface
{
    Q_OBJECT

    enum STATE_SENSORS {
        VOLUMETR1_SENSOR_PRESSURE,
        VOLUMETR1_HOL_STATUS,
        VOLUMETR1_POSITION,
        VOLUMETR1_CONTROLLER_STATUS,
        VOLUMETR2_SENSOR_PRESSURE,
        VOLUMETR2_HOL_STATUS,
        VOLUMETR2_POSITION,
        VOLUMETR2_CONTROLLER_STATUS,
        LOADFRAME_SENSOR_FORCE,
        LOADFRAME_SENSOR_DEFORM,
        LOADFRAME_HOL_STATUS,
        LOADFRAME_POSITION,
        LOADFRAME_CONTROLLER_STATUS
        } vol_stat = VOLUMETR1_SENSOR_PRESSURE;

    enum STATE_MODE { STATE_MODE_IDLE, STATE_MODE_EXECCMD } state_mode = STATE_MODE_IDLE;

protected:
    enum STATE_EXPERIMENT { STATE_EXPERIMENT_IDLE, STATE_EXPERIMENT_PROCESS, STATE_EXPERIMENT_PAUSE, STATE_EXPERIMENT_TRANSIT_TO_PAUSE, STATE_EXPERIMENT_TRANSIT_TO_STOP } stateExperiment = STATE_EXPERIMENT_IDLE;


public:
    Operations(quint8 addr);
    ~Operations();

    void resetCommunicationState();
    bool execut();
    virtual void stateSwitch() = 0;

    QJsonObject jStatusOperation;
    QJsonObject jExperiment;
    QJsonObject jConfig;
    int counter = 0;
    LoadFrame *loadFrame = nullptr;
    Volumeter1 volumetr1;
    Volumeter2 volumetr2;
    Plata plata;
    StoreData *store = nullptr;
    RETCODE execCMD(QJsonObject &jobj);

    RETCODE readSensors(QJsonObject &jobj);


private:
    QString getStateExperiment();
};

#endif // OPERATIONS_H
