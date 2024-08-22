#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>

#include "abstractunit.h"

class Controller : public AbstractUnit
{
    enum STATE_SET { STATE_SET_TARGET_MIN_VALUE, STATE_SET_TARGET_MIN, STATE_SET_TARGET_VALUE, STATE_SET_TARGET, STATE_ACTIVATE_PID_VALUE, STATE_ACTIVATE_PID} stateSet = STATE_SET_TARGET_MIN_VALUE;
    enum STATE_UNLOCK {STATE_UNLOCK_START, STATE_UNLOCK_SET_VALUE, STATE_UNLOCK_SET_CMD, STATE_HANDLE_ERROR} stateUnlock = STATE_UNLOCK_SET_VALUE;
    enum STATE_SET_HZ {STATE_SET_HZ_WRITE, STATE_SET_HZ_CMD} stateSetHz = STATE_SET_HZ_WRITE;
    enum STATE_PID {STATE_SET_PID_VALUE, STATE_SET_PID} state_pid = STATE_SET_PID_VALUE;
    enum STATE_SET_STATE_PID {STATE_SET_STATE_WRITE, STATE_SET_STATE_CMD} stateSetStateController = STATE_SET_STATE_WRITE;
    struct PIDParameters {
        double p;
        double i;
        double d;
    };
    PIDParameters defaultPID = {0.3, 0.0, 3.0};
public:
    qint32 status = 0;

    Controller(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_) {}

    RETCODE readStatus(QJsonObject &jOperation);

    RETCODE setTarget(QJsonObject &jOperation, float newtonsMin, float newtons);

    RETCODE setKPID(QJsonObject &jOperation, float value, CMD cmd);

//    RETCODE setKi(QJsonObject &jOperation);

//    RETCODE setKd(QJsonObject &jOperation, float d, CMD cmd);

    RETCODE setHz(QJsonObject &jOperation, float hz);

    RETCODE setStatePidController(QJsonObject &jOperation, int set_state_pid_controller);

//    RETCODE setPID(StatusOperation &operation, float p, float i, float d);

    RETCODE stopPID(QJsonObject &jOperation);


private:
    void setValue(quint16 *data, AbstractUnit::CMD cmd);
};

#endif // CONTROLLER_H
