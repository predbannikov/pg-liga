#include "stepper.h"


RETCODE Stepper::readStatus(QJsonObject &jOperation) {
    return read(jOperation, StepperStatus);
}

RETCODE Stepper::readPos(QJsonObject &jOperation) {
    return read(jOperation, StepperPos);
}

RETCODE Stepper::setSpeed(QJsonObject &jOperation, qint16 speed_)
{
    if (speed_ > 100 || speed_ < -100) {
        qDebug() << "speed " << speed_ << "overflow";
        return ERROR;
    }
    switch (state_speed) {
    case SPEED_SET_ARG:
        speed = Measurements::Speed::fromMillimetresPerMinute(speed_);
        speedHz = speed.micrometresPerSecond() / stepDistance.micrometres();
        if (write(jOperation, speedHz) == COMPLATE)
            state_speed = SPEED_ENABLE;
        break;
    case SPEED_ENABLE:
        if (write(jOperation, StepperSpeed) == COMPLATE) {
            state_speed = SPEED_SET_ARG;
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
}

RETCODE Stepper::stop(QJsonObject &jOperation)
{
    return write(jOperation, StepperStop);
}

RETCODE Stepper::setNull(QJsonObject &jOperation)
{
    return write(jOperation, StepperNull);
}

void Stepper::setValue(quint16 *data, AbstractUnit::CMD cmd) {
    if (cmd == StepperStatus)
        status = qFromLittleEndian<qint32>(data);
    else if (cmd == StepperPos)
        position = qFromLittleEndian<qint32>(data);
}

void test() {

//    const auto speedHz = speed.micrometresPerSecond() / commonParameters().stepDistance.micrometres();
//    return m_stepper->setSpeed(commonParameters().direction == Normal ? speedHz : -speedHz);


}
