#include "sensors.h"

RETCODE Sensor::read(StatusOperation &operation) {
    return AbstractUnit::read(operation, SensorRead);
}

RETCODE Sensor::readRaw(StatusOperation &operation)
{
    return AbstractUnit::read(operation, SensorReadRaw);
}

RETCODE Sensor::setNull(StatusOperation &operation) {
    return write(operation, SensorNull);
}

RETCODE Sensor::reset(StatusOperation &operation)
{
    return write(operation, SensorReset);
}

void Sensor::setValue(quint16 *data, AbstractUnit::CMD cmd) {
    if (cmd == SensorRead)
        value = qFromLittleEndian<float>(data);
    else if (cmd == SensorReadRaw)
        rawValue = qFromLittleEndian<quint32>(data);
}
