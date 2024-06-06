#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>

#include "abstractunit.h"

class Controller : public AbstractUnit
{
    enum STATE_SET { STATE_SET_TARGET_VALUE, STATE_SET_TARGET, STATE_SET_PID_P_VALUE, STATE_SET_PID_P, STATE_SET_PID_D_VALUE, STATE_SET_PID_D, STATE_ACTIVATE_PID_VALUE, STATE_ACTIVATE_PID} stateSet;
    enum STATE_UNLOCK {STATE_UNLOCK_START, STATE_UNLOCK_SET_VALUE, STATE_UNLOCK_SET_CMD, STATE_HANDLE_ERROR} stateUnlock = STATE_UNLOCK_SET_VALUE;
    struct PIDParameters {
        double p;
        double i;
        double d;
    };
    PIDParameters defaultPID = {0.3, 0.0, 3.0};
    qint32 status;
public:
    Controller(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_) {}

    RETCODE readStatus(QJsonObject &jOperation);

    RETCODE setTarget(QJsonObject &jOperation, float newtons);

    RETCODE setKp(QJsonObject &jOperation);

    RETCODE setKi(QJsonObject &jOperation);

    RETCODE setKd(QJsonObject &jOperation);

//    RETCODE setPID(StatusOperation &operation, float p, float i, float d);

    RETCODE stopPID(QJsonObject &jOperation);




private:
    void setValue(quint16 *data, AbstractUnit::CMD cmd);
};

#endif // CONTROLLER_H
