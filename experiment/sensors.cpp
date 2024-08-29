#include "sensors.h"

RETCODE Sensor::read(QJsonObject &jOperation) {
    return AbstractUnit::read(jOperation, SensorRead);
}

RETCODE Sensor::readRaw(QJsonObject &jOperation)
{
    return AbstractUnit::read(jOperation, SensorReadRaw);
}

RETCODE Sensor::setNull(QJsonObject &jOperation) {
    return write(jOperation, SensorNull);
}

RETCODE Sensor::resetOffset(QJsonObject &jOperation)
{
    return write(jOperation, SensorReset);
}

void Sensor::setValue(quint16 *data, AbstractUnit::CMD cmd) {
    if (cmd == SensorRead)
        value = qFromLittleEndian<float>(data);
    else if (cmd == SensorReadRaw)
        rawValue = qFromLittleEndian<quint32>(data);
}
