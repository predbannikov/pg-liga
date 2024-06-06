#ifndef SENSORS_H
#define SENSORS_H

#include "abstractunit.h"


class Sensor : public AbstractUnit
{
public:
    Sensor(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_) {}
    ~Sensor() {}

    float value;
    qint64 rawValue;

    RETCODE read(QJsonObject &jOperation);

    RETCODE readRaw(QJsonObject &jOperation);

    RETCODE setNull(QJsonObject &jOperation);

    RETCODE reset(QJsonObject &jOperation);


private:
    void setValue(quint16 *data, CMD cmd) override;
};

#endif // SENSORS_H
