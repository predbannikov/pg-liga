#include "stepper.h"


RETCODE Stepper::readStatus(StatusOperation &operation) {
    return read(operation, StepperStatus);
}

RETCODE Stepper::readPos(StatusOperation &operation) {
    return read(operation, StepperPos);
}

RETCODE Stepper::setSpeed(StatusOperation &operation, qint16 speed_)
{
    if (speed_ > 100 || speed_ < -100) {
        qDebug() << "speed " << speed_ << "overflow";
        return ERROR;
    }
    switch (state_speed) {
    case SPEED_SET_ARG:
        speed = Measurements::Speed::fromMillimetresPerMinute(speed_);
        speedHz = speed.micrometresPerSecond() / stepDistance.micrometres();
        if (write(operation, speedHz) == COMPLATE)
            state_speed = SPEED_ENABLE;
        break;
    case SPEED_ENABLE:
        if (write(operation, StepperSpeed) == COMPLATE) {
            state_speed = SPEED_SET_ARG;
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
}

RETCODE Stepper::stop(StatusOperation &operation)
{
    return write(operation, StepperStop);
}

RETCODE Stepper::setNull(StatusOperation &operation)
{
    return write(operation, StepperNull);
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
