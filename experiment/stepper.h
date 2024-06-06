#ifndef STEPPER_H
#define STEPPER_H

#include "abstractunit.h"


class Stepper : public AbstractUnit
{


    enum Direction {
        Normal,
        Inverted
    };

    enum SPEED {SPEED_SET_ARG, SPEED_ENABLE} state_speed = SPEED_SET_ARG;

    Direction direction = Inverted;
    Measurements::Length stepDistance = Measurements::Length::fromMicrometres(0.3125);

    Measurements::Speed speed;
    float speedHz;

public:
    Stepper(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_) {}
    ~Stepper() {}

    qint32 status = 0;
    qint32 position = 0;

    RETCODE readStatus(QJsonObject &jOperation);
    RETCODE readPos(QJsonObject &jOperation);
    RETCODE setSpeed(QJsonObject &jOperation, qint16 speed_);
    RETCODE stop(QJsonObject &jOperation);
    RETCODE setNull(QJsonObject &jOperation);


private:
    void setValue(quint16 *data, CMD cmd) override;

};

#endif // STEPPER_H
